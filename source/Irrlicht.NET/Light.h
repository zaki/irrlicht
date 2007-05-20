// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once 

#include "Vector3D.h"
#include "Color.h"

namespace Irrlicht
{
namespace Video
{

	/// <summary>
	/// structure for holding data describing a dynamic point light.
	/// ambient light and point lights are the only light supported 
	/// by the irrlicht engine.
	/// <summary>
	public __value class Light
	{
	public:

		Light() : AmbientColor(0.0f,0.0f,0.0f), DiffuseColor(1.0f, 1.0f, 1.0f), 
			SpecularColor(0.0f,0.0f,0.0f), Position(0.0f, 0.0f, 0.0f), Radius(100.0f),
			CastShadows(true)
			{};

		/// <summary>
		/// Ambient color emitted by the light
		/// </summary>
		Colorf AmbientColor; 

		/// <summary>
		/// Diffuse color emitted by the light. This is the primary color you
		/// might want to set.
		/// </summary>
		Colorf DiffuseColor; 

		/// <summary>
		/// Specular color emitted by the light. 
		/// </summary>
		Colorf SpecularColor; 

		/// <summary>
		/// Position of the light.
		/// </summary>
		Core::Vector3D Position; 

		/// <summary>
		/// Radius of light. Everything within this radius be be lighted.
		/// </summary>
		float Radius; 

		/// <summary>
		/// Does the light cast shadows?
		/// </summary>
		bool CastShadows;
	};



}
}