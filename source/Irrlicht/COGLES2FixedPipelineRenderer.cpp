// Copyright (C) 2014 Patryk Nadrowski
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#include "COGLES2FixedPipelineRenderer.h"

#ifdef _IRR_COMPILE_WITH_OGLES2_

#include "IVideoDriver.h"
#include "SLight.h"

namespace irr
{
namespace video
{

// Base callback

COGLES2MaterialBaseCB::COGLES2MaterialBaseCB() :
	FirstUpdateBase(true), WVPMatrixID(-1), WVMatrixID(-1), NMatrixID(-1), GlobalAmbientID(-1), MaterialAmbientID(-1), MaterialDiffuseID(-1), MaterialEmissiveID(-1), MaterialSpecularID(-1), MaterialShininessID(-1), LightCountID(-1), LightTypeID(-1),
	LightPositionID(-1), LightDirectionID(-1), LightAttenuationID(-1), LightAmbientID(-1), LightDiffuseID(-1), LightSpecularID(-1), FogEnableID(-1), FogTypeID(-1), FogColorID(-1), FogStartID(-1),
	FogEndID(-1), FogDensityID(-1), ThicknessID(-1), LightEnable(false), MaterialAmbient(SColorf(0.f, 0.f, 0.f)), MaterialDiffuse(SColorf(0.f, 0.f, 0.f)), MaterialEmissive(SColorf(0.f, 0.f, 0.f)), MaterialSpecular(SColorf(0.f, 0.f, 0.f)),
	MaterialShininess(0.f), FogEnable(0), FogType(1), FogColor(SColorf(0.f, 0.f, 0.f, 1.f)), FogStart(0.f), FogEnd(0.f), FogDensity(0.f), Thickness(1.f)
{
	for (u32 i = 0; i < 8; ++i)
	{
		LightType[i] = 0;
		LightPosition[i] = core::vector3df(0.f, 0.f, 0.f);
		LightDirection[i] = core::vector3df(0.f, 0.f, 0.f);
		LightAttenuation[i] = core::vector3df(0.f, 0.f, 0.f);
		LightAmbient[i] = SColorf(0.f, 0.f, 0.f);
		LightDiffuse[i] = SColorf(0.f, 0.f, 0.f);
		LightSpecular[i] = SColorf(0.f, 0.f, 0.f);
	}
}

void COGLES2MaterialBaseCB::OnSetMaterial(const SMaterial& material)
{
	LightEnable = material.Lighting;
	MaterialAmbient = SColorf(material.AmbientColor);
	MaterialDiffuse = SColorf(material.DiffuseColor);
	MaterialEmissive = SColorf(material.EmissiveColor);
	MaterialSpecular = SColorf(material.SpecularColor);
	MaterialShininess = material.Shininess;

	FogEnable = material.FogEnable ? 1 : 0;

	Thickness = (material.Thickness > 0.f) ? material.Thickness : 1.f;
}

void COGLES2MaterialBaseCB::OnSetConstants(IMaterialRendererServices* services, s32 userData)
{
	IVideoDriver* driver = services->getVideoDriver();

	if (FirstUpdateBase)
	{
		WVPMatrixID = services->getVertexShaderConstantID("uWVPMatrix");
		WVMatrixID = services->getVertexShaderConstantID("uWVMatrix");
		NMatrixID = services->getVertexShaderConstantID("uNMatrix");
		GlobalAmbientID = services->getVertexShaderConstantID("uGlobalAmbient");
		MaterialAmbientID = services->getVertexShaderConstantID("uMaterialAmbient");
		MaterialDiffuseID = services->getVertexShaderConstantID("uMaterialDiffuse");
		MaterialEmissiveID = services->getVertexShaderConstantID("uMaterialEmissive");
		MaterialSpecularID = services->getVertexShaderConstantID("uMaterialSpecular");
		MaterialShininessID = services->getVertexShaderConstantID("uMaterialShininess");
		LightCountID = services->getVertexShaderConstantID("uLightCount");
		LightTypeID = services->getVertexShaderConstantID("uLightType");
		LightPositionID = services->getVertexShaderConstantID("uLightPosition");
		LightDirectionID = services->getVertexShaderConstantID("uLightDirection");
		LightAttenuationID = services->getVertexShaderConstantID("uLightAttenuation");
		LightAmbientID = services->getVertexShaderConstantID("uLightAmbient");
		LightDiffuseID = services->getVertexShaderConstantID("uLightDiffuse");
		LightSpecularID = services->getVertexShaderConstantID("uLightSpecular");
		FogEnableID = services->getVertexShaderConstantID("uFogEnable");
		FogTypeID = services->getVertexShaderConstantID("uFogType");
		FogColorID = services->getVertexShaderConstantID("uFogColor");
		FogStartID = services->getVertexShaderConstantID("uFogStart");
		FogEndID = services->getVertexShaderConstantID("uFogEnd");
		FogDensityID = services->getVertexShaderConstantID("uFogDensity");
		ThicknessID = services->getVertexShaderConstantID("uThickness");

		FirstUpdateBase = false;
	}

	const core::matrix4 W = driver->getTransform(ETS_WORLD);
	const core::matrix4 V = driver->getTransform(ETS_VIEW);
	const core::matrix4 P = driver->getTransform(ETS_PROJECTION);

	core::matrix4 Matrix = P * V * W;
	services->setPixelShaderConstant(WVPMatrixID, Matrix.pointer(), 16);

	Matrix = V * W;
	services->setPixelShaderConstant(WVMatrixID, Matrix.pointer(), 16);

	Matrix.makeInverse();
	services->setPixelShaderConstant(NMatrixID, Matrix.getTransposed().pointer(), 16);

	s32 LightCount = LightEnable ? driver->getDynamicLightCount() : 0;
	services->setPixelShaderConstant(LightCountID, &LightCount, 1);

	if (LightCount > 0)
	{
		video::SColorf globalAmbient(driver->getAmbientLight());
		services->setVertexShaderConstant(GlobalAmbientID, reinterpret_cast<f32*>(&globalAmbient), 4);

		// TODO: this are all vertex shader constants, why are they all set as pixel shader constants? (it currently works so I'm scared to change it...)
		services->setPixelShaderConstant(MaterialAmbientID, reinterpret_cast<f32*>(&MaterialAmbient), 4);
		services->setPixelShaderConstant(MaterialDiffuseID, reinterpret_cast<f32*>(&MaterialDiffuse), 4);
		services->setPixelShaderConstant(MaterialEmissiveID, reinterpret_cast<f32*>(&MaterialEmissive), 4);
		services->setPixelShaderConstant(MaterialSpecularID, reinterpret_cast<f32*>(&MaterialSpecular), 4);
		services->setPixelShaderConstant(MaterialShininessID, &MaterialShininess, 1);

		Matrix = V;

		for (s32 i = 0; i < LightCount; ++i)
		{
			SLight CurrentLight = driver->getDynamicLight(i);

			Matrix.transformVect(CurrentLight.Position);

			switch (CurrentLight.Type)
			{
			case ELT_DIRECTIONAL:
				LightType[i] = 2;
				break;
			case ELT_SPOT:
				LightType[i] = 1;
				break;
			default: // ELT_POINT
				LightType[i] = 0;
				break;
			}

			LightPosition[i] = CurrentLight.Position;
			LightDirection[i] = CurrentLight.Direction;
			LightAttenuation[i] = CurrentLight.Attenuation;
			LightAmbient[i] = CurrentLight.AmbientColor;
			LightDiffuse[i] = CurrentLight.DiffuseColor;
			LightSpecular[i] = CurrentLight.SpecularColor;
		}

		const int MAX_SHADER_LIGHTS = 8;	// must be the same as MAX_LIGHTS define in the shader
		services->setPixelShaderConstant(LightTypeID, LightType, MAX_SHADER_LIGHTS);
		services->setPixelShaderConstant(LightPositionID, reinterpret_cast<f32*>(LightPosition), 3*MAX_SHADER_LIGHTS);
		services->setPixelShaderConstant(LightDirectionID, reinterpret_cast<f32*>(LightDirection), 3*MAX_SHADER_LIGHTS);
		services->setPixelShaderConstant(LightAttenuationID, reinterpret_cast<f32*>(LightAttenuation), 3*MAX_SHADER_LIGHTS);
		services->setPixelShaderConstant(LightAmbientID, reinterpret_cast<f32*>(LightAmbient), 4*MAX_SHADER_LIGHTS);
		services->setPixelShaderConstant(LightDiffuseID, reinterpret_cast<f32*>(LightDiffuse), 4*MAX_SHADER_LIGHTS);
		services->setPixelShaderConstant(LightSpecularID, reinterpret_cast<f32*>(LightSpecular), 4*MAX_SHADER_LIGHTS);
	}

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

	services->setPixelShaderConstant(ThicknessID, &Thickness, 1);
}

// EMT_SOLID + EMT_TRANSPARENT_ADD_COLOR + EMT_TRANSPARENT_ALPHA_CHANNEL + EMT_TRANSPARENT_VERTEX_ALPHA

COGLES2MaterialSolidCB::COGLES2MaterialSolidCB() :
	FirstUpdate(true), TMatrix0ID(-1), AlphaRefID(-1), TextureUsage0ID(-1), TextureUnit0ID(-1), AlphaRef(0.5f), TextureUsage0(0), TextureUnit0(0)
{
}

void COGLES2MaterialSolidCB::OnSetMaterial(const SMaterial& material)
{
	COGLES2MaterialBaseCB::OnSetMaterial(material);

	AlphaRef = material.MaterialTypeParam;
	TextureUsage0 = (material.TextureLayer[0].Texture) ? 1 : 0;
}

void COGLES2MaterialSolidCB::OnSetConstants(IMaterialRendererServices* services, s32 userData)
{
	COGLES2MaterialBaseCB::OnSetConstants(services, userData);

	IVideoDriver* driver = services->getVideoDriver();

	if (FirstUpdate)
	{
		TMatrix0ID = services->getVertexShaderConstantID("uTMatrix0");
		AlphaRefID = services->getVertexShaderConstantID("uAlphaRef");
		TextureUsage0ID = services->getVertexShaderConstantID("uTextureUsage0");
		TextureUnit0ID = services->getVertexShaderConstantID("uTextureUnit0");

		FirstUpdate = false;
	}

	core::matrix4 Matrix = driver->getTransform(ETS_TEXTURE_0);
	services->setPixelShaderConstant(TMatrix0ID, Matrix.pointer(), 16);

	services->setPixelShaderConstant(AlphaRefID, &AlphaRef, 1);
	services->setPixelShaderConstant(TextureUsage0ID, &TextureUsage0, 1);
	services->setPixelShaderConstant(TextureUnit0ID, &TextureUnit0, 1);
}

// EMT_SOLID_2_LAYER + EMT_DETAIL_MAP

COGLES2MaterialSolid2CB::COGLES2MaterialSolid2CB() :
	FirstUpdate(true), TMatrix0ID(-1), TMatrix1ID(-1), TextureUsage0ID(-1), TextureUsage1ID(-1), TextureUnit0ID(-1), TextureUnit1ID(-1),
	TextureUsage0(0), TextureUsage1(0), TextureUnit0(0), TextureUnit1(1)
{
}

void COGLES2MaterialSolid2CB::OnSetMaterial(const SMaterial& material)
{
	COGLES2MaterialBaseCB::OnSetMaterial(material);

	TextureUsage0 = (material.TextureLayer[0].Texture) ? 1 : 0;
	TextureUsage1 = (material.TextureLayer[1].Texture) ? 1 : 0;
}

void COGLES2MaterialSolid2CB::OnSetConstants(IMaterialRendererServices* services, s32 userData)
{
	COGLES2MaterialBaseCB::OnSetConstants(services, userData);

	IVideoDriver* driver = services->getVideoDriver();

	if (FirstUpdate)
	{
		TMatrix0ID = services->getVertexShaderConstantID("uTMatrix0");
		TMatrix1ID = services->getVertexShaderConstantID("uTMatrix1");
		TextureUsage0ID = services->getVertexShaderConstantID("uTextureUsage0");
		TextureUsage1ID = services->getVertexShaderConstantID("uTextureUsage1");
		TextureUnit0ID = services->getVertexShaderConstantID("uTextureUnit0");
		TextureUnit1ID = services->getVertexShaderConstantID("uTextureUnit1");

		FirstUpdate = false;
	}

	core::matrix4 Matrix = driver->getTransform(ETS_TEXTURE_0);
	services->setPixelShaderConstant(TMatrix0ID, Matrix.pointer(), 16);

	Matrix = driver->getTransform(ETS_TEXTURE_1);
	services->setPixelShaderConstant(TMatrix1ID, Matrix.pointer(), 16);

	services->setPixelShaderConstant(TextureUsage0ID, &TextureUsage0, 1);
	services->setPixelShaderConstant(TextureUsage1ID, &TextureUsage1, 1);
	services->setPixelShaderConstant(TextureUnit0ID, &TextureUnit0, 1);
	services->setPixelShaderConstant(TextureUnit1ID, &TextureUnit1, 1);
}

// EMT_LIGHTMAP + EMT_LIGHTMAP_ADD + EMT_LIGHTMAP_M2 + EMT_LIGHTMAP_M4

COGLES2MaterialLightmapCB::COGLES2MaterialLightmapCB(float modulate) :
	FirstUpdate(true), TMatrix0ID(-1), TMatrix1ID(-1), ModulateID(-1), TextureUsage0ID(-1), TextureUsage1ID(-1), TextureUnit0ID(-1), TextureUnit1ID(-1),
	Modulate(modulate), TextureUsage0(0), TextureUsage1(0), TextureUnit0(0), TextureUnit1(1)
{
}

void COGLES2MaterialLightmapCB::OnSetMaterial(const SMaterial& material)
{
	COGLES2MaterialBaseCB::OnSetMaterial(material);

	TextureUsage0 = (material.TextureLayer[0].Texture) ? 1 : 0;
	TextureUsage1 = (material.TextureLayer[1].Texture) ? 1 : 0;
}

void COGLES2MaterialLightmapCB::OnSetConstants(IMaterialRendererServices* services, s32 userData)
{
	COGLES2MaterialBaseCB::OnSetConstants(services, userData);

	IVideoDriver* driver = services->getVideoDriver();

	if (FirstUpdate)
	{
		TMatrix0ID = services->getVertexShaderConstantID("uTMatrix0");
		TMatrix1ID = services->getVertexShaderConstantID("uTMatrix1");
		ModulateID = services->getVertexShaderConstantID("uModulate");
		TextureUsage0ID = services->getVertexShaderConstantID("uTextureUsage0");
		TextureUsage1ID = services->getVertexShaderConstantID("uTextureUsage1");
		TextureUnit0ID = services->getVertexShaderConstantID("uTextureUnit0");
		TextureUnit1ID = services->getVertexShaderConstantID("uTextureUnit1");

		FirstUpdate = false;
	}

	core::matrix4 Matrix = driver->getTransform(ETS_TEXTURE_0);
	services->setPixelShaderConstant(TMatrix0ID, Matrix.pointer(), 16);

	Matrix = driver->getTransform(ETS_TEXTURE_1);
	services->setPixelShaderConstant(TMatrix1ID, Matrix.pointer(), 16);

	services->setPixelShaderConstant(ModulateID, &Modulate, 1);
	services->setPixelShaderConstant(TextureUsage0ID, &TextureUsage0, 1);
	services->setPixelShaderConstant(TextureUsage1ID, &TextureUsage1, 1);
	services->setPixelShaderConstant(TextureUnit0ID, &TextureUnit0, 1);
	services->setPixelShaderConstant(TextureUnit1ID, &TextureUnit1, 1);
}

// EMT_SPHERE_MAP + EMT_REFLECTION_2_LAYER + EMT_TRANSPARENT_REFLECTION_2_LAYER

COGLES2MaterialReflectionCB::COGLES2MaterialReflectionCB() :
	FirstUpdate(true), TMatrix0ID(-1), TextureUsage0ID(-1), TextureUsage1ID(-1), TextureUnit0ID(-1), TextureUnit1ID(-1),
	TextureUsage0(0), TextureUsage1(0), TextureUnit0(0), TextureUnit1(1)
{
}

void COGLES2MaterialReflectionCB::OnSetMaterial(const SMaterial& material)
{
	COGLES2MaterialBaseCB::OnSetMaterial(material);

	TextureUsage0 = (material.TextureLayer[0].Texture) ? 1 : 0;
	TextureUsage1 = (material.TextureLayer[1].Texture) ? 1 : 0;
}

void COGLES2MaterialReflectionCB::OnSetConstants(IMaterialRendererServices* services, s32 userData)
{
	COGLES2MaterialBaseCB::OnSetConstants(services, userData);

	IVideoDriver* driver = services->getVideoDriver();

	if (FirstUpdate)
	{
		TMatrix0ID = services->getVertexShaderConstantID("uTMatrix0");
		TextureUsage0ID = services->getVertexShaderConstantID("uTextureUsage0");
		TextureUsage1ID = services->getVertexShaderConstantID("uTextureUsage1");
		TextureUnit0ID = services->getVertexShaderConstantID("uTextureUnit0");
		TextureUnit1ID = services->getVertexShaderConstantID("uTextureUnit1");

		FirstUpdate = false;
	}

	core::matrix4 Matrix = driver->getTransform(ETS_TEXTURE_0);
	services->setPixelShaderConstant(TMatrix0ID, Matrix.pointer(), 16);

	services->setPixelShaderConstant(TextureUsage0ID, &TextureUsage0, 1);
	services->setPixelShaderConstant(TextureUsage1ID, &TextureUsage1, 1);
	services->setPixelShaderConstant(TextureUnit0ID, &TextureUnit0, 1);
	services->setPixelShaderConstant(TextureUnit1ID, &TextureUnit1, 1);
}

// EMT_ONETEXTURE_BLEND

COGLES2MaterialOneTextureBlendCB::COGLES2MaterialOneTextureBlendCB() :
	FirstUpdate(true), TMatrix0ID(-1), BlendTypeID(-1), TextureUsage0ID(-1), TextureUnit0ID(-1), BlendType(0), TextureUsage0(0), TextureUnit0(0)
{
}

void COGLES2MaterialOneTextureBlendCB::OnSetMaterial(const SMaterial& material)
{
	COGLES2MaterialBaseCB::OnSetMaterial(material);

	BlendType = 0;

	E_BLEND_FACTOR srcRGBFact,dstRGBFact,srcAlphaFact,dstAlphaFact;
	E_MODULATE_FUNC modulate;
	u32 alphaSource;
	unpack_textureBlendFuncSeparate(srcRGBFact, dstRGBFact, srcAlphaFact, dstAlphaFact, modulate, alphaSource, material.MaterialTypeParam);

	if (textureBlendFunc_hasAlpha(srcRGBFact) || textureBlendFunc_hasAlpha(dstRGBFact) || textureBlendFunc_hasAlpha(srcAlphaFact) || textureBlendFunc_hasAlpha(dstAlphaFact))
	{
		if (alphaSource == EAS_VERTEX_COLOR)
		{
			BlendType = 1;
		}
		else if (alphaSource == EAS_TEXTURE)
		{
			BlendType = 2;
		}
	}

	TextureUsage0 = (material.TextureLayer[0].Texture) ? 1 : 0;
}

void COGLES2MaterialOneTextureBlendCB::OnSetConstants(IMaterialRendererServices* services, s32 userData)
{
	COGLES2MaterialBaseCB::OnSetConstants(services, userData);

	IVideoDriver* driver = services->getVideoDriver();

	if (FirstUpdate)
	{
		TMatrix0ID = services->getVertexShaderConstantID("uTMatrix0");
		BlendTypeID = services->getVertexShaderConstantID("uBlendType");
		TextureUsage0ID = services->getVertexShaderConstantID("uTextureUsage0");
		TextureUnit0ID = services->getVertexShaderConstantID("uTextureUnit0");

		FirstUpdate = false;
	}

	core::matrix4 Matrix = driver->getTransform(ETS_TEXTURE_0);
	services->setPixelShaderConstant(TMatrix0ID, Matrix.pointer(), 16);

	services->setPixelShaderConstant(BlendTypeID, &BlendType, 1);
	services->setPixelShaderConstant(TextureUsage0ID, &TextureUsage0, 1);
	services->setPixelShaderConstant(TextureUnit0ID, &TextureUnit0, 1);
}

}
}

#endif

