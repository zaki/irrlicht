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
/// A billboard is like a 3d sprite: A 2d element,
/// which always looks to the camera. It is usually used for explosions, fire,
/// lensflares, particles and things like that.
/// </summary>
public __gc class IBillboardSceneNode : public ISceneNode
{
public:

	/// <summary>
	/// Creates a billboard scene node from a native C++ scene node. Don't use this, its better to create 
	/// scene nodes using the SceneManager with its addSceneNode() methods.
	/// </summary>
	IBillboardSceneNode(irr::scene::IBillboardSceneNode* realSceneNode);

    /// <summary>
	/// Sets or gets the size of the billboard.
	/// </summary>
	__property Core::Dimension2Df get_Size();

	 /// <summary>
	/// Sets or gets the size of the billboard.
	/// </summary>
	__property void set_Size(Core::Dimension2Df dim);

protected:

	inline irr::scene::IBillboardSceneNode* getBillBoardSceneNode()
	{
		return (irr::scene::IBillboardSceneNode*)SceneNode;
	}

};


}
}