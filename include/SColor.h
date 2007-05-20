// Copyright (C) 2002-2007 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __COLOR_H_INCLUDED__
#define __COLOR_H_INCLUDED__

#include "irrTypes.h"
#include "irrMath.h"

namespace irr
{
namespace video
{
	//! Creates a 16 bit A1R5G5B5 color
	inline u16 RGBA16(u32 r, u32 g, u32 b, u32 a)
	{
		return ((a & 0x80) << 8 |
			(r & 0xF8) << 7 |
			(g & 0xF8) << 2 |
			(b & 0xF8) >> 3);
	}


	//! Creates a 16 bit A1R5G5B5 color
	inline u16 RGB16(u32 r, u32 g, u32 b)
	{
		return RGBA16(r,g,b,0xFF);
	}


	//! Creates a 16 bit A1R5G5B5 color, based on 16 bit input values
	inline u16 RGB16from16(u16 r, u16 g, u16 b)
	{
		return (r & 0x1F) << 10 |
			   (g & 0x1F) << 5  |
			   (b & 0x1F);
	}


	//! Converts a 32 bit (X8R8G8B8) color to a 16 A1R5G5B5 color
	inline u16 X8R8G8B8toA1R5G5B5(u32 color)
	{
		return (	0x8000 |
			( color & 0x00F80000) >> 9 |
			( color & 0x0000F800) >> 6 |
			( color & 0x000000F8) >> 3);
	}


	//! Converts a 32 bit (A8R8G8B8) color to a 16 A1R5G5B5 color
	inline u16 A8R8G8B8toA1R5G5B5(u32 color)
	{
		return (( color & 0x80000000) >> 16|
			( color & 0x00F80000) >> 9 |
			( color & 0x0000F800) >> 6 |
			( color & 0x000000F8) >> 3);
	}


	//! Converts a 32 bit (A8R8G8B8) color to a 16 R5G6B5 color
	inline u16 A8R8G8B8toR5G6B5(u32 color)
	{
		return (( color & 0x00F80000) >> 8 |
			( color & 0x0000FC00) >> 5 |
			( color & 0x000000F8) >> 3);
	}


	//! Returns A8R8G8B8 Color from A1R5G5B5 color
	//! build a nicer 32 Bit Color by extending dest lower bits with source high bits
	inline u32 A1R5G5B5toA8R8G8B8(u32 color)
	{
		return	( (( -( (s32) color & 0x00008000 ) >> (s32) 31 ) & 0xFF000000 ) |
				(( color & 0x00007C00 ) << 9) | (( color & 0x00007000 ) << 4) |
				(( color & 0x000003E0 ) << 6) | (( color & 0x00000380 ) << 1) |
				(( color & 0x0000001F ) << 3) | (( color & 0x0000001C ) >> 2) 
				);
	}


	//! Returns A8R8G8B8 Color from R5G6B5 color
	inline u32 R5G6B5toA8R8G8B8(u16 color)
	{
		return 0xFF000000 |
			((color & 0xF800) << 8)|
			((color & 0x07E0) << 5)|
			((color & 0x001F) << 3);
	}


	//! Returns A1R5G5B5 Color from R5G6B5 color
	inline u16 R5G6B5toA1R5G5B5(u16 color)
	{
		return 0x8000 | (((color & 0xFFC0) >> 1) | (color & 0x1F));
	}


	//! Returns R5G6B5 Color from A1R5G5B5 color
	inline u16 A1R5G5B5toR5G6B5(u16 color)
	{
		return (((color & 0x7FE0) << 1) | (color & 0x1F));
	}



	//! Returns the alpha component from A1R5G5B5 color
	inline u32 getAlpha(u16 color)
	{
		return ((color >> 15)&0x1);
	}


	//! Returns the red component from A1R5G5B5 color.
	//! Shift left by 3 to get 8 bit value.
	inline u32 getRed(u16 color)
	{
		return ((color >> 10)&0x1F);
	}


	//! Returns the green component from A1R5G5B5 color
	//! Shift left by 3 to get 8 bit value.
	inline u32 getGreen(u16 color)
	{
		return ((color >> 5)&0x1F);
	}


