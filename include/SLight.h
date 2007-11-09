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
	//! spot light, it has a position in space, a direction, and a limited cone of influence
	ELT_SPOT,
	//! directional light, coming from a direction from an infinite distance
	ELT_DIRECTIONAL
};

//! Names for light types
const c8* const LightTypeNames[] =
{
	"Point",
	"Spot",
	"Directional",
	0
};

//! structure for holding data describing a dynamic point light.
/** Irrlicht supports point lights, spot lights, and directional lights.
*/
struct SLight
{
	SLight() : AmbientColor(0.0f,0.0f,0.0f), DiffuseColor(1.0f,1.0f,1.0f),
		SpecularColor(1.0f,1.0f,1.0f), Attenuation(1.0f,0.0f,0.0f),
		Radius(100.0f), OuterCone(45.0f), InnerCone(0.0f),
		Falloff(2.0f), CastShadows(true), Type(ELT_POINT),
		Position(0.0f,0.0f,0.0f), Direction(0.0f,0.0f,1.0f)
		 {};

	//! Ambient color emitted by the light
	SColorf AmbientColor;

	//! Diffuse color emitted by the light.
	/** This is the primary color you might want to set. */
	SColorf DiffuseColor; 

	//! Specular color emitted by the light. 
	/** For details how to use specular highlights, see SMaterial::Shininess */
	SColorf SpecularColor;

	//! Attenuation factors (constant, linear, quadratic)
	/** Changes the light strength fading over distance */
	core::vector3df Attenuation;

	//! Radius of light. Everything within this radius be be lighted.
	f32 Radius;

	//! The angle of the spot's outer cone. Ignored for other lights.
	f32 OuterCone;

	//! The angle of the spot's inner cone. Ignored for other lights.
	f32 InnerCone;

	//! The light strength's decrease between Outer and Inner cone.
	f32 Falloff;

	//! Does the light cast shadows?
	bool CastShadows;

	//! Type of the light. Default: ELT_POINT
	E_LIGHT_TYPE Type;

	//! Read-ONLY! Position of the light. If Type is ELT_DIRECTIONAL, this is ignored.
	core::vector3df Position;

	//! Read-ONLY! Direction of the light. If Type is ELT_POINT, this is ignored.
	core::vector3df Direction;
};

} // end namespace video
} // end namespace irr

#endif

