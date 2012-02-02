// Copyright (C) 2011-2012 Patryk Nadrowski
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_DIRECT3D_9_CG_MATERIAL_RENDERER_H_INCLUDED__
#define __C_DIRECT3D_9_CG_MATERIAL_RENDERER_H_INCLUDED__

#include "IrrCompileConfig.h"
#if defined(_IRR_COMPILE_WITH_DIRECT3D_9_) && defined(_IRR_COMPILE_WITH_CG_)

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include "CCgMaterialRenderer.h"
#include "Cg/cgD3D9.h"

#ifdef _MSC_VER
	#pragma comment(lib, "cgD3D9.lib")
#endif

namespace irr
{
namespace video
{

class CD3D9Driver;
class IShaderConstantSetCallBack;

class CD3D9CgUniformSampler2D : public CCgUniform
{
public:
	CD3D9CgUniformSampler2D(const CGparameter& pParameter, bool pIsGlobal);

	void update(const f32* pData, const SMaterial& pMaterial) const;
};

class CD3D9CgMaterialRenderer : public CCgMaterialRenderer
{
public:
	CD3D9CgMaterialRenderer(CD3D9Driver* pDriver, s32& pMaterialType,
		const c8* pVertexProgram = 0, const c8* pVertexEntry = "main",
		E_VERTEX_SHADER_TYPE pVertexProfile = video::EVST_VS_1_1,
		const c8* pFragmentProgram = 0, const c8* pFragmentEntry = "main",
		E_PIXEL_SHADER_TYPE pFragmentProfile = video::EPST_PS_1_1,
		const c8* pGeometryProgram = 0, const c8* pGeometryEntry = "main",
		E_GEOMETRY_SHADER_TYPE pGeometryProfile = video::EGST_GS_4_0,
		scene::E_PRIMITIVE_TYPE pInType = scene::EPT_TRIANGLES,
		scene::E_PRIMITIVE_TYPE pOutType = scene::EPT_TRIANGLE_STRIP,
		u32 pVertices = 0, IShaderConstantSetCallBack* pCallBack = 0,
		IMaterialRenderer* pBaseMaterial = 0, s32 pUserData = 0);

	virtual ~CD3D9CgMaterialRenderer();

	virtual void OnSetMaterial(const SMaterial& pMaterial, const SMaterial& pLastMaterial, bool pResetRS, IMaterialRendererServices* pService);
	virtual bool OnRender(IMaterialRendererServices* pService, E_VERTEX_TYPE pVertexType);
	virtual void OnUnsetMaterial();

	virtual void setBasicRenderStates(const SMaterial& pMaterial, const SMaterial& pLastMaterial, bool pResetRS);
	virtual IVideoDriver* getVideoDriver();

protected:
	void init(s32& pMaterialType,
		const c8* pVertexProgram = 0, const c8* pVertexEntry = "main",
		E_VERTEX_SHADER_TYPE pVertexProfile = video::EVST_VS_1_1,
		const c8* pFragmentProgram = 0, const c8* pFragmentEntry = "main",
		E_PIXEL_SHADER_TYPE pFragmentProfile = video::EPST_PS_1_1,
		const c8* pGeometryProgram = 0, const c8* pGeometryEntry = "main",
		E_GEOMETRY_SHADER_TYPE pGeometryProfile = video::EGST_GS_4_0,
		scene::E_PRIMITIVE_TYPE pInType = scene::EPT_TRIANGLES,
		scene::E_PRIMITIVE_TYPE pOutType = scene::EPT_TRIANGLE_STRIP,
		u32 pVertices = 0);

	CD3D9Driver* Driver;
};

}
}

#endif
#endif

