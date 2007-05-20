// Copyright (C) 2002-2007 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __S_LIGHT_H_INCLUDED__
#define __S_LIGHT_H_INCLUDED__

#include "SColor.h"

namespace irr
{
namespace video
{

//! Enumeration for different types of lights
enum E_LIGHT_TYPE
{
	//! point light, it has a position in space and radiates light in all directions
	ELT_POINT,

	//! directional light, coming from a direction from an infinite distance
	ELT_DIRECTIONAL
};

//! Names for light types
const c8* const LightTypeNames[] =
{
	"Point",
	"Directional",
	0
};

//! structure for holding data describing a dynamic point light.
/** ambient light and point lights are the only light supported 
by the irrlicht engine.
*/
struct SLight
{
	SLight() : AmbientColor(0.0f,0.0f,0.0f), DiffuseColor(1.0f, 1.0f, 1.0f), 
		SpecularColor(1.0f,1.0f,1.0f), Position(0.0f, 0.0f, 0.0f), Radius(100.0f),
		CastShadows(true), Type(ELT_POINT)
		 {};

	//! Ambient color emitted by the light
	SColorf AmbientColor;

	//! Diffuse color emitted by the light.
	/** This is the primary color you might want to set. */
	SColorf DiffuseColor; 

	//! Specular color emitted by the light. 
	/** For details how to use specular highlights, see SMaterial::Shininess */
	SColorf SpecularColor;

	//! Position of the light. If Type is ELT_DIRECTIONAL, this is the direction vector the light is coming from.
	core::vector3df Position;

	//! Radius of light. Everything within this radius be be lighted.
	f32 Radius;

	//! Does the light cast shadows?
	bool CastShadows;

	//! Type of the light. Default: ELT_POINT
	E_LIGHT_TYPE Type;
};

} // end namespace video
} // end namespace irr

#endif

