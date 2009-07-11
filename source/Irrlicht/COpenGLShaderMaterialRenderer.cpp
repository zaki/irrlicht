// Copyright (C) 2002-2009 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_OPENGL_

#include "COpenGLShaderMaterialRenderer.h"
#include "IGPUProgrammingServices.h"
#include "IShaderConstantSetCallBack.h"
#include "IVideoDriver.h"
#include "os.h"
#include "COpenGLDriver.h"

namespace irr
{
namespace video
{


//! Constructor
COpenGLShaderMaterialRenderer::COpenGLShaderMaterialRenderer(video::COpenGLDriver* driver,
	s32& outMaterialTypeNr, const c8* vertexShaderProgram, const c8* pixelShaderProgram,
	IShaderConstantSetCallBack* callback, IMaterialRenderer* baseMaterial, s32 userData)
	: Driver(driver), CallBack(callback), BaseMaterial(baseMaterial),
		VertexShader(0), UserData(userData)
{
	#ifdef _DEBUG
	setDebugName("COpenGLShaderMaterialRenderer");
	#endif

	PixelShader.set_used(4);
	for (u32 i=0; i<4; ++i)
	{
		PixelShader[i]=0;
	}

	if (BaseMaterial)
		BaseMaterial->grab();

	if (CallBack)
		CallBack->grab();

	init(outMaterialTypeNr, vertexShaderProgram, pixelShaderProgram, EVT_STANDARD);
}


//! constructor only for use by derived classes who want to
//! create a fall back material for example.
COpenGLShaderMaterialRenderer::COpenGLShaderMaterialRenderer(COpenGLDriver* driver,
				IShaderConstantSetCallBack* callback,
				IMaterialRenderer* baseMaterial, s32 userData)
: Driver(driver), CallBack(callback), BaseMaterial(baseMaterial),
		VertexShader(0), UserData(userData)
{
	PixelShader.set_used(4);
	for (u32 i=0; i<4; ++i)
	{
		PixelShader[i]=0;
	}

	if (BaseMaterial)
		BaseMaterial->grab();

	if (CallBack)
		CallBack->grab();
}


//! Destructor
COpenGLShaderMaterialRenderer::~COpenGLShaderMaterialRenderer()
{
	if (CallBack)
		CallBack->drop();

	if (VertexShader)
		Driver->extGlDeletePrograms(1, &VertexShader);

	for (u32 i=0; i<PixelShader.size(); ++i)
		if (PixelShader[i])
			Driver->extGlDeletePrograms(1, &PixelShader[i]);

	if (BaseMaterial)
		BaseMaterial->drop();
}


void COpenGLShaderMaterialRenderer::init(s32& outMaterialTypeNr, const c8* vertexShaderProgram,
	const c8* pixelShaderProgram, E_VERTEX_TYPE type)
{
	outMaterialTypeNr = -1;

	bool success;

	// create vertex shader
	success=createVertexShader(vertexShaderProgram);

	// create pixel shader
	if (!createPixelShader(pixelShaderProgram) || !success)
		return;

	// register as a new material
	outMaterialTypeNr = Driver->addMaterialRenderer(this);
}


bool COpenGLShaderMaterialRenderer::OnRender(IMaterialRendererServices* service, E_VERTEX_TYPE vtxtype)
{
	// call callback to set shader constants
	if (CallBack && (VertexShader || PixelShader[0]))
		CallBack->OnSetConstants(service, UserData);

	return true;
}


void COpenGLShaderMaterialRenderer::OnSetMaterial(const video::SMaterial& material, const video::SMaterial& lastMaterial,
	bool resetAllRenderstates, video::IMaterialRendererServices* services)
{
	if (material.MaterialType != lastMaterial.MaterialType || resetAllRenderstates)
	{
#ifdef GL_ARB_vertex_program
		if (VertexShader)
		{
			// set new vertex shader
			Driver->extGlBindProgram(GL_VERTEX_PROGRAM_ARB, VertexShader);
			glEnable(GL_VERTEX_PROGRAM_ARB);
		}
#endif

		// set new pixel shader
#ifdef GL_ARB_fragment_program
		if (PixelShader[0])
		{
			if (!material.FogEnable)
			{
				Driver->extGlBindProgram(GL_FRAGMENT_PROGRAM_ARB, PixelShader[0]);
			}
			else
			{
				GLint curFogMode;
				glGetIntegerv(GL_FOG_MODE, &curFogMode);
//				if (Driver->LinearFog && PixelShader[1])
				if (curFogMode==GL_LINEAR && PixelShader[1])
					Driver->extGlBindProgram(GL_FRAGMENT_PROGRAM_ARB, PixelShader[1]);
//				else if (!Driver->LinearFog && PixelShader[2])
				else if (curFogMode==GL_EXP && PixelShader[2])
					Driver->extGlBindProgram(GL_FRAGMENT_PROGRAM_ARB, PixelShader[2]);
				else if (curFogMode==GL_EXP2 && PixelShader[3])
					Driver->extGlBindProgram(GL_FRAGMENT_PROGRAM_ARB, PixelShader[3]);
				else
					Driver->extGlBindProgram(GL_FRAGMENT_PROGRAM_ARB, PixelShader[0]);
			}
			glEnable(GL_FRAGMENT_PROGRAM_ARB);
		}
#endif

		if (BaseMaterial)
			BaseMaterial->OnSetMaterial(material, material, true, services);
	}

	//let callback know used material
	if (CallBack)
		CallBack->OnSetMaterial(material);

	for (u32 i=0; i<MATERIAL_MAX_TEXTURES; ++i)
		Driver->setActiveTexture(i, material.getTexture(i));
	Driver->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);
}


void COpenGLShaderMaterialRenderer::OnUnsetMaterial()
{
	// disable vertex shader
#ifdef GL_ARB_vertex_program
	if (VertexShader)
		glDisable(GL_VERTEX_PROGRAM_ARB);
#endif

#ifdef GL_ARB_fragment_program
	if (PixelShader[0])
		glDisable(GL_FRAGMENT_PROGRAM_ARB);
#endif

	if (BaseMaterial)
		BaseMaterial->OnUnsetMaterial();
}


//! Returns if the material is transparent.
bool COpenGLShaderMaterialRenderer::isTransparent() const
{
	return BaseMaterial ? BaseMaterial->isTransparent() : false;
}


// This method needs a properly cleaned error state before the checked instruction is called
bool COpenGLShaderMaterialRenderer::checkError(const irr::c8* type)
{
#ifdef GL_ARB_vertex_program
	GLenum g = glGetError();
	if (g != GL_NO_ERROR)
	{
		GLint errPos;
		glGetIntegerv( GL_PROGRAM_ERROR_POSITION_ARB, &errPos );

		core::stringc errString = type;
		errString += " compilation failed at position ";
		errString += core::stringc(errPos);
		errString += ":\n";
		errString += reinterpret_cast<const char*>(glGetString(GL_PROGRAM_ERROR_STRING_ARB));

		os::Printer::log(errString.c_str(), ELL_ERROR);
		return true;
	}
#endif
	return false;
}


bool COpenGLShaderMaterialRenderer::createPixelShader(const c8* pxsh)
{
	if (!pxsh)
		return true;

	const core::stringc inshdr(pxsh);
	core::stringc shdr;
	const s32 pos = inshdr.find("#_IRR_FOG_MODE_");
	const u32 numShaders = (-1 != pos)?4:1;

	for (u32 i=0; i<numShaders; ++i)
	{
		if (i==0)
		{
			shdr=inshdr;
		}
		else
		{
			shdr = inshdr.subString(0, pos);
			switch (i) {
				case 1: shdr += "OPTION ARB_fog_linear;"; break;
				case 2: shdr += "OPTION ARB_fog_exp;"; break;
				case 3: shdr += "OPTION ARB_fog_exp2;"; break;
			}
			shdr += inshdr.subString(pos+16, inshdr.size()-pos-16);
		}
#ifdef GL_ARB_fragment_program
		Driver->extGlGenPrograms(1, &PixelShader[i]);
		Driver->extGlBindProgram(GL_FRAGMENT_PROGRAM_ARB, PixelShader[i]);

		// clear error buffer
		while(glGetError() != GL_NO_ERROR)
			{}

		// compile
		Driver->extGlProgramString(GL_FRAGMENT_PROGRAM_ARB, GL_PROGRAM_FORMAT_ASCII_ARB,
			shdr.size(), shdr.c_str());

		if (checkError("Pixel shader"))
		{
			Driver->extGlDeletePrograms(1, &PixelShader[i]);
			PixelShader[i]=0;

			return false;
		}
#else
		return false;
#endif
	}

	return true;
}


bool COpenGLShaderMaterialRenderer::createVertexShader(const c8* vtxsh)
{
	if (!vtxsh)
		return true;

#ifdef GL_ARB_vertex_program
	Driver->extGlGenPrograms(1, &VertexShader);
	Driver->extGlBindProgram(GL_VERTEX_PROGRAM_ARB, VertexShader);

	// clear error buffer
	while(glGetError() != GL_NO_ERROR)
	{}

	// compile
	Driver->extGlProgramString(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_FORMAT_ASCII_ARB,
		strlen(vtxsh), vtxsh);

	if (checkError("Vertex shader"))
	{
		Driver->extGlDeletePrograms(1, &VertexShader);
		VertexShader=0;

		return false;
	}
#else
	return false;
#endif

	return true;
}


} // end namespace video
} // end namespace irr


#endif
