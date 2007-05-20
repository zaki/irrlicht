// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "IGUIFont.h"
#include "NativeConverter.h"

namespace Irrlicht
{
namespace GUI
{

	IGUIFont::IGUIFont(irr::gui::IGUIFont* fnt)
		: Font(fnt)
	{
		Font->grab();
	}


	IGUIFont::~IGUIFont()
	{
		if ( Font )
			Font->drop();
		Font = 0;
	}

	void IGUIFont::Draw(System::String* text, Core::Rect position, 
		Video::Color color, bool hcenter, bool vcenter,
		Core::Rect cliprect)
	{
		char* str = (char*)(void*)
			System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(text);

		irr::core::rect<irr::s32> r = irr::NativeConverter::getNativeRect(cliprect);

		Font->draw(irr::core::stringw(str).c_str(), 
			irr::NativeConverter::getNativeRect(position),
			color.color, hcenter, vcenter, 
			&r);

		System::Runtime::InteropServices::Marshal::FreeHGlobal(str);
	}

	void IGUIFont::Draw(System::String* text, Core::Rect position, 
		Video::Color color, bool hcenter, bool vcenter)
	{
		char* str = (char*)(void*)
			System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(text);

		Font->draw(irr::core::stringw(str).c_str(), 
			irr::NativeConverter::getNativeRect(position),
			color.color, hcenter, vcenter, 0);

		System::Runtime::InteropServices::Marshal::FreeHGlobal(str);
	}

	Core::Dimension2D IGUIFont::GetDimension(System::String* text)
	{
		char* str = (char*)(void*)
			System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(text);

		return irr::NativeConverter::getNETDim((Font->getDimension(
			irr::core::stringw(str).c_str())));

		System::Runtime::InteropServices::Marshal::FreeHGlobal(str);
	}

	void IGUIFont::Draw(System::String* text, Core::Position2D position, Video::Color color)
	{
		char* str = (char*)(void*)
			System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(text);

		Font->draw(irr::core::stringw(str).c_str(), 
			irr::core::rect<irr::s32>(irr::NativeConverter::getNativePos(position), 
				irr::core::dimension2d<irr::s32>(0,0)),
			color.color, false, false, 0);

		System::Runtime::InteropServices::Marshal::FreeHGlobal(str);
	}

	irr::gui::IGUIFont* IGUIFont::get_NativeFont()
	{
		return Font;
	}

}
}