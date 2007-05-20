// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#include "irrMath.h"

namespace Irrlicht
{
namespace Core
{
	/// <summary>
	/// 3d vector class with lots of operators and methods.
	/// This class has been ported directly from the native C++ Irrlicht Engine, so it may not 
	/// be 100% complete yet and the design may not be 100% .NET like.
	/// </summary>
	public __value class Vector2D
	{
	public:

		Vector2D(): X(0), Y(0) {};
		Vector2D(float nx, float ny) : X(nx), Y(ny) {};

		static bool op_Equality(Vector2D o1, Vector2D o2)
		{
			return (o1.X == o2.X && o1.Y == o2.Y);	
		}

		static Vector2D op_Addition(Vector2D o1, Vector2D& o2)
		{
			return Vector2D(o1.X + o2.X, o1.Y + o2.Y);	
		}

		static Vector2D op_Subtraction(Vector2D o1, Vector2D& o2)
		{
			return Vector2D(o1.X - o2.X, o1.Y - o2.Y);	
		}

		static Vector2D op_Multiply(const Vector2D o, float scal)
		{
			return Vector2D(o.X * scal, o.Y * scal);
		}

		static Vector2D op_Multiply(float scal, const Vector2D& o)
		{
			return Vector2D(o.X * scal, o.Y * scal);
		}

		void Set( float nx,  float ny) {X=nx; Y=ny; }
		void Set( Vector2D p) { X=p.X; Y=p.Y;}

		/// <summary>
		/// Returns length of the vector.
		/// </summary>
		double GetLength()  { return System::Math::Sqrt(X*X + Y*Y); }

		/// <summary>
		/// Returns squared length of the vector.
		/// This is useful because it is much faster then
		/// GetLength().
		/// </summary>
		double GetLengthSQ()  { return X*X + Y*Y; }

		/// <summary>
		/// Returns the dot product with another vector.
		/// </summary>
		float DotProduct( Vector2D other) 
		{
			return X*other.X + Y*other.Y;
		}

		/// <summary>
		/// Returns distance from an other point. Here, the vector is interpreted as
		/// point in 3 dimensional space.
		/// </summary>
		double GetDistanceFrom( Vector2D other) 
		{
			double vx = X - other.X; double vy = Y - other.Y;
			return Math::Sqrt(vx*vx + vy*vy);
		}

		/// <summary>
		/// Returns squared distance from an other point. 
		/// Here, the vector is interpreted as point in 3 dimensional space.
		/// </summary>
		float GetDistanceFromSQ( Vector2D other) 
		{
			float vx = X - other.X; float vy = Y - other.Y; 
			return (vx*vx + vy*vy);
		}

		/// <summary>
		/// Returns if the point represented by this vector is between to points
		///</summary>
		///<param name="begin">Start point of line</param>
		///<param name="end">End point of line</param>
		///<returns> True if between points, false if not. </returns>
		bool IsBetweenPoints( Vector2D begin,  Vector2D end) 
		{
			float f = (float)(end - begin).GetLengthSQ();
			return (float)GetDistanceFromSQ(begin) < f && 
				(float)GetDistanceFromSQ(end) < f;
		}

		/// <summary>
		/// Normalizes the vector.
		/// </summary>
		Vector2D Normalize()
		{
			float inv = (float)1.0 / (float)GetLength();
			X *= inv;
			Y *= inv;
			return *this;
		}

		/// <summary>
		/// Sets the lenght of the vector to a new value
		/// </summary>
		void SetLength(float newlength)
		{
			Normalize();
			(*this) = (*this)*newlength;
		}

		/// <summary>
		/// Inverts the vector.
		/// </summary>
		void Invert()
		{
			X *= -1.0f;
			Y *= -1.0f;
		}

		/// <summary>
		/// Compares the vector to another vector.
		/// </summary>
		bool Equals(Object* rhs) 
		{
			Vector2D* c = dynamic_cast<Vector2D *>(rhs);

			if(!c) 
				return false;

			return c->X == X && c->Y == Y;
		}

		// member variables

		float X, Y;
	};

}
}