// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "ITriangleSelector.h"
#include "NativeConverter.h"

namespace Irrlicht
{
namespace Scene
{

	ITriangleSelector::ITriangleSelector(irr::scene::ITriangleSelector* realSceneNodeAnimator)
		: TriangleSelector(realSceneNodeAnimator)
	{
		TriangleSelector->grab();
	}

	ITriangleSelector::~ITriangleSelector()
	{
		TriangleSelector->drop();
	}

	irr::scene::ITriangleSelector* ITriangleSelector::get_NativeTriangleSelector()
	{
		return TriangleSelector;
	}

	//! Returns amount of all available triangles in this selector
	int ITriangleSelector::get_TriangleCount()
	{
		return TriangleSelector->getTriangleCount();
	}

}
}
