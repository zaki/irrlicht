// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#using <mscorlib.dll>
using namespace System;

#pragma unmanaged
#include "..\\..\\include\\irrlicht.h"
#pragma managed

#include "IGUIElement.h"
#include "Color.h"

namespace Irrlicht
{
namespace GUI
{
	public __gc class IGUIFont;

	/// <summary>
	/// Multi or single line text label.
	/// </summary>
	public __gc class IGUIStaticText : public IGUIElement
	{
	public:

		/// <summary>
		/// You should access the IGUIElement 
		/// through the methods in IGUIEnvironment. Simply don't use
		/// this constructor.
		///</summary>
		///<param name="font">The real, unmanaged C++ element</param>
		IGUIStaticText(irr::gui::IGUIStaticText* statictext);

		~IGUIStaticText();

		/// <summary>
		/// Sets another skin independent font.
		/// If this is set to null, the button uses the font of the skin.
		/// </summary>
		__property void set_OverrideFont(IGUIFont* font);

		/// <summary>
		/// Sets another color for the text.
		/// If set, the static text does not use the EGDC_BUTTON_TEXT color defined
        /// in the skin, but the set color instead. You don't need to call 
		/// IGUIStaticText::enableOverrrideColor(true) after this, this is done
		/// by this function.
		/// If you set a color, and you want the text displayed with the color
		/// of the skin again, call IGUIStaticText::enableOverrideColor(false);
		/// </summary>
		__property	void set_OverrideColor(Video::Color color);

		/// <summary>
		/// Sets if the static text should use the overide color or the color in the gui skin.
		/// </summary>
		__property void set_OverrideColorEnabled(bool enable);

		/// <summary>
		/// Enables or disables word wrap for using the static text as multiline text control.
		/// </summary>
		__property void set_WordWrap(bool enable);

		/// <summary>
		/// Returns the height of the text in pixels when it is drawn. 
		/// This is useful for adjusting the layout of gui elements based on the height 
		/// of the multiline text in this element. 
		/// </summary>
		__property int get_TextHeight();

		/// <summary>
		/// Returns the internal pointer to the native C++ irrlicht element.
		/// Do not use this, only needed by the internal .NET wrapper.
		///</summary>
		__property inline irr::gui::IGUIStaticText* get_NativeStaticText()
		{
			return (irr::gui::IGUIStaticText*)Element;
		}
	};

}
}
