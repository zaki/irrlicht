// Copyright (C) 2002-2007 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_GUI_CONTEXT_MENU_H_INCLUDED__
#define __I_GUI_CONTEXT_MENU_H_INCLUDED__

#include "IGUIElement.h"

namespace irr
{
namespace gui
{

	//! GUI Context menu interface.
	class IGUIContextMenu : public IGUIElement
	{
	public:

		//! constructor
		IGUIContextMenu(IGUIEnvironment* environment, IGUIElement* parent, s32 id, core::rect<s32> rectangle)
			: IGUIElement(EGUIET_CONTEXT_MENU, environment, parent, id, rectangle) {}

		//! destructor
		~IGUIContextMenu() {};

		//! Get amount of menu items
		virtual s32 getItemCount() const = 0;

		//! Adds a menu item.
		/** \param text: Text of menu item. Set this to 0 to create
		 an separator instead of a real item, which is the same like
		 calling addSeparator();
		 \param commandId: Command id of menu item, a simple id you may 
		 set to whatever you want.
		 \param enabled: Specifies if the menu item should be enabled.
		 \param hasSubMenu: Set this to true if there should be a submenu
		 at this item. You can acess this submenu via getSubMenu().
		 \param checked: Specifies if the menu item should be initially checked.
		 \return Returns the index of the new item */
		virtual s32 addItem(const wchar_t* text, s32 commandId=-1, bool enabled=true,
			bool hasSubMenu=false,
			bool checked=false
			) = 0;

		//! Adds a separator item to the menu
		virtual void addSeparator() = 0;

		//! Get text of the menu item.
		/** \param idx: Zero based index of the menu item */
		virtual const wchar_t* getItemText(s32 idx) = 0;

		//! Sets text of the menu item.
		/** \param idx: Zero based index of the menu item
		 \param text: New text of the item. */
		virtual void setItemText(s32 idx, const wchar_t* text) = 0;

		//! Check if a menu item is enabled
		/** \param idx: Zero based index of the menu item */
		virtual bool isItemEnabled(s32 idx) = 0;

		//! Sets if the menu item should be enabled.
		/** \param idx: Zero based index of the menu item
		 \param enabled: True if it is enabled, otherwise false. */
		virtual void setItemEnabled(s32 idx, bool enabled) = 0;

		//! Sets if the menu item should be checked.
		/** \param idx: Zero based index of the menu item
		 \param enabled: True if it is enabled, otherwise false. */
		virtual void setItemChecked(s32 idx, bool enabled) = 0;

		//! Check if a menu item is checked
		/** \param idx: Zero based index of the menu item */
		virtual bool isItemChecked(s32 idx) = 0;

		//! Removes a menu item
		/** \param idx: Zero based index of the menu item */
		virtual void removeItem(s32 idx) = 0;

		//! Removes all menu items
		virtual void removeAllItems() = 0;

		//! Get the selected item in the menu
		/** \return Index of the selected item, -1 if none selected. */
		virtual s32 getSelectedItem() = 0;

		//! Get the command id of a menu item
		/** \param idx: Zero based index of the menu item */
		virtual s32 getItemCommandId(s32 idx) = 0;

		//! Sets the command id of a menu item
		/** \param idx: Zero based index of the menu item
		 \param id: Command id of menu item, a simple id you may 
		 set to whatever you want. */
		virtual void setItemCommandId(s32 idx, s32 id) = 0;

		//! Get a pointer to the submenu of an item. 
		/** 0 is returned if there is no submenu
		 \param idx: Zero based index of the menu item
		 \return Returns a pointer to the submenu of an item. */
		virtual IGUIContextMenu* getSubMenu(s32 idx) = 0;
	};

} // end namespace gui
} // end namespace irr

#endif

