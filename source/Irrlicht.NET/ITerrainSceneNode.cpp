// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "ITerrainSceneNode.h"
#include "NativeConverter.h"

namespace Irrlicht
{
namespace Scene
{

	ITerrainSceneNode::ITerrainSceneNode(irr::scene::ITerrainSceneNode* realSceneNode)
		: ISceneNode(realSceneNode)
	{
	}

	Core::Vector3D ITerrainSceneNode::get_TerrainCenter()
	{
		return irr::NativeConverter::getNETVector(getTerrainSceneNode()->getTerrainCenter());
	}

	void ITerrainSceneNode::set_CameraMovementDelta(float delta)
	{
		getTerrainSceneNode()->setCameraMovementDelta(delta);
	}

	void ITerrainSceneNode::set_CameraRotationDelta(float delta)
	{
		getTerrainSceneNode()->setCameraRotationDelta(delta);
	}

	void ITerrainSceneNode::set_DynamicSelectorUpdate(bool bVal)
	{
		getTerrainSceneNode()->setDynamicSelectorUpdate(bVal);
	}

	bool ITerrainSceneNode::OverrideLODDistance(int LOD, double newDistance)
	{
		return getTerrainSceneNode()->overrideLODDistance(LOD, newDistance);
	}

	void ITerrainSceneNode::ScaleTexture(float scale, float scale2)
	{
		getTerrainSceneNode()->scaleTexture(scale, scale2);
	}

}
}