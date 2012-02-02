// Copyright (C) 2011-2012 Patryk Nadrowski
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_OPENGL_CG_MATERIAL_RENDERER_H_INCLUDED__
#define __C_OPENGL_CG_MATERIAL_RENDERER_H_INCLUDED__

#include "IrrCompileConfig.h"
#if defined(_IRR_COMPILE_WITH_OPENGL_) && defined(_IRR_COMPILE_WITH_CG_)

#ifdef _IRR_WINDOWS_API_
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#include <GL/gl.h>
	#include "glext.h"
#else
#if defined(_IRR_OPENGL_USE_EXTPOINTER_)
	#define GL_GLEXT_LEGACY 1
#else
	#define GL_GLEXT_PROTOTYPES 1
#endif
#if defined(_IRR_OSX_PLATFORM_)
	#include <OpenGL/gl.h>
#else
	#include <GL/gl.h>
#endif
#if defined(_IRR_OPENGL_USE_EXTPOINTER_)
	#include "glext.h"
#endif
#endif

#include "CCgMaterialRenderer.h"
#include "Cg/cgGL.h"

#ifdef _MSC_VER
	#pragma comment(lib, "cgGL.lib")
#endif

namespace irr
{
namespace video
{

class COpenGLDriver;
class IShaderConstantSetCallBack;

class COpenGLCgUniformSampler2D : public CCgUniform
{
public:
	COpenGLCgUniformSampler2D(const CGparameter& pParameter, bool pIsGlobal);

	void update(const f32* pData, const SMaterial& pMaterial) const;
};

class COpenGLCgMaterialRenderer : public CCgMaterialRenderer
{
public:
	COpenGLCgMaterialRenderer(COpenGLDriver* pDriver, s32& pMaterialType,
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

	virtual ~COpenGLCgMaterialRenderer();

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

	COpenGLDriver* Driver;
};

}
}

#endif
#endif

