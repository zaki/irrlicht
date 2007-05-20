// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "ILightSceneNode.h"
#include "NativeConverter.h"

namespace Irrlicht
{
namespace Scene
{


	ILightSceneNode::ILightSceneNode(irr::scene::ILightSceneNode* realSceneNode)
		: ISceneNode(realSceneNode)
	{
	}

	Video::Light ILightSceneNode::get_LightData()
	{
		return irr::NativeConverter::getNETLight(getLightSceneNode()->getLightData());
	}

	void ILightSceneNode::set_LightData(Video::Light light)
	{
		getLightSceneNode()->getLightData() = irr::NativeConverter::getNativeLight(light);
	}


}
}