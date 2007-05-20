// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#include "Vector3D.h"
#include "Plane3D.h"
#include "Line3D.h"

namespace Irrlicht
{
namespace Core
{

/// <summary>
/// Axis aligned bounding box in 3d dimensional space.
/// Has some useful methods used with occlusion culling or clipping.
/// </summary>
public __value class Box3D
{
	public:

		// Constructors

		Box3D(): MinEdge(-1,-1,-1), MaxEdge(1,1,1) {};
		Box3D(const Vector3D min, const Vector3D max): MinEdge(min), MaxEdge(max) {};
		Box3D(const Vector3D init): MinEdge(init), MaxEdge(init) {};
		Box3D(float minx, float miny, float minz, float maxx, float maxy, float maxz): MinEdge(minx, miny, minz), MaxEdge(maxx, maxy, maxz) {};

		/// <summary>
		/// Adds a point to the bounding box, causing it to grow bigger, 
		/// if point is outside of the box
		/// </summary>
		/// <param name="p"> Point to add into the box.</param>
		void AddInternalPoint(Vector3D p)
		{
			AddInternalPoint(p.X, p.Y, p.Z);
		}

		/// <summary>
		/// Adds an other bounding box to the bounding box, causing it to grow bigger,
		/// if the box is outside of the box
		/// </summary>
		/// <param name="b"> Other bounding box to add into this box.</param>
		void AddInternalBox(Box3D b)
		{
			AddInternalPoint(b.MaxEdge);
			AddInternalPoint(b.MinEdge);
		}

		/// <summary>
		/// Resets the bounding box.
		/// </summary>
		void Reset(float x, float y, float z)
		{
			MaxEdge.Set(x,y,z);
			MinEdge = MaxEdge;
		}

		/// <summary>
		/// Resets the bounding box.
		/// </summary>
		void Reset(Box3D initValue)
		{
			*this = initValue;
		}

		/// <summary>
		/// Resets the bounding box.
		/// </summary>
		void Reset(const Vector3D initValue)
		{
			MaxEdge = initValue;
			MinEdge = initValue;
		}

		/// <summary>
		/// Adds a point to the bounding box, causing it to grow bigger, 
		/// if point is outside of the box.
		/// </summary>
		/// <param name="x"> X Coordinate of the point to add to this box.</param>
		/// <param name="y"> Y Coordinate of the point to add to this box.</param>
		/// <param name="z"> Z Coordinate of the point to add to this box.</param>
		void AddInternalPoint(float x, float y, float z)
		{
			if (x>MaxEdge.X) MaxEdge.X = x;
			if (y>MaxEdge.Y) MaxEdge.Y = y;
			if (z>MaxEdge.Z) MaxEdge.Z = z;

			if (x<MinEdge.X) MinEdge.X = x;
			if (y<MinEdge.Y) MinEdge.Y = y;
			if (z<MinEdge.Z) MinEdge.Z = z;
		}

		/// <summary>
		/// Determinates if a point is within this box.
		/// </summary>
		/// <param name="p">: Point to check.</param>
		/// <returns> Returns true if the point is withing the box, and false if it is not.</returns>
		bool IsPointInside(Vector3D p)
		{
			return (	p.X >= MinEdge.X && p.X <= MaxEdge.X &&
						p.Y >= MinEdge.Y && p.Y <= MaxEdge.Y &&
						p.Z >= MinEdge.Z && p.Z <= MaxEdge.Z);
		};

		/// <summary>
		/// Determinates if a point is within this box and its borders.
		/// </summary>
		/// <param name="p"> Point to check.</param>
		/// <returns> Returns true if the point is withing the box, and false if it is not.</returns>
		bool IsPointTotalInside(Vector3D p)
		{
			return (	p.X > MinEdge.X && p.X < MaxEdge.X &&
						p.Y > MinEdge.Y && p.Y < MaxEdge.Y &&
						p.Z > MinEdge.Z && p.Z < MaxEdge.Z);
		};

		/// <summary>
		/// Determinates if the box intersects with an other box.
		/// </summary>
		/// <param other: Other box to check a intersection with.</param>
		/// <returns> Returns true if there is a intersection with the other box, 
		/// otherwise false.</returns>
		bool IntersectsWithBox(Box3D other)
		{
			return (MinEdge.X <= other.MaxEdge.X &&
				MinEdge.Y <= other.MaxEdge.Y &&
				MinEdge.Z <= other.MaxEdge.Z &&
				MaxEdge.X >= other.MinEdge.X &&
				MaxEdge.Y >= other.MinEdge.Y &&
				MaxEdge.Z >= other.MinEdge.Z);
		}

		/// <summary>
		/// Tests if the box intersects with a line
		/// </summary>
		/// <param name="line"> Line to test intersection with.</param>
		/// <returns> Returns true if there is an intersection and false if not.</returns>
		bool IntersectsWithLine(Line3D line) 
		{
			return IntersectsWithLine(line.GetMiddle(), line.GetVector().Normalize(), 
					(float)(line.GetLength() * 0.5));
		}

		/// <summary>
		/// Tests if the box intersects with a line
		/// </summary>
		/// <returns> Returns true if there is an intersection and false if not.</returns>
		bool IntersectsWithLine(Vector3D linemiddle, 
								Vector3D linevect,
								float halflength)
		{
			Vector3D e = (MaxEdge - MinEdge) * (float)0.5;
			Vector3D t = (MinEdge + e) - linemiddle;

			float r;

			if ((mfabs(t.X) > e.X + halflength * mfabs(linevect.X)) || 
				(mfabs(t.Y) > e.Y + halflength * mfabs(linevect.Y)) ||
				(mfabs(t.Z) > e.Z + halflength * mfabs(linevect.Z)) )
				return false;

			r = e.Y * (float)mfabs(linevect.Z) + e.Z * (float)mfabs(linevect.Y);
			if (mfabs(t.Y*linevect.Z - t.Z*linevect.Y) > r )
				return false;

			r = e.X * (float)mfabs(linevect.Z) + e.Z * (float)mfabs(linevect.X);
			if (mfabs(t.Z*linevect.X - t.X*linevect.Z) > r )
				return false;

			r = e.X * (float)mfabs(linevect.Y) + e.Y * (float)mfabs(linevect.X);
			if (mfabs(t.X*linevect.Y - t.Y*linevect.X) > r)
				return false;

			return true;
		}

		/// <summary>
		/// Classifies a relation with a plane.
		/// </summary>
		/// <param name="plane"> Plane to classify relation to.</param>
		/// <returns> Returns ISREL3D_FRONT if the box is in front of the plane,
		/// ISREL3D_BACK if the box is back of the plane, and
		/// ISREL3D_CLIPPED if is on both sides of the plane.</returns>
		IntersectionRelation3D classifyPlaneRelation(Plane3D plane)
		{
			Vector3D nearPoint(MaxEdge);
			Vector3D farPoint(MinEdge);

			if (plane.Normal.X > (float)0)
			{
				nearPoint.X = MinEdge.X;
				farPoint.X = MaxEdge.X;
			}

			if (plane.Normal.Y > (float)0)
			{
				nearPoint.Y = MinEdge.Y;
				farPoint.Y = MaxEdge.Y;
			}

			if (plane.Normal.Z > (float)0)
			{
				nearPoint.Z = MinEdge.Z;
				farPoint.Z = MaxEdge.Z;
			}

			if (plane.Normal.DotProduct(nearPoint) + plane.D > (float)0)
				return ISREL3D_FRONT;

			if (plane.Normal.DotProduct(farPoint) + plane.D > (float)0)
				return ISREL3D_CLIPPED;

			return ISREL3D_BACK;
		}

		/// <summary>
		/// returns center of the bounding box
		/// </summary>
		Vector3D getCenter()
		{
			return (MinEdge + MaxEdge) / 2;
		}

		/// <summary>
		/// returns extent of the box
		/// </summary>
		Vector3D getExtent()
		{
			return MaxEdge - MinEdge;
		}


		/// <summary>
		/// stores all 8 edges of the box into a array
		/// </summary>
		/// <param name="edges"> Pointer to array of 8 edges</param>
        void getEdges(Vector3D edges __gc[])
		{
			Vector3D middle = (MinEdge + MaxEdge) / 2;
			Vector3D diag = middle - MaxEdge;

			/*
			Edges are stored in this way:
			Hey, am I an ascii artist, or what? :) niko.
                  /1--------/3
                 /  |      / |
                /   |     /  |
                5---------7  |
                |   0- - -| -2
                |  /      |  /
                |/        | /
                4---------6/ 
			*/

			edges[0].Set(middle.X + diag.X, middle.Y + diag.Y, middle.Z + diag.Z);
			edges[1].Set(middle.X + diag.X, middle.Y - diag.Y, middle.Z + diag.Z);
			edges[2].Set(middle.X + diag.X, middle.Y + diag.Y, middle.Z - diag.Z);
			edges[3].Set(middle.X + diag.X, middle.Y - diag.Y, middle.Z - diag.Z);
			edges[4].Set(middle.X - diag.X, middle.Y + diag.Y, middle.Z + diag.Z);
			edges[5].Set(middle.X - diag.X, middle.Y - diag.Y, middle.Z + diag.Z);
			edges[6].Set(middle.X - diag.X, middle.Y + diag.Y, middle.Z - diag.Z);
			edges[7].Set(middle.X - diag.X, middle.Y - diag.Y, middle.Z - diag.Z);
		}


		/// <summary>
		/// returns if the box is empty, which means that there is
		/// no space within the min and the max edge.
		/// </summary>
		bool IsEmpty()
		{
			Vector3D d = MinEdge - MaxEdge;
			if (d.X < 0) d.X = -d.X;
			if (d.Y < 0) d.Y = -d.Y;
			if (d.Z < 0) d.Z = -d.Z;

			return (d.X < ROUNDING_ERROR && 
					d.Y < ROUNDING_ERROR && 
					d.Z < ROUNDING_ERROR);
		}

		/// <summary>
		/// repairs the box, if for example MinEdge and MaxEdge are swapped.
		/// </summary>
		void Repair()
		{
			float t;

			if (MinEdge.X > MaxEdge.X)
				{ t=MinEdge.X; MinEdge.X = MaxEdge.X; MaxEdge.X=t; }
			if (MinEdge.Y > MaxEdge.Y)
				{ t=MinEdge.Y; MinEdge.Y = MaxEdge.Y; MaxEdge.Y=t; }
			if (MinEdge.Z > MaxEdge.Z)
				{ t=MinEdge.Z; MinEdge.Z = MaxEdge.Z; MaxEdge.Z=t; }
		}

		/// <summary>
		/// Calculates a new interpolated bounding box.
		/// </summary>
		/// <param name="other"> other box to interpolate between</param>
		/// <param name="d"> value between 0.0f and 1.0f.</param>
		Box3D GetInterpolated(Box3D other, float d)
		{
			float inv = 1.0f - d;
			return Box3D((other.MinEdge*inv) + (MinEdge*d),
				(other.MaxEdge*inv) + (MaxEdge*d));
		}

		// member variables
		
		Vector3D MinEdge;
		Vector3D MaxEdge;
};

}
}