precision mediump float;

/* Uniforms */

uniform int uTextureUsage0;
uniform sampler2D uTextureUnit0;

/* Varyings */

varying vec2 vTextureCoord0;
varying vec4 vVertexColor;

void main()
{
	vec4 Color = vVertexColor;

	if (bool(uTextureUsage0))
		Color *= texture2D(uTextureUnit0, vTextureCoord0);

	gl_FragColor = Color;

}
