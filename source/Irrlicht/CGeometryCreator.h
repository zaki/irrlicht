// Copyright (C) 2002-2007 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_GEOMETRY_CREATOR_H_INCLUDED__
#define __C_GEOMETRY_CREATOR_H_INCLUDED__

#include "IAnimatedMesh.h"
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

//! class for creating geometry on the fly
class CGeometryCreator
{
public:

	static IAnimatedMesh* createHillPlaneMesh(
		const core::dimension2d<f32>& tileSize, const core::dimension2d<s32>& tileCount,
		video::SMaterial* material,	f32 hillHeight, const core::dimension2d<f32>& countHills,
		const core::dimension2d<f32>& textureRepeatCount);

	static IAnimatedMesh* createTerrainMesh(video::IImage* texture,
		video::IImage* heightmap, const core::dimension2d<f32>& stretchSize, f32 maxHeight,
		video::IVideoDriver* driver,
		const core::dimension2d<s32> defaultVertexBlockSize,
		bool debugBorders=false);

	static IAnimatedMesh* createArrowMesh (	const u32 tesselationCylinder,
											const u32 tesselationCone,
											const f32 height,
											const f32 cylinderHeight,
											const f32 width0,
											const f32 width1,
											const video::SColor vtxColor0,
											const video::SColor vtxColor1
										);


};


} // end namespace scene
} // end namespace irr

#endif

