// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "IGUIStaticText.h"
#include "NativeConverter.h"
#include <vcclr.h> // for PtrToStringChars
#include "IGUIFont.h"

namespace Irrlicht
{
namespace GUI
{

	IGUIStaticText::IGUIStaticText(irr::gui::IGUIStaticText* e)
		: IGUIElement(e)
	{
	}


	IGUIStaticText::~IGUIStaticText()
	{
	}

	void IGUIStaticText::set_OverrideFont(IGUIFont* font)
	{
		get_NativeStaticText()->setOverrideFont(font ? font->get_NativeFont() : 0 );
	}


	void IGUIStaticText::set_OverrideColor(Video::Color color)
	{
		get_NativeStaticText()->setOverrideColor(irr::NativeConverter::getNativeColor(color));
	}


	void IGUIStaticText::set_OverrideColorEnabled(bool enable)
	{
		get_NativeStaticText()->enableOverrideColor(enable);
	}


	void IGUIStaticText::set_WordWrap(bool enable)
	{
		get_NativeStaticText()->setWordWrap(enable);
	}


	int IGUIStaticText::get_TextHeight()
	{
		return get_NativeStaticText()->getTextHeight();
	}


}
}