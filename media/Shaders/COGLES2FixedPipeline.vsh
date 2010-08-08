// Copyright (C) 2009-2010 Amundis
// Heavily based on the OpenGL driver implemented by Nikolaus Gebhardt
// and OpenGL ES driver implemented by Christian Stehno
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h
#define MAX_TEXTURE_UNITS 4
#define MAX_LIGHTS 8

#define SphereMap        10
#define Reflection2Layer 11

const vec4 red   = vec4(1.0, 0.0, 0.0, 1.0);
const vec4 green = vec4(0.0, 1.0, 0.0, 1.0);
const vec4 blue  = vec4(0.0, 0.0, 1.0, 1.0);
const vec4 white = vec4(1.0);
const vec4 black = vec4(0.0);

/* Vertex Attributes */
 
attribute vec4 inVertexPosition;
attribute vec4 inVertexColor;
attribute vec4 inTexCoord0;
attribute vec4 inTexCoord1;
attribute vec3 inVertexNormal;

uniform int uRenderMode;

/* Matrix Uniforms */

uniform mat4 uMvpMatrix;
uniform mat4 uWorldMatrix;

uniform bool uNormalize;
uniform vec3 uEyePos;

/* Light Uniforms */

uniform bool  uUseLight         [MAX_LIGHTS];
uniform vec4  uLightPosition    [MAX_LIGHTS];
uniform vec4  uLightAmbient     [MAX_LIGHTS];
uniform vec4  uLightDiffuse     [MAX_LIGHTS];
#ifdef USE_SPECULAR
uniform vec4  uLightSpecular    [MAX_LIGHTS];
#endif
#ifdef USE_LIGHT_CUTOFF
uniform vec3  uLightDirection   [MAX_LIGHTS];
uniform float uLightCutoff      [MAX_LIGHTS];
uniform float uLightExponent    [MAX_LIGHTS];
#endif
uniform vec3  uLightAttenuation [MAX_LIGHTS];
uniform vec4  uAmbientColor;
uniform bool  uLighting;

/* Material Uniforms */
uniform vec4  uMaterialAmbient;
uniform vec4  uMaterialEmission;
uniform vec4  uMaterialDiffuse;
uniform vec4  uMaterialSpecular;
uniform float uMaterialShininess;
uniform int   uColorMaterial;

#define ECM_NONE     0
#define ECM_DIFFUSE  1
#define ECM_AMBIENT  2
#define ECM_EMISSIVE 3
#define ECM_SPECULAR 4
#define ECM_DIFFUSE_AND_AMBIENT 5

/* Texture Uniforms */
uniform bool uUseTexture    [MAX_TEXTURE_UNITS];
uniform mat4 uTextureMatrix [MAX_TEXTURE_UNITS];
uniform bool uUseTexMatrix  [MAX_TEXTURE_UNITS];

/* Clip Plane Uniforms */
uniform bool uClip;
uniform vec4 uClipPlane;
varying float varClipDist;

/* Varyings */

varying vec4 varTexCoord[MAX_TEXTURE_UNITS];
varying vec4 varVertexColor;
varying float eyeDist;

/* shader variables */

vec3 gNormal;
vec3 gWorldPos;
vec4 gColor;

struct material {
   vec4    Ambient;
   vec4    Diffuse;
   vec4    Specular;
   vec4    Emission;
   float   Shininess;
} ;

material gMaterial;

