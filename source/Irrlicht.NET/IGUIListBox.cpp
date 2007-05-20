// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "IGUIListBox.h"
#include "NativeConverter.h"
#include <vcclr.h> // for PtrToStringChars
#include "IGUIFont.h"

namespace Irrlicht
{
namespace GUI
{

	IGUIListBox::IGUIListBox(irr::gui::IGUIListBox* e)
		: IGUIElement(e)
	{
	}


	IGUIListBox::~IGUIListBox()
	{
	}

	int IGUIListBox::get_ItemCount()
	{
		return get_NativeListBox()->getItemCount();
	}

	System::String* IGUIListBox::GetListItem(int id)
	{
		return new System::String(get_NativeListBox()->getListItem(id));
	}

	int IGUIListBox::AddItem(System::String* text)
	{
		const wchar_t __pin* pinchars = PtrToStringChars(text); 
		return get_NativeListBox()->addItem(pinchars);
	}

	int IGUIListBox::AddItem(System::String* text, int icon)
	{
		const wchar_t __pin* pinchars = PtrToStringChars(text); 
		return get_NativeListBox()->addItem(pinchars, icon);
	}

	// TODO: replace with sprite bank
	/*void IGUIListBox::set_IconFont(IGUIFont* font)
	{
		get_NativeListBox()->setIconFont(font ? font->get_NativeFont() : 0);
	}*/

	void IGUIListBox::Clear()
	{
		get_NativeListBox()->clear();
	}

	int IGUIListBox::get_Selected()
	{
		return get_NativeListBox()->getSelected();
	}

	void IGUIListBox::set_Selected(int id)
	{
		get_NativeListBox()->setSelected(id);
	}
}
}