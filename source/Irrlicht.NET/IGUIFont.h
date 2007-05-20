// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#using <mscorlib.dll>
using namespace System;

#pragma unmanaged
#include "..\\..\\include\\irrlicht.h"
#pragma managed

#include "Rect.h"
#include "Color.h"

namespace Irrlicht
{
namespace GUI
{
	public __gc class IGUIFont
	{
	public:

		/// <summary>
		/// You should access the IGUIFont 
		/// through the IGUIEnvironment::getFont() property. Simply don't use
		/// this constructor.
		///</summary>
		///<param name="font">The real, unmanaged C++ font</param>
		IGUIFont(irr::gui::IGUIFont* font);

		~IGUIFont();

		/// <summary>
		/// Draws an text and clips it to the specified rectangle.
		/// </summary>
		/// <param name="text"> Text to draw </param>
		/// <param name="position> Rectangle specifying position where to draw the text.
		/// <param name="color> Color of the text </param>
		/// <param name="hcenter> Specifiies if the text should be centered horizontally into the rectangle.</param>
		/// <param name="vcenter> Specifiies if the text should be centered vertically into the rectangle.</param>
		/// <param name="clip> Rectangle against which the text will be clipped.</param>
		void Draw(System::String* text, Core::Rect position, 
			Video::Color color, bool hcenter, bool vcenter,
			Core::Rect cliprect);

		/// <summary>
		/// Draws an text.
		/// </summary>
		/// <param name="text"> Text to draw </param>
		/// <param name="position> Position where to draw the text.
		/// <param name="color> Color of the text </param>
		void Draw(System::String* text, Core::Position2D position, Video::Color color);

		/// <summary>
		/// Draws an text.
		/// </summary>
		/// <param name="text"> Text to draw </param>
		/// <param name="position> Rectangle specifying position where to draw the text.
		/// <param name="color> Color of the text </param>
		/// <param name="hcenter> Specifiies if the text should be centered horizontally into the rectangle.</param>
		/// <param name="vcenter> Specifiies if the text should be centered vertically into the rectangle.</param>
		void Draw(System::String* text, Core::Rect position, 
			Video::Color color, bool hcenter, bool vcenter);

		/// <summary>
		/// Calculates the dimension of a text.
		/// </summary>
		/// <returns> Returns width and height of the area covered by the text if it would be
		/// drawn. </returns>
		Core::Dimension2D GetDimension(System::String* text);

		/// <summary>
		/// Returns the internal pointer to the native C++ irrlicht font.
		/// Do not use this, only needed by the internal .NET wrapper.
		///</summary>
		__property irr::gui::IGUIFont* get_NativeFont();

	private:

		irr::gui::IGUIFont* Font;
	};

}
}
