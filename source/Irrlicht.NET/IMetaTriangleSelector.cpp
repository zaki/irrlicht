// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "IMetaTriangleSelector.h"
#include "NativeConverter.h"

namespace Irrlicht
{
namespace Scene
{

	IMetaTriangleSelector::IMetaTriangleSelector(irr::scene::IMetaTriangleSelector* realSceneNodeAnimator)
		: ITriangleSelector(realSceneNodeAnimator), MetaTriangleSelector(realSceneNodeAnimator)
	{
		MetaTriangleSelector->grab();
	}

	IMetaTriangleSelector::~IMetaTriangleSelector()
	{
		MetaTriangleSelector->drop();
	}

	irr::scene::IMetaTriangleSelector* IMetaTriangleSelector::get_NativeMetaTriangleSelector()
	{
		return MetaTriangleSelector;
	}

	void IMetaTriangleSelector::AddTriangleSelector(ITriangleSelector* toAdd)
	{
		if (!toAdd)
			return;

		MetaTriangleSelector->addTriangleSelector(toAdd->get_NativeTriangleSelector());
	}

	bool IMetaTriangleSelector::RemoveTriangleSelector(ITriangleSelector* toRemove)
	{
		if (!toRemove)
			return false;

		return MetaTriangleSelector->removeTriangleSelector(toRemove->get_NativeTriangleSelector());
	}

	void IMetaTriangleSelector::RemoveAllTriangleSelectors()
	{
		MetaTriangleSelector->removeAllTriangleSelectors();
	}

}
}
