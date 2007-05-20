// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#using <mscorlib.dll>
using namespace System;

#pragma unmanaged
#include "..\\..\\include\\irrlicht.h"
#pragma managed

#include "ITriangleSelector.h"

namespace Irrlicht
{
namespace Scene
{

/// <summary> 
/// Interface for making multiple triangle selectors work as one big selector.
/// This is nothing more than a collection of one or more triangle selectors
/// providing together the interface of one triangle selector. In this way, 
/// collision tests can be done with different triangle soups in one pass.
/// </summary>
public __gc class IMetaTriangleSelector : public ITriangleSelector
{
public:

	/// <summary>
	/// You should create an IMetaTriangleSelector
	/// through the Irrlicht::IrrlichtDevice::SceneManager::createMetaTriangleSelector method. 
	/// Simply don't use this constructor.
	///</summary>
	///<param name="realTriangleSelector">The real, unmanaged C++ selector</param>
	IMetaTriangleSelector(irr::scene::IMetaTriangleSelector* realTriangleSelector);

	~IMetaTriangleSelector();

	/// <summary>
	/// Adds a triangle selector to the collection of triangle selectors
	/// in this metaTriangleSelector.
	/// </summary>
	/// <param name="toAdd"> Pointer to an triangle selector to add to the list.</param>
	void AddTriangleSelector(ITriangleSelector* toAdd); 

	/// <summary>
	/// Removes a specific triangle selector which was added before	from the collection.
	/// </summary>
	/// <param name="toRemove"> Pointer to an triangle selector which is in the list
	/// but will be removed.</param>
	/// <returns> Returns true if successful, false if not.</returns>
	bool RemoveTriangleSelector(ITriangleSelector* toRemove);

	/// <summary>
	/// Removes all triangle selectors from the collection.
	/// </summary>
	void RemoveAllTriangleSelectors();

	/// <summary>
	/// Returns the internal pointer to the native C++ irrlicht triangle selector.
	/// Do not use this, only needed by the internal .NET wrapper.
	///</summary>
	__property irr::scene::IMetaTriangleSelector* get_NativeMetaTriangleSelector();
	
protected:

	irr::scene::IMetaTriangleSelector* MetaTriangleSelector;
};


}
}