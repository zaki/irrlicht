// Copyright (C) 2014 Patryk Nadrowski
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "COGLES2NormalMapRenderer.h"

#ifdef _IRR_COMPILE_WITH_OGLES2_

#include "IMaterialRendererServices.h"
#include "IGPUProgrammingServices.h"
#include "IShaderConstantSetCallBack.h"
#include "IVideoDriver.h"
#include "SLight.h"
#include "os.h"

namespace irr
{
namespace video
{

// EMT_NORMAL_MAP_SOLID + EMT_NORMAL_MAP_TRANSPARENT_ADD_COLOR + EMT_NORMAL_MAP_TRANSPARENT_VERTEX_ALPHA

COGLES2MaterialNormalMapCB::COGLES2MaterialNormalMapCB() :
	FirstUpdate(true), WVPMatrixID(-1), WVMatrixID(-1), LightPositionID(-1), LightColorID(-1), TextureUnit0ID(-1), TextureUnit1ID(-1),
	FogEnableID(-1), FogTypeID(-1), FogColorID(-1), FogStartID(-1), FogEndID(-1), FogDensityID(-1), TextureUnit0(0), TextureUnit1(1),
	FogEnable(0), FogType(1), FogColor(SColorf(0.f, 0.f, 0.f, 1.f)), FogStart(0.f), FogEnd(0.f), FogDensity(0.f)
{
	for (u32 i = 0; i < 2; ++i)
	{
		LightPosition[i] = core::vector3df(0.f, 0.f, 0.f);
		LightColor[i] = SColorf(0.f, 0.f, 0.f, 1.f);
	}
}

void COGLES2MaterialNormalMapCB::OnSetMaterial(const SMaterial& material)
{
	if (material.FogEnable)
		FogEnable = 1;
	else
		FogEnable = 0;
}

void COGLES2MaterialNormalMapCB::OnSetConstants(IMaterialRendererServices* services, s32 userData)
{
	IVideoDriver* driver = services->getVideoDriver();

	if (FirstUpdate)
	{
		WVPMatrixID = services->getVertexShaderConstantID("uWVPMatrix");
		WVMatrixID = services->getVertexShaderConstantID("uWVMatrix");
		LightPositionID = services->getVertexShaderConstantID("uLightPosition");
		LightColorID = services->getVertexShaderConstantID("uLightColor");
		TextureUnit0ID = services->getVertexShaderConstantID("uTextureUnit0");
		TextureUnit1ID = services->getVertexShaderConstantID("uTextureUnit1");
		FogEnableID = services->getVertexShaderConstantID("uFogEnable");
		FogTypeID = services->getVertexShaderConstantID("uFogType");
		FogColorID = services->getVertexShaderConstantID("uFogColor");
		FogStartID = services->getVertexShaderConstantID("uFogStart");
		FogEndID = services->getVertexShaderConstantID("uFogEnd");
		FogDensityID = services->getVertexShaderConstantID("uFogDensity");

		FirstUpdate = false;
	}

	const core::matrix4 W = driver->getTransform(ETS_WORLD);
	const core::matrix4 V = driver->getTransform(ETS_VIEW);
	const core::matrix4 P = driver->getTransform(ETS_PROJECTION);

	core::matrix4 Matrix = P * V * W;
	services->setPixelShaderConstant(WVPMatrixID, Matrix.pointer(), 16);

	Matrix = V * W;
	services->setPixelShaderConstant(WVMatrixID, Matrix.pointer(), 16);

	Matrix = W;
	Matrix.makeInverse();

	const u32 LightCount = driver->getDynamicLightCount();

	for (u32 i = 0; i < 2; ++i)
	{
		SLight CurrentLight;

		if (i < LightCount)
			CurrentLight = driver->getDynamicLight(i);
		else
		{
			CurrentLight.DiffuseColor.set(0.f, 0.f, 0.f);
			CurrentLight.Radius = 1.f;
		}

		CurrentLight.DiffuseColor.a = 1.f / (CurrentLight.Radius*CurrentLight.Radius);

		Matrix.transformVect(CurrentLight.Position);

		LightPosition[i] = CurrentLight.Position;
		LightColor[i] = CurrentLight.DiffuseColor;
	}

	services->setPixelShaderConstant(LightPositionID, reinterpret_cast<f32*>(LightPosition), 6);
	services->setPixelShaderConstant(LightColorID, reinterpret_cast<f32*>(LightColor), 8);

	services->setPixelShaderConstant(TextureUnit0ID, &TextureUnit0, 1);
	services->setPixelShaderConstant(TextureUnit1ID, &TextureUnit1, 1);

	services->setPixelShaderConstant(FogEnableID, &FogEnable, 1);

	if (FogEnable)
	{
		SColor TempColor(0);
		E_FOG_TYPE TempType = EFT_FOG_LINEAR;
		bool TempPerFragment = false;
		bool TempRange = false;

		driver->getFog(TempColor, TempType, FogStart, FogEnd, FogDensity, TempPerFragment, TempRange);

		FogType = (s32)TempType;
		FogColor = SColorf(TempColor);

		services->setPixelShaderConstant(FogTypeID, &FogType, 1);
		services->setPixelShaderConstant(FogColorID, reinterpret_cast<f32*>(&FogColor), 4);
		services->setPixelShaderConstant(FogStartID, &FogStart, 1);
		services->setPixelShaderConstant(FogEndID, &FogEnd, 1);
		services->setPixelShaderConstant(FogDensityID, &FogDensity, 1);
	}
}

}
}

#endif

