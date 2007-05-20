// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#include "irrMath.h"
#include "..\\..\\include\\matrix4.h"

#include "Vector3D.h"

namespace Irrlicht
{
namespace Core
{
	
	/// <summary>
	/// 4x4 matrix. Mostly used as transformation matrix for 3d calculations.
	/// Matrix4 is mainly used by the Irrlicht engine for doing transformations.
	/// The matrix is a D3D style matrix, row major with translations in the 4th row.
	/// This class has been ported directly from the native C++ Irrlicht Engine, so it may not 
	/// be 100% complete yet and the design may not be 100% .NET like.
	/// </summary>
	public __value struct Matrix4
	{
		public:

			/// <summary> Constructor </summary>
			Matrix4()
			{
				//Members = new float __gc[16];
				MakeIdentity();
			}

			/// <summary> Set matrix to identity. </summary>
			void MakeIdentity()
			{
				for (int i=0; i<16; ++i)
					Members[i] = 0.0f;

				Members[0] = Members[5] = Members[10] = Members[15] = 1;
			}

			/// <summary> Direct accessing every row and colum of the matrix values </summary>
			__property inline float get_M(int row, int col) 
			{
				if (row < 0 || row >= 4 ||
					col < 0 || col >= 4)
					throw new System::Exception(new System::String("Invalid index for accessing matrix members"));

				return Members[col * 4 + row];
			}

			/// <summary> Direct accessing every row and colum of the matrix values </summary>
			inline void set_M(int row, int col, float v)
			{
				if (row < 0 || row >= 4 ||
					col < 0 || col >= 4)
					throw new System::Exception(new System::String("Invalid index for accessing matrix members"));

				Members[col * 4 + row] = v;
			}

			/*
			/// <summary> Sets this matrix equal to the other matrix.</summary>
			matrix4& operator=(const matrix4 &other);

			/// <summary> Returns true if other matrix is equal to this matrix.</summary>
			bool operator==(const matrix4 &other) const;

			/// <summary> Returns true if other matrix is not equal to this matrix.</summary>
			bool operator!=(const matrix4 &other) const;

			/// <summary> Multiply by another matrix.</summary>
			matrix4& operator*=(const matrix4& other);
			*/

			/// <summary> Multiply by another matrix. </summary>
			static Matrix4 op_Multiply(Matrix4 a, Matrix4 b)
			{
				Matrix4 tmtrx;

				#define m1 a.Members
				#define m2 b.Members
				#define m3 tmtrx.Members

				m3[0] = m1[0]*m2[0] + m1[4]*m2[1] + m1[8]*m2[2] + m1[12]*m2[3];
				m3[1] = m1[1]*m2[0] + m1[5]*m2[1] + m1[9]*m2[2] + m1[13]*m2[3];
				m3[2] = m1[2]*m2[0] + m1[6]*m2[1] + m1[10]*m2[2] + m1[14]*m2[3];
				m3[3] = m1[3]*m2[0] + m1[7]*m2[1] + m1[11]*m2[2] + m1[15]*m2[3];
				
				m3[4] = m1[0]*m2[4] + m1[4]*m2[5] + m1[8]*m2[6] + m1[12]*m2[7];
				m3[5] = m1[1]*m2[4] + m1[5]*m2[5] + m1[9]*m2[6] + m1[13]*m2[7];
				m3[6] = m1[2]*m2[4] + m1[6]*m2[5] + m1[10]*m2[6] + m1[14]*m2[7];
				m3[7] = m1[3]*m2[4] + m1[7]*m2[5] + m1[11]*m2[6] + m1[15]*m2[7];
				
				m3[8] = m1[0]*m2[8] + m1[4]*m2[9] + m1[8]*m2[10] + m1[12]*m2[11];
				m3[9] = m1[1]*m2[8] + m1[5]*m2[9] + m1[9]*m2[10] + m1[13]*m2[11];
				m3[10] = m1[2]*m2[8] + m1[6]*m2[9] + m1[10]*m2[10] + m1[14]*m2[11];
				m3[11] = m1[3]*m2[8] + m1[7]*m2[9] + m1[11]*m2[10] + m1[15]*m2[11];
				
				m3[12] = m1[0]*m2[12] + m1[4]*m2[13] + m1[8]*m2[14] + m1[12]*m2[15];
				m3[13] = m1[1]*m2[12] + m1[5]*m2[13] + m1[9]*m2[14] + m1[13]*m2[15];
				m3[14] = m1[2]*m2[12] + m1[6]*m2[13] + m1[10]*m2[14] + m1[14]*m2[15];
				m3[15] = m1[3]*m2[12] + m1[7]*m2[13] + m1[11]*m2[14] + m1[15]*m2[15];

				#undef m1
				#undef m2
				#undef m3

				return tmtrx;
			}

			/// <summary> Returns true if the matrix is the identity matrix. </summary>
			inline bool IsIdentity()
			{
				for (int i=0; i<4; ++i)
					for (int j=0; j<4; ++j)
						if (j != i)
						{
							if (get_M(i,j) < -0.0000001f ||
								get_M(i,j) >  0.0000001f)
								return false;
						}
						else
						{
							if (get_M(i,j) < 0.9999999f ||
								get_M(i,j) > 1.0000001f)
								return false;
						}

				return true;
			}

			/// <summary> Set the translation of the current matrix. Will erase any previous values.</summary>
			void SetTranslation( const Vector3D translation )
			{
				Members[12] = translation.X;
				Members[13] = translation.Y;
				Members[14] = translation.Z;
			}

			/// <summary> Gets the current translation</summary>
			Vector3D GetTranslation()
			{
				return Vector3D(Members[12], Members[13], Members[14]);
			}

			/// <summary> Set the inverse translation of the current matrix.
			/// Will erase any previous values.</summary>
			void SetInverseTranslation( const Vector3D translation )
			{
				Members[12] = -translation.X;
				Members[13] = -translation.Y;
				Members[14] = -translation.Z;
			}

			/// <summary> Make a rotation matrix from Euler angles. 
			/// The 4th row and column are unmodified.</summary>
			void SetRotationRadians( const Vector3D rotation )
			{
				double cr = Math::Cos( rotation.X );
				double sr = Math::Sin( rotation.X );
				double cp = Math::Cos( rotation.Y );
				double sp = Math::Sin( rotation.Y );
				double cy = Math::Cos( rotation.Z );
				double sy = Math::Sin( rotation.Z );

				Members[0] = (float)( cp*cy );
				Members[1] = (float)( cp*sy );
				Members[2] = (float)( -sp );

				double srsp = sr*sp;
				double crsp = cr*sp;

				Members[4] = (float)( srsp*cy-cr*sy );
				Members[5] = (float)( srsp*sy+cr*cy );
				Members[6] = (float)( sr*cp );

				Members[8] = (float)( crsp*cy+sr*sy );
				Members[9] = (float)( crsp*sy-sr*cy );
				Members[10] = (float)( cr*cp );
			}

	
			/// <summary> Make a rotation matrix from Euler angles. 
			/// The 4th row and column are unmodified.</summary>
			void SetRotationDegrees( const Vector3D rotation )
			{
				SetRotationRadians( rotation * (float)(3.1415926535897932384626433832795 / 180.0) );
			}

			/// <summary> Returns the rotation, as set by setRotation(). 
			/// This code was orginally written by by Chev.</summary>
			Vector3D GetRotationDegrees() 
			{ 
				Matrix4 &mat = *this; 

				double Y = -asin(mat.get_M(2,0)); 
				double D = Y; 
				double C = cos(Y); 
				Y *= GRAD_PI; 

				double rotx, roty, X, Z; 

				if (fabs(C) > 0.0005f)  
				{ 
					rotx = mat.get_M(2,2) / C; 
					roty = mat.get_M(2,1)  / C; 

					X = atan2( roty, rotx ) * GRAD_PI; 

					rotx = mat.get_M(0,0) / C; 
					roty = mat.get_M(1,0) / C; 
					Z = atan2( roty, rotx ) * GRAD_PI; 
				} 
				else 
				{ 
				X  = 0.0f; 
				rotx = mat.get_M(1,1); 
				roty = -mat.get_M(0,1); 
				Z  = atan2( roty, rotx ) * GRAD_PI; 
				} 

				// fix values that get below zero 
				// before it would set (!) values to 360 
				// that where above 360: 
				if (X < 0.00) X += 360.00; 
				if (Y < 0.00) Y += 360.00; 
				if (Z < 0.00) Z += 360.00; 

				return Vector3D((float)X, (float)Y, (float)Z); 
			}
/*
			/// <summary> Make an inverted rotation matrix from Euler angles. The 4th row and column are unmodified.</summary>
			void setInverseRotationRadians( const Vector3D& rotation );	

			/// <summary> Make an inverted rotation matrix from Euler angles. The 4th row and column are unmodified.</summary>
			void setInverseRotationDegrees( const Vector3D& rotation );	
*/
			/// <summary> Set Scale </summary>
			void SetScale( Vector3D scale )
			{
				Members[0] = scale.X;
				Members[5] = scale.Y;
				Members[10] = scale.Z;
			}
			
			/*
			/// <summary> Translate a vector by the inverse of the translation part of this matrix.</summary>
			void inverseTranslateVect( Vector3D& vect ) const;			

			/// <summary> Rotate a vector by the inverse of the rotation part of this matrix.</summary>
			void inverseRotateVect( Vector3D& vect ) const;				

			/// <summary> Rotate a vector by the rotation part of this matrix.</summary>
			void rotateVect( Vector3D& vect ) const;				
*/
			/// <summary> Transforms the vector by this matrix </summary>
			void TransformVect( Vector3D& vect)
			{
				float vector[3];

				vector[0] = vect.X*Members[0] + vect.Y*Members[4] + vect.Z*Members[8] + Members[12];
				vector[1] = vect.X*Members[1] + vect.Y*Members[5] + vect.Z*Members[9] + Members[13];
				vector[2] = vect.X*Members[2] + vect.Y*Members[6] + vect.Z*Members[10] + Members[14];

				vect.X = vector[0];
				vect.Y = vector[1];
				vect.Z = vector[2];
			}

			/// <summary> Transforms input vector by this matrix and stores result in output vector </summary>
			void TransformVect( const Vector3D& in, Vector3D& out)
			{
				out.X = in.X*Members[0] + in.Y*Members[4] + in.Z*Members[8] + Members[12];
				out.Y = in.X*Members[1] + in.Y*Members[5] + in.Z*Members[9] + Members[13];
				out.Z = in.X*Members[2] + in.Y*Members[6] + in.Z*Members[10] + Members[14];
			}

			/// <summary> Translate a vector by the translation part of this matrix.</summary>
			void TranslateVect( Vector3D& vect ) 
			{
				vect.X = vect.X+Members[12];
				vect.Y = vect.Y+Members[13];
				vect.Z = vect.Z+Members[14];
			}
/*
			/// <summary> Transforms a plane by this matrix </summary>
			void transformPlane( core::plane3d<float> &plane) const

			/// <summary> Transforms a plane by this matrix</summary>
			void transformPlane( const core::plane3d<float> &in, core::plane3d<float> &out) const;

			/// <summary> Transforms a axis aligned bounding box</summary>
			void transformBox( core::aabbox3d<float> &box) const;

			/// <summary> Multiplies this matrix by a 1x4 matrix</summary>
			void multiplyWith1x4Matrix(float* matrix) const;
*/
			/// <summary> Calculates inverse of matrix. Slow.</summary>
			/// <returns> Returns false if there is no inverse matrix.</returns>
			bool MakeInverse()
			{
				Matrix4 temp;

				if (GetInverse(temp))
				{
					*this = temp;
					return true;
				}

				return false;
			}

			/// <summary> returns the inversed matrix of this one</summary>
			/// <param name="Target"> where result matrix is written to.</param>
			/// <returns>Returns false if there is no inverse matrix.</returns>
			bool GetInverse(Matrix4& out)
			{
				/// Calculates the inverse of this Matrix 
				/// The inverse is calculated using Cramers rule.
				/// If no inverse exists then 'false' is returned.

				#define m getMInsecure
				#define out out.setMInsecure

				float d = (m(0, 0) * m(1, 1) - m(1, 0) * m(0, 1)) * (m(2, 2) * m(3, 3) - m(3, 2) * m(2, 3))	- (m(0, 0) * m(2, 1) - m(2, 0) * m(0, 1)) * (m(1, 2) * m(3, 3) - m(3, 2) * m(1, 3))
						+ (m(0, 0) * m(3, 1) - m(3, 0) * m(0, 1)) * (m(1, 2) * m(2, 3) - m(2, 2) * m(1, 3))	+ (m(1, 0) * m(2, 1) - m(2, 0) * m(1, 1)) * (m(0, 2) * m(3, 3) - m(3, 2) * m(0, 3))
						- (m(1, 0) * m(3, 1) - m(3, 0) * m(1, 1)) * (m(0, 2) * m(2, 3) - m(2, 2) * m(0, 3))	+ (m(2, 0) * m(3, 1) - m(3, 0) * m(2, 1)) * (m(0, 2) * m(1, 3) - m(1, 2) * m(0, 3));
				
				if (d == 0.f)
					return false;

				d = 1.f / d;

				out(0, 0, d * (m(1, 1) * (m(2, 2) * m(3, 3) - m(3, 2) * m(2, 3)) + m(2, 1) * (m(3, 2) * m(1, 3) - m(1, 2) * m(3, 3)) + m(3, 1) * (m(1, 2) * m(2, 3) - m(2, 2) * m(1, 3))));
				out(1, 0, d * (m(1, 2) * (m(2, 0) * m(3, 3) - m(3, 0) * m(2, 3)) + m(2, 2) * (m(3, 0) * m(1, 3) - m(1, 0) * m(3, 3)) + m(3, 2) * (m(1, 0) * m(2, 3) - m(2, 0) * m(1, 3))));
				out(2, 0, d * (m(1, 3) * (m(2, 0) * m(3, 1) - m(3, 0) * m(2, 1)) + m(2, 3) * (m(3, 0) * m(1, 1) - m(1, 0) * m(3, 1)) + m(3, 3) * (m(1, 0) * m(2, 1) - m(2, 0) * m(1, 1))));
				out(3, 0, d * (m(1, 0) * (m(3, 1) * m(2, 2) - m(2, 1) * m(3, 2)) + m(2, 0) * (m(1, 1) * m(3, 2) - m(3, 1) * m(1, 2)) + m(3, 0) * (m(2, 1) * m(1, 2) - m(1, 1) * m(2, 2))));
				out(0, 1, d * (m(2, 1) * (m(0, 2) * m(3, 3) - m(3, 2) * m(0, 3)) + m(3, 1) * (m(2, 2) * m(0, 3) - m(0, 2) * m(2, 3)) + m(0, 1) * (m(3, 2) * m(2, 3) - m(2, 2) * m(3, 3))));
				out(1, 1, d * (m(2, 2) * (m(0, 0) * m(3, 3) - m(3, 0) * m(0, 3)) + m(3, 2) * (m(2, 0) * m(0, 3) - m(0, 0) * m(2, 3)) + m(0, 2) * (m(3, 0) * m(2, 3) - m(2, 0) * m(3, 3))));
				out(2, 1, d * (m(2, 3) * (m(0, 0) * m(3, 1) - m(3, 0) * m(0, 1)) + m(3, 3) * (m(2, 0) * m(0, 1) - m(0, 0) * m(2, 1)) + m(0, 3) * (m(3, 0) * m(2, 1) - m(2, 0) * m(3, 1))));
				out(3, 1, d * (m(2, 0) * (m(3, 1) * m(0, 2) - m(0, 1) * m(3, 2)) + m(3, 0) * (m(0, 1) * m(2, 2) - m(2, 1) * m(0, 2)) + m(0, 0) * (m(2, 1) * m(3, 2) - m(3, 1) * m(2, 2))));
				out(0, 2, d * (m(3, 1) * (m(0, 2) * m(1, 3) - m(1, 2) * m(0, 3)) + m(0, 1) * (m(1, 2) * m(3, 3) - m(3, 2) * m(1, 3)) + m(1, 1) * (m(3, 2) * m(0, 3) - m(0, 2) * m(3, 3))));
				out(1, 2, d * (m(3, 2) * (m(0, 0) * m(1, 3) - m(1, 0) * m(0, 3)) + m(0, 2) * (m(1, 0) * m(3, 3) - m(3, 0) * m(1, 3)) + m(1, 2) * (m(3, 0) * m(0, 3) - m(0, 0) * m(3, 3))));
				out(2, 2, d * (m(3, 3) * (m(0, 0) * m(1, 1) - m(1, 0) * m(0, 1)) + m(0, 3) * (m(1, 0) * m(3, 1) - m(3, 0) * m(1, 1)) + m(1, 3) * (m(3, 0) * m(0, 1) - m(0, 0) * m(3, 1))));
				out(3, 2, d * (m(3, 0) * (m(1, 1) * m(0, 2) - m(0, 1) * m(1, 2)) + m(0, 0) * (m(3, 1) * m(1, 2) - m(1, 1) * m(3, 2)) + m(1, 0) * (m(0, 1) * m(3, 2) - m(3, 1) * m(0, 2))));
				out(0, 3, d * (m(0, 1) * (m(2, 2) * m(1, 3) - m(1, 2) * m(2, 3)) + m(1, 1) * (m(0, 2) * m(2, 3) - m(2, 2) * m(0, 3)) + m(2, 1) * (m(1, 2) * m(0, 3) - m(0, 2) * m(1, 3))));
				out(1, 3, d * (m(0, 2) * (m(2, 0) * m(1, 3) - m(1, 0) * m(2, 3)) + m(1, 2) * (m(0, 0) * m(2, 3) - m(2, 0) * m(0, 3)) + m(2, 2) * (m(1, 0) * m(0, 3) - m(0, 0) * m(1, 3))));
				out(2, 3, d * (m(0, 3) * (m(2, 0) * m(1, 1) - m(1, 0) * m(2, 1)) + m(1, 3) * (m(0, 0) * m(2, 1) - m(2, 0) * m(0, 1)) + m(2, 3) * (m(1, 0) * m(0, 1) - m(0, 0) * m(1, 1))));
				out(3, 3, d * (m(0, 0) * (m(1, 1) * m(2, 2) - m(2, 1) * m(1, 2)) + m(1, 0) * (m(2, 1) * m(0, 2) - m(0, 1) * m(2, 2)) + m(2, 0) * (m(0, 1) * m(1, 2) - m(1, 1) * m(0, 2))));

				return true;
			}

			/// <summary> Builds a right-handed perspective projection matrix based on a field of view</summary>
			void buildProjectionMatrixPerspectiveFovRH(float fieldOfViewRadians, float aspectRatio, float zNear, float zFar)
			{
				float h = (float)(Math::Cos(fieldOfViewRadians/2) / Math::Sin(fieldOfViewRadians/2));
				float w = h / aspectRatio;

				setMInsecure(0,0,2*zNear/w);
				setMInsecure(1,0,0);
				setMInsecure(2,0,0);
				setMInsecure(3,0,0);

				setMInsecure(0,1,0);
				setMInsecure(1,1,2*zNear/h);
				setMInsecure(2,1,0);
				setMInsecure(3,1,0);

				setMInsecure(0,2,0);
				setMInsecure(1,2,0);
				setMInsecure(2,2,zFar/(zFar-zNear));
				setMInsecure(3,2,-1);

				setMInsecure(0,3,0);
				setMInsecure(1,3,0);
				setMInsecure(2,3,zNear*zFar/(zNear-zFar));
				setMInsecure(3,3,0);
			}

			/// <summary> Builds a left-handed perspective projection matrix based on a field of view</summary>
			void buildProjectionMatrixPerspectiveFovLH(float fieldOfViewRadians, float aspectRatio, float zNear, float zFar)
			{
				float h = (float)(Math::Cos(fieldOfViewRadians/2) / Math::Sin(fieldOfViewRadians/2));
				float w = h / aspectRatio;

				setMInsecure(0,0,2*zNear/w);
				setMInsecure(1,0,0);
				setMInsecure(2,0,0);
				setMInsecure(3,0,0);

				setMInsecure(0,1,0);
				setMInsecure(1,1,2*zNear/h);
				setMInsecure(2,1,0);
				setMInsecure(3,1,0);

				setMInsecure(0,2,0);
				setMInsecure(1,2,0);
				setMInsecure(2,2,zFar/(zFar-zNear));
				setMInsecure(3,2,1);

				setMInsecure(0,3,0);
				setMInsecure(1,3,0);
				setMInsecure(2,3,zNear*zFar/(zNear-zFar));
				setMInsecure(3,3,0);
			}

			/// <summary> Builds a right-handed perspective projection matrix.</summary>
			void buildProjectionMatrixPerspectiveRH(float widthOfViewVolume, float heightOfViewVolume, float zNear, float zFar)
			{
				setMInsecure(0,0,2*zNear/widthOfViewVolume);
				setMInsecure(1,0,0);
				setMInsecure(2,0,0);
				setMInsecure(3,0,0);

				setMInsecure(0,1,0);
				setMInsecure(1,1,2*zNear/heightOfViewVolume);
				setMInsecure(2,1,0);
				setMInsecure(3,1,0);

				setMInsecure(0,2,0);
				setMInsecure(1,2,0);
				setMInsecure(2,2,zFar/(zNear-zFar));
				setMInsecure(3,2,-1);

				setMInsecure(0,3,0);
				setMInsecure(1,3,0);
				setMInsecure(2,3,zNear*zFar/(zNear-zFar));
				setMInsecure(3,3,0);
			}

			/// <summary> Builds a left-handed perspective projection matrix.</summary>
			void buildProjectionMatrixPerspectiveLH(float widthOfViewVolume, float heightOfViewVolume, float zNear, float zFar)
			{
				setMInsecure(0,0,2*zNear/widthOfViewVolume);
				setMInsecure(1,0,0);
				setMInsecure(2,0,0);
				setMInsecure(3,0,0);

				setMInsecure(0,1,0);
				setMInsecure(1,1,2*zNear/heightOfViewVolume);
				setMInsecure(2,1,0);
				setMInsecure(3,1,0);

				setMInsecure(0,2,0);
				setMInsecure(1,2,0);
				setMInsecure(2,2,zFar/(zNear-zFar));
				setMInsecure(3,2,1);

				setMInsecure(0,3,0);
				setMInsecure(1,3,0);
				setMInsecure(2,3,zNear*zFar/(zNear-zFar));
				setMInsecure(3,3,0);
			}

			/// <summary> Builds a left-handed orthogonal projection matrix.</summary>
			void buildProjectionMatrixOrthoLH(float widthOfViewVolume, float heightOfViewVolume, float zNear, float zFar)
			{
				setMInsecure(0,0,2/widthOfViewVolume);
				setMInsecure(1,0,0);
				setMInsecure(2,0,0);
				setMInsecure(3,0,0);

				setMInsecure(0,1,0);
				setMInsecure(1,1,2/heightOfViewVolume);
				setMInsecure(2,1,0);
				setMInsecure(3,1,0);

				setMInsecure(0,2,0);
				setMInsecure(1,2,0);
				setMInsecure(2,2,1/(zNear-zFar));
				setMInsecure(3,2,0);

				setMInsecure(0,3,0);
				setMInsecure(1,3,0);
				setMInsecure(2,3,zNear/(zNear-zFar));
				setMInsecure(3,3,1);
			}

			/// <summary> Builds a right-handed orthogonal projection matrix.</summary>
			void buildProjectionMatrixOrthoRH(float widthOfViewVolume, float heightOfViewVolume, float zNear, float zFar)
			{
				setMInsecure(0,0,2/widthOfViewVolume);
				setMInsecure(1,0,0);
				setMInsecure(2,0,0);
				setMInsecure(3,0,0);

				setMInsecure(0,1,0);
				setMInsecure(1,1,2/heightOfViewVolume);
				setMInsecure(2,1,0);
				setMInsecure(3,1,0);

				setMInsecure(0,2,0);
				setMInsecure(1,2,0);
				setMInsecure(2,2,1/(zNear-zFar));
				setMInsecure(3,2,0);

				setMInsecure(0,3,0);
				setMInsecure(1,3,0);
				setMInsecure(2,3,zNear/(zNear-zFar));
				setMInsecure(3,3,-1);
			}

			/// <summary> Builds a left-handed look-at matrix.</summary>
			void buildCameraLookAtMatrixLH(Vector3D position, Vector3D target, Vector3D upVector)
			{
				Vector3D zaxis = target - position;
				zaxis.Normalize();

				Vector3D xaxis = upVector.CrossProduct(zaxis);
				xaxis.Normalize();

				Vector3D yaxis = zaxis.CrossProduct(xaxis);

				setMInsecure(0,0,xaxis.X);
				setMInsecure(1,0,yaxis.X);
				setMInsecure(2,0,zaxis.X);
				setMInsecure(3,0,0);

				setMInsecure(0,1,xaxis.Y);
				setMInsecure(1,1,yaxis.Y);
				setMInsecure(2,1,zaxis.Y);
				setMInsecure(3,1,0);

				setMInsecure(0,2,xaxis.Z);
				setMInsecure(1,2,yaxis.Z);
				setMInsecure(2,2,zaxis.Z);
				setMInsecure(3,2,0);

				setMInsecure(0,3,-xaxis.DotProduct(position));
				setMInsecure(1,3,-yaxis.DotProduct(position));
				setMInsecure(2,3,-zaxis.DotProduct(position));
				setMInsecure(3,3,1.0f);
			}

			/// <summary> Builds a right-handed look-at matrix.</summary>
			void buildCameraLookAtMatrixRH(Vector3D position, Vector3D target, Vector3D upVector)
			{
				Vector3D zaxis = position - target;
				zaxis.Normalize();

				Vector3D xaxis = upVector.CrossProduct(zaxis);
				xaxis.Normalize();

				Vector3D yaxis = zaxis.CrossProduct(xaxis);

				setMInsecure(0,0,xaxis.X);
				setMInsecure(1,0,yaxis.X);
				setMInsecure(2,0,zaxis.X);
				setMInsecure(3,0,0);

				setMInsecure(0,1,xaxis.Y);
				setMInsecure(1,1,yaxis.Y);
				setMInsecure(2,1,zaxis.Y);
				setMInsecure(3,1,0);

				setMInsecure(0,2,xaxis.Z);
				setMInsecure(1,2,yaxis.Z);
				setMInsecure(2,2,zaxis.Z);
				setMInsecure(3,2,0);

				setMInsecure(0,3,-xaxis.DotProduct(position));
				setMInsecure(1,3,-yaxis.DotProduct(position));
				setMInsecure(2,3,-zaxis.DotProduct(position));
				setMInsecure(3,3,1.0f);			
			}

			/// <summary> Returns the transposed matrix. </summary>
			Matrix4 GetTransposed()
			{
				Matrix4 t;	
				
				for (int r=0; r<4; ++r)
					for (int c=0; c<4; ++c)
						t.set_M(r, c, this->get_M( c, r ));

				return t;
			}

			/// <summary> Returns array of floats representing the matrix </summary>
			float GetFloats() __gc[]
			{
				float f __gc[] = new float __gc [16];
				for (int i=0; i<16; ++i)
					f[i] = Members[i];
				return f;
			}


			/// <summary> Matrix data, stored in column-major order </summary>
			float Members __nogc [16];
			//float Members __gc [];

		private:

			/// <summary>
			/// Direct accessing every row and colum of the matrix values without boundary checking 
			/// </summary>
			inline float getMInsecure(int row, int col) 
			{
				return Members[col * 4 + row];
			}

			/// <summary> 
			/// Direct accessing every row and colum of the matrix values without boundary checking 
			/// </summary>
			inline void setMInsecure(int row, int col, float v) 
			{
				Members[col * 4 + row] = v;
			}
	};
}
}