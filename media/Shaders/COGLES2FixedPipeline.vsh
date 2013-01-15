attribute vec3 inVertexPosition;
attribute vec3 inVertexNormal;
attribute vec4 inVertexColor;
attribute vec2 inTexCoord0;

uniform mat4 uMvpMatrix;

/* Varyings */

varying vec2 varTexCoord0;
varying vec2 varTexCoord1;
varying vec4 varVertexColor;
varying float eyeDist;

void main(void)
{
	gl_Position = uMvpMatrix * vec4(inVertexPosition,1.0);
	
	varTexCoord0 = inTexCoord0;
	varTexCoord1 = inTexCoord0;
}
