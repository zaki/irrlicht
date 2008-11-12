// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CSceneNodeAnimatorCameraFPS.h"
#include "IVideoDriver.h"
#include "ISceneManager.h"
#include "Keycodes.h"
#include "ICursorControl.h"
#include "ICameraSceneNode.h"

namespace irr
{
namespace scene
{

//! constructor
CSceneNodeAnimatorCameraFPS::CSceneNodeAnimatorCameraFPS(gui::ICursorControl* cursorControl,
		f32 rotateSpeed, f32 moveSpeed, f32 jumpSpeed,
		SKeyMap* keyMapArray, u32 keyMapSize, bool noVerticalMovement)
: CursorControl(cursorControl), MaxVerticalAngle(88.0f),
	MoveSpeed(moveSpeed/1000.0f), RotateSpeed(rotateSpeed), JumpSpeed(jumpSpeed),
	LastAnimationTime(0), firstUpdate(true), NoVerticalMovement(noVerticalMovement),
	KeyMapArray(keyMapArray), KeyMapSize(keyMapSize)
{
	#ifdef _DEBUG
	setDebugName("CCameraSceneNodeAnimatorFPS");
	#endif

	if (CursorControl)
		CursorControl->grab();

	allKeysUp();

	// create key map
	if (!KeyMapArray || !KeyMapSize)
	{
		// create default key map
		KeyMap.push_back(SCamKeyMap(0, irr::KEY_UP));
		KeyMap.push_back(SCamKeyMap(1, irr::KEY_DOWN));
		KeyMap.push_back(SCamKeyMap(2, irr::KEY_LEFT));
		KeyMap.push_back(SCamKeyMap(3, irr::KEY_RIGHT));
		KeyMap.push_back(SCamKeyMap(4, irr::KEY_KEY_J));
	}
	else
	{
		// create custom key map
		setKeyMap(KeyMapArray, KeyMapSize);
	}
}


//! destructor
CSceneNodeAnimatorCameraFPS::~CSceneNodeAnimatorCameraFPS()
{
	if (CursorControl)
		CursorControl->drop();
}


//! It is possible to send mouse and key events to the camera. Most cameras
//! may ignore this input, but camera scene nodes which are created for
//! example with scene::ISceneManager::addMayaCameraSceneNode or
//! scene::ISceneManager::addFPSCameraSceneNode, may want to get this input
//! for changing their position, look at target or whatever.
bool CSceneNodeAnimatorCameraFPS::OnEvent(const SEvent& evt)
{
	switch(evt.EventType)
	{
	case EET_KEY_INPUT_EVENT:
		for (u32 i=0; i<KeyMap.size(); ++i)
		{
			if (KeyMap[i].keycode == evt.KeyInput.Key)
			{
				CursorKeys[KeyMap[i].action] = evt.KeyInput.PressedDown;
				return true;
			}
		}
		break;

	case EET_MOUSE_INPUT_EVENT:
		if (evt.MouseInput.Event == EMIE_MOUSE_MOVED)
		{
			CursorPos = CursorControl->getRelativePosition();
			return true;
		}
		break;

	default:
		break;
	}

	return false;
}


void CSceneNodeAnimatorCameraFPS::animateNode(ISceneNode* node, u32 timeMs)
{
	if (node->getType() != ESNT_CAMERA)
		return;

	ICameraSceneNode* camera = static_cast<ICameraSceneNode*>(node);

	if (firstUpdate)
	{
		if (CursorControl && camera)
		{
			CursorControl->setPosition(0.5f, 0.5f);
			CursorPos = CenterCursor = CursorControl->getRelativePosition();
		}

		LastAnimationTime = timeMs;

		firstUpdate = false;
	}

	// get time
	f32 timeDiff = (f32) ( timeMs - LastAnimationTime );
	LastAnimationTime = timeMs;

	// update position
	core::vector3df pos = camera->getPosition();

	// Update rotation
	core::vector3df target = (camera->getTarget() - camera->getAbsolutePosition());
	core::vector3df relativeRotation = target.getHorizontalAngle();

	if (CursorControl)
	{
		if (CursorPos != CenterCursor)
		{
			relativeRotation.Y -= (0.5f - CursorPos.X) * RotateSpeed;
			relativeRotation.X -= (0.5f - CursorPos.Y) * RotateSpeed;

			// X < MaxVerticalAngle or X > 360-MaxVerticalAngle

			if (relativeRotation.X > MaxVerticalAngle*2 &&
				relativeRotation.X < 360.0f-MaxVerticalAngle)
			{
				relativeRotation.X = 360.0f-MaxVerticalAngle;
			}
			else
			if (relativeRotation.X > MaxVerticalAngle &&
				relativeRotation.X < 360.0f-MaxVerticalAngle)
			{
				relativeRotation.X = MaxVerticalAngle;
			}

			// reset cursor position
			CursorControl->setPosition(0.5f, 0.5f);
			CenterCursor = CursorControl->getRelativePosition();
			// needed to avoid problems when the ecent receiver is
			// disabled
			CursorPos = CenterCursor;
		}
	}

	// set target

	target.set(0,0,100);
	core::vector3df movedir = target;

	core::matrix4 mat;
	mat.setRotationDegrees(core::vector3df(relativeRotation.X, relativeRotation.Y, 0));
	mat.transformVect(target);

	if (NoVerticalMovement)
	{
		mat.setRotationDegrees(core::vector3df(0, relativeRotation.Y, 0));
		mat.transformVect(movedir);
	}
	else
	{
		movedir = target;
	}

	movedir.normalize();

	if (CursorKeys[0])
		pos += movedir * timeDiff * MoveSpeed;

	if (CursorKeys[1])
		pos -= movedir * timeDiff * MoveSpeed;

	// strafing

	core::vector3df strafevect = target;
	strafevect = strafevect.crossProduct(camera->getUpVector());

	if (NoVerticalMovement)
		strafevect.Y = 0.0f;

	strafevect.normalize();

	if (CursorKeys[2])
		pos += strafevect * timeDiff * MoveSpeed;

	if (CursorKeys[3])
		pos -= strafevect * timeDiff * MoveSpeed;

	// jumping ( need's a gravity , else it's a fly to the World-UpVector )
	if (CursorKeys[4])
	{
		pos += camera->getUpVector() * timeDiff * JumpSpeed;
	}

	// write translation
	camera->setPosition(pos);

	// write right target

	TargetVector = target;
	target += pos;
	camera->setTarget(target);
}


void CSceneNodeAnimatorCameraFPS::allKeysUp()
{
	for (u32 i=0; i<6; ++i)
		CursorKeys[i] = false;
}


//! Sets the rotation speed
void CSceneNodeAnimatorCameraFPS::setRotateSpeed(f32 speed)
{
	RotateSpeed = speed;
}


//! Sets the movement speed
void CSceneNodeAnimatorCameraFPS::setMoveSpeed(f32 speed)
{
	MoveSpeed = speed;
}


//! Gets the rotation speed
f32 CSceneNodeAnimatorCameraFPS::getRotateSpeed() const
{
	return RotateSpeed;
}


// Gets the movement speed
f32 CSceneNodeAnimatorCameraFPS::getMoveSpeed() const
{
	return MoveSpeed;
}


//! Sets the keyboard mapping for this animator
void CSceneNodeAnimatorCameraFPS::setKeyMap(SKeyMap *map, u32 count)
{
	// clear the keymap
	KeyMap.clear();

	// add actions
	for (u32 i=0; i<count; ++i)
	{
		switch(map[i].Action)
		{
		case EKA_MOVE_FORWARD: KeyMap.push_back(SCamKeyMap(0, map[i].KeyCode));
			break;
		case EKA_MOVE_BACKWARD: KeyMap.push_back(SCamKeyMap(1, map[i].KeyCode));
			break;
		case EKA_STRAFE_LEFT: KeyMap.push_back(SCamKeyMap(2, map[i].KeyCode));
			break;
		case EKA_STRAFE_RIGHT: KeyMap.push_back(SCamKeyMap(3, map[i].KeyCode));
			break;
		case EKA_JUMP_UP: KeyMap.push_back(SCamKeyMap(4, map[i].KeyCode));
			break;
		default:
			break;
		}
	}
}

//! Sets whether vertical movement should be allowed.
void CSceneNodeAnimatorCameraFPS::setVerticalMovement(bool allow)
{
	NoVerticalMovement = !allow;
}

ISceneNodeAnimator* CSceneNodeAnimatorCameraFPS::createClone(ISceneNode* node, ISceneManager* newManager)
{
	CSceneNodeAnimatorCameraFPS * newAnimator = 
		new CSceneNodeAnimatorCameraFPS(CursorControl,	RotateSpeed, (MoveSpeed * 1000.0f), JumpSpeed,
											KeyMapArray, KeyMapSize, NoVerticalMovement);
	return newAnimator;
}

} // namespace scene
} // namespace irr

