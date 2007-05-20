// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

namespace Irrlicht
{
namespace Scene
{
	/// <summary>
	/// An enum for all types of built in scene nodes
	/// </summary>
	__value public enum SceneNodeType
	{
		// Test Scene Node
		TEST = 0,

		// Text Scene Node
		TEXT,

		// Water Surface Scene Node
		WATER_SURFACE,

		// Terrain Scene Node
		TERRAIN,

		// Sky Box Scene Node
		SKY_BOX,

		// Shadow Volume Scene Node
		SHADOW_VOLUME,

		// OctTree Scene Node
		OCT_TREE,

		// Mesh Scene Node
		MESH,

		// Light Scene Node
		LIGHT,

		// Empty Scene Node
		EMPTY,

		// Dummy Transformation Scene Node
		DUMMY_TRANSFORMATION,

		// Camera Scene Node
		CAMERA,

		// Maya Camera Scene Node
		CAMERA_MAYA,

		// First Person Shooter style Camera
		CAMERA_FPS,

		// Billboard Scene Node
		BILLBOARD,

		// Animated Mesh Scene Node
		ANIMATED_MESH,

		// Particle System Scene Node
		PARTICLE_SYSTEM,

		// Amount of build in Scene Nodes
		COUNT,

		// Unknown scene node
		UNKNOWN
	};

} // end namespace video
} // end namespace irr

