// Copyright (C) 2014 Patryk Nadrowski
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#ifndef __C_OGLES2_FIXED_PIPELINE_SHADER_H_INCLUDED__
#define __C_OGLES2_FIXED_PIPELINE_SHADER_H_INCLUDED__

#include "IrrCompileConfig.h"

#ifdef _IRR_COMPILE_WITH_OGLES2_

#include "IShaderConstantSetCallBack.h"
#include "IMaterialRendererServices.h"

namespace irr
{
namespace video
{

class COGLES2MaterialBaseCB : public IShaderConstantSetCallBack
{
public:
	COGLES2MaterialBaseCB();

	virtual void OnSetMaterial(const SMaterial& material);
	virtual void OnSetConstants(IMaterialRendererServices* services, s32 userData);

protected:
	bool FirstUpdateBase;

	s32 WVPMatrixID;
	s32 WVMatrixID;
	s32 NMatrixID;

	s32 GlobalAmbientID;
	s32 MaterialAmbientID;
	s32 MaterialDiffuseID;
	s32 MaterialEmissiveID;
	s32 MaterialSpecularID;
	s32 MaterialShininessID;

	s32 LightCountID;
	s32 LightTypeID;
	s32 LightPositionID;
	s32 LightDirectionID;
	s32 LightAttenuationID;
	s32 LightAmbientID;
	s32 LightDiffuseID;
	s32 LightSpecularID;

	s32 FogEnableID;
	s32 FogTypeID;
	s32 FogColorID;
	s32 FogStartID;
	s32 FogEndID;
	s32 FogDensityID;

	s32 ThicknessID;

	bool LightEnable;
	SColorf GlobalAmbient;
	SColorf MaterialAmbient;
	SColorf MaterialDiffuse;
	SColorf MaterialEmissive;
	SColorf MaterialSpecular;
	f32 MaterialShininess;

	s32 LightType[8];
	core::vector3df LightPosition[8];
	core::vector3df LightDirection[8];
	core::vector3df LightAttenuation[8];
	SColorf LightAmbient[8];
	SColorf LightDiffuse[8];
	SColorf LightSpecular[8];

	s32 FogEnable;
	s32 FogType;
	SColorf FogColor;
	f32 FogStart;
	f32 FogEnd;
	f32 FogDensity;

	f32 Thickness;
};

class COGLES2MaterialSolidCB : public COGLES2MaterialBaseCB
{
public:
	COGLES2MaterialSolidCB();

	virtual void OnSetMaterial(const SMaterial& material);
	virtual void OnSetConstants(IMaterialRendererServices* services, s32 userData);

protected:
	bool FirstUpdate;

	s32 TMatrix0ID;
	s32 AlphaRefID;
	s32 TextureUsage0ID;
	s32 TextureUnit0ID;

	f32 AlphaRef;
	s32 TextureUsage0;
	s32 TextureUnit0;
};

class COGLES2MaterialSolid2CB : public COGLES2MaterialBaseCB
{
public:
	COGLES2MaterialSolid2CB();

	virtual void OnSetMaterial(const SMaterial& material);
	virtual void OnSetConstants(IMaterialRendererServices* services, s32 userData);

protected:
	bool FirstUpdate;

	s32 TMatrix0ID;
	s32 TMatrix1ID;
	s32 TextureUsage0ID;
	s32 TextureUsage1ID;
	s32 TextureUnit0ID;
	s32 TextureUnit1ID;

	s32 TextureUsage0;
	s32 TextureUsage1;
	s32 TextureUnit0;
	s32 TextureUnit1;
};

class COGLES2MaterialLightmapCB : public COGLES2MaterialBaseCB
{
public:
	COGLES2MaterialLightmapCB(float modulate);

	virtual void OnSetMaterial(const SMaterial& material);
	virtual void OnSetConstants(IMaterialRendererServices* services, s32 userData);

protected:
	bool FirstUpdate;

	s32 TMatrix0ID;
	s32 TMatrix1ID;
	s32 ModulateID;
	s32 TextureUsage0ID;
	s32 TextureUsage1ID;
	s32 TextureUnit0ID;
	s32 TextureUnit1ID;

	f32 Modulate;
	s32 TextureUsage0;
	s32 TextureUsage1;
	s32 TextureUnit0;
	s32 TextureUnit1;
};

class COGLES2MaterialReflectionCB : public COGLES2MaterialBaseCB
{
public:
	COGLES2MaterialReflectionCB();

	virtual void OnSetMaterial(const SMaterial& material);
	virtual void OnSetConstants(IMaterialRendererServices* services, s32 userData);

protected:
	bool FirstUpdate;

	s32 TMatrix0ID;
	s32 TextureUsage0ID;
	s32 TextureUsage1ID;
	s32 TextureUnit0ID;
	s32 TextureUnit1ID;

	s32 TextureUsage0;
	s32 TextureUsage1;
	s32 TextureUnit0;
	s32 TextureUnit1;
};

class COGLES2MaterialOneTextureBlendCB : public COGLES2MaterialBaseCB
{
public:
	COGLES2MaterialOneTextureBlendCB();

	virtual void OnSetMaterial(const SMaterial& material);
	virtual void OnSetConstants(IMaterialRendererServices* services, s32 userData);

protected:
	bool FirstUpdate;

	s32 TMatrix0ID;
	s32 BlendTypeID;
	s32 TextureUsage0ID;
	s32 TextureUnit0ID;

	s32 BlendType;
	s32 TextureUsage0;
	s32 TextureUnit0;
};

}
}

#endif
#endif

