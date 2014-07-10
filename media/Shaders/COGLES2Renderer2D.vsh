attribute vec4 inVertexPosition;
attribute vec4 inVertexColor;
attribute vec2 inTexCoord0;

uniform mat4 uOrthoMatrix;

varying vec4 vVertexColor;
varying vec2 vTexCoord;

void main(void)
{
	gl_Position = uOrthoMatrix * inVertexPosition;
	vVertexColor = inVertexColor.bgra;
	vTexCoord = inTexCoord0;
}
