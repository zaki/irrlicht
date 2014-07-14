#define MAX_LIGHTS 2

precision mediump float;

/* Uniforms */

uniform float uFactor;
uniform sampler2D uTextureUnit0;
uniform sampler2D uTextureUnit1;
uniform int uFogEnable;
uniform int uFogType;
uniform vec4 uFogColor;
uniform float uFogStart;
uniform float uFogEnd;
uniform float uFogDensity;

/* Varyings */

varying vec2 vTexCoord;
varying vec3 vEyeVector;
varying vec3 vLightVector[MAX_LIGHTS];
varying vec4 vLightColor[MAX_LIGHTS];
varying float vFogCoord;

float computeFog()
{
	const float LOG2 = 1.442695;
	float FogFactor = 0.0;

	if (uFogType == 0) // Exp
	{
		FogFactor = exp2(-uFogDensity * vFogCoord * LOG2);
	}
	else if (uFogType == 1) // Linear
	{
		float Scale = 1.0 / (uFogEnd - uFogStart);
		FogFactor = (uFogEnd - vFogCoord) * Scale;
	}
	else if (uFogType == 2) // Exp2
	{
		FogFactor = exp2(-uFogDensity * uFogDensity * vFogCoord * vFogCoord * LOG2);
	}

	FogFactor = clamp(FogFactor, 0.0, 1.0);

	return FogFactor;
}

void main()
{
	vec4 TempFetch = texture2D(uTextureUnit1, vTexCoord) *  2.0 - 1.0;
	TempFetch *= uFactor;

	vec3 EyeVector = normalize(vEyeVector);
	vec2 TexCoord = EyeVector.xy * TempFetch.w + vTexCoord;

	vec4 Color  = texture2D(uTextureUnit0, TexCoord);
	vec3 Normal = texture2D(uTextureUnit1, TexCoord).xyz *  2.0 - 1.0;

	vec4 FinalColor = vec4(0.0, 0.0, 0.0, 0.0);

	for (int i = 0; i < int(MAX_LIGHTS); i++)
	{
		vec3 LightVector = normalize(vLightVector[i]);

		float Lambert = max(dot(LightVector, Normal), 0.0);
		FinalColor += vec4(Lambert) * vLightColor[i];
	}

	FinalColor *= Color;
	FinalColor.w = vLightColor[0].w;

	if (bool(uFogEnable))
	{
		float FogFactor = computeFog();
		vec4 FogColor = uFogColor;
		FogColor.a = 1.0;
		FinalColor = mix(FogColor, FinalColor, FogFactor);
	}
	
	gl_FragColor = FinalColor;
}
