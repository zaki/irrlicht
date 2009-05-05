// Copyright (C) 2002-2009 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CSceneNodeAnimatorRotation.h"

namespace irr
{
namespace scene
{


//! constructor
CSceneNodeAnimatorRotation::CSceneNodeAnimatorRotation(u32 time, const core::vector3df& rotation)
: Rotation(rotation), StartTime(time)
{
	#ifdef _DEBUG
	setDebugName("CSceneNodeAnimatorRotation");
	#endif
}


//! animates a scene node
void CSceneNodeAnimatorRotation::animateNode(ISceneNode* node, u32 timeMs)
{
	if (node) // thanks to warui for this fix
	{
		u32 diffTime = timeMs - StartTime;

		if (diffTime != 0)
		{
			node->setRotation(node->getRotation() + Rotation*(diffTime*0.1f));
			StartTime=timeMs; 
		}
	}
}


//! Writes attributes of the scene node animator.
void CSceneNodeAnimatorRotation::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options) const
{
	out->addVector3d("Rotation", Rotation);
}


//! Reads attributes of the scene node animator.
void CSceneNodeAnimatorRotation::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options)
{
	Rotation = in->getAttributeAsVector3d("Rotation");
}


ISceneNodeAnimator* CSceneNodeAnimatorRotation::createClone(ISceneNode* node, ISceneManager* newManager)
{
	CSceneNodeAnimatorRotation * newAnimator = 
		new CSceneNodeAnimatorRotation(StartTime, Rotation);

	return newAnimator;
}


} // end namespace scene
} // end namespace irr

