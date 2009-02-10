// Copyright (C) 2002-2009 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_GEOMETRY_CREATOR_H_INCLUDED__
#define __I_GEOMETRY_CREATOR_H_INCLUDED__

#include "IMesh.h"
#include "IImage.h"

namespace irr
{
namespace video
{
	class IVideoDriver;
	class SMaterial;
}

namespace scene
{

//! Helper class for creating geometry on the fly. 
/** You can get an instance of this class through ISceneManager::getGeometryCreator() */
class IGeometryCreator
{
public:

	//! Create a psuedo-random mesh representing a hilly terrain.
	/** 
	\param tileSize The size of each time.
	\param tileCount The number of tiles in each dimension.
	\param material The material to apply to the mesh.
	\param hillHeight The maximum height of the hills.
	\param countHills The number of hills along each dimension.
	\param textureRepeatCount The number of times to repeat the material texture along each dimension.
	*/
	virtual IMesh* createHillPlaneMesh(
		const core::dimension2d<f32>& tileSize, const core::dimension2d<u32>& tileCount,
		video::SMaterial* material, f32 hillHeight, const core::dimension2d<f32>& countHills,
		const core::dimension2d<f32>& textureRepeatCount) const = 0;

	//! Create a terrain mesh from an image representing a heightfield.
	/** 
	\param texture The texture to apply to the terrain.
	\param heightmap An image that will be interpreted as a heightmap. The brightness 
	(average colour) of each pixel is interpreted as a height, with a 255 brightness
	pixel producing the maximum height.
	\param stretchSize The size that each pixel will produce, i.e. a 512x512 heightmap
	and a stretchSize of (10.f, 20.f) will produce a mesh of size 5120.f x 10240.f
	\param maxHeight The maximum height of the terrain.
	\param driver The current video driver.
	\param defaultVertexBlockSize (to be documented)
	\param debugBorders (to be documented)
	*/
	virtual IMesh* createTerrainMesh(video::IImage* texture,
		video::IImage* heightmap, const core::dimension2d<f32>& stretchSize,
		f32 maxHeight, video::IVideoDriver* driver,
		const core::dimension2d<u32>& defaultVertexBlockSize,
		bool debugBorders=false) const = 0;

	//! Create an arrow mesh, composed of a cylinder and a cone.
	/**
	\param tesselationCylinder Number of quads composing the cylinder.
	\param tesselationCone Number of triangles composing the cone's roof.
	\param height Total height of the arrow
	\param cylinderHeight Total height of the cylinder, should be lesser than total height
	\param widthCylinder Diameter of the cylinder
	\param widthCone Diameter of the cone's base, should be not smaller than the cylinder's diameter
	\param colorCylinder color of the cylinder
	\param colorCone color of the cone
	*/
	virtual IMesh* createArrowMesh(const u32 tesselationCylinder = 4,
			const u32 tesselationCone = 8, const f32 height = 1.f,
			const f32 cylinderHeight = 0.6f, const f32 widthCylinder = 0.05f,
			const f32 widthCone = 0.3f, const video::SColor colorCylinder = 0xFFFFFFFF,
			const video::SColor colorCone = 0xFFFFFFFF) const = 0;


	//! Create a sphere mesh.
	/**
	\param radius Radius of the sphere
	\param polyCountX Number of quads used for the horizontal tiling
	\param polyCountY Number of quads used for the vertical tiling
	*/
	virtual IMesh* createSphereMesh(f32 radius = 5.f, u32 polyCountX = 16, 
							u32 polyCountY = 16) const = 0;

	//! Create a cylinder mesh.
	/**
	\param radius Radius of the cylinder.
	\param length Length of the cylinder.
	\param tesselation Number of quads around the circumference of the cylinder.
	\param color The color of the cylinder.
	\param closeTop If true, close the ends of the cylinder, otherwise leave them open.
	\param oblique (to be documented)
	*/
	virtual IMesh* createCylinderMesh(f32 radius, f32 length, u32 tesselation, 
							const video::SColor& color=video::SColor(0xffffffff), 
							bool closeTop=true, f32 oblique=0.f) const = 0;

	//! Create a cone mesh.
	/**
	\param radius Radius of the cone.
	\param length Length of the cone.
	\param tesselation Number of quads around the circumference of the cone.
	\param colorTop The color of the top of the cone.
	\param colorBottom The color of the bottom of the cone.
	\param oblique (to be documented)
	*/
	virtual IMesh* createConeMesh(f32 radius, f32 length, u32 tesselation, 
						const video::SColor& colorTop=video::SColor(0xffffffff), 
						const video::SColor& colorBottom=video::SColor(0xffffffff),
						f32 oblique=0.f) const = 0;
};


} // end namespace scene
} // end namespace irr

#endif // __I_GEOMETRY_CREATOR_H_INCLUDED__

