// Copyright (C) 2014 Patryk Nadrowski
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_OGLES2_NORMAL_MAP_RENDERER_H_INCLUDED__
#define __C_OGLES2_NORMAL_MAP_RENDERER_H_INCLUDED__

#include "IrrCompileConfig.h"

#ifdef _IRR_COMPILE_WITH_OGLES2_

#include "IMaterialRenderer.h"
#include "IShaderConstantSetCallBack.h"

#include "COGLES2Common.h"

namespace irr
{
namespace video
{

class COGLES2MaterialNormalMapCB : public IShaderConstantSetCallBack
{
public:
	COGLES2MaterialNormalMapCB();

	virtual void OnSetMaterial(const SMaterial& material);
	virtual void OnSetConstants(IMaterialRendererServices* services, s32 userData);

protected:
	bool FirstUpdate;
	s32 WVPMatrixID;
	s32 WVMatrixID;
	s32 LightPositionID;
	s32 LightColorID;
	s32 TextureUnit0ID;
	s32 TextureUnit1ID;
	s32 FogEnableID;
	s32 FogTypeID;
	s32 FogColorID;
	s32 FogStartID;
	s32 FogEndID;
	s32 FogDensityID;

	core::vector3df LightPosition[2];
	SColorf LightColor[2];
	s32 TextureUnit0;
	s32 TextureUnit1;
	s32 FogEnable;
	s32 FogType;
	SColorf FogColor;
	f32 FogStart;
	f32 FogEnd;
	f32 FogDensity;
};

}
}

#endif
#endif

