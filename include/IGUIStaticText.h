// Copyright (C) 2002-2007 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_GUI_STATIC_TEXT_H_INCLUDED__
#define __I_GUI_STATIC_TEXT_H_INCLUDED__

#include "IGUIElement.h"
#include "SColor.h"

namespace irr
{
namespace gui
{
	class IGUIFont;

	//! Multi or single line text label.
	class IGUIStaticText : public IGUIElement
	{
	public:

		//! constructor
		IGUIStaticText(IGUIEnvironment* environment, IGUIElement* parent, s32 id, core::rect<s32> rectangle)
			: IGUIElement(EGUIET_STATIC_TEXT, environment, parent, id, rectangle) {}

		//! destructor
		~IGUIStaticText() {};

		//! Sets another skin independent font.
		/** If this is set to zero, the button uses the font of the skin.
		\param font: New font to set. */
		virtual void setOverrideFont(IGUIFont* font=0) = 0;

		//! Gets the override font (if any)
		//! \return The override font (may be 0)
		virtual IGUIFont * getOverrideFont(void) = 0;

		//! Sets another color for the text.
		/** If set, the static text does not use the EGDC_BUTTON_TEXT color defined
         in the skin, but the set color instead. You don't need to call 
		 IGUIStaticText::enableOverrrideColor(true) after this, this is done
		 by this function.
		 If you set a color, and you want the text displayed with the color
		 of the skin again, call IGUIStaticText::enableOverrideColor(false);
		 \param color: New color of the text. */
		virtual void setOverrideColor(video::SColor color) = 0;

		//! Gets the override color
		//! \return: The override color
		virtual video::SColor const & getOverrideColor(void) = 0;

		//! Sets if the static text should use the overide color or the color in the gui skin.
		/** \param enable: If set to true, the override color, which can be set
		with IGUIStaticText::setOverrideColor is used, otherwise the
		EGDC_BUTTON_TEXT color of the skin. */
		virtual void enableOverrideColor(bool enable) = 0;

		//! Checks if an override color is enabled
		//! \return true if the override color is enabled, false otherwise
		virtual bool isOverrideColorEnabled(void) = 0;

		//! Enables or disables word wrap for using the static text as multiline text control.
		/** \param enable: If set to true, words going over one line are 
		 breaked to the next line. */
		virtual void setWordWrap(bool enable) = 0;

		//! Checks if word wrap is enabled
		//! \return true if word wrap is enabled, false otherwise
		virtual bool isWordWrapEnabled(void) = 0;

		//! Returns the height of the text in pixels when it is drawn. 
		/** This is useful for adjusting the layout of gui elements based on the height 
		 of the multiline text in this element. 
		 \return Returns height of text in pixels. */
		virtual s32 getTextHeight() = 0;

		//! Returns the width of the current text, in the current font
		/** If the text is broken, this returns the width of the widest line
		 \return The width of the text, or the widest broken line. */
		virtual s32 getTextWidth(void) = 0;

	};


} // end namespace gui
} // end namespace irr

#endif


