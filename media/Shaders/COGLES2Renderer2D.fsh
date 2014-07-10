precision mediump float;

uniform bool uUseTexture;
uniform sampler2D uTextureUnit;

varying vec4 vVertexColor;
varying vec2 vTexCoord;

void main(void)
{
	vec4 Color = vVertexColor;

	if(uUseTexture)
		Color *= texture2D(uTextureUnit, vTexCoord);
	
	gl_FragColor = Color;
}
