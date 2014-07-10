/* Attributes */

attribute vec3 inVertexPosition;
attribute vec3 inVertexNormal;
attribute vec4 inVertexColor;
attribute vec2 inTexCoord0;
attribute vec2 inTexCoord1;

/* Uniforms */

uniform mat4 uMVPMatrix;
uniform mat4 uTMatrix0;

/* Varyings */

varying vec2 vTextureCoord0;
varying vec4 vVertexColor;

void main()
{
	gl_Position = uMVPMatrix * vec4(inVertexPosition, 1.0);
	
	vec4 TextureCoord0 = vec4(inTexCoord0.x, inTexCoord0.y, 0.0, 0.0);
	vTextureCoord0 = vec4(uTMatrix0 * TextureCoord0).xy;

	vVertexColor = inVertexColor.bgra;
}
