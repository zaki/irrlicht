// Copyright (C) 2012 Patryk Nadrowski
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
	CCgUniform(const CGparameter& parameter, bool global);
	virtual ~CCgUniform();

	const core::stringc& getName() const;
	const CGparameter& getParameter() const;
	CGenum getSpace() const;
	CGtype getType() const;

	virtual void update(const void* data, const SMaterial& material) const = 0;

protected:
	core::stringc Name;
	CGparameter Parameter;
	CGenum Space;
	CGtype Type;
};

class CCgUniform1f : public CCgUniform
{
public:
	CCgUniform1f(const CGparameter& parameter, bool global);

	virtual void update(const void* data, const SMaterial& material) const _IRR_OVERRIDE_;
};

class CCgUniform2f : public CCgUniform
{
public:
	CCgUniform2f(const CGparameter& parameter, bool global);

	virtual void update(const void* data, const SMaterial& material) const _IRR_OVERRIDE_;
};

class CCgUniform3f : public CCgUniform
{
public:
	CCgUniform3f(const CGparameter& parameter, bool global);

	virtual void update(const void* data, const SMaterial& material) const _IRR_OVERRIDE_;
};

class CCgUniform4f : public CCgUniform
{
public:
	CCgUniform4f(const CGparameter& parameter, bool global);

	virtual void update(const void* data, const SMaterial& material) const _IRR_OVERRIDE_;
};

class CCgUniform1i : public CCgUniform
{
public:
	CCgUniform1i(const CGparameter& parameter, bool global);

	virtual void update(const void* data, const SMaterial& material) const _IRR_OVERRIDE_;
};

class CCgUniform2i : public CCgUniform
{
public:
	CCgUniform2i(const CGparameter& parameter, bool global);

	virtual void update(const void* data, const SMaterial& material) const _IRR_OVERRIDE_;
};

class CCgUniform3i : public CCgUniform
{
public:
	CCgUniform3i(const CGparameter& parameter, bool global);

	virtual void update(const void* data, const SMaterial& material) const _IRR_OVERRIDE_;
};

class CCgUniform4i : public CCgUniform
{
public:
	CCgUniform4i(const CGparameter& parameter, bool global);

	virtual void update(const void* data, const SMaterial& material) const _IRR_OVERRIDE_;
};

class CCgUniform4x4f : public CCgUniform
{
public:
	CCgUniform4x4f(const CGparameter& parameter, bool global);

	virtual void update(const void* data, const SMaterial& material) const _IRR_OVERRIDE_;
};

class CCgUniformSampler2D : public CCgUniform
{
public:
	CCgUniformSampler2D(const CGparameter& parameter, bool global);

	virtual void update(const void* data, const SMaterial& material) const _IRR_OVERRIDE_;
};

class CCgMaterialRenderer : public IMaterialRenderer, public IMaterialRendererServices
{
public:
	CCgMaterialRenderer(IShaderConstantSetCallBack* callback = 0, s32 userData = 0);
	virtual ~CCgMaterialRenderer();

	virtual bool isTransparent() const = 0;

	virtual s32 getVertexShaderConstantID(const c8* name) _IRR_OVERRIDE_;
	virtual s32 getPixelShaderConstantID(const c8* name) _IRR_OVERRIDE_;
	virtual void setVertexShaderConstant(const f32* data, s32 startRegister, s32 constantAmount=1) _IRR_OVERRIDE_;
	virtual void setPixelShaderConstant(const f32* data, s32 startRegister, s32 constantAmount=1) _IRR_OVERRIDE_;
	virtual bool setVertexShaderConstant(s32 index, const f32* floats, int count) _IRR_OVERRIDE_;
	virtual bool setVertexShaderConstant(s32 index, const s32* ints, int count) _IRR_OVERRIDE_;
	virtual bool setPixelShaderConstant(s32 index, const f32* floats, int count) _IRR_OVERRIDE_;
	virtual bool setPixelShaderConstant(s32 index, const s32* ints, int count) _IRR_OVERRIDE_;

protected:
	void getUniformList();

	IShaderConstantSetCallBack* CallBack;
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
