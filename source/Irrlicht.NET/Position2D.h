// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#using <mscorlib.dll>

namespace Irrlicht
{
namespace Core
{

	/// <summary> 
	/// Specifies a two dimensional position with integer values. 
	/// </summary>
	public __value class Position2D
	{
		public:

			/// <summary> Constructs a position at (0,0) </summary>
			Position2D()
				: X(0), Y(0)
			{
			}
				
			/// <summary> Constructs a position. </summary>
			Position2D(int x, int y)
				: X(x), Y(y) 
			{
			}

			/// <summary>
			/// Compares the position to another position.
			/// </summary>
			bool Equals(Object* rhs) 
			{
				Position2D* c = dynamic_cast<Position2D *>(rhs);

				if(!c) 
					return false;

				return c->X == X && c->Y == Y;
			}

			int X, Y;
	};


	/// <summary> 
	/// Specifies a two dimensional position with float values. 
	/// </summary>
	public __value class Position2Df
	{
		public:

			/// <summary> Constructs a position at (0,0) </summary>
			Position2Df()
				: X(0), Y(0)
			{
			}
				
			/// <summary> Constructs a position. </summary>
			Position2Df(float x, float y)
				: X(x), Y(y) 
			{
			}

			/// <summary>
			/// Compares the position to another position.
			/// </summary>
			bool Equals(Object* rhs) 
			{
				Position2Df* c = dynamic_cast<Position2Df *>(rhs);

				if(!c) 
					return false;

				return c->X == X && c->Y == Y;
			}

			float X, Y;
	};

} // end namespace Core
} // end namespace Irrlicht


