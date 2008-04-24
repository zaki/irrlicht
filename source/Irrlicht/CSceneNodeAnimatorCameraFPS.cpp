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

const f32 MAX_VERTICAL_ANGLE = 88.0f;


//! constructor
CSceneNodeAnimatorCameraFPS::CSceneNodeAnimatorCameraFPS(gui::ICursorControl* cursorControl, f32 rotateSpeed,
		f32 moveSpeed, f32 jumpSpeed, SKeyMap* keyMapArray, s32 keyMapSize, bool noVerticalMovement)
: CursorControl(cursorControl),	MoveSpeed(moveSpeed), RotateSpeed(rotateSpeed), JumpSpeed(jumpSpeed),
	firstUpdate(true), LastAnimationTime(0), NoVerticalMovement(noVerticalMovement)
{
	#ifdef _DEBUG
	setDebugName("CCameraSceneNodeAnimatorFPS");
	#endif

	if (CursorControl)
		CursorControl->grab();

	MoveSpeed /= 1000.0f;

	allKeysUp();

	// create key map
	if (!keyMapArray || !keyMapSize)
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
		setKeyMap(keyMapArray, keyMapSize);
	}// end if
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
			if (KeyMap[i].keycode == evt.KeyInput.Key)
			{
				CursorKeys[KeyMap[i].action] = evt.KeyInput.PressedDown;
				return true;
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
	ICameraSceneNode* camera = 0;

	if (node->getType() != ESNT_CAMERA)
		return;

	camera = static_cast<ICameraSceneNode*>(node);

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
	f32 timeDiff = 0.f;

	timeDiff = (f32) ( timeMs - LastAnimationTime );
	LastAnimationTime = timeMs;

	// update position
	core::vector3df pos = camera->getPosition();

	// Update rotation
	core::vector3df Target = (camera->getTarget() - camera->getAbsolutePosition());
	core::vector3df RelativeRotation = Target.getHorizontalAngle();

	if (CursorControl)
	{
		if (CursorPos != CenterCursor)
		{
			RelativeRotation.Y -= (0.5f - CursorPos.X) * RotateSpeed;
			RelativeRotation.X -= (0.5f - CursorPos.Y) * RotateSpeed;

			// X < MAX_VERTICAL_ANGLE or X > 360-MAX_VERTICAL_ANGLE

			if (RelativeRotation.X > MAX_VERTICAL_ANGLE*2 &&
				RelativeRotation.X < 360.0f-MAX_VERTICAL_ANGLE)
			{
				RelativeRotation.X = 360.0f-MAX_VERTICAL_ANGLE;
			}
			else
			if (RelativeRotation.X > MAX_VERTICAL_ANGLE &&
				RelativeRotation.X < 360.0f-MAX_VERTICAL_ANGLE)
			{
				RelativeRotation.X = MAX_VERTICAL_ANGLE;
			}

			// reset cursor position
			CursorControl->setPosition(0.5f, 0.5f);
			CenterCursor = CursorControl->getRelativePosition();
		}

	}

	// set target

	Target.set(0,0,1);

	core::matrix4 mat;
	mat.setRotationDegrees(core::vector3df( RelativeRotation.X, RelativeRotation.Y, 0));
	mat.transformVect(Target);

	core::vector3df movedir = Target;

	if (NoVerticalMovement)
		movedir.Y = 0.f;

	movedir.normalize();

	if (CursorKeys[0])
		pos += movedir * timeDiff * MoveSpeed;

	if (CursorKeys[1])
		pos -= movedir * timeDiff * MoveSpeed;

	// strafing

	core::vector3df strafevect = Target;
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

	TargetVector = Target;
	Target += pos;
	camera->setTarget(Target);

}

void CSceneNodeAnimatorCameraFPS::allKeysUp()
{
	for (s32 i=0; i<6; ++i)
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



} // namespace scene
} // namespace irr
