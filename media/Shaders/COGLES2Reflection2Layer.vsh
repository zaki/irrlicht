/* Attributes */

attribute vec3 inVertexPosition;
attribute vec3 inVertexNormal;
attribute vec4 inVertexColor;
attribute vec2 inTexCoord0;
attribute vec2 inTexCoord1;

/* Uniforms */

uniform mat4 uMVPMatrix;
uniform mat4 uMVMatrix;
uniform mat4 uNMatrix;
uniform mat4 uTMatrix0;

/* Varyings */

varying vec2 vTextureCoord0;
varying vec2 vTextureCoord1;
varying vec4 vVertexColor;

void main()
{
	gl_Position = uMVPMatrix * vec4(inVertexPosition, 1.0);

	vec4 TextureCoord0 = vec4(inTexCoord0.x, inTexCoord0.y, 0.0, 0.0);
	vTextureCoord0 = vec4(uTMatrix0 * TextureCoord0).xy;

	vec3 P = normalize(vec4(uMVMatrix * vec4(inVertexPosition, 1.0)).xyz);
	vec3 N = normalize(vec4(uNMatrix * vec4(inVertexNormal, 0.0)).xyz);
	vec3 R = reflect(P, N);

	float V = 2.0 * sqrt(R.x*R.x + R.y*R.y + (R.z+1.0)*(R.z+1.0));
	vTextureCoord1 = vec2(R.x/V + 0.5, R.y/V + 0.5);

	vVertexColor = inVertexColor.bgra;
}
