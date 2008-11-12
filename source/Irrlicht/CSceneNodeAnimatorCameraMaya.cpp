// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CSceneNodeAnimatorCameraMaya.h"
#include "ICursorControl.h"
#include "ICameraSceneNode.h"
#include "SViewFrustum.h"

namespace irr
{
namespace scene
{

//! constructor
CSceneNodeAnimatorCameraMaya::CSceneNodeAnimatorCameraMaya(gui::ICursorControl* cursor, f32 rotate, f32 zoom, f32 translate)
 :	CursorControl(cursor), Zooming(false), Rotating(false), Moving(false), Translating(false),
	ZoomSpeed(zoom), RotateSpeed(rotate), TranslateSpeed(translate),
	RotateStartX(0.0f), RotateStartY(0.0f), ZoomStartX(0.0f), ZoomStartY(0.0f),
	TranslateStartX(0.0f), TranslateStartY(0.0f), CurrentZoom(70.0f), RotX(0.0f), RotY(0.0f), 
	Target(0,0,0), OldTarget(0,0,0), OldCamera(0), MousePos(0.5f, 0.5f)
{
	#ifdef _DEBUG
	setDebugName("CSceneNodeAnimatorCameraMaya");
	#endif

	if (CursorControl)
	{
		CursorControl->grab();
		MousePos = CursorControl->getRelativePosition();
	}

	allKeysUp();
}


//! destructor
CSceneNodeAnimatorCameraMaya::~CSceneNodeAnimatorCameraMaya()
{
	if (CursorControl)
		CursorControl->drop();
}


//! It is possible to send mouse and key events to the camera. Most cameras
//! may ignore this input, but camera scene nodes which are created for 
//! example with scene::ISceneManager::addMayaCameraSceneNode or
//! scene::ISceneManager::addMeshViewerCameraSceneNode, may want to get this input
//! for changing their position, look at target or whatever.
bool CSceneNodeAnimatorCameraMaya::OnEvent(const SEvent& event)
{
	if (event.EventType != EET_MOUSE_INPUT_EVENT)
		return false;

	switch(event.MouseInput.Event)
	{
	case EMIE_LMOUSE_PRESSED_DOWN:
		MouseKeys[0] = true;
		break;
	case EMIE_RMOUSE_PRESSED_DOWN:
		MouseKeys[2] = true;
		break;
	case EMIE_MMOUSE_PRESSED_DOWN:
		MouseKeys[1] = true;
		break;
	case EMIE_LMOUSE_LEFT_UP:
		MouseKeys[0] = false;
		break;
	case EMIE_RMOUSE_LEFT_UP:
		MouseKeys[2] = false;
		break;
	case EMIE_MMOUSE_LEFT_UP:
		MouseKeys[1] = false;
		break;
	case EMIE_MOUSE_MOVED:
		MousePos = CursorControl->getRelativePosition();
		break;
	case EMIE_MOUSE_WHEEL:
	case EMIE_COUNT:
		return false;
	}
	return true;
}


//! OnAnimate() is called just before rendering the whole scene.
//! nodes may calculate or store animations here, and may do other useful things,
//! dependent on what they are.
void CSceneNodeAnimatorCameraMaya::animateNode(ISceneNode *node, u32 timeMs)
{
	//Alt + LM = Rotate around camera pivot
	//Alt + LM + MM = Dolly forth/back in view direction (speed % distance camera pivot - max distance to pivot)
	//Alt + MM = Move on camera plane (Screen center is about the mouse pointer, depending on move speed)

	if (node->getType() != ESNT_CAMERA)
		return;

	ICameraSceneNode* camera = static_cast<ICameraSceneNode*>(node);

	if (OldCamera != camera)
	{
		OldTarget = camera->getTarget();
		OldCamera = camera;
	}

	Target = camera->getTarget();

	const SViewFrustum* va = camera->getViewFrustum();

	f32 nRotX = RotX;
	f32 nRotY = RotY;
	f32 nZoom = CurrentZoom;

	if ( (isMouseKeyDown(0) && isMouseKeyDown(2)) || isMouseKeyDown(1) )
	{
		if (!Zooming)
		{
			ZoomStartX = MousePos.X;
			ZoomStartY = MousePos.Y;
			Zooming = true;
			nZoom = CurrentZoom;
		}
		else
		{
			f32 old = nZoom;
			nZoom += (ZoomStartX - MousePos.X) * ZoomSpeed;

			f32 targetMinDistance = 0.1f;
			if (nZoom < targetMinDistance) // jox: fixed bug: bounce back when zooming to close
				nZoom = targetMinDistance;

			if (nZoom < 0)
				nZoom = old;
		}
	}
	else
	{
		if (Zooming)
		{
			f32 old = CurrentZoom;
			CurrentZoom = CurrentZoom + (ZoomStartX - MousePos.X ) * ZoomSpeed;
			nZoom = CurrentZoom;

			if (nZoom < 0)
				nZoom = CurrentZoom = old;
		}

		Zooming = false;
	}

	// Translation ---------------------------------

	core::vector3df translate(OldTarget), UpVector(camera->getUpVector());

	core::vector3df tvectX = Pos - Target;
	tvectX = tvectX.crossProduct(UpVector);
	tvectX.normalize();

	core::vector3df tvectY = (va->getFarLeftDown() - va->getFarRightDown());
	tvectY = tvectY.crossProduct(UpVector.Y > 0 ? Pos - Target : Target - Pos);
	tvectY.normalize();
	

	if (isMouseKeyDown(2) && !Zooming)
	{
		if (!Translating)
		{
			TranslateStartX = MousePos.X;
			TranslateStartY = MousePos.Y;
			Translating = true;
		}
		else
		{
			translate +=  tvectX * (TranslateStartX - MousePos.X)*TranslateSpeed + 
			              tvectY * (TranslateStartY - MousePos.Y)*TranslateSpeed;
		}
	}
	else
	{
		if (Translating)
		{
			translate += tvectX * (TranslateStartX - MousePos.X)*TranslateSpeed + 
			             tvectY * (TranslateStartY - MousePos.Y)*TranslateSpeed;
			OldTarget = translate;
		}

		Translating = false;
	}

	// Rotation ------------------------------------

	if (isMouseKeyDown(0) && !Zooming)
	{
		if (!Rotating)
		{
			RotateStartX = MousePos.X;
			RotateStartY = MousePos.Y;
			Rotating = true;
			nRotX = RotX;
			nRotY = RotY;
		}
		else
		{
			nRotX += (RotateStartX - MousePos.X) * RotateSpeed;
			nRotY += (RotateStartY - MousePos.Y) * RotateSpeed;
		}
	}
	else
	{
		if (Rotating)
		{
			RotX = RotX + (RotateStartX - MousePos.X) * RotateSpeed;
			RotY = RotY + (RotateStartY - MousePos.Y) * RotateSpeed;
			nRotX = RotX;
			nRotY = RotY;
		}

		Rotating = false;
	}

	// Set Pos ------------------------------------

	Target = translate;

	Pos.X = nZoom + Target.X;
	Pos.Y = Target.Y;
	Pos.Z = Target.Z;

	Pos.rotateXYBy(nRotY, Target);
	Pos.rotateXZBy(-nRotX, Target);

	// Rotation Error ----------------------------

	// jox: fixed bug: jitter when rotating to the top and bottom of y
	UpVector.set(0,1,0);
	UpVector.rotateXYBy(-nRotY);
	UpVector.rotateXZBy(-nRotX+180.f);

	camera->setPosition(Pos);
	camera->setTarget(Target);
	camera->setUpVector(UpVector);
}


bool CSceneNodeAnimatorCameraMaya::isMouseKeyDown(s32 key)
{
	return MouseKeys[key];
}


void CSceneNodeAnimatorCameraMaya::allKeysUp()
{
	for (s32 i=0; i<3; ++i)
		MouseKeys[i] = false;
}


// function added by jox
void CSceneNodeAnimatorCameraMaya::updateAnimationState() 
{
	core::vector3df pos(Pos - Target);

	// X rotation
	core::vector2df vec2d(pos.X, pos.Z);
	RotX = (f32)vec2d.getAngle();

	// Y rotation
	pos.rotateXZBy(RotX);
	vec2d.set(pos.X, pos.Y);
	RotY = -(f32)vec2d.getAngle();

	// Zoom
	CurrentZoom = (f32)Pos.getDistanceFrom(Target);
}


//! Sets the rotation speed
void CSceneNodeAnimatorCameraMaya::setRotateSpeed(f32 speed)
{
	RotateSpeed = speed;	
}


//! Sets the movement speed
void CSceneNodeAnimatorCameraMaya::setMoveSpeed(f32 speed)
{
	TranslateSpeed = speed;
}


//! Sets the zoom speed
void CSceneNodeAnimatorCameraMaya::setZoomSpeed(f32 speed)
{
	ZoomSpeed = speed;
}


//! Gets the rotation speed
f32 CSceneNodeAnimatorCameraMaya::getRotateSpeed() const
{
	return RotateSpeed;
}


// Gets the movement speed
f32 CSceneNodeAnimatorCameraMaya::getMoveSpeed() const
{
	return TranslateSpeed;
}


//! Gets the zoom speed
f32 CSceneNodeAnimatorCameraMaya::getZoomSpeed() const
{
	return ZoomSpeed;
}

ISceneNodeAnimator* CSceneNodeAnimatorCameraMaya::createClone(ISceneNode* node, ISceneManager* newManager)
{
	CSceneNodeAnimatorCameraMaya * newAnimator = 
		new CSceneNodeAnimatorCameraMaya(CursorControl, RotateSpeed, ZoomSpeed, TranslateSpeed);
	return newAnimator;
}

} // end namespace
} // end namespace

