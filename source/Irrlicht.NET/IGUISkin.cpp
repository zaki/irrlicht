// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "IGUISkin.h"
#include "IGUIFont.h"
#include "NativeConverter.h"
#include <vcclr.h> // for PtrToStringChars

namespace Irrlicht
{
namespace GUI
{

	IGUISkin::IGUISkin(irr::gui::IGUISkin* s)
		: Skin(s)
	{
		Skin->grab();
	}

	IGUISkin::~IGUISkin()
	{
		Skin->drop();
	}

	irr::gui::IGUISkin* IGUISkin::get_NativeSkin()
	{
		return Skin;
	}

	Video::Color IGUISkin::GetColor(SkinColor color)
	{
		return Irrlicht::Video::Color(
			Skin->getColor((irr::gui::EGUI_DEFAULT_COLOR)color).color);
	}

	void IGUISkin::SetColor(SkinColor which, Video::Color newColor)
	{
		Skin->setColor((irr::gui::EGUI_DEFAULT_COLOR)which, 
			newColor.color);
	}

	int IGUISkin::GetSize(SkinSize size)
	{
		return Skin->getSize((irr::gui::EGUI_DEFAULT_SIZE)size);
	}

	IGUIFont* IGUISkin::get_Font()
	{
		irr::gui::IGUIFont* font = Skin->getFont();
		if (!font)
			return 0;
		return new IGUIFont(font);
	}

	void IGUISkin::set_Font(IGUIFont* font)
	{
		if (font)
			Skin->setFont(font->get_NativeFont());
		else
			Skin->setFont(0);
	}

}
}