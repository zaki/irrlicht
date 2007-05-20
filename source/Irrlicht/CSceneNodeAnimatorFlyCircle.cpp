// Copyright (C) 2002-2007 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CSceneNodeAnimatorFlyCircle.h"

namespace irr
{
namespace scene
{

//! constructor
CSceneNodeAnimatorFlyCircle::CSceneNodeAnimatorFlyCircle(u32 time, const core::vector3df& center, f32 radius, f32 speed, const core::vector3df& direction)
: Center(center), Direction(direction), Radius(radius), Speed(speed), StartTime(time)
{
	#ifdef _DEBUG
	setDebugName("CSceneNodeAnimatorFlyCircle");
	#endif
	Direction.normalize();
}



//! destructor
CSceneNodeAnimatorFlyCircle::~CSceneNodeAnimatorFlyCircle()
{
}



//! animates a scene node
void CSceneNodeAnimatorFlyCircle::animateNode(ISceneNode* node, u32 timeMs)
{
	if ( 0 == node )
		return;

	const f32 t = (timeMs-StartTime) * Speed;

	core::vector3df circle(Radius * sinf(t), 0, Radius * cosf(t));
	circle = circle.crossProduct ( Direction );

	node->setPosition(Center + circle);
}


//! Writes attributes of the scene node animator.
void CSceneNodeAnimatorFlyCircle::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options)
{
	out->addVector3d("Center", Center);
	out->addFloat("Radius", Radius);
	out->addFloat("Speed", Speed);
	out->addVector3d("Direction", Direction);
}


//! Reads attributes of the scene node animator.
void CSceneNodeAnimatorFlyCircle::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options)
{
	Center = in->getAttributeAsVector3d("Center");
	Radius = in->getAttributeAsFloat("Radius");
	Speed = in->getAttributeAsFloat("Speed");
	Direction = in->getAttributeAsVector3d("Direction");
	
	if (Direction.equals(core::vector3df(0,0,0)))
		Direction.set(0,1,0); // irrlicht 1.1 backwards compatibility
	else
		Direction.normalize();
}



} // end namespace scene
} // end namespace irr

