// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#using <mscorlib.dll>
using namespace System;
using namespace System::Runtime::InteropServices;

#include "Vector3d.h"

namespace Irrlicht
{
namespace Core
{

	/// <summary>
	/// A 3D line from an start point to an end point
	/// </summary>
	public __value class Line3D
	{
	public:

		// Constructors
		Line3D(): start(0,0,0), end(1,1,1) {};
		Line3D(float xa, float ya, float za, float xb, float yb, float zb) : start(xa, ya, za), end(xb, yb, zb) {};
		Line3D(Vector3D start, Vector3D end) : start(start), end(end) {};

		void Set(float xa, float ya, float za, float xb, float yb, float& zb){start.Set(xa, ya, za); end.Set(xb, yb, zb);}
		void Set(Vector3D nstart, Vector3D nend) {start.Set(nstart); end.Set(nend);}
		void Set(Line3D line){start.Set(line.start); end.Set(line.end);}

		/// <summary>
		/// Returns length of line
		/// <returns> Returns length of line.</returns>
		double GetLength()  { return start.GetDistanceFrom(end); };

		/// <summary>
		/// Returns sqared length of line
		/// </summary>
		/// <returns> Returns sqared length of line.</returns>
		double GetLengthSQ() { return start.GetDistanceFromSQ(end); };

		/// <summary>
		/// Returns middle of line
		/// </summary>
		Vector3D GetMiddle() 	
		{
			return (start + end) * (float)0.5;
		}

		/// <summary>
		/// Returns vector of line
		/// </summary>
		Vector3D GetVector() 	
		{
			return end - start;
		}

		/// <summary>
		/// Returns if the overgiven point is between start and end of the
		/// line. Assumes that the point is already somewhere on the line.
		/// </summary>
		bool IsPointBetweenStartAndEnd(Vector3D point)
		{
			return point.IsBetweenPoints(start, end);
		}

		/// Returns the closest point on this line to a point
		Vector3D GetClosestPoint(Vector3D point)
		{
			Vector3D c = point - start;
			Vector3D v = end - start;
			float d = (float)v.GetLength();
			v = v / d;
			float t = v.DotProduct(c);

			if (t < (float)0.0) return start;
			if (t > d) return end;

			v = v * t;
			return start + v;
		}

		/// <summary>
		/// Returns if the line intersects with a shpere
		/// </summary>
		/// <param name="sorigin"> Origin of the shpere.</param>
		/// <param name="sradius"> Radius if the sphere.</param>
		/// <returns> Returns true if there is an intersection.
		/// If there is one, the distance to the first intersection point
		/// is stored in outdistance.</returns>
		bool GetIntersectionWithSphere(Vector3D sorigin, float sradius, 
									   [PARAMFLAG::Out] double& outdistance)
		{
			Vector3D q = sorigin - start;
			double c = q.GetLength();
			Vector3D vv = GetVector();
			vv.Normalize();
			double v = q.DotProduct(vv);
			double d = sradius * sradius - (c*c - v*v);

            if (d < 0.0) 
				return false;

			outdistance = v - System::Math::Sqrt(d);
			return true;
		}

		// member variables
		
		Vector3D start;
		Vector3D end;

	};


}
}