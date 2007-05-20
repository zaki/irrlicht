// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#include "Position2D.h"
#include "Dimension2D.h"

namespace Irrlicht
{
namespace Core
{
	/// <summary>
	///	Rectangle class.
	/// Mostly used by 2D GUI elements and for 2D drawing methods.
	/// It has 2 positions instead of position and dimension
	///	and a fast method for collision detection with other rectangles and points.
	/// </summary>
	public __value class Rect
	{
	public:

		Rect()
		: UpperLeftCorner(0,0), LowerRightCorner(0,0) 
		{
		}


		Rect(int x, int y, int x2, int y2)
			: UpperLeftCorner(x,y), LowerRightCorner(x2,y2) {};


		Rect(Position2D upperLeft, Position2D lowerRight)
			: UpperLeftCorner(upperLeft), LowerRightCorner(lowerRight) 
		{
		}

		Rect(Position2D pos, Dimension2D size)
			: UpperLeftCorner(pos), LowerRightCorner(pos.X + size.Width, pos.Y + size.Height) 
		{
		}

		/// <summary>
		/// Returns if a 2d point is within this rectangle.
		/// </summary>
		/// <param name="pos"> Position to test if it lies within this rectangle.</param>
		/// <returns> Returns true if the position is within the rectangle, false if not.</returns>
		bool IsPointInside(Position2D pos)
		{
			return UpperLeftCorner.X <= pos.X && UpperLeftCorner.Y <= pos.Y &&
				LowerRightCorner.X >= pos.X && LowerRightCorner.Y >= pos.Y;
		}

		/// <summary>
		/// Returns if the rectangle collides with an other rectangle.
		/// </summary>
		bool IsRectCollided(Rect other)
		{
			return (LowerRightCorner.Y > other.UpperLeftCorner.Y && UpperLeftCorner.Y < other.LowerRightCorner.Y &&
					LowerRightCorner.X > other.UpperLeftCorner.X && UpperLeftCorner.X < other.LowerRightCorner.X);
		}

		/// <summary>
		/// Clips this rectangle with another one.
		/// <summary>
		void ClipAgainst(Rect other) 
		{
			if (other.LowerRightCorner.X < LowerRightCorner.X)
				LowerRightCorner.X = other.LowerRightCorner.X;
			if (other.LowerRightCorner.Y < LowerRightCorner.Y)
				LowerRightCorner.Y = other.LowerRightCorner.Y;

			if (other.UpperLeftCorner.X > UpperLeftCorner.X)
				UpperLeftCorner.X = other.UpperLeftCorner.X;
			if (other.UpperLeftCorner.Y > UpperLeftCorner.Y)
				UpperLeftCorner.Y = other.UpperLeftCorner.Y;
		}


		/// <summary>
		/// Returns width of rectangle.
		/// </summary>
		__property int get_Width() 	{ return LowerRightCorner.X - UpperLeftCorner.X; }

		/// <summary>
		/// Sets width of rectangle.
		/// </summary>
		__property void set_Width(int w) 	{ LowerRightCorner.X = UpperLeftCorner.X + w; }

		/// <summary>
		/// Returns height of rectangle.
		/// </summary>
		__property int get_Height()	{ return LowerRightCorner.Y - UpperLeftCorner.Y; }

		/// <summary>
		/// Sets height of rectangle.
		/// </summary>
		__property void set_Height(int h)	{ LowerRightCorner.Y = UpperLeftCorner.Y + h; }

		/// <summary>
		/// If the lower right corner of the rect is smaller then the upper left,
		/// the points are swapped.
		/// </summary>
		void Repair()
		{
			if (LowerRightCorner.X < UpperLeftCorner.X)
			{
				System::Int32 t = LowerRightCorner.X;
				LowerRightCorner.X = UpperLeftCorner.X;
				UpperLeftCorner.X = t;
			}

			if (LowerRightCorner.Y < UpperLeftCorner.Y)
			{
				System::Int32 t = LowerRightCorner.Y;
				LowerRightCorner.Y = UpperLeftCorner.Y;
				UpperLeftCorner.Y = t;
			}
		}

		/// <summary>
		/// Returns if the rect is valid to draw. It could be invalid, if
		/// The UpperLeftCorner is lower or more right than the LowerRightCorner,
		/// or if the area described by the rect is 0.
		/// </summary>
		bool IsValid()
		{
			return ((LowerRightCorner.X - UpperLeftCorner.X) *
				(LowerRightCorner.Y - UpperLeftCorner.Y) >= 0);
		}

		/// <summary>
		/// Returns the center of the rectangle
		/// </summary>
		Position2D GetCenter()
		{
			return Position2D((UpperLeftCorner.X + LowerRightCorner.X) / 2,
				(UpperLeftCorner.Y + LowerRightCorner.Y) / 2);
		}

		Position2D UpperLeftCorner;
		Position2D LowerRightCorner;
	};

}
}