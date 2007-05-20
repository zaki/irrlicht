// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once 

namespace Irrlicht
{
namespace Video
{
	/// <summary>
	/// Class representing a 32 bit ARGB color.
	/// The color values for red, green, blue
	/// and alpha are stored in a single System::Int32. So all four values may be between 0 and 255.
	/// This class is used by most parts of the Irrlicht Engine
	/// to specify a color. An other way is using the class Colorf, which
	/// stores the color values in 4 floats.
	/// </summary>
	public __value class Color
	{
	public:

		/// <summary> Constructor of the Color. Does nothing.
		/// This class has been ported directly from the native C++ Irrlicht Engine, so it may not 
		/// be 100% complete yet and the design may not be 100% .NET like.
		/// </summary>
		inline Color() {}

		/// <summary> 
		/// Constructs the color from 4 values representing the alpha, red, green and 
		/// blue components of the color. Must be values between 0 and 255.
		/// </summary> 
		inline Color (System::Int32 a, System::Int32 r, System::Int32 g, System::Int32 b)
		{
			color =  ((a & 0xff)<<24) | 
				     ((r & 0xff)<<16) | 
					 ((g & 0xff)<<8)  |
					  (b & 0xff);
		}

		/// <summary> 
		/// Constructs the color from a 32 bit value. Could be another color.
		/// </summary> 
		inline Color(System::Int32 clr)
		{
			color = clr;
		}

		static bool op_Equality(Color o1, Color o2)
		{
			return (o1.color == o2.color);	
		}

		/// <summary> 
		/// Returns the alpha component of the color. The alpha component
		/// defines how transparent a color should be.
		/// 0 means not transparent, 255 means fully transparent.
		/// </summary> 
		__property inline System::Int32 get_Alpha() {	return (color>>24) & 0xff;	}

		/// <summary> 
		/// Returns the red component of the color. 
		/// </summary> 
		/// <returns> Returns a value between 0 and 255, specifying how red the color is.
		/// 0 means dark, 255 means full red. </returns>
		__property inline System::Int32 get_Red() {	return (color>>16) & 0xff;	}

		/// <summary> 
		/// Returns the green component of the color. 
		/// </summary> 
		/// <returns> Returns a value between 0 and 255, specifying how green the color is.
		/// 0 means dark, 255 means full green. </returns>
		__property inline System::Int32 get_Green() {	return (color>>8) & 0xff;	}

		/// <summary> 
		/// Returns the blue component of the color. 
		/// </summary> 
		/// <returns>Returns a value between 0 and 255, specifying how blue the color is.
		/// 0 means dark, 255 means full blue. </returns>
		__property inline System::Int32 get_Blue() {	return color & 0xff;	}

		/// <summary> 
		/// Sets the alpha comonent of the Color. The alpha component
		/// defines how transparent a color should be.
		/// </summary>
		/// <param name="a">Has to be a value between 0 and 255. 
		/// 0 means not transparent, 255 means fully transparent.</param>
		__property inline void set_Alpha(System::Int32 a) { color = ((a & 0xff)<<24) | (((color>>16)& 0xff)<<16) | ((color>>8 & 0xff)<<8) | (color & 0xff); }

		/// <summary>
		/// Sets the red comonent of the Color. 
		/// </summary>
		/// <param name"r"> Has to be a value between 0 and 255. 
		/// 0 means dark red (=black), 255 means full red. </param>
		__property inline void set_Red(System::Int32 r) { color = (((color>>24) & 0xff)<<24) | ((r & 0xff)<<16) | ((color>>8 & 0xff)<<8) | (color & 0xff); }

		/// <summary>
		/// Sets the green comonent of the Color. 
		/// </summary>
		/// <param name="g"> Has to be a value between 0 and 255. 
		/// 0 means dark green (=black), 255 means full green.</param>
		__property inline void set_Green(System::Int32 g) { color = (((color>>24) & 0xff)<<24) | (((color>>16)& 0xff)<<16) | ((g & 0xff)<<8) | (color & 0xff); }

		/// <summary>
		/// Sets the blue comonent of the Color. 
		/// </summary>
		/// <param name="b"> Has to be a value between 0 and 255. 
		/// 0 means dark blue (=black), 255 means full blue.</param>
		__property inline void set_Blue(System::Int32 b) { color = (((color>>24) & 0xff)<<24) | (((color>>16)& 0xff)<<16) | ((color>>8 & 0xff)<<8) | (b & 0xff); }

		/// <summary>
		/// Calculates a 16 bit A1R5G5B5 value of this color.
		/// </summary>
		/// <returns>Returns the 16 bit A1R5G5B5 value of this color.</returns>
		inline System::Int16 ToA1R5G5B5() 
		{
			return ((((color>>16)>>3) & 0x1F)<<10) |
				((((color>>8)>>3) & 0x1F)<<5) |
				((color>>3) & 0x1F);
		};

		/// <summary>
		/// Converts color to open gl color format.
		/// </summary>
		/// <returns>Returns the 32 bit openGL color value.</returns>
		inline System::Int32 ToOpenGLColor() 
		{
			return (((color>>24) & 0xff)<<24) |
					(((color)& 0xff)<<16) |
					((color>>8 & 0xff)<<8) |
					((color>>16) & 0xff);
		};

		/// <summary>
		/// Sets all four components of the color at once.
		/// Constructs the color from 4 values representing the alpha, red, green and 
		/// blue components of the color. Must be values between 0 and 255.
		/// </summary>
		/// <param name="a"> Alpha component of the color. 
		/// The alpha component defines how transparent a color should be.
		/// Has to be a value between 0 and 255. 
		/// 0 means not transparent, 255 means fully transparent.</param>
		/// <param name="r"> Sets the red comonent of the Color. 
		/// Has to be a value between 0 and 255.
		/// 0 means dark red (=black), 255 means full red.</param>
		/// <param name="g"> Sets the green comonent of the Color. 
		/// Has to be a value between 0 and 255. 
		/// 0 means dark green (=black), 255 means full green.</param>
		/// <param name="b">Sets the blue comonent of the Color. 
		/// Has to be a value between 0 and 255. 
		/// 0 means dark blue (=black), 255 means full blue.</param>
		inline void Set(System::Int32 a, System::Int32 r, System::Int32 g, System::Int32 b) { color = (((a & 0xff)<<24) | ((r & 0xff)<<16) | ((g & 0xff)<<8) | (b & 0xff)); }

		/// <summary>
		/// Compares the color to another color.
		/// </summary>
		/// <returns>Returns true if the colors are the same, and false if not.</returns>
		bool Equals(Object* rhs) 
		{
			Color* c = dynamic_cast<Color *>(rhs);

			if(!c) 
				return false;

			return c->color == color;
		}

		/// <summary>
		/// Compares the color to another color.
		/// </summary>
		/// <returns>Returns true if the colors are different, and false if they are the same.</returns>
		static bool op_Inequality(Color m1, Color m2)
		{
			return m1.color != m2.color;
		}

		/// <summary>
		/// Interpolates the color with a float value to an other color. The float must be between 0 and 1.
		/// </summary>
		inline Color GetInterpolated(Color other, float d) 
		{
			float inv = 1.0f - d;
			return Color((System::Int32)(other.Alpha*inv + Alpha*d),
				(System::Int32)(other.Red*inv + Red*d),
				(System::Int32)(other.Green*inv + Green*d),
				(System::Int32)(other.Blue*inv + Blue*d));
		}
        
		/// <summary>
		/// color in A8R8G8B8 Format
		/// </summary>
		System::Int32 color; 
	};


