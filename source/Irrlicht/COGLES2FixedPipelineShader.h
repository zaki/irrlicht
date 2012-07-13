// Copyright (C) 2009-2010 Amundis
// Heavily based on the OpenGL driver implemented by Nikolaus Gebhardt
// and OpenGL ES driver implemented by Christian Stehno
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#ifndef __C_OGLES2_FIXED_PIPELINE_SHADER_H_INCLUDED__
#define __C_OGLES2_FIXED_PIPELINE_SHADER_H_INCLUDED__

#include "IrrCompileConfig.h"

#ifdef _IRR_COMPILE_WITH_OGLES2_

#include "COGLES2Driver.h"
#include "COGLES2SLMaterialRenderer.h"
#include "EVertexAttributes.h"

namespace irr
{
namespace video
{
#define MAX_LIGHTS 8

	enum AlphaFunc
	{
		ALPHA_GREATER = 0
	};

	class COGLES2FixedPipelineShader : public COGLES2SLMaterialRenderer
	{

	public:

		COGLES2FixedPipelineShader(video::COGLES2Driver* driver, io::IFileSystem* fs);

		void updateMatrices();
		void bindTexture();
		virtual bool OnRender(IMaterialRendererServices* service, E_VERTEX_TYPE vtxtype);

	public:
		/*void enableNormalize() {Normalize = true;}
		void disableNormalize() {Normalize = false;}*/

		void enableAlphaTest()
		{
			AlphaTest = true;
		}
		void disableAlphaTest()
		{
			AlphaTest = false;
		}
		void setAlphaValue(float value)
		{
			AlphaValue = value;
		}

		/*void enableLighting()	{Lighting = true;}
		void disableLighting() {Lighting = false;}
		bool isLightingEnabled() {return (Lighting == 1);}*/
		void setAmbientLight(const SColorf& color);

		void enableFog()
		{
			Fog = true;
		}
		void disableFog()
		{
			Fog = false;
		}
		bool isFogEnabled()
		{
			return (Fog == 1);
		}
		void setFog(E_FOG_TYPE type, const SColor& color, f32 start, f32 end, f32 density);

		void setMaterial(const SMaterial& material);

		void setRenderMode(E_MATERIAL_TYPE mode)
		{
			RenderMode = mode;
		}

		void reload();

	private:

		void initData();

	private:

		struct vec3
		{
			float data[3];
		};
		struct vec4
		{
			float data[4];
		};
		struct mat4
		{
			float data[16];
		};

		float Normalize;


		int TextureUnits[MATERIAL_MAX_TEXTURES];
		int UseTexture[MATERIAL_MAX_TEXTURES];
		mat4 TextureMatrix[MATERIAL_MAX_TEXTURES];
		int UseTexMatrix[MATERIAL_MAX_TEXTURES];


		float AlphaTest;
		float AlphaValue;
		AlphaFunc AlphaFunction;

		int Lighting;
		int UseLight[MAX_LIGHTS];
		vec4 LightPosition[MAX_LIGHTS];
		SColorf LightAmbient[MAX_LIGHTS];
		SColorf LightDiffuse[MAX_LIGHTS];
		SColorf LightSpecular[MAX_LIGHTS];
		core::vector3df LightDirection[MAX_LIGHTS];
		core::vector3df LightAttenuation[MAX_LIGHTS];
		float LightExponent[MAX_LIGHTS];
		float LightCutoff[MAX_LIGHTS];
		SColorf AmbientColor;

		int Fog;
		int FogType;
		float FogColor[4];
		float FogStart;
		float FogEnd;
		float FogDensity;

		int Clip;
		core::plane3df ClipPlane;

		u32 ColorMaterial;
		SColorf MaterialAmbient;
		SColorf MaterialEmission;
		SColorf MaterialDiffuse;
		SColorf MaterialSpecular;
		float MaterialShininess;

		E_MATERIAL_TYPE RenderMode;
	private :
		enum SHADER_UNIFORM
		{
            VRENDER_MODE = 0,
			FRENDER_MODE,
			MVP_MATRIX,
			WORLD_MATRIX,
			NORMALIZE,
			EYE_POSITION,
			USE_LIGHT,
			LIGHT_POSITION,
			LIGHT_AMBIENT,
			LIGHT_DIFFUSE,
			LIGHT_SPECULAR,
			LIGHT_DIRECTION,
			LIGHT_ATTENUATION,
			LIGHT_EXPONENT,
			LIGHT_CUTOFF,
			AMBIENT_COLOR,
			LIGHTING,
			MATERIAL_AMBIENT,
			MATERIAL_EMISSION,
			MATERIAL_DIFFUSE,
			MATERIAL_SPECULAR,
			MATERIAL_SHININESS,
			COLOR_MATERIAL,
			USE_TEXTURE,
			TEXTURE_MATRIX,
			USE_TEXTURE_MATRIX,
			CLIP,
			CLIP_PLANE,
			ALPHA_TEST,
			ALPHA_VALUE,
			FOG,
			FOG_TYPE,
			FOG_COLOR,
			FOG_START,
			FOG_END,
			FOG_DENSITY,
			TEXTURE_UNIT0,
			TEXTURE_UNIT1,
			UNIFORM_COUNT
		};
		static const char* const sBuiltInShaderUniformNames[];
	};
}
}

#endif //_IRR_COMPILE_WITH_OGLES2_

#endif //__C_OGLES2_FIXED_PIPELINE_SHADER_H_INCLUDED__
