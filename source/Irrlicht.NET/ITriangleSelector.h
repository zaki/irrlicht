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
/// Every ISceneNode may have a triangle selector, available with 
/// ISceneNode::getTriangleScelector() or ISceneManager::createTriangleSelector.
/// This is used for doing collision detection: For example if you know, that a collision may happened in the 
/// area between (1,1,1) and (10,10,10), you can get all triangles of the
/// scene node in this area with the ITriangleSelector easily and check every triangle
/// if it collided.
/// </summary>
public __gc class ITriangleSelector
{
public:

	/// <summary>
	/// You should create an ITriangleSelector
	/// through the Irrlicht::IrrlichtDevice::SceneManager::createTriangleSelector method. 
	/// Simply don't use this constructor.
	///</summary>
	///<param name="realTriangleSelector">The real, unmanaged C++ selector</param>
	ITriangleSelector(irr::scene::ITriangleSelector* realTriangleSelector);

	~ITriangleSelector();

	//! Returns amount of all available triangles in this selector
	__property int get_TriangleCount();

	/// <summary>
	/// Returns the internal pointer to the native C++ irrlicht triangle selector.
	/// Do not use this, only needed by the internal .NET wrapper.
	///</summary>
	__property irr::scene::ITriangleSelector* get_NativeTriangleSelector();
	
protected:

	irr::scene::ITriangleSelector* TriangleSelector;
};


}
}