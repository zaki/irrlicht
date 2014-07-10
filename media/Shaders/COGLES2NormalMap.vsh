#define MAX_LIGHTS 2

/* Attributes */

attribute vec3 inVertexPosition;
attribute vec3 inVertexNormal;
attribute vec3 inVertexTangent;
attribute vec3 inVertexBinormal;
attribute vec4 inVertexColor;
attribute vec2 inTexCoord0;

/* Uniforms */

uniform mat4 uWVPMatrix;
uniform mat4 uWVMatrix;
uniform vec3 uLightPosition[MAX_LIGHTS];
uniform vec4 uLightColor[MAX_LIGHTS];

/* Varyings */

varying vec2 vTexCoord;
varying vec3 vLightVector[MAX_LIGHTS];
varying vec4 vLightColor[MAX_LIGHTS];
varying float vFogCoord;

void main()
{
	gl_Position = uWVPMatrix * vec4(inVertexPosition, 1.0);

	vTexCoord = inTexCoord0;

	for (int i = 0; i < int(MAX_LIGHTS); i++)
	{
		vec3 LightVector = uLightPosition[i] - inVertexPosition;

		vLightVector[i].x = dot(inVertexTangent, LightVector);
		vLightVector[i].y = dot(inVertexBinormal, LightVector);
		vLightVector[i].z = dot(inVertexNormal, LightVector);

		vLightColor[i].x = dot(LightVector, LightVector);
		vLightColor[i].x *= uLightColor[i].a;
		vLightColor[i] = vec4(inversesqrt(vLightColor[i].x));
		vLightColor[i] *= uLightColor[i];
		vLightColor[i].a = inVertexColor.a;

		vLightColor[i].x = clamp(vLightColor[i].x, 0.0, 1.0);
		vLightColor[i].y = clamp(vLightColor[i].y, 0.0, 1.0);
		vLightColor[i].z = clamp(vLightColor[i].z, 0.0, 1.0);
	}

	vFogCoord = length((uWVMatrix * vec4(inVertexPosition, 1.0)).xyz);
}
