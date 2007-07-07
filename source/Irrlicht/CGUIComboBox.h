// Copyright (C) 2002-2007 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_GUI_COMBO_BOX_H_INCLUDED__
#define __C_GUI_COMBO_BOX_H_INCLUDED__

#include "IGUIComboBox.h"
#include "irrString.h"
#include "irrArray.h"

namespace irr
{
namespace gui
{
	//! Single line edit box for editing simple text.
	class CGUIComboBox : public IGUIComboBox
	{
	public:

		//! constructor
		CGUIComboBox(IGUIEnvironment* environment, IGUIElement* parent,
			s32 id, core::rect<s32> rectangle);

		//! destructor
		~CGUIComboBox();

		//! Returns amount of items in box
		virtual s32 getItemCount();

		//! returns string of an item. the idx may be a value from 0 to itemCount-1
		virtual const wchar_t* getItem(s32 idx);

		//! adds an item and returns the index of it
		virtual s32 addItem(const wchar_t* text);

		//! Removes an item from the combo box.
		virtual void removeItem(s32 id);

		//! deletes all items in the combo box
		virtual void clear();

		//! returns the text of the currently selected item
		virtual const wchar_t* getText();

		//! returns id of selected item. returns -1 if no item is selected.
		virtual s32 getSelected();

		//! sets the selected item. Set this to -1 if no item should be selected
		virtual void setSelected(s32 id);

		//! update the position
		virtual void updateAbsolutePosition();

		//! called if an event happened.
		virtual bool OnEvent(SEvent event);

		//! draws the element and its children
		virtual void draw();

		//! Writes attributes of the element.
		virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options);

		//! Reads attributes of the element
		virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options);

	private:

		void openCloseMenu();
		void sendSelectionChangedEvent();

		IGUIButton* ListButton;
		IGUIListBox* ListBox;
		core::array< core::stringw > Items;
		s32 Selected;
		bool HasFocus;
		IGUIElement *LastFocus;
	};


} // end namespace gui
} // end namespace irr

#endif

