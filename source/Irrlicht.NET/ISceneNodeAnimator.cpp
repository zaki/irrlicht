// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "ISceneNode.h"
#include "ISceneNodeAnimator.h"
#include "NativeConverter.h"

namespace Irrlicht
{
namespace Scene
{

	ISceneNodeAnimator::ISceneNodeAnimator(irr::scene::ISceneNodeAnimator* realSceneNodeAnimator)
		: SceneNodeAnimator(realSceneNodeAnimator)
	{
		SceneNodeAnimator->grab();
	}

	ISceneNodeAnimator::~ISceneNodeAnimator()
	{
		SceneNodeAnimator->drop();
	}

	irr::scene::ISceneNodeAnimator* ISceneNodeAnimator::get_NativeSceneNodeAnimator()
	{
		return SceneNodeAnimator;
	}

	/// Animates a scene node.
	void ISceneNodeAnimator::animateNode(ISceneNode* node, int timeMs)
	{
		SceneNodeAnimator->animateNode(node->get_NativeSceneNode(), timeMs);
	}

}
}
