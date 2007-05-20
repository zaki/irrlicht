// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "ICameraSceneNode.h"
#include "NativeConverter.h"

namespace Irrlicht
{
namespace Scene
{

	ICameraSceneNode::ICameraSceneNode(irr::scene::ICameraSceneNode* realSceneNode)
		: ISceneNode(realSceneNode)
	{
	}

	ICameraSceneNode::~ICameraSceneNode()
	{
	}

	void ICameraSceneNode::set_ProjectionMatrix(Core::Matrix4 projection)
	{
		get_NativeCameraSceneNode()->setProjectionMatrix(
			irr::NativeConverter::getNativeMatrix(projection));
	}

	Core::Matrix4 ICameraSceneNode::get_ProjectionMatrix()
	{
		return irr::NativeConverter::getNETMatrix(
			get_NativeCameraSceneNode()->getProjectionMatrix());
	}

	Core::Matrix4 ICameraSceneNode::get_ViewMatrix()
	{
		return irr::NativeConverter::getNETMatrix(
			get_NativeCameraSceneNode()->getViewMatrix());
	}

	bool ICameraSceneNode::OnEvent(Event event)
	{
		return get_NativeCameraSceneNode()->OnEvent(irr::NativeConverter::getNativeEvent(event));
	}

	void ICameraSceneNode::set_Target(Core::Vector3D pos)
	{
		get_NativeCameraSceneNode()->setTarget(irr::NativeConverter::getNativeVector(pos));
	}

	Core::Vector3D ICameraSceneNode::get_Target()
	{
		return irr::NativeConverter::getNETVector(get_NativeCameraSceneNode()->getTarget());
	}

	void ICameraSceneNode::set_UpVector(Core::Vector3D pos)
	{
		get_NativeCameraSceneNode()->setUpVector(irr::NativeConverter::getNativeVector(pos));
	}

	Core::Vector3D ICameraSceneNode::get_UpVector()
	{
		return irr::NativeConverter::getNETVector(get_NativeCameraSceneNode()->getUpVector());
	}

	float ICameraSceneNode::get_NearValue()
	{
		return get_NativeCameraSceneNode()->getNearValue();
	}

	void ICameraSceneNode::set_NearValue(float value)
	{
		get_NativeCameraSceneNode()->setNearValue(value);
	}

	float ICameraSceneNode::get_FarValue()
	{
		return get_NativeCameraSceneNode()->getFarValue();
	}

	void ICameraSceneNode::set_FarValue(float value)
	{
		get_NativeCameraSceneNode()->setFarValue(value);
	}

	float ICameraSceneNode::get_AspectRatio()
	{
		return get_NativeCameraSceneNode()->getFarValue();
	}

	void ICameraSceneNode::set_AspectRatio(float f)
	{
		get_NativeCameraSceneNode()->setAspectRatio(f);
	}

	float ICameraSceneNode::get_FOV()
	{
		return get_NativeCameraSceneNode()->getFOV();
	}

	void ICameraSceneNode::set_FOV(float value)
	{
		get_NativeCameraSceneNode()->setFOV(value);
	}

	ViewFrustum ICameraSceneNode::get_ViewFrustum()
	{
		return irr::NativeConverter::getNETFrustum(
			*get_NativeCameraSceneNode()->getViewFrustum());
	}

	void ICameraSceneNode::set_InputReceiverEnabled(bool enabled)
	{
		get_NativeCameraSceneNode()->setInputReceiverEnabled(enabled);
	}

	bool ICameraSceneNode::get_InputReceiverEnabled()
	{
		return get_NativeCameraSceneNode()->isInputReceiverEnabled();
	}

	ViewFrustum::ViewFrustum( const irr::scene::SViewFrustum& orig )
	{
		CameraPosition = irr::NativeConverter::getNETVector( orig.cameraPosition );

		Planes = __gc new Irrlicht::Core::Plane3D[6];
		for ( int i=0; i<6; ++i )
		{
			//Planes[i] = __gc new Irrlicht::Core::Plane3D();
			Planes[i].Normal = irr::NativeConverter::getNETVector( orig.planes[i].Normal );
			Planes[i].D = orig.planes[i].D;
		}
	}

	void ICameraSceneNode::set_IsOrthogonal(bool enabled)
	{
		get_NativeCameraSceneNode()->setIsOrthogonal(enabled);
	}

	bool ICameraSceneNode::get_IsOrthogonal()
	{
		return get_NativeCameraSceneNode()->isOrthogonal();
	}
}
}