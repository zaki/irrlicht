// Copyright (C) 2002-2007 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_GUI_EDIT_BOX_H_INCLUDED__
#define __C_GUI_EDIT_BOX_H_INCLUDED__

#include "IGUIEditBox.h"
#include "irrArray.h"
#include "IOSOperator.h"

namespace irr
{
namespace gui
{
	class CGUIEditBox : public IGUIEditBox
	{
	public:

		//! constructor
		CGUIEditBox(const wchar_t* text, bool border, IGUIEnvironment* environment,
			IGUIElement* parent, s32 id, const core::rect<s32>& rectangle);

		//! destructor
		~CGUIEditBox();

		//! Sets another skin independent font.
		virtual void setOverrideFont(IGUIFont* font=0);

		//! Sets another color for the text.
		virtual void setOverrideColor(video::SColor color);

		//! Sets if the text should use the overide color or the
		//! color in the gui skin.
		virtual void enableOverrideColor(bool enable);

		//! called if an event happened.
		virtual bool OnEvent(SEvent event);

		//! draws the element and its children
		virtual void draw();

		//! Sets the new caption of this element.
		virtual void setText(const wchar_t* text);

		//! Sets the maximum amount of characters which may be entered in the box.
		//! \param max: Maximum amount of characters. If 0, the character amount is 
		//! infinity.
		virtual void setMax(s32 max);

		//! Returns maximum amount of characters, previously set by setMax();
		virtual s32 getMax();

		//! Writes attributes of the element.
		virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options);

		//! Reads attributes of the element
		virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options);

	protected:

		bool processKey(const SEvent& event);
		bool processMouse(const SEvent& event);
		s32 getCursorPos(s32 x);

		bool MouseMarking;
		bool Border;
		bool OverrideColorEnabled;
		s32 MarkBegin;
		s32 MarkEnd;

		video::SColor OverrideColor;
		gui::IGUIFont* OverrideFont;
		IOSOperator* Operator;

		u32 BlinkStartTime;
		s32 CursorPos;
		s32 ScrollPos; // scrollpos in characters
		s32 Max;
	};

} // end namespace gui
} // end namespace irr

#endif

