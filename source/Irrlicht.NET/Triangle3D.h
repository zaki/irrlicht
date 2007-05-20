// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#include "Vector3D.h"
#include "Line3D.h"
#include "Plane3D.h"
#include "Box3D.h"

namespace Irrlicht
{
namespace Core
{
	/// <summary>
	/// 3D triangle class for doing collision detection and other things.
	/// This class has been ported directly from the native C++ Irrlicht Engine, so it may not 
	/// be 100% complete yet and the design may not be 100% .NET like.
	/// </summary>
	public __value class Triangle3D
	{
	public:
		
		/// <summary>
		/// Determinates if the triangle is totally inside a bounding box.
		/// </summary>
		/// <param name="box"> Box to check.</param>
		/// <returns> Returns true if the triangle is withing the box
		/// and false if it is not.</returns>
		bool IsTotalInsideBox(Box3D box)
		{
			return (box.IsPointInside(pointA) && 
				    box.IsPointInside(pointB) &&
					box.IsPointInside(pointC));
		}

		/// <summary>
		/// Returns the closest point on a triangle to a point on the same plane.
		/// <summary>
		/// <param name="p" Point which must be on the same plane as the triangle.</param>
		Vector3D ClosestPointOnTriangle(Vector3D p)
		{
			Vector3D rab = Line3D(pointA, pointB).GetClosestPoint(p);
			Vector3D rbc = Line3D(pointB, pointC).GetClosestPoint(p);
			Vector3D rca = Line3D(pointC, pointA).GetClosestPoint(p);

			float d1 = (float)rab.GetDistanceFrom(p);
			float d2 = (float)rbc.GetDistanceFrom(p);
			float d3 = (float)rca.GetDistanceFrom(p);

			if (d1 < d2)
				return d1 < d3 ? rab : rca;
            
			return d2 < d3 ? rbc : rca;
		}

		/// <summary>
		/// Returns if a point is inside the triangle
		/// </summary>
		/// <param name="p"> Point to test. Assumes that this point is already on the plane
		/// of the triangle.</param>
		/// <returns> Returns true if the point is inside the triangle, otherwise false.</returns>
		bool IsPointInside(Vector3D p)
		{
			return (IsOnSameSide(p, pointA, pointB, pointC) &&
				IsOnSameSide(p, pointB, pointA, pointC) &&
				IsOnSameSide(p, pointC, pointA, pointB));
		}

		/// <summary>
		/// Returns if a point is inside the triangle. This method is an implementation
		/// of the example used in a paper by Kasper Fauerby original written
		/// by Keidy from Mr-Gamemaker.
		/// </summary>
		/// <param name="p"> Point to test. Assumes that this point is already on the plane
		/// of the triangle.</param>
		/// <returns> Returns true if the point is inside the triangle, otherwise false.</returns>
		bool IsPointInsideFast(Vector3D p)
		{
			Vector3D f = pointB - pointA;
			Vector3D g = pointC - pointA;

			float a = f.DotProduct(f);
			float b = f.DotProduct(g);
			float c = g.DotProduct(g);

            float ac_bb = (a*c)-(b*b);
			Vector3D vp = p - pointA;

			float d = vp.DotProduct(f);
			float e = vp.DotProduct(g);
			float x = (d*c)-(e*b);
			float y = (e*a)-(d*b);
			float z = x+y-ac_bb;

			return (( ((unsigned int&)z)& ~(((unsigned int&)x)|((unsigned int&)y))) & 0x80000000)!=0;
		}


		bool IsOnSameSide(Vector3D p1, Vector3D p2, 
			Vector3D a, Vector3D b) 
		{
			Vector3D bminusa = b - a;
			Vector3D cp1 = bminusa.CrossProduct(p1 - a);
			Vector3D cp2 = bminusa.CrossProduct(p2 - a);
			return (cp1.DotProduct(cp2) > 0.0f);
		}


		/// <summary>
		/// Returns an intersection with a 3d line.
		/// </summary>
		/// <param name="line"> Line to intersect with.</name>
		/// <param name="outIntersection"> Place to store the intersection point, if there is one.</name>
		/// <returns> Returns true if there was an intersection, false if there was not.</returns>
		bool GetIntersectionWithLimitedLine(Line3D line, Vector3D outIntersection)
		{
			return GetIntersectionWithLine(line.start,
				line.GetVector(), outIntersection) &&
				outIntersection.IsBetweenPoints(line.start, line.end);
		}


		/// <summary>
		/// Returns an intersection with a 3d line.
		/// Please note that also points are returned as intersection, which
		/// are on the line, but not between the start and end point of the line.
		/// If you want the returned point be between start and end, please
		/// use getIntersectionWithLimitedLine().
		/// </summary>
		/// <param name="lineVect"> Vector of the line to intersect with.</param>
		/// <param name="linePoint"> Point of the line to intersect with.</name>
		/// <param name="outIntersection"> Place to store the intersection point, if there is one.</name>
		/// <returns> Returns true if there was an intersection, false if there was not.</returns>
		bool GetIntersectionWithLine(Vector3D linePoint,
			Vector3D lineVect, Vector3D outIntersection)
		{
			if (GetIntersectionOfPlaneWithLine(linePoint, lineVect, outIntersection))
				return IsPointInside(outIntersection);

			return false;			
		}


		/// <summary>
		/// Calculates the intersection between a 3d line and 
		/// the plane the triangle is on.
		/// </summary>
		/// <param name="lineVect"> Vector of the line to intersect with.</param>
		/// <param name="linePoint"> Point of the line to intersect with.</param>
		/// <param name="outIntersection"> Place to store the intersection point, if there is one.</param>
		/// <return> Returns true if there was an intersection, false if there was not.</returns>
		bool GetIntersectionOfPlaneWithLine(Vector3D linePoint,
			Vector3D lineVect, Vector3D outIntersection)
		{
			Vector3D normal = GetNormal();
			float t2 = normal.DotProduct(lineVect);
            
			if (t2 == 0.0f)
				return false;

            float d = pointA.DotProduct(normal);
			float t =- (normal.DotProduct(linePoint) - d) / t2;
			outIntersection = linePoint + (lineVect * t);
			return true;
		}

		
		/// <summary>
		/// Returns the normal of the triangle.
		/// Please note: The normal is not normalized.
		/// </summary>
		Vector3D GetNormal()
		{
			return (pointB - pointA).CrossProduct(pointC - pointA);
		}

		/// <summary>
		/// Returns if the triangle is front of backfacing.
		/// </summary>
		/// <param name="lookDirection"> Look direction.</param>
		/// <returns> Returns true if the plane is front facing, which mean it would
		/// be visible, and false if it is backfacing.</returns>
		bool IsFrontFacting(Vector3D lookDirection)
		{
			Vector3D n = GetNormal();
			n.Normalize();
			return n.DotProduct(lookDirection) <= 0.0f;
		}

		/// <summary>
		/// Returns the plane of this triangle.
		/// </summary>
		Plane3D GetPlane()
		{
			return Plane3D(pointA, pointB, pointC);
		}

		void Set(Vector3D a, Vector3D b, Vector3D c)
		{
			pointA = a;
			pointB = b;
			pointC = c;
		}

		/// the three points of the triangle
		Vector3D pointA; 
		Vector3D pointB; 
		Vector3D pointC; 
	};

}
}