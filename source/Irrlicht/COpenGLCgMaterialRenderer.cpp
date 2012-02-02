// Copyright (C) 2011-2012 Patryk Nadrowski
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "IrrCompileConfig.h"
#if defined(_IRR_COMPILE_WITH_OPENGL_) && defined(_IRR_COMPILE_WITH_CG_)

#include "COpenGLCgMaterialRenderer.h"
#include "COpenGLDriver.h"
#include "COpenGLTexture.h"

namespace irr
{
namespace video
{

COpenGLCgUniformSampler2D::COpenGLCgUniformSampler2D(const CGparameter& pParameter, bool pIsGlobal) : CCgUniform(pParameter, pIsGlobal)
{
	Type = CG_SAMPLER2D;
}

void COpenGLCgUniformSampler2D::update(const float* pData, const SMaterial& pMaterial) const
{
	int LayerID = (int)*pData;

	if (pMaterial.TextureLayer[LayerID].Texture)
	{
		int TextureID = reinterpret_cast<COpenGLTexture*>(pMaterial.TextureLayer[LayerID].Texture)->getOpenGLTextureName();

		cgGLSetTextureParameter(Parameter, TextureID);
		cgGLEnableTextureParameter(Parameter);
	}
}

COpenGLCgMaterialRenderer::COpenGLCgMaterialRenderer(COpenGLDriver* pDriver, s32& pMaterialType,
	const c8* pVertexProgram, const c8* pVertexEntry, E_VERTEX_SHADER_TYPE pVertexProfile,
	const c8* pFragmentProgram, const c8* pFragmentEntry, E_PIXEL_SHADER_TYPE pFragmentProfile,
	const c8* pGeometryProgram, const c8* pGeometryEntry, E_GEOMETRY_SHADER_TYPE pGeometryProfile,
	scene::E_PRIMITIVE_TYPE pInType, scene::E_PRIMITIVE_TYPE pOutType, u32 pVertices,
	IShaderConstantSetCallBack* pCallBack, IMaterialRenderer* pBaseMaterial, s32 pUserData) :
	Driver(pDriver), CCgMaterialRenderer(pCallBack, pBaseMaterial, pUserData)
{
	#ifdef _DEBUG
	setDebugName("COpenGLCgMaterialRenderer");
	#endif

	init(pMaterialType, pVertexProgram, pVertexEntry, pVertexProfile, pFragmentProgram, pFragmentEntry, pFragmentProfile,
		pGeometryProgram, pGeometryEntry, pGeometryProfile, pInType, pOutType, pVertices);
}

COpenGLCgMaterialRenderer::~COpenGLCgMaterialRenderer()
{
	if (VertexProgram)
	{
		cgGLUnloadProgram(VertexProgram);
		cgDestroyProgram(VertexProgram);
	}
	if (FragmentProgram)
	{
		cgGLUnloadProgram(FragmentProgram);
		cgDestroyProgram(FragmentProgram);
	}
	if (GeometryProgram)
	{
		cgGLUnloadProgram(GeometryProgram);
		cgDestroyProgram(GeometryProgram);
	}
}

void COpenGLCgMaterialRenderer::OnSetMaterial(const SMaterial& pMaterial, const SMaterial& pLastMaterial, bool pResetRS, IMaterialRendererServices* pService)
{
	Material = pMaterial;

	if (pMaterial.MaterialType != pLastMaterial.MaterialType || pResetRS)
	{
		if (VertexProgram)
		{
			cgGLEnableProfile(VertexProfile);
			cgGLBindProgram(VertexProgram);
		}

		if (FragmentProgram)
		{
			cgGLEnableProfile(FragmentProfile);
			cgGLBindProgram(FragmentProgram);
		}

		if (GeometryProgram)
		{
			cgGLEnableProfile(GeometryProfile);
			cgGLBindProgram(GeometryProgram);
		}

		if (BaseMaterial)
			BaseMaterial->OnSetMaterial(pMaterial, pMaterial, true, this);
	}

	if (CallBack)
		CallBack->OnSetMaterial(pMaterial);

	for (u32 i=0; i<MATERIAL_MAX_TEXTURES; ++i)
		Driver->setActiveTexture(i, pMaterial.getTexture(i));

	Driver->setBasicRenderStates(pMaterial, pLastMaterial, pResetRS);
}

bool COpenGLCgMaterialRenderer::OnRender(IMaterialRendererServices* pService, E_VERTEX_TYPE pVertexType)
{
	if (CallBack && (VertexProgram || FragmentProgram || GeometryProgram))
		CallBack->OnSetConstants(this, UserData);

	return true;
}

void COpenGLCgMaterialRenderer::OnUnsetMaterial()
{
	if (VertexProgram)
	{
		cgGLUnbindProgram(VertexProfile);
		cgGLDisableProfile(VertexProfile);
	}
	if (FragmentProgram)
	{
		cgGLUnbindProgram(FragmentProfile);
		cgGLDisableProfile(FragmentProfile);
	}
	if (GeometryProgram)
	{
		cgGLUnbindProgram(GeometryProfile);
		cgGLDisableProfile(GeometryProfile);
	}

	if (BaseMaterial)
		BaseMaterial->OnUnsetMaterial();

	Material = IdentityMaterial;;
}

void COpenGLCgMaterialRenderer::setBasicRenderStates(const SMaterial& pMaterial, const SMaterial& pLastMaterial, bool pResetRS)
{
	Driver->setBasicRenderStates(pMaterial, pLastMaterial, pResetRS);
}

IVideoDriver* COpenGLCgMaterialRenderer::getVideoDriver()
{
	return Driver;
}

void COpenGLCgMaterialRenderer::init(s32& pMaterialType,
	const c8* pVertexProgram, const c8* pVertexEntry, E_VERTEX_SHADER_TYPE pVertexProfile,
	const c8* pFragmentProgram, const c8* pFragmentEntry, E_PIXEL_SHADER_TYPE pFragmentProfile,
	const c8* pGeometryProgram, const c8* pGeometryEntry, E_GEOMETRY_SHADER_TYPE pGeometryProfile,
	scene::E_PRIMITIVE_TYPE pInType, scene::E_PRIMITIVE_TYPE pOutType, u32 pVertices)
{
	bool Status = true;
	CGerror Error = CG_NO_ERROR;
	pMaterialType = -1;

	// TODO: add profile selection

	if (pVertexProgram)
	{
		VertexProfile = cgGLGetLatestProfile(CG_GL_VERTEX);

		if (VertexProfile)
			VertexProgram = cgCreateProgram(Driver->getCgContext(), CG_SOURCE, pVertexProgram, VertexProfile, pVertexEntry, 0);

		if (!VertexProgram)
		{
			Error = cgGetError();
			os::Printer::log("Cg vertex program failed to compile:", ELL_ERROR);
			os::Printer::log(cgGetLastListing(Driver->getCgContext()), ELL_ERROR);

			Status = false;
		}
		else
			cgGLLoadProgram(VertexProgram);
	}

	if (pFragmentProgram)
	{
		FragmentProfile = cgGLGetLatestProfile(CG_GL_FRAGMENT);

		if (FragmentProfile)
			FragmentProgram = cgCreateProgram(Driver->getCgContext(), CG_SOURCE, pFragmentProgram, FragmentProfile, pFragmentEntry, 0);

		if (!FragmentProgram)
		{
			Error = cgGetError();
			os::Printer::log("Cg fragment program failed to compile:", ELL_ERROR);
			os::Printer::log(cgGetLastListing(Driver->getCgContext()), ELL_ERROR);

			Status = false;
		}
		else
			cgGLLoadProgram(FragmentProgram);
	}

	if (pGeometryProgram)
	{
		GeometryProfile = cgGLGetLatestProfile(CG_GL_GEOMETRY);

		if (GeometryProfile)
			GeometryProgram = cgCreateProgram(Driver->getCgContext(), CG_SOURCE, pGeometryProgram, GeometryProfile, pGeometryEntry, 0);

		if (!GeometryProgram)
		{
			Error = cgGetError();
			os::Printer::log("Cg geometry program failed to compile:", ELL_ERROR);
			os::Printer::log(cgGetLastListing(Driver->getCgContext()), ELL_ERROR);

			Status = false;
		}
		else
			cgGLLoadProgram(GeometryProgram);
	}

	getUniformList();

	// create OpenGL specifics sampler uniforms.
	for (unsigned int i = 0; i < UniformInfo.size(); ++i)
	{
		if (UniformInfo[i]->getType() == CG_SAMPLER2D)
		{
			bool IsGlobal = true;

			if (UniformInfo[i]->getSpace() == CG_PROGRAM)
				IsGlobal = false;

			CCgUniform* Uniform = new COpenGLCgUniformSampler2D(UniformInfo[i]->getParameter(), IsGlobal);
			delete UniformInfo[i];
			UniformInfo[i] = Uniform;
		}
	}

	if (Status)
		pMaterialType = Driver->addMaterialRenderer(this);
}

}
}

#endif
