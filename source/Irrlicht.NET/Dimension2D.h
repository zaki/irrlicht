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
	/// Specifies a two dimensional size. 
	/// </summary>
	public __value class Dimension2D
	{
		public:

			/// <summary> Constructs a dimension of widht 0 and height 0. </summary>
			Dimension2D()
				: Width(0), Height(0)
			{
			}
				
			/// <summary> Constructs a dimension of specified size. </summary>
			Dimension2D(int width, int height)
				: Width(width), Height(height) 
			{
			}

			/// <summary>
			/// Compares the size to another size.
			/// </summary>
			bool Equals(Object* rhs) 
			{
				Dimension2D* c = dynamic_cast<Dimension2D *>(rhs);

				if(!c) 
					return false;

				return c->Width == Width && c->Height == Height;
			}

			int Width, Height;
	};


	/// <summary> 
	/// Specifies a two dimensional size. 
	/// </summary>
	public __value class Dimension2Df
	{
		public:

			/// <summary> Constructs a dimension of widht 0 and height 0. </summary>
			Dimension2Df()
				: Width(0), Height(0)
			{
			}
				
			/// <summary> Constructs a dimension of specified size. </summary>
			Dimension2Df(float width, float height)
				: Width(width), Height(height) 
			{
			}

			/// <summary>
			/// Compares the size to another size.
			/// </summary>
			bool Equals(Object* rhs) 
			{
				Dimension2Df* c = dynamic_cast<Dimension2Df *>(rhs);

				if(!c) 
					return false;

				return c->Width == Width && c->Height == Height;
			}

			float Width, Height;
	};

} // end namespace Core
} // end namespace Irrlicht


