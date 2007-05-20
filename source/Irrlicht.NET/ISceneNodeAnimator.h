// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#using <mscorlib.dll>
using namespace System;

#pragma unmanaged
#include "..\\..\\include\\irrlicht.h"
#pragma managed

namespace Irrlicht
{
namespace Scene
{

/// <summary> 
/// Scene node interface.
/// Animates a scene node. Can animate position, rotation, material, and so on.
/// A scene node animator is able to animate a scene node in a very simple way. It may
///	change its position, rotation, scale and/or material. There are lots of animators
///	to choose from. You can create scene node animators with the ISceneManager interface.
/// </summary>
public __gc class ISceneNodeAnimator
{
public:

	/// <summary>
	/// Creates a scene node animator from a native C++ scene node animator.
	/// Don't use this, its better to create 
	/// scene nodes using the SceneManager with its addSceneNode() methods.
	/// </summary>
	ISceneNodeAnimator(irr::scene::ISceneNodeAnimator* realSceneNode);

	/// Destructor
	~ISceneNodeAnimator();

	/// <summary>
	/// Animates a scene node.
	/// </summary>
	/// <param name="node"> Node to animate.</param>
	//! <param name="timeMs" >Current time in milli seconds.</param>
	void animateNode(ISceneNode* node, int timeMs);

	/// <summary>
	/// Returns the internal pointer to the native C++ irrlicht scene node animator.
	/// Do not use this, only needed by the internal .NET wrapper.
	///</summary>
	__property irr::scene::ISceneNodeAnimator* get_NativeSceneNodeAnimator();
	
protected:

	irr::scene::ISceneNodeAnimator* SceneNodeAnimator;
};


}
}