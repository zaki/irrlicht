// Copyright (C) 2009-2010 Amundis
// Heavily based on the OpenGL driver implemented by Nikolaus Gebhardt
// and OpenGL ES driver implemented by Christian Stehno
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h
#define MAX_TEXTURE_UNITS 4
#define FOG_EXP 1
#define FOG_LINEAR 2

#define TwoD             24
#define Solid            0
#define Solid2Layer      1
#define LightMap         2
#define DetailMap		 9
#define SphereMap		 10
#define Reflection2Layer 11
#define TransparentAlphaChannel 13
#define TransparentVertexAlpha 15

precision mediump float;

vec4 red   = vec4(1.0, 0.0, 0.0, 1.0);
vec4 green = vec4(0.0, 1.0, 0.0, 1.0);
vec4 blue  = vec4(0.0, 0.0, 1.0, 1.0);


/* Varyings */

varying vec4 varTexCoord[MAX_TEXTURE_UNITS];
varying vec4 varVertexColor;
varying float eyeDist;
varying float varClipDist;

/* Uniforms */

uniform int uRenderMode;

uniform bool  uAlphaTest;
uniform float uAlphaValue;

/* Fog Uniforms */

uniform bool  uFog;
uniform int   uFogType;
uniform vec4  uFogColor;
uniform float uFogStart;
uniform float uFogEnd;
uniform float uFogDensity;

/* Texture Uniforms */

uniform sampler2D uTextureUnit0;
uniform sampler2D uTextureUnit1;
uniform bool      uUseTexture [MAX_TEXTURE_UNITS];

vec4 render2D(void)
{
	vec4 color = varVertexColor;
	vec4 texel = texture2D(uTextureUnit0, varTexCoord[0].xy);
	if(uUseTexture[0])
	{
		color *= texel;
	}
	return color;
}

vec4 renderSolid(void)
{
	vec4 color = varVertexColor;
	vec4 texel = texture2D(uTextureUnit0, varTexCoord[0].xy);
	if(uUseTexture[0])
		color *= texel;
	return color;
}

vec4 renderTransparentVertexAlpha(void)
{
	vec4 color = renderSolid();
	color.a = varVertexColor.a;
	return color;
}

vec4 renderTransparentAlphaChannel(void)
{
	vec4 texel = texture2D(uTextureUnit0, varTexCoord[0].xy);
	vec4 color = texel * varVertexColor;
	color.a = texel.a;
	return color;
}

vec4 render2LayerSolid(void)
{
	float blendFactor = varVertexColor.a;
	vec4 texel0 = texture2D(uTextureUnit0, varTexCoord[0].xy);
	vec4 texel1 = texture2D(uTextureUnit1, varTexCoord[1].xy);
	vec4 color = texel0 * blendFactor + texel1 * ( 1.0 - blendFactor );
	return color;
}

vec4 renderLightMap(void)
{
	vec4 texel0  = texture2D(uTextureUnit0, varTexCoord[0].xy);
	vec4 texel1  = texture2D(uTextureUnit1, varTexCoord[1].xy);
	vec4 color   = texel0 * texel1 * 4.0;
	color.a = texel0.a * texel0.a;
	return color;
}

vec4 renderDetailMap(void)
{
	vec4 texel0  = texture2D(uTextureUnit0, varTexCoord[0].xy);
	vec4 texel1  = texture2D(uTextureUnit1, varTexCoord[1].xy);
	vec4 color   = texel0;
	color += texel1 - 0.5;
	return color;
}

vec4 renderReflection2Layer(void)
{
	vec4 color = varVertexColor;
	vec4 texel0 = texture2D(uTextureUnit0, varTexCoord[0].xy);
	vec4 texel1 = texture2D(uTextureUnit1, varTexCoord[1].xy);
	color *=  texel0 * texel1;
	return color;
}

float ComputeFog()
{
	float factor = 1.0;
	if(uFogType == FOG_LINEAR)
	{
		factor = (uFogEnd - eyeDist) / (uFogEnd - uFogStart);
	}
	else if(uFogType == FOG_EXP)
	{
		 factor = exp(-(eyeDist * uFogDensity));
	}
	else //uFogType == FOG_EXP2
	{
		factor = eyeDist * uFogDensity;
		factor = exp(-(factor * factor)) ;
	}
	factor = clamp(factor, 0.0, 1.0);
	return factor;
}

void main (void)
{
	if(varClipDist < 0.0)
		discard;
	
	if( uRenderMode == TwoD)
		gl_FragColor = render2D();
	else if( uRenderMode == Solid)
		gl_FragColor = renderSolid();
	else if(uRenderMode == LightMap)
		gl_FragColor = renderLightMap();
	else if(uRenderMode == DetailMap)
		gl_FragColor = renderDetailMap();
	else if(uRenderMode == SphereMap)
		gl_FragColor = renderSolid();
	else if(uRenderMode == Reflection2Layer)
		gl_FragColor = renderReflection2Layer();
	else if(uRenderMode == TransparentVertexAlpha)
		gl_FragColor = renderTransparentVertexAlpha();
	else if(uRenderMode == TransparentAlphaChannel)
		gl_FragColor = renderTransparentAlphaChannel();
	else
		gl_FragColor = vec4(0.0, 1.0, 1.0, 1.0);
		
	//gl_FragColor = varVertexColor;

	if(uFog)
	{
		float fogFactor = ComputeFog();
		gl_FragColor = gl_FragColor * fogFactor + uFogColor * (1.0 - fogFactor);
	}
			
	if(uAlphaTest && uAlphaValue > gl_FragColor.a)
		discard;
	
}
