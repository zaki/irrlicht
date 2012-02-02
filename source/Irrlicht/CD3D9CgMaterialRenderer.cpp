// Copyright (C) 2011-2012 Patryk Nadrowski
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "IrrCompileConfig.h"
#if defined(_IRR_COMPILE_WITH_DIRECT3D_9_) && defined(_IRR_COMPILE_WITH_CG_)

#include "CD3D9CgMaterialRenderer.h"
#include "CD3D9Driver.h"
#include "CD3D9Texture.h"

namespace irr
{
namespace video
{

CD3D9CgUniformSampler2D::CD3D9CgUniformSampler2D(const CGparameter& pParameter, bool pIsGlobal) : CCgUniform(pParameter, pIsGlobal)
{
	Type = CG_SAMPLER2D;
}

void CD3D9CgUniformSampler2D::update(const float* pData, const SMaterial& pMaterial) const
{
	int LayerID = (int)*pData;

	if (pMaterial.TextureLayer[LayerID].Texture)
	{
		IDirect3DBaseTexture9* Texture = reinterpret_cast<irr::video::CD3D9Texture*>(pMaterial.TextureLayer[LayerID].Texture)->getDX9Texture();

		cgD3D9SetTextureParameter(Parameter, Texture);
	}
}

CD3D9CgMaterialRenderer::CD3D9CgMaterialRenderer(CD3D9Driver* pDriver, s32& pMaterialType,
	const c8* pVertexProgram, const c8* pVertexEntry, E_VERTEX_SHADER_TYPE pVertexProfile,
	const c8* pFragmentProgram, const c8* pFragmentEntry, E_PIXEL_SHADER_TYPE pFragmentProfile,
	const c8* pGeometryProgram, const c8* pGeometryEntry, E_GEOMETRY_SHADER_TYPE pGeometryProfile,
	scene::E_PRIMITIVE_TYPE pInType, scene::E_PRIMITIVE_TYPE pOutType, u32 pVertices,
	IShaderConstantSetCallBack* pCallBack, IMaterialRenderer* pBaseMaterial, s32 pUserData) :
	Driver(pDriver), CCgMaterialRenderer(pCallBack, pBaseMaterial, pUserData)
{
	#ifdef _DEBUG
	setDebugName("CD3D9CgMaterialRenderer");
	#endif

	init(pMaterialType, pVertexProgram, pVertexEntry, pVertexProfile, pFragmentProgram, pFragmentEntry, pFragmentProfile,
		pGeometryProgram, pGeometryEntry, pGeometryProfile, pInType, pOutType, pVertices);
}

CD3D9CgMaterialRenderer::~CD3D9CgMaterialRenderer()
{
	if (VertexProgram)
	{
		cgD3D9UnloadProgram(VertexProgram);
		cgDestroyProgram(VertexProgram);
	}
	if (FragmentProgram)
	{
		cgD3D9UnloadProgram(FragmentProgram);
		cgDestroyProgram(FragmentProgram);
	}
	/*if (GeometryProgram)
	{
		cgD3D9UnloadProgram(GeometryProgram);
		cgDestroyProgram(GeometryProgram);
	}*/
}

void CD3D9CgMaterialRenderer::OnSetMaterial(const SMaterial& pMaterial, const SMaterial& pLastMaterial, bool pResetRS, IMaterialRendererServices* pService)
{
	Material = pMaterial;

	if (pMaterial.MaterialType != pLastMaterial.MaterialType || pResetRS)
	{
		if (VertexProgram)
			cgD3D9BindProgram(VertexProgram);

		if (FragmentProgram)
			cgD3D9BindProgram(FragmentProgram);

		/*if (GeometryProgram)
			cgD3D9BindProgram(GeometryProgram);*/

		if (BaseMaterial)
			BaseMaterial->OnSetMaterial(pMaterial, pMaterial, true, this);
	}

	if (CallBack)
		CallBack->OnSetMaterial(pMaterial);

	Driver->setBasicRenderStates(pMaterial, pLastMaterial, pResetRS);
}

bool CD3D9CgMaterialRenderer::OnRender(IMaterialRendererServices* pService, E_VERTEX_TYPE pVertexType)
{
	if (CallBack && (VertexProgram || FragmentProgram || GeometryProgram))
		CallBack->OnSetConstants(this, UserData);

	return true;
}

void CD3D9CgMaterialRenderer::OnUnsetMaterial()
{
	if (VertexProgram)
		cgD3D9UnbindProgram(VertexProgram);
	if (FragmentProgram)
		cgD3D9UnbindProgram(FragmentProgram);
	/*if (GeometryProgram)
		cgD3D9UnbindProgram(GeometryProgram);*/

	if (BaseMaterial)
		BaseMaterial->OnUnsetMaterial();

	Material = IdentityMaterial;;
}

void CD3D9CgMaterialRenderer::setBasicRenderStates(const SMaterial& pMaterial, const SMaterial& pLastMaterial, bool pResetRS)
{
	Driver->setBasicRenderStates(pMaterial, pLastMaterial, pResetRS);
}

IVideoDriver* CD3D9CgMaterialRenderer::getVideoDriver()
{
	return Driver;
}

void CD3D9CgMaterialRenderer::init(s32& pMaterialType,
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
		VertexProfile = cgD3D9GetLatestVertexProfile();

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
			cgD3D9LoadProgram(VertexProgram, 0, 0);
	}

	if (pFragmentProgram)
	{
		FragmentProfile = cgD3D9GetLatestPixelProfile();

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
			cgD3D9LoadProgram(FragmentProgram, 0, 0);
	}

	/*if (pGeometryProgram)
	{
		GeometryProfile = cgD3D9GetLatestGeometryProfile();

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
			cgD3D9LoadProgram(GeometryProgram, 0, 0);
	}*/

	getUniformList();

	// create D3D9 specifics sampler uniforms.
	for(unsigned int i = 0; i < UniformInfo.size(); ++i)
	{
		if (UniformInfo[i]->getType() == CG_SAMPLER2D)
		{
			bool IsGlobal = true;

			if (UniformInfo[i]->getSpace() == CG_PROGRAM)
				IsGlobal = false;

			CCgUniform* Uniform = new CD3D9CgUniformSampler2D(UniformInfo[i]->getParameter(), IsGlobal);
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
