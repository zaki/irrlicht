// Copyright (C) 2011 Patryk Nadrowski
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_CG_MATERIAL_RENDERER_H_INCLUDED__
#define __C_CG_MATERIAL_RENDERER_H_INCLUDED__

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_CG_

#include "IMaterialRenderer.h"
#include "IMaterialRendererServices.h"
#include "IShaderConstantSetCallBack.h"
#include "IGPUProgrammingServices.h"
#include "irrArray.h"
#include "irrString.h"
#include "IVideoDriver.h"
#include "os.h"
#include "Cg/cg.h"

#ifdef _MSC_VER
	#pragma comment(lib, "cg.lib")
#endif

namespace irr
{
namespace video
{

class CCgUniform
{
public:
	CCgUniform(const CGparameter& pParameter, bool pIsGlobal);

	const core::stringc& getName() const;
	const CGparameter& getParameter() const;
	CGenum getSpace() const;
	CGtype getType() const;

	virtual void update(const f32* pData, const SMaterial& pMaterial) const = 0;

protected:
	core::stringc Name;
	CGparameter Parameter;
	CGenum Space;
	CGtype Type;
};

class CCgUniform1f : public CCgUniform
{
public:
	CCgUniform1f(const CGparameter& pParameter, bool pIsGlobal);

	void update(const f32* pData, const SMaterial& pMaterial) const;
};

class CCgUniform2f : public CCgUniform
{
public:
	CCgUniform2f(const CGparameter& pParameter, bool pIsGlobal);

	void update(const f32* pData, const SMaterial& pMaterial) const;
};

class CCgUniform3f : public CCgUniform
{
public:
	CCgUniform3f(const CGparameter& pParameter, bool pIsGlobal);

	void update(const f32* pData, const SMaterial& pMaterial) const;
};

class CCgUniform4f : public CCgUniform
{
public:
	CCgUniform4f(const CGparameter& pParameter, bool pIsGlobal);

	void update(const f32* pData, const SMaterial& pMaterial) const;
};

class CCgUniform4x4f : public CCgUniform
{
public:
	CCgUniform4x4f(const CGparameter& pParameter, bool pIsGlobal);

	void update(const f32* pData, const SMaterial& pMaterial) const;
};

class CCgUniformSampler2D : public CCgUniform
{
public:
	CCgUniformSampler2D(const CGparameter& pParameter, bool pIsGlobal);

	void update(const f32* pData, const SMaterial& pMaterial) const;
};

class CCgMaterialRenderer : public IMaterialRenderer, public IMaterialRendererServices
{
public:
	CCgMaterialRenderer(IShaderConstantSetCallBack* pCallBack = 0, IMaterialRenderer* pBaseMaterial = 0, s32 pUserData = 0);
	virtual ~CCgMaterialRenderer();

	virtual void OnSetMaterial(const SMaterial& pMaterial, const SMaterial& pLastMaterial, bool pResetRS, IMaterialRendererServices* pService) = 0;
	virtual bool OnRender(IMaterialRendererServices* pService, E_VERTEX_TYPE pVertexType) = 0;
	virtual void OnUnsetMaterial() = 0;

	virtual bool isTransparent() const;

	virtual void setBasicRenderStates(const SMaterial& pMaterial, const SMaterial& pLastMaterial, bool pResetRS) = 0;
	virtual bool setVertexShaderConstant(const c8* pName, const f32* pData, int pCount);
	virtual void setVertexShaderConstant(const f32* pData, s32 pStartRegister, s32 pConstantAmount = 1);
	virtual bool setPixelShaderConstant(const c8* pName, const f32* pData, int pCount);
	virtual void setPixelShaderConstant(const f32* pData, s32 pStartRegister, s32 pConstantAmount = 1);
	virtual IVideoDriver* getVideoDriver() = 0;

protected:
	void getUniformList();

	IShaderConstantSetCallBack* CallBack;
	IMaterialRenderer* BaseMaterial;
	s32 UserData;

	core::array<CCgUniform*> UniformInfo;

	CGprogram VertexProgram;
	CGprogram FragmentProgram;
	CGprogram GeometryProgram;
	CGprofile VertexProfile;
	CGprofile FragmentProfile;
	CGprofile GeometryProfile;

	SMaterial Material;
	CGerror Error;
};

}
}

#endif
#endif
