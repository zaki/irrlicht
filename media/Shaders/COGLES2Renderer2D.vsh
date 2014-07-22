/* Attributes */

attribute vec4 inVertexPosition;
attribute vec4 inVertexColor;
attribute vec2 inTexCoord0;

/* Varyings */

varying vec2 vTextureCoord;
varying vec4 vVertexColor;

void main()
{
	gl_Position = inVertexPosition;
	vTextureCoord = inTexCoord0;
	vVertexColor = inVertexColor.bgra;
}
