// Copyright (C) 2002-2007 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_GUI_STATIC_TEXT_H_INCLUDED__
#define __C_GUI_STATIC_TEXT_H_INCLUDED__

#include "IGUIStaticText.h"
#include "irrArray.h"

namespace irr
{
namespace gui
{
	class CGUIStaticText : public IGUIStaticText
	{
	public:

		//! constructor
		CGUIStaticText(const wchar_t* text, bool border, IGUIEnvironment* environment,
			IGUIElement* parent, s32 id, const core::rect<s32>& rectangle,
			bool background = false);

		//! destructor
		~CGUIStaticText();

		//! draws the element and its children
		virtual void draw();

		//! Sets another skin independent font.
		virtual void setOverrideFont(IGUIFont* font=0);

		//! Gets the override font (if any)
		virtual IGUIFont * getOverrideFont(void);

		//! Sets another color for the text.
		virtual void setOverrideColor(video::SColor color);

		//! Sets another color for the background.
		virtual void setBackgroundColor(video::SColor color);

		//! Sets whether to draw the background
		virtual void setDrawBackground(bool draw);

		//! Gets the override color
		virtual video::SColor const & getOverrideColor(void);

		//! Sets if the static text should use the overide color or the
		//! color in the gui skin.
		virtual void enableOverrideColor(bool enable);

		//! Checks if an override color is enabled
		virtual bool isOverrideColorEnabled(void);

		//! Enables or disables word wrap for using the static text as
		//! multiline text control.
		virtual void setWordWrap(bool enable);

		//! Checks if word wrap is enabled
		virtual bool isWordWrapEnabled(void);

		//! Sets the new caption of this element.
		virtual void setText(const wchar_t* text);

		//! Returns the height of the text in pixels when it is drawn.
		virtual s32 getTextHeight();

		//! Returns the width of the current text, in the current font
		virtual s32 getTextWidth(void);

		//! Writes attributes of the element.
		virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options);

		//! Reads attributes of the element
		virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options);

	private:

		//! Breaks the single text line.
		void breakText();

		bool Border;
		bool OverrideColorEnabled;
		bool WordWrap;
		bool Background;

		video::SColor OverrideColor, BGColor;
		gui::IGUIFont* OverrideFont;
		gui::IGUIFont* LastBreakFont; // stored because: if skin changes, line break must be recalculated.

		core::array< core::stringw > BrokenText;
	};

} // end namespace gui
} // end namespace irr

#endif


