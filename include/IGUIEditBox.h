// Copyright (C) 2002-2007 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_GUI_EDIT_BOX_H_INCLUDED__
#define __I_GUI_EDIT_BOX_H_INCLUDED__

#include "IGUIElement.h"
#include "SColor.h"

namespace irr
{
namespace gui
{
	class IGUIFont;

	//! Single line edit box for editing simple text.
	class IGUIEditBox : public IGUIElement
	{
	public:

		//! constructor
		IGUIEditBox(IGUIEnvironment* environment, IGUIElement* parent, s32 id, core::rect<s32> rectangle)
			: IGUIElement(EGUIET_EDIT_BOX, environment, parent, id, rectangle) {}

		//! destructor
		~IGUIEditBox() {};

		//! Sets another skin independent font.
		/** If this is set to zero, the button uses the font of the skin.
		 \param font: New font to set. */
		virtual void setOverrideFont(IGUIFont* font=0) = 0;

		//! Sets another color for the text.
		/** If set, the edit box does not use the EGDC_BUTTON_TEXT color defined
		 in the skin, but the set color instead. You don't need to call 
		 IGUIEditBox::enableOverrrideColor(true) after this, this is done
		 by this function.
		 If you set a color, and you want the text displayed with the color
		 of the skin again, call IGUIEditBox::enableOverrideColor(false);
		 \param color: New color of the text. */
		virtual void setOverrideColor(video::SColor color) = 0;

		//! Sets if the text should use the overide color or the color in the gui skin.
		/** \param enable: If set to true, the override color, which can be set
		 with IGUIEditBox::setOverrideColor is used, otherwise the
		 EGDC_BUTTON_TEXT color of the skin. */
		virtual void enableOverrideColor(bool enable) = 0;

		//! Sets the maximum amount of characters which may be entered in the box.
		/** \param max: Maximum amount of characters. If 0, the character amount is 
		 infinity. */
		virtual void setMax(s32 max) = 0;

		//! Returns maximum amount of characters, previously set by setMax();
		virtual s32 getMax() = 0;
	};


} // end namespace gui
} // end namespace irr

#endif

