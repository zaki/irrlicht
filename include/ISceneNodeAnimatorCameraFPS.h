// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_SCENE_NODE_ANIMATOR_CAMERA_FPS_H_INCLUDED__
#define __I_SCENE_NODE_ANIMATOR_CAMERA_FPS_H_INCLUDED__

#include "ISceneNodeAnimator.h"
#include "IEventReceiver.h"

namespace irr
{
	struct SKeyMap;

namespace scene
{

	//! Special scene node animator for FPS cameras
	/** This scene node animator can be attached to a camera to make it act like a first
	person shooter 
	*/
	class ISceneNodeAnimatorCameraFPS : public ISceneNodeAnimator
	{
	public:

		//! Returns the speed of movement in units per millisecond
		virtual f32 getMoveSpeed() const = 0;

		//! Sets the speed of movement in units per millisecond
		virtual void setMoveSpeed(f32 moveSpeed) = 0;

		//! Returns the rotation speed
		virtual f32 getRotateSpeed() const = 0;

		//! Set the rotation speed
		virtual void setRotateSpeed(f32 rotateSpeed) = 0;

		//! Sets the keyboard mapping for this animator
		//! \param keymap: an array of keyboard mappings, see SKeyMap
		//! \param count: the size of the keyboard map array
		virtual void setKeyMap(SKeyMap *map, u32 count) = 0;

		//! Sets whether vertical movement should be allowed.
		//! If vertical movement is enabled then the camera may fight with 
		//! gravity causing camera shake. Disable this if the camera has 
		//! a collision animator with gravity enabled.
		virtual void setVerticalMovement(bool allow) = 0;
	};
} // end namespace scene
} // end namespace irr

#endif

