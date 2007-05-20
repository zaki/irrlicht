// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#using <mscorlib.dll>
using namespace System;
using namespace System::Runtime::InteropServices;


namespace Irrlicht
{
namespace Core
{
	/// <summary>
	/// Enumeration for intersection relations of 3d objects
	/// </summary>
	public __value enum IntersectionRelation3D 
	{
		ISREL3D_FRONT = 0,
		ISREL3D_BACK,
		ISREL3D_PLANAR,
		ISREL3D_SPANNING,
		ISREL3D_CLIPPED
	};

	/// <summary> 
	/// Specifies a two dimensional size. 
	/// </summary>
	public __value class Plane3D
	{
	public:

		// Constructors

		Plane3D(): Normal(0,1,0) { RecalculateD(Vector3D(0,0,0)); };
		Plane3D(Vector3D MPoint, Vector3D Normal) : Normal(Normal) { RecalculateD(MPoint); };
		Plane3D(float px, float py, float pz, float nx, float ny, float nz) : Normal(nx, ny, nz) { RecalculateD(Vector3D(px, py, pz)); };
		Plane3D(Vector3D point1, Vector3D point2, Vector3D point3)	{ SetPlane(point1, point2, point3);	};

		void SetPlane(Vector3D point, Vector3D nvector)
		{
			Normal = nvector;
			Normal.Normalize();
			RecalculateD(point);
		}

		void SetPlane(Vector3D nvect, float d)
		{
			Normal = nvect;
			D = d;
		}

		void SetPlane(Vector3D point1, Vector3D point2, Vector3D point3)
		{
			// creates the plane from 3 memberpoints
			Normal = (point2 - point1).CrossProduct(point3 - point1);
			Normal.Normalize();

			RecalculateD(point1);
		}


		/// <summary>
		/// Returns an intersection with a 3d line.
		/// </summary>
		/// <param name="lineVect"> Vector of the line to intersect with.</param>
		/// <param name="linePoint"> Point of the line to intersect with.</param>
		/// <param name="outIntersection"> Place to store the intersection point, if there is one.</param>
		/// <returns> Returns true if there was an intersection, false if there was not.
		bool GetIntersectionWithLine(Vector3D linePoint, Vector3D lineVect,
					[PARAMFLAG::Out] Vector3D& outIntersection)
		{
			float t2 = Normal.DotProduct(lineVect);

			if (t2 == 0)
				return false;

			float t =- (Normal.DotProduct(linePoint) + D) / t2;			
			outIntersection = linePoint + (lineVect * t);
			return true;
		}

		/// <summary>
		/// Returns an intersection with a 3d line, limited between two 3d points.
		/// </summary>
		/// <param name="linePoint1"> Point 1 of the line.</param>
		/// <param name="linePoint2"> Point 2 of the line.</param>
		/// <param name="outIntersection"> Place to store the intersection point, if there is one.</param>
		/// <returns> Returns true if there was an intersection, false if there was not.</returns>
		bool GetIntersectionWithLimitedLine(Vector3D linePoint1, 
					Vector3D linePoint2, [PARAMFLAG::Out] Vector3D& outIntersection)
		{
			return (	GetIntersectionWithLine(linePoint1, linePoint2 - linePoint1, outIntersection) &&
						outIntersection.IsBetweenPoints(linePoint1, linePoint2));
		}

		/// <summary>
		/// Classifies the relation of a point to this plane.
		/// </summary>
		/// <param name="point"> Point to classify its relation.</param>
		/// <returns> Returns ISREL3D_FRONT if the point is in front of the plane,
		/// ISREL3D_BACK if the point is behind of the plane, and
		/// ISREL3D_PLANAR if the point is within the plane.</returns>
		IntersectionRelation3D ClassifyPointRelation(Vector3D point) 
		{
			float d = Normal.DotProduct(point) + D;

			if (d < -ROUNDING_ERROR)
				return ISREL3D_FRONT;

			if (d > ROUNDING_ERROR)
				return ISREL3D_BACK;

			return ISREL3D_PLANAR;
		}

		/// <summary>
		/// Recalculates the distance from origin by applying
		/// a new member point to the plane.
		/// </summary>
		void RecalculateD(Vector3D MPoint)
		{
			D = - MPoint.DotProduct(Normal);
		}

		/// <summary>
		/// Returns a member point of the plane.
		/// </summary>
		Vector3D GetMemberPoint()
		{
			return Normal * -D;
		}

		/// <summary>
		/// Tests if there is a intersection between this plane and another
		/// </summary>
		/// <returns> Returns true if there is a intersection.</returns>
		bool ExistsInterSection(Plane3D other)
		{
			Vector3D cross = other.Normal.CrossProduct(Normal);
			return cross.GetLength() > 1e-08f;
		}

		/// <summary>
		/// Intersects this plane with another.
		/// </summary>
		/// <returns> Returns true if there is a intersection, false if not.</returns>
		bool GetIntersectionWithPlane(Plane3D other, [PARAMFLAG::Out] Vector3D& outLinePoint,
				[PARAMFLAG::Out] Vector3D& outLineVect) 
		{
			double fn00 = Normal.GetLength();
			double fn01 = Normal.DotProduct(other.Normal);
			double fn11 = other.Normal.GetLength();
			double det = fn00*fn11 - fn01*fn01;

			if (mfabs((float)det) < 1e-08f)
				return false;

            det = 1.0 / det;
			double fc0 = (fn11*-D + fn01*other.D) * det;
			double fc1 = (fn00*-other.D + fn01*D) * det;

			outLineVect = Normal.CrossProduct(other.Normal);
			outLinePoint = Normal*(float)fc0 + other.Normal*(float)fc1;
			return true;
		}

		/// <summary>
		/// Returns the intersection point with two other planes if there is one.
		/// </summary>
		bool GetIntersectionWithPlanes(Plane3D o1, 
				Plane3D o2, [PARAMFLAG::Out] Vector3D& outPoint)
		{
			Vector3D linePoint, lineVect;
			if (GetIntersectionWithPlane(o1, linePoint, lineVect))
				return o2.GetIntersectionWithLine(linePoint, lineVect, outPoint);

			return false;
		}

		/// <summary>
		/// Returns if the plane is front of backfacing. Note that this only
		/// works if the normal is Normalized.
		/// </summary>
		/// <param name="lookDirection"> Look direction.</param>
		/// <returns> Returns true if the plane is front facing, which mean it would
		/// be visible, and false if it is backfacing.</returns>
		bool IsFrontFacting(Vector3D lookDirection)
		{
			return Normal.DotProduct(lookDirection) <= 0.0f;
		}

		/// <summary>
		/// Returns the distance to a point.  Note that this only
		/// works if the normal is Normalized.
		/// </summary>
		float GetDistanceTo(Vector3D point)
		{
			return point.DotProduct(Normal) + D;
		}
	
		// member variables
		
		float D;					// distance from origin
		Vector3D Normal;		// normal vector
	};

}
}