	/// <summary>
	/// Class representing a color with four floats.
	///	The color values for red, green, blue
	/// and alpha are each stored in a 32 bit floating point variable.
	///	So all four values may be between 0.0f and 1.0f.
	///	This class is used rarely used by the Irrlicht Engine
	///	to specify a color. An other, faster way is using the class Color, which
	///	stores the color values in a single 32 bit integer. 
	/// This class has been ported directly from the native C++ Irrlicht Engine, so it may not 
	/// be 100% complete yet and the design may not be 100% .NET like.
	/// </summary>
	public __value class Colorf
	{
	public:

		/// <summary>
		/// Constructs a color. All values are initialised with 0.0f, resulting
		/// in a black color.
		/// </summary>
		Colorf() : r(0.0f), g(0.0f), b(0.0f), a(0.0f) {};

		/// <summary>
		/// Constructs a color from three color values: red, green and blue.
		/// </summary>
		/// <param name="r">Red color component. Should be a value between 0.0f meaning
		/// very dark red (=black) and 1.0f, meaning full red.</param>
		/// <param name="g">Green color component. Should be a value between 0.0f meaning
		/// very dark green (=black) and 1.0f, meaning full green.</param>
		/// <param name="b"> Blue color component. Should be a value between 0.0f meaning
		/// very dark blue (=black) and 1.0f, meaning full blue.</param>
		Colorf(float r, float g, float b) : r(r), g(g), b(b), a(1.0f) {};

		/// <summary>
		/// Constructs a color from four color values: alpha, red, green and blue.
		/// </summary>
		/// <param name="a"> Alpha color component of the color.
		/// The alpha component defines how transparent a color should be.
		/// Has to be a value between 0.0f and 1.0f,  
		/// 0.0f means not transparent, 1.0f means fully transparent.</param>
		/// <param name="r"> Red color component. Should be a value between 0.0f meaning
		/// very dark red (=black) and 1.0f, meaning full red.</param>
		/// <param name="g"> Green color component. Should be a value between 0.0f meaning
		/// very dark green (=black) and 1.0f, meaning full green.</param>
		/// <param name="b"> Blue color component. Should be a value between 0.0f meaning
		/// very dark blue (=black) and 1.0f, meaning full blue.</param>
		Colorf(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {};

		/// <summary>
		/// Constructs a color from 32 bit Color.
		/// </summary>
		/// <param name="c">32 bit color value from which this Colorf class is 
		/// constructed from.</param>
		Colorf(Color c) 
		{ 
			const float inv = 1.0f / 255.0f; 
			r = c.Red * inv;
			g = c.Green * inv; 
			b = c.Blue * inv;
			a = c.Alpha * inv;	
		};

		/// <summary>
		/// Converts this color to a Color without floats.
		/// </summary>
		Color toSColor()
		{
			return Color((System::Int32)(a*255.0f), (System::Int32)(r*255.0f), (System::Int32)(g*255.0f), (System::Int32)(b*255.0f));
		}

		/// <summary>
		/// Sets the three color comonents to new values at once.
		/// </summary>
		/// <param name="rr"> Red color component. Should be a value between 0.0f meaning
		/// very dark red (=black) and 1.0f, meaning full red.</param>
		/// <param name="gg">Green color component. Should be a value between 0.0f meaning
		/// very dark green (=black) and 1.0f, meaning full green.</param>
		/// <param name="bb">Blue color component. Should be a value between 0.0f meaning
		/// very dark blue (=black) and 1.0f, meaning full blue.</param>
		void Set(float rr, float gg, float bb)
		{
			r = rr;
			g =gg;
			b = bb; 
		}

		/// <summary>
		/// red color component
		/// </summary>
		float r;

		/// <summary>
		/// green color component
		/// </summary>
		float g;

		/// <summary>
		/// blue component
		/// </summary>
		float b;

		/// <summary>
		/// alpha color component
		/// </summary>
		float a;
	};

}
}