	//! Returns the blue component from A1R5G5B5 color
	//! Shift left by 3 to get 8 bit value.
	inline u32 getBlue(u16 color)
	{
		return (color & 0x1F);
	}

		//! Returns the red component from A1R5G5B5 color.
	//! Shift left by 3 to get 8 bit value.
	inline s32 getRedSigned(u16 color)
	{
		return ((color >> 10)&0x1F);
	}


	//! Returns the green component from A1R5G5B5 color
	//! Shift left by 3 to get 8 bit value.
	inline s32 getGreenSigned(u16 color)
	{
		return ((color >> 5)&0x1F);
	}


	//! Returns the blue component from A1R5G5B5 color
	//! Shift left by 3 to get 8 bit value.
	inline s32 getBlueSigned(u16 color)
	{
		return (color & 0x1F);
	}

	//! Returns the average from a 16 bit A1R5G5B5 color
	inline s32 getAverage(s16 color)
	{
		return ((getRed(color)<<3) + (getGreen(color)<<3) + (getBlue(color)<<3)) / 3;
	}



	//! Class representing a 32 bit ARGB color.
	/** The color values for alpha, red, green, and blue are
	stored in a single s32. So all four values may be between 0 and 255.
	This class is used by most parts of the Irrlicht Engine
	to specify a color. Another way is using the class Colorf, which
	stores the color values in 4 floats.
	*/
	class SColor
	{
	public:

		//! Constructor of the Color. Does nothing. The color value
		//! is not initialized to save time.
		inline SColor() {}

		//! Constructs the color from 4 values representing the alpha, red, green and
		//! blue components of the color. Must be values between 0 and 255.
		inline SColor (u32 a, u32 r, u32 g, u32 b)
			: color(((a & 0xff)<<24) | ((r & 0xff)<<16) | ((g & 0xff)<<8) | (b & 0xff))	{}

		//! Constructs the color from a 32 bit value. Could be another color.
		inline SColor(u32 clr)
			: color(clr) {}

		//! Returns the alpha component of the color. The alpha component
		//! defines how transparent a color should be.
		//! 0 means not transparent (opaque), 255 means fully transparent.
		inline u32 getAlpha() const { return color>>24; }

		//! Returns the red component of the color.
		//! \return Returns a value between 0 and 255, specifying how red the color is.
		//! 0 means no red, 255 means full red.
		inline u32 getRed() const { return (color>>16) & 0xff; }

		//! Returns the green component of the color.
		//! \return Returns a value between 0 and 255, specifying how green the color is.
		//! 0 means no green, 255 means full green.
		inline u32 getGreen() const { return (color>>8) & 0xff; }

		//! Returns the blue component of the color.
		//! \return Returns a value between 0 and 255, specifying how blue the color is.
		//! 0 means no blue, 255 means full blue.
		inline u32 getBlue() const { return color & 0xff; }

		//! Returns the luminance of the color.
		inline f32 getLuminance() const
		{
			return 0.3f*getRed() + 0.59f*getGreen() + 0.11f*getBlue();
		}

		//! Returns the average intensity of the color.
		inline u32 getAverage() const
		{
			return ( getRed() + getGreen() + getBlue() ) / 3;
		}

		//! Sets the alpha component of the Color. The alpha component
		//! defines how transparent a color should be.
		//! \param a: Has to be a value between 0 and 255.
		//! 0 means not transparent (opaque), 255 means fully transparent.
		inline void setAlpha(u32 a) { color = ((a & 0xff)<<24) | (color & 0x00ffffff); }

		//! Sets the red component of the Color.
		//! \param r: Has to be a value between 0 and 255.
		//! 0 means no red (=black), 255 means full red.
		inline void setRed(u32 r) { color = ((r & 0xff)<<16) | (color & 0xff00ffff); }

		//! Sets the green component of the Color.
		//! \param g: Has to be a value between 0 and 255.
		//! 0 means no green (=black), 255 means full green.
		inline void setGreen(u32 g) { color = ((g & 0xff)<<8) | (color & 0xffff00ff); }

