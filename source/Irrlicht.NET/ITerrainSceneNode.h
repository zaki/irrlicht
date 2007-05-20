// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#using <mscorlib.dll>
using namespace System;

#pragma unmanaged
#include "..\\..\\include\\irrlicht.h"
#pragma managed

#include "ISceneNode.h"

namespace Irrlicht
{
namespace Scene
{

/// <summary>
/// A scene node for displaying terrain using the geo mip map algorithm.
/// The code for the TerrainSceneNode is based on the Terrain renderer by Soconne and 
/// the GeoMipMapSceneNode developed by Spinz. They made their code available for Irrlicht 
/// and allowed it to be  distributed under this licence. I only modified some parts.
/// A lot of thanks go to them. 
/// </summary>
public __gc class ITerrainSceneNode : public ISceneNode
{
public:

	/// <summary>
	/// Creates a terrain scene node from a native C++ scene node. Don't use this, its better to create 
	/// scene nodes using the SceneManager with its addSceneNode() methods.
	/// </summary>
	ITerrainSceneNode(irr::scene::ITerrainSceneNode* realSceneNode);

	/// <summary>
	/// Returns center of terrain.
	/// </summary>
	__property Core::Vector3D get_TerrainCenter();

	/// <summary>
	/// Sets the movement camera threshold.
	/// It is used to determine when to recalculate
	/// indices for the scene node.  The default value is 10.0f. 
	/// </summary>	
	__property void set_CameraMovementDelta(float delta);

	/// <summary>
	/// Sets the rotation camera threshold.
	/// It is used to determine when to recalculate
	/// indices for the scene node.  The default value is 1.0f. 
	/// </summary>
	__property void set_CameraRotationDelta(float delta);

	/// <summary>
	/// Sets whether or not the node should dynamically update its associated selector 
	/// when the geomipmap data changes.
	/// <param name="bVal">Boolean value representing whether or not 
	/// to update selector dynamically.</param>
	/// </summary>
	__property void set_DynamicSelectorUpdate(bool bVal);

	/// <summary>
	/// Override the default generation of distance thresholds.
	/// For determining the LOD a patch
	/// is rendered at. If any LOD is overridden, then the scene node will no longer apply
	/// scaling factors to these values.  If you override these distances, and then apply
	/// a scale to the scene node, it is your responsibility to update the new distances to
	/// work best with your new terrain size. 
	/// </summary> 
	bool OverrideLODDistance(int LOD, double newDistance);

	/// <summary> 
	/// Scales the base texture, similar to makePlanarTextureMapping.
	/// <param name="scale"> The scaling amount.  Values above 1.0 increase the number of time the 
	/// texture is drawn on the terrain.  Values below 0 will decrease the number of times the
	/// texture is drawn on the terrain.  Using negative values will flip the texture, as
	/// well as still scaling it. 
	/// <param name="scale2"> If set to 0 (default value), this will set the second texture coordinate set
	/// to the same values as in the first set. If this is another value than zero, it will scale
	/// the second texture coordinate set by this value. </param>
	void ScaleTexture(float scale, float scale2);
  
protected:

	inline irr::scene::ITerrainSceneNode* getTerrainSceneNode()
	{
		return (irr::scene::ITerrainSceneNode*)SceneNode;
	}

};


}
}