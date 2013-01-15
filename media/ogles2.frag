precision mediump float;

varying vec2 v_texCoord;
varying vec4 v_color;

uniform sampler2D myTexture;

void main (void)
{
    vec4 col = texture2D(myTexture, v_texCoord);
    col *= v_color;
    gl_FragColor = col * 4.0;
}