vec4 lightEquation(int lidx)
{		
	vec4 color = vec4(0.0);
	
	float att = 1.0;
	vec3 lightDir;
	
	
	if( uLightPosition[lidx].w == 0.0) // Directional light
	{
		lightDir = -uLightPosition[lidx].xyz;
	}
	else
	{
		lightDir = uLightPosition[lidx].xyz - inVertexPosition.xyz;
		att = 1.0 / (uLightAttenuation[lidx].y * length(lightDir));
		lightDir = normalize(lightDir);
		
#ifdef USE_LIGHT_CUTOFF
		if(uLightCutoff[lidx] < 180.0)
		{
			// compute spot factor
			float spotEffect = dot(-lightDir, uLightDirection[lidx]);
			if( spotEffect >= cos( radians( uLightCutoff[lidx])))
				spotEffect = pow( spotEffect, uLightExponent[lidx]);
			else
				spotEffect = 0.0;
			att *= spotEffect;
		}
#endif
	}
	
	if(att >= 0.0 )
	{
		color += uLightAmbient[lidx] * gMaterial.Ambient;
		
		//Compute cos(Light, Normal)
		float NdotL = max(dot(normalize(gNormal), lightDir), 0.0);
		color += NdotL * uLightDiffuse[lidx] * gMaterial.Diffuse;
		
		//Compute cos(hvec, Normal)
#ifdef USE_SPECULAR
		vec3 hvec = normalize(lightDir + vec3(0.0, 0.0, 1.0));
		float NdotH = dot(gNormal, hvec);
		if(NdotH > 0.0)
		{
			color += pow(NdotH, uMaterialShininess) * uLightSpecular[lidx] * gMaterial.Specular;
		}
#endif
		color *= att;
	}
	return color;
}

vec4 computeLighting(void)
{
	vec4 color = gMaterial.Emission + gMaterial.Ambient * uAmbientColor;

	for ( int i = 0 ; i < MAX_LIGHTS; ++i)
	{
		if ( uUseLight[i] )
		{
			color += lightEquation(i) ;
		}
	}
	color.a = gMaterial.Diffuse.a;
	return color;
}

void ReplaceColorMaterial(void)
{
	gMaterial.Ambient = uMaterialAmbient;
	gMaterial.Diffuse = uMaterialDiffuse;
	gMaterial.Emission = uMaterialEmission;
	gMaterial.Specular = uMaterialSpecular;
	gMaterial.Shininess = uMaterialShininess;

	if(uColorMaterial == ECM_DIFFUSE)
		gMaterial.Diffuse = gColor;
	else if(uColorMaterial == ECM_AMBIENT)
		gMaterial.Ambient = gColor;
	else if(uColorMaterial == ECM_DIFFUSE_AND_AMBIENT)
	{
		gMaterial.Diffuse = gColor;
		gMaterial.Ambient = gColor;
	}	
	else if(uColorMaterial == ECM_EMISSIVE)
		gMaterial.Emission = gColor;
	else if(uColorMaterial == ECM_SPECULAR)
		gMaterial.Specular = gColor;
}

void main(void)
{
	gl_Position = uMvpMatrix * inVertexPosition;
	
	gWorldPos = (uWorldMatrix * inVertexPosition).xyz;
	
	gColor = inVertexColor.bgra;
	
	gNormal = inVertexNormal.xyz;
	if(uNormalize)
		gNormal = normalize(gNormal);
	
	ReplaceColorMaterial();
	if(uLighting)
		varVertexColor = computeLighting();
	else
		varVertexColor = gColor;
	
	for(int i = 0; i < MAX_TEXTURE_UNITS; ++i)
		varTexCoord[i] = vec4(0.0);
	
	if( uUseTexture[0])
	{
		if(uRenderMode == SphereMap || uRenderMode == Reflection2Layer)
		{
			vec3 eyeDir = normalize(inVertexPosition.xyz - uEyePos);
			vec3 reflection = reflect(eyeDir, gNormal);
			float m = 2.0 * sqrt(reflection.x * reflection.x +
								 reflection.y * reflection.y +
								 (reflection.z + 1.0) * (reflection.z + 1.0));
			varTexCoord[0] = vec4((reflection.x / m + 0.5), (reflection.y / m + 0.5), 0.0, 0.0); 
		}
		else
		{
			varTexCoord[0] = inTexCoord0;
			if(uUseTexMatrix[0])
				varTexCoord[0] *= uTextureMatrix[0];
		}
	}
	if(uUseTexture[1])
	{
		varTexCoord[1] = inTexCoord1;
		if(uUseTexMatrix[1])
			varTexCoord[1] *= uTextureMatrix[1];
	}
	
	eyeDist = length(uEyePos);
	
	varClipDist = uClip ? dot(gWorldPos, uClipPlane.xyz)-uClipPlane.w : 0.0;
	
	varVertexColor.rgb  = clamp(varVertexColor.rgb, 0.0, 1.0);
	varVertexColor.a = gColor.a;
}
