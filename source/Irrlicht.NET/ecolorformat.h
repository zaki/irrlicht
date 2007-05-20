// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once


namespace Irrlicht
{
namespace Video
{

/// <summary>
/// An enum for the color format of textures used by the Irrlicht Engine.
/// A color format specifies how color information is stored. The Irrlicht Engine
/// mostly uses the format A1R5G5B5.
/// </summary>
public __value enum ColorFormat
{
	/// <summary>
	/// 16 bit color format used by the software driver, and thus preferred
	/// by all other irrlicht engine video drivers. There are 5 bits for every
	/// color component, and a single bit is left for alpha information.
	/// </summary>
	A1R5G5B5 = 0,	

	/// <summary> Standard 16 bit color format. </summary>
	R5G6B5,

	/// <summary> 24 bit color, no alpha channel, but 8 bit for red, green and blue. </summary>
	R8G8B8,

	/// <summary>
	/// Default 32 bit color format. 8 bits are used for every component:
	/// red, green, blue and alpha.
	/// <summary>
	A8R8G8B8,
};


}
}