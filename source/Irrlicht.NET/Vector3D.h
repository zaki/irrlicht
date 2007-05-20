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
	public __value class Vector3D
	{
	public:

		Vector3D(): X(0), Y(0), Z(0) {};
		Vector3D(float nx, float ny, float nz) : X(nx), Y(ny), Z(nz) {};

		static bool op_Equality(Vector3D o1, Vector3D o2)
		{
			return (o1.X == o2.X && o1.Y == o2.Y && o1.Z == o2.Z);	
		}

		static Vector3D op_Addition(Vector3D o1, Vector3D o2)
		{
			return Vector3D(o1.X + o2.X, o1.Y + o2.Y, o1.Z + o2.Z);	
		}

		static Vector3D op_Subtraction(Vector3D o1, Vector3D o2)
		{
			return Vector3D(o1.X - o2.X, o1.Y - o2.Y, o1.Z - o2.Z);	
		}

		static Vector3D op_Multiply(Vector3D o, float scal)
		{
			return Vector3D(o.X * scal, o.Y * scal, o.Z * scal);
		}

		static Vector3D op_Multiply(float scal, const Vector3D& o)
		{
			return Vector3D(o.X * scal, o.Y * scal, o.Z * scal);
		}

		static Vector3D op_Division(Vector3D o, float scal)
		{
			return Vector3D(o.X / scal, o.Y / scal, o.Z / scal);
		}

		void Set( float nx,  float ny,  float nz) {X=nx; Y=ny; Z=nz; }
		void Set( Vector3D p) { X=p.X; Y=p.Y; Z=p.Z;}

		/// <summary>
		/// Returns length of the vector.
		/// </summary>
		double GetLength()
		{
			return get_Length();
		}

		/// <summary>
		/// Returns length of the vector.
		/// </summary>
		__property double get_Length()
		{
			return System::Math::Sqrt(X*X + Y*Y + Z*Z); 
		}

		/// <summary>
		/// Returns squared length of the vector.
		/// This is useful because it is much faster then
		/// GetLength().
		/// </summary>
		double GetLengthSQ()
		{
			return get_LengthSQ();
		}

		/// <summary>
		/// Returns squared length of the vector.
		/// This is useful because it is much faster then
		/// GetLength().
		/// </summary>
		__property double get_LengthSQ()
		{
			return X*X + Y*Y + Z*Z; 
		}

		/// <summary>
		/// Returns the dot product with another vector.
		/// </summary>
		float DotProduct( Vector3D other) 
		{
			return X*other.X + Y*other.Y + Z*other.Z;
		}

		/// <summary>
		/// Returns distance from an other point. Here, the vector is interpreted as
		/// point in 3 dimensional space.
		/// </summary>
		double GetDistanceFrom( Vector3D other) 
		{
			double vx = X - other.X; double vy = Y - other.Y; double vz = Z - other.Z;
			return Math::Sqrt(vx*vx + vy*vy + vz*vz);
		}

		/// <summary>
		/// Returns squared distance from an other point. 
		/// Here, the vector is interpreted as point in 3 dimensional space.
		/// </summary>
		float GetDistanceFromSQ( Vector3D other) 
		{
			float vx = X - other.X; float vy = Y - other.Y; float vz = Z - other.Z;
			return (vx*vx + vy*vy + vz*vz);
		}

		/// <summary>
		/// Returns cross product with an other vector
		///</summary>
		///<param name="p"> other vector </param>
		///<returns>cross product </returns>
		Vector3D CrossProduct( Vector3D p) 
		{
			return Vector3D(Y * p.Z - Z * p.Y, Z * p.X - X * p.Z, X * p.Y - Y * p.X);
		}

		/// <summary>
		/// Returns if the point represented by this vector is between to points
		///</summary>
		///<param name="begin">Start point of line</param>
		///<param name="end">End point of line</param>
		///<returns> True if between points, false if not. </returns>
		bool IsBetweenPoints( Vector3D begin,  Vector3D end) 
		{
			float f = (float)(end - begin).GetLengthSQ();
			return (float)GetDistanceFromSQ(begin) < f && 
				(float)GetDistanceFromSQ(end) < f;
		}

		/// <summary>
		/// Normalizes the vector.
		/// </summary>
		Vector3D Normalize()
		{
			float inv = (float)GetLength();

			if ( inv == 0 )
				return *this;

			inv = 1.0f / inv;
			X *= inv;
			Y *= inv;
			Z *= inv;
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
			Z *= -1.0f;
		}

		/// <summary>
		/// Rotates the vector around one axis
		///</summary>
		///<param name="degrees"> Degrees to rotate vector </param>
		///<param name="center"> Center of rotation </param>
		void RotateXZBy(double degrees,  Vector3D center)
		{
			degrees *=GRAD_PI2;
			float cs = (float)Math::Cos(degrees);
			float sn = (float)Math::Sin(degrees);
			X -= center.X;
			Z -= center.Z;
			Set(X*cs - Z*sn, Y, X*sn + Z*cs);
			X += center.X;
			Z += center.Z;
		}

		/// <summary>
		/// Rotates the vector around one axis
		/// </summary>
		/// <param name="degrees"> Degrees to rotate vector </param>
		/// <param name="center"> Center of rotation </param>
		void RotateXYBy(double degrees,  Vector3D center)
		{
			degrees *=GRAD_PI2;
			float cs = (float)Math::Cos(degrees);
			float sn = (float)Math::Sin(degrees);
			X -= center.X;
			Y -= center.Y;
			Set(X*cs - Y*sn, X*sn + Y*cs, Z);
			X += center.X;
			Y += center.Y;
		}

		/// <summary>
		/// Rotates the vector around one axis
		///</summary>
		///<param name="degrees"> Degrees to rotate vector </param>
		///<param name="center"> Center of rotation </param>
		void RotateYZBy(double degrees,  Vector3D center)
		{
			degrees *=GRAD_PI2;
			float cs = (float)Math::Cos(degrees);
			float sn = (float)Math::Sin(degrees);
			Z -= center.Z;
			Y -= center.Y;
			Set(X, Y*cs - Z*sn, Y*sn + Z*cs);
			Z += center.Z;
			Y += center.Y;
		}

		/// <summary>
		/// returns interpolated vector
		/// </summary>
		/// <param name="other"> other vector to interpolate between </param>
		/// <param name="d"> value between 0.0f and 1.0f.</param>
		Vector3D GetInterpolated( Vector3D other, float d) 
		{
			float inv = 1.0f - d;
			return Vector3D(other.X*inv + X*d,
								other.Y*inv + Y*d,
								other.Z*inv + Z*d);
		}

		/// <summary>
		/// Compares the vector to another vector.
		/// </summary>
		bool Equals(Object* rhs) 
		{
			Vector3D* c = dynamic_cast<Vector3D *>(rhs);

			if(!c) 
				return false;

			return c->X == X && c->Y == Y && c->Z == Z;
		}

		virtual System::String __gc * ToString()
		{
			return System::String::Concat( System::String::Concat( "(", 
																   X.ToString(),
																   ", ",
																   Y.ToString()), 
										   ", ",
										   Z.ToString(),
										   ")" );
		}

		// member variables

		float X, Y, Z;
	};

}
}