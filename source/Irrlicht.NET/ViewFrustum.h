// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#using <mscorlib.dll>
using namespace System;

#pragma unmanaged
#include "..\\..\\include\\irrlicht.h"
#pragma managed

namespace Irrlicht
{
namespace Scene
{
	

	/// <summary>
	/// Defines the view frustum. Thats the space viewed by the camera.
	/// The view frustum is enclosed by 6 planes. These six planes share
	/// four points. A bounding box around these four points is also stored in
	/// this structure.
	/// </summary>
	public __value class ViewFrustum
	{
	public:

		/// <summary>
		/// An enum for all types view frustum planes
		/// </summary>
		__value enum PlaneType
		{
			FarPlane = 0,
			NearPlane = 1,
			LeftPlane = 2,
			RightPlane = 3,
			BottomPlane = 4,
			TopPlane = 5,

			PlaneCount = 6
		};

		ViewFrustum( const irr::scene::SViewFrustum& orig );
		
		__property Core::Vector3D get_FarLeftUp()
		{
			Core::Vector3D f;

			Planes[PlaneType::FarPlane].GetIntersectionWithPlanes(
				Planes[PlaneType::TopPlane], Planes[PlaneType::LeftPlane], f );

			return f;
		}

		__property Core::Vector3D get_FarLeftDown()
		{
			Core::Vector3D f;

			Planes[PlaneType::FarPlane].GetIntersectionWithPlanes(
				Planes[PlaneType::BottomPlane], Planes[PlaneType::LeftPlane], f );

			return f;
		}

		__property Core::Vector3D get_FarRightUp()
		{
			Core::Vector3D f;

			Planes[PlaneType::FarPlane].GetIntersectionWithPlanes(
				Planes[PlaneType::TopPlane], Planes[PlaneType::RightPlane], f );

			return f;
		}

		__property Core::Vector3D get_FarRightDown()
		{
			Core::Vector3D f;

			Planes[PlaneType::FarPlane].GetIntersectionWithPlanes(
				Planes[PlaneType::BottomPlane], Planes[PlaneType::RightPlane], f );

			return f;
		}

	private:

		Core::Vector3D CameraPosition;
		Core::Plane3D  Planes[];
		Core::Box3D    BoundingBox;
	};

}
}