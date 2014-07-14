#define MAX_LIGHTS 8

/* Attributes */

attribute vec3 inVertexPosition;
attribute vec3 inVertexNormal;
attribute vec4 inVertexColor;
attribute vec2 inTexCoord0;
attribute vec2 inTexCoord1;

/* Uniforms */

uniform mat4 uWVPMatrix;
uniform mat4 uWVMatrix;
uniform mat4 uNMatrix;
uniform mat4 uTMatrix0;
uniform mat4 uTMatrix1;

uniform vec4 uMaterialAmbient;
uniform vec4 uMaterialDiffuse;
uniform vec4 uMaterialSpecular;
uniform float uMaterialShininess;

uniform int uLightCount;
uniform int uLightType[MAX_LIGHTS];
uniform vec3 uLightPosition[MAX_LIGHTS];
uniform vec3 uLightDirection[MAX_LIGHTS];
uniform vec3 uLightAttenuation[MAX_LIGHTS];
uniform vec4 uLightAmbient[MAX_LIGHTS];
uniform vec4 uLightDiffuse[MAX_LIGHTS];
uniform vec4 uLightSpecular[MAX_LIGHTS];

/* Varyings */

varying vec2 vTextureCoord0;
varying vec2 vTextureCoord1;
varying vec4 vVertexColor;
varying float vFogCoord;

void dirLight(in int index, in vec3 position, in vec3 normal, inout vec4 ambient, inout vec4 diffuse, inout vec4 specular)
{
	vec3 L = normalize(-(uNMatrix * vec4(uLightDirection[index], 0.0)).xyz);

	ambient += uLightAmbient[index];

	float NdotL = dot(normal, L);

	if (NdotL > 0.0)
	{
		diffuse += uLightDiffuse[index] * NdotL;

		vec3 HalfVector = normalize(L + vec3(0.0, 0.0, 1.0));
		float NdotH = max(0.0, dot(normal, HalfVector));

		float SpecularFactor = pow(NdotH, uMaterialShininess);
		specular += uLightSpecular[index] * SpecularFactor;
	}
}

void pointLight(in int index, in vec3 position, in vec3 normal, inout vec4 ambient, inout vec4 diffuse, inout vec4 specular)
{
	vec3 L = uLightPosition[index] - position;
	float D = length(L);
	L = normalize(L);

	float Attenuation = 1.0 / (uLightAttenuation[index].x + uLightAttenuation[index].y * D +
		uLightAttenuation[index].z * D * D);

	ambient += uLightAmbient[index] * Attenuation;

	float NdotL = dot(normal, L);

	if (NdotL > 0.0)
	{
		diffuse += uLightDiffuse[index] * NdotL * Attenuation;

		vec3 HalfVector = normalize(L + vec3(0.0, 0.0, 1.0));
		float NdotH = max(0.0, dot(normal, HalfVector));

		float SpecularFactor = pow(NdotH, uMaterialShininess);
		specular += uLightSpecular[index] * SpecularFactor * Attenuation;
	}
}

void spotLight(in int index, in vec3 position, in vec3 normal, inout vec4 ambient, inout vec4 diffuse, inout vec4 specular)
{
	// TO-DO
}

void main()
{
	gl_Position = uWVPMatrix * vec4(inVertexPosition, 1.0);
	
	vec4 TextureCoord0 = vec4(inTexCoord0.x, inTexCoord0.y, 0.0, 0.0);
	vTextureCoord0 = vec4(uTMatrix0 * TextureCoord0).xy;

	vec4 TextureCoord1 = vec4(inTexCoord1.x, inTexCoord1.y, 0.0, 0.0);
	vTextureCoord1 = vec4(uTMatrix1 * TextureCoord1).xy;

	vVertexColor = inVertexColor.bgra;

	vec3 Position = (uWVMatrix * vec4(inVertexPosition, 1.0)).xyz;

	if (uLightCount > 0)
	{
		vec3 Normal = normalize((uNMatrix * vec4(inVertexNormal, 0.0)).xyz);

		vec4 Ambient = vec4(0.0, 0.0, 0.0, 0.0);
		vec4 Diffuse = vec4(0.0, 0.0, 0.0, 0.0);
		vec4 Specular = vec4(0.0, 0.0, 0.0, 0.0);

		for (int i = 0; i < int(uLightCount); i++)
		{
			if (uLightType[i] == 0)
				pointLight(i, Position, Normal, Ambient, Diffuse, Specular);
		}

		for (int i = 0; i < int(uLightCount); i++)
		{
			if (uLightType[i] == 1)
				spotLight(i, Position, Normal, Ambient, Diffuse, Specular);
		}

		for (int i = 0; i < int(uLightCount); i++)
		{
			if (uLightType[i] == 2)
				dirLight(i, Position, Normal, Ambient, Diffuse, Specular);
		}

		vec4 LightColor = Ambient * uMaterialAmbient + Diffuse * uMaterialDiffuse + Specular * uMaterialSpecular;
		LightColor = clamp(LightColor, 0.0, 1.0);
		LightColor.w = 1.0;

		vVertexColor *= LightColor;
	}

	vFogCoord = length(Position);
}
