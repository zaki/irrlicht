precision mediump float;

varying vec2 varTexCoord0;
varying vec2 varTexCoord1;
varying vec4 varVertexColor;
varying float eyeDist;

uniform sampler2D uTextureUnit0;
uniform sampler2D uTextureUnit1;

vec4 renderSolid(void)
{
	vec4 color = vec4(1.0);//varVertexColor;
	vec4 texel = texture2D(uTextureUnit0, varTexCoord0);
	color *= texel;

	return color;
}

void main (void)
{
    gl_FragColor = renderSolid();
}