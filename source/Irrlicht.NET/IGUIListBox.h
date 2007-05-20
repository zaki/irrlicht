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

namespace Irrlicht
{
namespace GUI
{
	public __gc class IGUIFont;

	/// <summary>
	/// Default list box GUI element.
	/// </summary>
	public __gc class IGUIListBox : public IGUIElement
	{
	public:

		/// <summary>
		/// You should access the IGUIElement 
		/// through the methods in IGUIEnvironment. Simply don't use
		/// this constructor.
		///</summary>
		///<param name="font">The real, unmanaged C++ element</param>
		IGUIListBox(irr::gui::IGUIListBox* listBox);

		~IGUIListBox();

		/// <summary>
		/// returns amount of list items
		/// </summary>
		__property int get_ItemCount();

		/// <summary>
		/// returns string of a list item. the may id be a value from 0 to itemCount-1
		/// </summary>
		System::String* GetListItem(int id);

		/// <summary>
		/// adds an list item, returns id of item
		/// </summary>
		int AddItem(System::String* text);

		/// <summary>
		/// adds an list item with an icon
		/// </summary>
		/// <param name="text">Text of list entry</param>
		/// <param name="icon">Icon index.</param>
		/// <returns> returns the id of the new created item</returns>
		int AddItem(System::String* text, int icon);

		/// <summary>
		/// Sets the font which should be used as icon font. This font is set to the Irrlicht engine
		/// built-in-font by default. Icons can be displayed in front of every list item.
		/// An icon is a string, displayed with the icon font. When using the build-in-font of the
		/// Irrlicht engine as icon font, the icon strings defined in GUIIcons.h can be used.
		/// </summary>
		//__property void set_IconFont(IGUIFont* font);

		/// <summary>
		/// clears the list, deletes all items in the listbox
		/// </summary>
		void Clear();

		/// <summary>
		/// returns or sets the id of selected item. returns -1 if no item is selected.
		/// </summary>
		__property int get_Selected();

		/// <summary>
		/// returns or sets the id of selected item. returns -1 if no item is selected.
		/// </summary>
		__property void set_Selected(int id);

		/// <summary>
		/// Returns the internal pointer to the native C++ irrlicht element.
		/// Do not use this, only needed by the internal .NET wrapper.
		///</summary>
		__property inline irr::gui::IGUIListBox* get_NativeListBox()
		{
			return (irr::gui::IGUIListBox*)Element;
		}
	};

}
}
