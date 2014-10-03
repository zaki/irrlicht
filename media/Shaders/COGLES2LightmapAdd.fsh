precision mediump float;

/* Uniforms */

uniform int uTextureUsage0;
uniform int uTextureUsage1;
uniform sampler2D uTextureUnit0;
uniform sampler2D uTextureUnit1;
uniform int uFogEnable;
uniform int uFogType;
uniform vec4 uFogColor;
uniform float uFogStart;
uniform float uFogEnd;
uniform float uFogDensity;

/* Varyings */

varying vec2 vTextureCoord0;
varying vec2 vTextureCoord1;
varying vec4 vVertexColor;
varying vec4 vSpecularColor;
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
	vec4 Color0 = vec4(1.0, 1.0, 1.0, 1.0);
	vec4 Color1 = vec4(1.0, 1.0, 1.0, 1.0);

	if (bool(uTextureUsage0))
		Color0 = texture2D(uTextureUnit0, vTextureCoord0);

	if (bool(uTextureUsage1))
		Color1 = texture2D(uTextureUnit1, vTextureCoord1);

	vec4 FinalColor = (Color0 + Color1) * vVertexColor + vSpecularColor;

	if (bool(uFogEnable))
	{
		float FogFactor = computeFog();
		vec4 FogColor = uFogColor;
		FogColor.a = 1.0;
		FinalColor = mix(FogColor, FinalColor, FogFactor);
	}
	
	gl_FragColor = FinalColor;
}
