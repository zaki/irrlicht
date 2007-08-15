// Copyright (C) 2002-2007 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_LIGHT_SCENE_NODE_H_INCLUDED__
#define __I_LIGHT_SCENE_NODE_H_INCLUDED__

#include "ISceneNode.h"
#include "SLight.h"

namespace irr
{
namespace scene
{

//! Scene node which is a dynamic light. 
/** You can switch the light on and off by 
making it visible or not, and let it be animated by ordinary scene node animators.
If you set the light type to be directional, you will need to set the direction of the
light source manually in the SLight structure, the position of the scene node will have no
effect on this direction.
*/
class ILightSceneNode : public ISceneNode
{
public:

	//! constructor
	ILightSceneNode(ISceneNode* parent, ISceneManager* mgr, s32 id, 
		const core::vector3df& position = core::vector3df(0,0,0))
		: ISceneNode(parent, mgr, id, position) {}

	//! Sets the light data associated with this ILightSceneNode
	virtual void setLightData(const video::SLight& light) = 0;

	//! Gets the light data associated with this ILightSceneNode
	//! \return Returns the light data.
	virtual const video::SLight& getLightData() const = 0;

	//! Gets the light data associated with this ILightSceneNode
	//! \return Returns the light data.
	virtual video::SLight& getLightData() = 0;
};

} // end namespace scene
} // end namespace irr


#endif