		//! Sets the blue component of the Color.
		//! \param b: Has to be a value between 0 and 255.
		//! 0 means no blue (=black), 255 means full blue.
		inline void setBlue(u32 b) { color = (b & 0xff) | (color & 0xffffff00); }

		//! Calculates a 16 bit A1R5G5B5 value of this color.
		//! \return Returns the 16 bit A1R5G5B5 value of this color.
		inline u16 toA1R5G5B5() const { return A8R8G8B8toA1R5G5B5(color); };

		//! Converts color to OpenGL color format,
		//! from ARGB to RGBA in 4 byte components for endian aware
		//! passing to OpenGL
		//! \param dest: address where the 4x8 bit OpenGL color is stored.
		inline void toOpenGLColor(u8* dest) const
		{
			*dest =   getRed();
			*++dest = getGreen();
			*++dest = getBlue();
			*++dest = getAlpha();
		};

		//! Sets all four components of the color at once.
		//! Constructs the color from 4 values representing the alpha, red, green and
		//! blue components of the color. Must be values between 0 and 255.
		//! \param a: Alpha component of the color.
		//! The alpha component defines how transparent a color should be.
		//! Has to be a value between 0 and 255.
		//! 0 means not transparent (opaque), 255 means fully transparent.
		//! \param r: Sets the red component of the Color.
		//! Has to be a value between 0 and 255.
		//! 0 means no red (=black), 255 means full red.
		//! \param g: Sets the green component of the Color.
		//! Has to be a value between 0 and 255.
		//! 0 means no green (=black), 255 means full green.
		//! \param b: Sets the blue component of the Color.
		//! Has to be a value between 0 and 255.
		//! 0 means no blue (=black), 255 means full blue.
		inline void set(u32 a, u32 r, u32 g, u32 b) { color = (((a & 0xff)<<24) | ((r & 0xff)<<16) | ((g & 0xff)<<8) | (b & 0xff)); }
		inline void set(u32 col) { color = col; }

		//! Compares the color to another color.
		//! \return Returns true if the colors are the same, and false if not.
		inline bool operator==(const SColor& other) const { return other.color == color; }

		//! Compares the color to another color.
		//! \return Returns true if the colors are different, and false if they are the same.
		inline bool operator!=(const SColor& other) const { return other.color != color; }

		//! Adds two lights
		inline SColor operator+(const SColor& other) const
		{
			s32 a = getAlpha() + other.getAlpha();
			if (a > 255) 
				a = 255;

			s32 r = getRed() + other.getRed();
			if (r > 255) 
				r = 255;

			s32 g = getGreen() + other.getGreen();
			if (g > 255) 
				g = 255;

			s32 b = getBlue() + other.getBlue();
			if (b > 255) 
				b = 255;

			return SColor(a,r,g,b);
		}

		//! Interpolates the color with a f32 value to another color
		//! \param other: Other color
		//! \param d: value between 0.0f and 1.0f
		//! \return Returns interpolated color.
		inline SColor getInterpolated(const SColor &other, f32 d) const
		{
			const f32 inv = 1.0f - d;
			return SColor((u32)(other.getAlpha()*inv + getAlpha()*d),
				(u32)(other.getRed()*inv + getRed()*d),
				(u32)(other.getGreen()*inv + getGreen()*d),
				(u32)(other.getBlue()*inv + getBlue()*d));
		}

		//! Returns interpolated color. ( quadratic )
		/** \param c1: first color to interpolate with
		    \param c2: second color to interpolate with
		\param d: value between 0.0f and 1.0f. */
		inline SColor getInterpolated_quadratic(const SColor& c1, const SColor& c2, const f32 d) const
		{
			// this*(1-d)*(1-d) + 2 * c1 * (1-d) + c2 * d * d;
			const f32 inv = 1.f - d;
			const f32 mul0 = inv * inv;
			const f32 mul1 = 2.f * d * inv;
			const f32 mul2 = d * d;

			return SColor ( core::clamp ( core::floor32 ( getAlpha() * mul0 + c1.getAlpha() * mul1 + c2.getAlpha() * mul2 ), 0, 255 ),
					core::clamp ( core::floor32 ( getRed()   * mul0 + c1.getRed()   * mul1 + c2.getRed()   * mul2 ), 0, 255 ),
					core::clamp ( core::floor32 ( getGreen() * mul0 + c1.getGreen() * mul1 + c2.getGreen() * mul2 ), 0, 255 ),
					core::clamp ( core::floor32 ( getBlue()  * mul0 + c1.getBlue()  * mul1 + c2.getBlue()  * mul2 ), 0, 255 ));
		}

