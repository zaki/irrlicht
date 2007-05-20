// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

using namespace System;

#pragma unmanaged
#include "..\\..\\include\\irrlicht.h"
#pragma managed

#include "Material.h"

namespace Irrlicht
{
namespace Video
{
	public __gc class IVideoDriver;


	//// <summary>
	/// Interface for material rendering. Can be used to extend the engine with new materials.
	/// Refer to IVideoDriver::addMaterialRenderer() for more informations on how to extend the engine
	/// with new materials.<summary>
	///</summary>
	public __gc class IMaterialRendererServices
	{
	public:

		IMaterialRendererServices( irr::video::IMaterialRendererServices* realServices,
								   Irrlicht::Video::IVideoDriver* videoDriver );

		/// <summary>
		/// Can be called by an IMaterialRenderer to make its work easier.
		/// Sets all basic renderstates if needed.
		/// Basic render states are diffuse, ambient, specular, and emissive color, specular power,
		/// bilinear and trilinear filtering, wireframe mode,
		/// grouraudshading, lighting, zbuffer, zwriteenable, backfaceculling and fog enabling.
		/// </summary>
		void SetBasicRenderStates( Material material, Material lastMaterial, bool resetAllRenderstates );

		/// <summary>
		/// Sets a constant for the vertex shader based on a name. This can be used if you used
		/// a high level shader language like GLSL or HLSL to create a shader. 
		/// </summary>
		bool SetVertexShaderConstant( String* name, float floats __gc [], int count );

		/// <summary>
		/// Sets a vertex shader constant. Can be used if you created a shader using 
		/// pixel/vertex shader assembler or ARB_fragment_program or ARB_vertex_program.
		/// </summary>
		void SetVertexShaderConstant( float data __gc [], int startRegister, int constantAmount );

		/// <summary>
		/// Sets a constant for the pixel shader based on a name. This can be used if you used
		/// a high level shader language like GLSL or HLSL to create a shader. 
		/// </summary>
		bool SetPixelShaderConstant( String* name, float floats __gc [], int count );

		/// <summary>
		/// Sets a pixel shader constant. Can be used if you created a shader using 
		/// pixel/vertex shader assembler or ARB_fragment_program or ARB_vertex_program.
		/// </summary>
		void SetPixelShaderConstant( float data __gc [], int startRegister, int constantAmount );

		/// <summary>
		/// Returns the video driver.
		/// </summary>
		__property Irrlicht::Video::IVideoDriver* get_VideoDriver();

		inline irr::video::IMaterialRendererServices* getMaterialRendererServices()
		{
			return (irr::video::IMaterialRendererServices*)materialRendererServices;
		}

	protected:

		irr::video::IMaterialRendererServices* materialRendererServices;
		Irrlicht::Video::IVideoDriver* TheVideoDriver;

	};
}
}