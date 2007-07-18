// Copyright (C) 2002-2007 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_GUI_LIST_BOX_BAR_H_INCLUDED__
#define __C_GUI_LIST_BOX_BAR_H_INCLUDED__

#include "IGUIListBox.h"
#include "irrArray.h"

namespace irr
{
namespace gui
{

	class IGUIFont;
	class IGUIScrollBar;

	class CGUIListBox : public IGUIListBox
	{
	public:

		//! constructor
		CGUIListBox(IGUIEnvironment* environment, IGUIElement* parent, 
			s32 id, core::rect<s32> rectangle, bool clip=true,
			bool drawBack=false, bool moveOverSelect=false);

		//! destructor
		~CGUIListBox();

		//! returns amount of list items
		virtual s32 getItemCount();

		//! returns string of a list item. the id may be a value from 0 to itemCount-1
		virtual const wchar_t* getListItem(s32 id);

		//! adds an list item, returns id of item
		virtual s32 addItem(const wchar_t* text);

		//! clears the list
		virtual void clear();

		//! returns id of selected item. returns -1 if no item is selected.
		virtual s32 getSelected();

		//! sets the selected item. Set this to -1 if no item should be selected
		virtual void setSelected(s32 id);

		//! called if an event happened.
		virtual bool OnEvent(SEvent event);

		//! draws the element and its children
		virtual void draw();

		//! adds an list item with an icon
		//! \param text Text of list entry
		//! \param icon Sprite index of the Icon within the current sprite bank. Set it to -1 if you want no icon
		//! \return
		//! returns the id of the new created item
		virtual s32 addItem(const wchar_t* text, s32 icon);

		//! Returns the icon of an item
		virtual s32 getIcon(s32 id) const;

		//! removes an item from the list
		virtual void removeItem(s32 id);

		//! Sets the sprite bank which should be used to draw list icons. This font is set to the sprite bank of
		//! the built-in-font by default. A sprite can be displayed in front of every list item.
		//! An icon is an index within the icon sprite bank. Several default icons are available in the
		//! skin through getIcon
		virtual void setSpriteBank(IGUISpriteBank* bank);
		
		//! sets if automatic scrolling is enabled or not. Default is true.
		virtual void setAutoScrollEnabled(bool scroll);

		//! returns true if automatic scrolling is enabled, false if not.
		virtual bool isAutoScrollEnabled();

		//! Update the position and size of the listbox, and update the scrollbar
		virtual void updateAbsolutePosition();

		//! Writes attributes of the element.
		virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options);

		//! Reads attributes of the element
		virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options);

	private:

		struct ListItem
		{
			ListItem() : icon(-1) {}

			core::stringw text;
			s32 icon;
		};

		void recalculateItemHeight();
		void selectNew(s32 ypos, bool onlyHover=false);
		void recalculateScrollPos();

		core::array< ListItem > Items;
		s32 Selected;
		s32 ItemHeight;
		s32 TotalItemHeight;
		s32 ItemsIconWidth;
		gui::IGUIFont* Font;
		gui::IGUISpriteBank* IconBank;
		gui::IGUIScrollBar* ScrollBar;
		bool Selecting;
		bool DrawBack;
		bool MoveOverSelect;
		u32 selectTime;
		bool AutoScroll;
		core::stringw KeyBuffer;
		u32 LastKeyTime;
		bool HighlightWhenNotFocused;
	};


} // end namespace gui
} // end namespace irr

#endif