		//! color in A8R8G8B8 Format
		u32 color;
	};


	//! Class representing a color with four floats.
	/**	The color values for red, green, blue
	and alpha are each stored in a 32 bit floating point variable.
	So all four values may be between 0.0f and 1.0f.
	This class is rarely used by the Irrlicht Engine
	to specify a color. Another, faster way is using the class Color, which
	stores the color values in a single 32 bit integer.
	*/
	class SColorf
	{
	public:

		//! Constructs a color. All values are initialised with 0.0f, resulting
		//! in a black color.
		SColorf() : r(0.0f), g(0.0f), b(0.0f), a(0.0f) {};

		//! Constructs a color from three color values: red, green and blue.
		//! \param r: Red color component. Should be a value between 0.0f meaning
		//! no red (=black) and 1.0f, meaning full red.
		//! \param g: Green color component. Should be a value between 0.0f meaning
		//! no green (=black) and 1.0f, meaning full green.
		//! \param b: Blue color component. Should be a value between 0.0f meaning
		//! no blue (=black) and 1.0f, meaning full blue.
		SColorf(f32 r, f32 g, f32 b) : r(r), g(g), b(b), a(1.0f) {};

		//! Constructs a color from four color values: red, green, blue, and alpha.
		//! \param r: Red color component. Should be a value between 0.0f meaning
		//! no red (=black) and 1.0f, meaning full red.
		//! \param g: Green color component. Should be a value between 0.0f meaning
		//! no green (=black) and 1.0f, meaning full green.
		//! \param b: Blue color component. Should be a value between 0.0f meaning
		//! no blue (=black) and 1.0f, meaning full blue.
		//! \param a: Alpha color component of the color.
		//! The alpha component defines how transparent a color should be.
		//! Has to be a value between 0.0f and 1.0f,
		//! 0.0f means not transparent (opaque), 1.0f means fully transparent.
		SColorf(f32 r, f32 g, f32 b, f32 a) : r(r), g(g), b(b), a(a) {};

		//! Constructs a color from 32 bit Color.
		//! \param c: 32 bit color value from which this Colorf class is
		//! constructed from.
		SColorf(SColor c) { const f32 inv = 1.0f / 255.0f; r = c.getRed() * inv; g = c.getGreen() * inv; b = c.getBlue() * inv;	a = c.getAlpha() * inv;	};

		//! Converts this color to a SColor without floats.
		SColor toSColor() const
		{
			return SColor((s32)(a*255.0f), (s32)(r*255.0f), (s32)(g*255.0f), (s32)(b*255.0f));
		}

		//! red color component
		f32 r;

		//! green color component
		f32 g;

		//! blue component
		f32 b;

		//! alpha color component
		f32 a;

		//! Sets three color components to new values at once.
		//! \param rr: Red color component. Should be a value between 0.0f meaning
		//! no red (=black) and 1.0f, meaning full red.
		//! \param gg: Green color component. Should be a value between 0.0f meaning
		//! no green (=black) and 1.0f, meaning full green.
		//! \param bb: Blue color component. Should be a value between 0.0f meaning
		//! no blue (=black) and 1.0f, meaning full blue.
		void set(f32 rr, f32 gg, f32 bb) {r = rr; g =gg; b = bb; };

		//! Sets all four color components to new values at once.
		//! \param aa: Alpha component.
		//! \param rr: Red color component. Should be a value between 0.0f meaning
		//! no red (=black) and 1.0f, meaning full red.
		//! \param gg: Green color component. Should be a value between 0.0f meaning
		//! no green (=black) and 1.0f, meaning full green.
		//! \param bb: Blue color component. Should be a value between 0.0f meaning
		//! no blue (=black) and 1.0f, meaning full blue.
		void set(f32 aa, f32 rr, f32 gg, f32 bb) {a = aa; r = rr; g =gg; b = bb; };

