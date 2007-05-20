// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#using <mscorlib.dll>
using namespace System;

#pragma unmanaged
#include "..\\..\\include\\irrlicht.h"
#pragma managed

#include "Event.h"
#include "ISceneNode.h"
#include "ViewFrustum.h"

namespace Irrlicht
{
namespace Scene
{

public __gc class ICameraSceneNode : public ISceneNode
{
public:

	/// <summary>
	/// Creates a scene node from a native C++ scene node. Don't use this, its better to create 
	/// scene nodes using the SceneManager with its addSceneNode() methods.
	/// </summary>
	ICameraSceneNode(irr::scene::ICameraSceneNode* realSceneNode);

	/// Destructor
	~ICameraSceneNode();

	/// <summary>
	/// Sets or gets the projection matrix of the camera. The core::matrix4 class has some methods
	/// to build a projection matrix. e.g: core::matrix4::buildProjectionMatrixPerspectiveFovLH.
	/// Note that the matrix will only stay as set by this method until one of 
	/// the following Methods are called: setNearValue, setFarValue, setAspectRatio, setFOV.
	/// </summary>
	/// <param name="projection"> The new projection matrix of the camera. </param>
	__property void set_ProjectionMatrix(Core::Matrix4 projection);

	/// <summary>
	/// Sets or gets the projection matrix of the camera. The core::matrix4 class has some methods
	/// to build a projection matrix. e.g: core::matrix4::buildProjectionMatrixPerspectiveFovLH.
	/// Note that the matrix will only stay as set by this method until one of 
	/// the following Methods are called: setNearValue, setFarValue, setAspectRatio, setFOV.
	/// </summary>
	/// <param name="projection"> The new projection matrix of the camera. </param>	
	__property Core::Matrix4 get_ProjectionMatrix();

	/// Gets the current view matrix of the camera.
	/// <returns> Returns the current view matrix of the camera.</returns>
	__property Core::Matrix4 get_ViewMatrix();

	/// <summary>
	/// It is possible to send mouse and key events to the camera. Most cameras
	/// may ignore this input, but camera scene nodes which are created for 
	/// example with ISceneManager::addMayaCameraSceneNode or
	/// ISceneManager::addMeshViewerCameraSceneNode, may want to get this input
	/// for changing their position, look at target or whatever. 
	/// </summary>
	bool OnEvent(Event event);

	/// <summary>
	/// Sets or gets the look at target of the camera
	/// </summary>
	__property void set_Target(Core::Vector3D pos);

	/// <summary>
	/// Sets or gets the look at target of the camera
	/// </summary>	
	__property Core::Vector3D get_Target();

	/// <summary>
	/// Sets or gets the up vector of the camera.
	/// </summary>
	__property void set_UpVector(Core::Vector3D pos);

	/// <summary>
	/// Sets or gets the up vector of the camera.
	/// </summary>
	__property Core::Vector3D get_UpVector();

	/// <summary>
	/// Sets or gets the value of the near plane of the camera.
	/// </summary>
	__property float get_NearValue();

	/// <summary>
	/// Sets or gets the value of the near plane of the camera.
	/// </summary>
	__property void set_NearValue(float value);

	/// <summary>
	/// Sets or gets the value of the far plane of the camera.
	/// </summary>
	__property float get_FarValue();

	/// <summary>
	/// Sets or gets the value of the far plane of the camera.
	/// </summary>
	__property void set_FarValue(float value);

	/// <summary>
	/// Sets or gets the value of the aspect ratio of the camera.
	/// </summary>
	__property float get_AspectRatio();

	/// <summary>
	/// Sets or gets the value of the aspect ratio of the camera.
	/// </summary>
	__property void set_AspectRatio(float f);

	/// <summary>
	/// Sets or gets the value of the field of view of the camera.
	/// </summary>
	__property float get_FOV();

	/// <summary>
	/// Sets or gets the value of the field of view of the camera.
	/// </summary>
	__property void set_FOV(float value);

	/// <summary>
	/// Returns the current view frustum. Needed sometimes by bspTree or LOD render nodes.
	/// </summary>
	__property ViewFrustum get_ViewFrustum();

	/// <summary>
	/// Disables or enables the camera to get key or mouse inputs.
	/// If this is set to true, the camera will respond to key inputs
	/// otherwise not.
	/// </summary>
	__property void set_InputReceiverEnabled(bool enabled);

	/// <summary>
	/// Disables or enables the camera to get key or mouse inputs.
	/// If this is set to true, the camera will respond to key inputs
	/// otherwise not.
	/// </summary>
	__property bool get_InputReceiverEnabled();

	inline irr::scene::ICameraSceneNode* get_NativeCameraSceneNode()
	{
		return (irr::scene::ICameraSceneNode*)SceneNode;
	}

	/// <summary>
	/// Returns if a camera is orthogonal.
	/// This setting does not change anything of the view or projection matrix. However
	///	it influences how collision detection and picking is done with this camera. */
	/// </summary>
	__property void set_IsOrthogonal(bool enabled);

	/// <summary>
	/// Returns if a camera is orthogonal.
	/// This setting does not change anything of the view or projection matrix. However
	///	it influences how collision detection and picking is done with this camera. */
	/// </summary>
	__property bool get_IsOrthogonal();

protected:

};


}
}