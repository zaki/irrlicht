// Copyright (C) 2002-2007 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_GUI_LIST_BOX_BAR_H_INCLUDED__
#define __I_GUI_LIST_BOX_BAR_H_INCLUDED__

#include "IGUIElement.h"

namespace irr
{
namespace gui
{
	class IGUIFont;
	class IGUISpriteBank;

	//! Default list box GUI element.
	class IGUIListBox : public IGUIElement
	{
	public:

		//! constructor
		IGUIListBox(IGUIEnvironment* environment, IGUIElement* parent, s32 id, core::rect<s32> rectangle)
			: IGUIElement(EGUIET_LIST_BOX, environment, parent, id, rectangle) {}

		//! destructor
		~IGUIListBox() {};

		//! returns amount of list items
		virtual s32 getItemCount() = 0;

		//! returns string of a list item. the may id be a value from 0 to itemCount-1
		virtual const wchar_t* getListItem(s32 id) = 0;

		//! adds an list item, returns id of item
		virtual s32 addItem(const wchar_t* text) = 0;

		//! adds an list item with an icon
		//! \param text Text of list entry
		//! \param icon Sprite index of the Icon within the current sprite bank. Set it to -1 if you want no icon
		//! \return
		//! returns the id of the new created item
		virtual s32 addItem(const wchar_t* text, s32 icon) = 0;

		//! Removes an item from the list
		virtual void removeItem(s32 index) = 0;

		//! Returns the icon of an item
		virtual s32 getIcon(s32 id) const = 0;

		//! Sets the sprite bank which should be used to draw list icons. This font is set to the sprite bank of
		//! the built-in-font by default. A sprite can be displayed in front of every list item.
		//! An icon is an index within the icon sprite bank. Several default icons are available in the
		//! skin through getIcon
		virtual void setSpriteBank(IGUISpriteBank* bank) = 0;

		//! clears the list, deletes all items in the listbox
		virtual void clear() = 0;

		//! returns id of selected item. returns -1 if no item is selected.
		virtual s32 getSelected() = 0;

		//! sets the selected item. Set this to -1 if no item should be selected
		virtual void setSelected(s32 id) = 0;
	};


} // end namespace gui
} // end namespace irr

#endif

