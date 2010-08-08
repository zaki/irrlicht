// Copyright (C) 2009-2010 Amundis
// Heavily based on the OpenGL driver implemented by Nikolaus Gebhardt
// and OpenGL ES driver implemented by Christian Stehno
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h
precision mediump float;

uniform bool uUseTexture;
uniform sampler2D uTextureUnit;
uniform bool uAlphaTest;
uniform float uAlphaValue;

varying vec4 varVertexColor;
varying vec4 varTexCoord;

void main(void)
{
	vec4 color = varVertexColor;
	vec4 texel = texture2D(uTextureUnit, varTexCoord.xy);
	if(uUseTexture)
	{
		color *= texel;
	}
	
	if(uAlphaTest && !(color.a > uAlphaValue))
		discard;
	
	gl_FragColor = color;
}
