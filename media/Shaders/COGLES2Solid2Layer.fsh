precision mediump float;

/* Uniforms */

uniform int uTextureUsage0;
uniform int uTextureUsage1;
uniform sampler2D uTextureUnit0;
uniform sampler2D uTextureUnit1;

/* Varyings */

varying vec2 vTextureCoord0;
varying vec2 vTextureCoord1;
varying vec4 vVertexColor;

void main()
{
	vec4 Color0 = vec4(1.0, 1.0, 1.0, 1.0);
	vec4 Color1 = vec4(1.0, 1.0, 1.0, 1.0);

	if (bool(uTextureUsage0))
		Color0 = texture2D(uTextureUnit0, vTextureCoord0);

	if (bool(uTextureUsage1))
		Color1 = texture2D(uTextureUnit1, vTextureCoord1);

	gl_FragColor = (Color0 * vVertexColor.a + Color1 * (1.0 - vVertexColor.a)) * vVertexColor;

}
