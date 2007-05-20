// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#using <mscorlib.dll>
using namespace System;

#pragma unmanaged
#include "..\\..\\include\\irrlicht.h"
#pragma managed

#include "Light.h"
#include "ISceneNode.h"

namespace Irrlicht
{
namespace Scene
{

/// <summary>
/// Scene node which is a dynamic light. 
/// You can switch the light on and off by 
/// making it visible or not, and let it be animated by ordinary scene node animators.
/// </summary>
public __gc class ILightSceneNode : public ISceneNode
{
public:

	/// <summary>
	/// Creates a light scene node from a native C++ scene node. Don't use this, its better to create 
	/// scene nodes using the SceneManager with its addSceneNode() methods.
	/// </summary>
	ILightSceneNode(irr::scene::ILightSceneNode* realSceneNode);

    /// <summary>
	/// Sets or gets the light data.
	/// </summary>
	__property Video::Light get_LightData();

	/// <summary>
	/// Sets or gets the light data.
	/// </summary>
	__property void set_LightData(Video::Light light);

protected:

	inline irr::scene::ILightSceneNode* getLightSceneNode()
	{
		return (irr::scene::ILightSceneNode*)SceneNode;
	}

};


}
}