		//! Interpolates the color with a f32 value to another color
		//! \param other: Other color
		//! \param d: value between 0.0f and 1.0f
		//! \return Returns interpolated color.
		inline SColorf getInterpolated(const SColorf &other, f32 d) const
		{
			const f32 inv = 1.0f - d;
			return SColorf(other.r*inv + r*d,
				other.g*inv + g*d, other.b*inv + b*d, other.a*inv + a*d);
		}

		//! Returns interpolated color. ( quadratic )
		/** \param c1: first color to interpolate with
		\param c2: second color to interpolate with
		\param d: value between 0.0f and 1.0f. */
		inline SColorf getInterpolated_quadratic(const SColorf& c1, const SColorf& c2, const f32 d) const
		{
			// this*(1-d)*(1-d) + 2 * c1 * (1-d) + c2 * d * d;
			const f32 inv = 1.f - d;
			const f32 mul0 = inv * inv;
			const f32 mul1 = 2.f * d * inv;
			const f32 mul2 = d * d;

			return SColorf ( r * mul0 + c1.r * mul1 + c2.r * mul2,
					 g * mul0 + c1.g * mul1 + c2.g * mul2,
					 g * mul0 + c1.b * mul1 + c2.b * mul2,
					 a * mul0 + c1.a * mul1 + c2.a * mul2);
		}



		//! Sets a color component by index. R=0, G=1, B=2, A=3
		inline void setColorComponentValue(s32 index, f32 value)
		{
			switch(index)
			{
			case 0: r = value; break;
			case 1: g = value; break;
			case 2: b = value; break;
			case 3: a = value; break;
			}
		}
	};

	//! Class representing a color in HSV format
	/**	The color values for hue, saturation, value
	are stored in a 32 bit floating point variable.
	*/
	class SColorHSL
	{
	public:
		SColorHSL ( f32 h = 0.f, f32 s = 0.f, f32 l = 0.f )
			: Hue ( h ), Saturation ( s ), Luminance ( l ) {}

		void setfromRGB ( const SColor &color );
		void settoRGB ( SColor &color ) const;

		f32 Hue;
		f32 Saturation;
		f32 Luminance;

		private:
			inline u32 toRGB1(f32 rm1, f32 rm2, f32 rh) const;

	};

	inline void SColorHSL::settoRGB ( SColor &color ) const
	{
		if ( Saturation == 0.0f) // grey
		{
			u8 c = (u8) ( Luminance * 255.0 );
			color.setRed ( c );
			color.setGreen ( c );
			color.setBlue ( c );
			return;
		}

		f32 rm1, rm2;
			
		if ( Luminance <= 0.5f )
		{
			rm2 = Luminance + Luminance * Saturation;  
		}
		else
		{
			rm2 = Luminance + Saturation - Luminance * Saturation;
		}

		rm1 = 2.0f * Luminance - rm2;   

		color.setRed ( toRGB1(rm1, rm2, Hue + (120.0f * core::DEGTORAD )) );
		color.setGreen ( toRGB1(rm1, rm2, Hue) );
		color.setBlue ( toRGB1(rm1, rm2, Hue - (120.0f * core::DEGTORAD) ) );
	}


	inline u32 SColorHSL::toRGB1(f32 rm1, f32 rm2, f32 rh) const
	{
		while ( rh > 2.f * core::PI )
			rh -= 2.f * core::PI;

		while ( rh < 0.f )
			rh += 2.f * core::PI;

		if      (rh <  60.0f * core::DEGTORAD ) rm1 = rm1 + (rm2 - rm1) * rh / (60.0f * core::DEGTORAD);
		else if (rh < 180.0f * core::DEGTORAD ) rm1 = rm2;
		else if (rh < 240.0f * core::DEGTORAD ) rm1 = rm1 + (rm2 - rm1) * ( ( 240.0f * core::DEGTORAD ) - rh) / (60.0f * core::DEGTORAD);
		                
		return (u32) (rm1 * 255.f);
	}

} // end namespace video
} // end namespace irr

#endif

