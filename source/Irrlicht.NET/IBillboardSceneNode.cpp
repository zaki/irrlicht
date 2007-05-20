// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "IBillboardSceneNode.h"
#include "NativeConverter.h"

namespace Irrlicht
{
namespace Scene
{

	IBillboardSceneNode::IBillboardSceneNode(irr::scene::IBillboardSceneNode* realSceneNode)
		: ISceneNode(realSceneNode)
	{
	}

	Core::Dimension2Df IBillboardSceneNode::get_Size()
	{
		return irr::NativeConverter::getNETDim(getBillBoardSceneNode()->getSize());
	}

	void IBillboardSceneNode::set_Size(Core::Dimension2Df dim)
	{
		getBillBoardSceneNode()->setSize(irr::NativeConverter::getNativeDim(dim));
	}


}
}