// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "IGUIEnvironment.h"
#include "NativeConverter.h"
#include "IGUIFont.h"
#include "IGUIElement.h"
#include "IGUIListBox.h"
#include "IGUIStaticText.h"
#include <vcclr.h> // for PtrToStringChars

namespace Irrlicht
{
namespace GUI
{

	IGUIEnvironment::IGUIEnvironment(irr::gui::IGUIEnvironment* env)
		: Environment(env), BuildInFont(0)
	{
		Environment->grab();

		irr::gui::IGUIFont* font = Environment->getBuiltInFont();
		if (font)
			BuildInFont = new IGUIFont(font); 
	}


	IGUIEnvironment::~IGUIEnvironment()
	{
		if ( Environment )
			Environment->drop();
		Environment = 0;
	}

	IGUIFont* IGUIEnvironment::GetFont(System::String* filename)
	{
		char* str = (char*)(void*)
			System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(filename);

		irr::gui::IGUIFont* font = Environment->getFont(str);

		System::Runtime::InteropServices::Marshal::FreeHGlobal(str);

		if (!font)
			return 0;

		return new IGUIFont(font);
	}

	IGUIFont* IGUIEnvironment::get_BuiltInFont()
	{
		return BuildInFont;
	}

	void IGUIEnvironment::SetFocus(IGUIElement* element)
	{
		Environment->setFocus(element ? element->get_NativeElement() : 0);
	}


	void IGUIEnvironment::RemoveFocus(IGUIElement* element)
	{
		Environment->removeFocus(element ? element->get_NativeElement() : 0);
	}

	void IGUIEnvironment::DrawAll()
	{
		Environment->drawAll();
	}

	bool IGUIEnvironment::HasFocus(IGUIElement* element)
	{
		return Environment->hasFocus(element ? element->get_NativeElement() : 0);
	}


	IGUISkin* IGUIEnvironment::CreateSkin(SkinType type)
	{
		return new IGUISkin(Environment->createSkin((irr::gui::EGUI_SKIN_TYPE)type));
	}

	void IGUIEnvironment::set_Skin(IGUISkin* skin)
	{
		Environment->setSkin(skin ? skin->get_NativeSkin() : 0);
	}


	IGUISkin* IGUIEnvironment::get_Skin()
	{
		irr::gui::IGUISkin* s = Environment->getSkin();
		if (!s)
			return 0;

		return new IGUISkin(s);
	}


	IGUIElement* IGUIEnvironment::get_RootGUIElement()
	{
		return new IGUIElement(Environment->getRootGUIElement());
	}


	IGUIElement* IGUIEnvironment::AddButton(Core::Rect position, IGUIElement* parent, int id, 
		System::String* text)
	{
		const wchar_t __pin* pinchars = PtrToStringChars(text); 

		irr::gui::IGUIElement* e = Environment->addButton(
			irr::NativeConverter::getNativeRect(position), 
			parent ? parent->get_NativeElement() : 0,
			id, pinchars);

		if (!e)
			return 0;

		return new IGUIElement(e);
	}


	IGUIElement* IGUIEnvironment::AddWindow(Core::Rect position, bool modal, 
		System::String* text, IGUIElement* parent, int id)
	{
		const wchar_t __pin* pinchars = PtrToStringChars(text); 

		irr::gui::IGUIElement* e = Environment->addWindow(
			irr::NativeConverter::getNativeRect(position), modal, pinchars,
			parent ? parent->get_NativeElement() : 0, id);

		if (!e)
			return 0;

		return new IGUIElement(e);
	}


	IGUIElement* IGUIEnvironment::AddMessageBox(System::String* caption, System::String* text, 
		bool modal, MessageBoxFlag flags, IGUIElement* parent, int id)
	{
		const wchar_t __pin* ncaption = PtrToStringChars(caption); 
		const wchar_t __pin* ntext = PtrToStringChars(text); 

		irr::gui::IGUIElement* e = Environment->addMessageBox(
			ncaption, ntext, modal, flags,
			parent ? parent->get_NativeElement() : 0, id);

		if (!e)
			return 0;

		return new IGUIElement(e);
	}

	IGUIElement* IGUIEnvironment::AddScrollBar(bool horizontal, Core::Rect position,
		IGUIElement* parent, int id)
	{
		irr::gui::IGUIElement* e = Environment->addScrollBar(horizontal, 
			irr::NativeConverter::getNativeRect(position), 
			parent ? parent->get_NativeElement() : 0, id);

		if (!e)
			return 0;

		return new IGUIElement(e);
	}

	IGUIElement* IGUIEnvironment::AddImage(Video::ITexture* image, Core::Position2D pos, 
			bool useAlphaChannel, IGUIElement* parent, int id, System::String* text)
	{
		const wchar_t __pin* ntext = PtrToStringChars(text); 

		irr::gui::IGUIElement* e = Environment->addImage(
			image ? image->get_NativeTexture() : 0,
			irr::NativeConverter::getNativePos(pos), 
			useAlphaChannel,
			parent ? parent->get_NativeElement() : 0, id,
			ntext);

		if (!e)
			return 0;

		return new IGUIElement(e);
	}

	IGUIElement* IGUIEnvironment::AddCheckBox(bool checked, Core::Rect position,
			IGUIElement* parent, int id, System::String* text)
	{
		const wchar_t __pin* ntext = PtrToStringChars(text); 

		irr::gui::IGUIElement* e = Environment->addCheckBox(
			checked,
			irr::NativeConverter::getNativeRect(position), 
			parent ? parent->get_NativeElement() : 0, id, ntext);

		if (!e)
			return 0;

		return new IGUIElement(e);
	}

	IGUIListBox* IGUIEnvironment::AddListBox(Core::Rect position, IGUIElement* parent, 
			int id, bool drawBackGround)
	{
		irr::gui::IGUIListBox* e = Environment->addListBox( 
			irr::NativeConverter::getNativeRect(position), 
			parent ? parent->get_NativeElement() : 0, id, drawBackGround);

		if (!e)
			return 0;

		return new IGUIListBox(e);
	}

	IGUIElement* IGUIEnvironment::AddFileOpenDialog(System::String* text, bool modal,
		IGUIElement* parent, int id)
	{
		const wchar_t __pin* ntext = PtrToStringChars(text); 

		irr::gui::IGUIElement* e = Environment->addFileOpenDialog(
			ntext, modal, parent ? parent->get_NativeElement() : 0, id);

		if (!e)
			return 0;

		return new IGUIElement(e);
	}

	IGUIStaticText* IGUIEnvironment::AddStaticText(System::String* text, Core::Rect position,
		bool border, bool wordWrap, IGUIElement* parent, int id)
	{
		const wchar_t __pin* ntext = PtrToStringChars(text); 

		irr::gui::IGUIStaticText* e = Environment->addStaticText(
			ntext,
			irr::NativeConverter::getNativeRect(position), 
			border, wordWrap,
			parent ? parent->get_NativeElement() : 0, id);

		if (!e)
			return 0;

		return new IGUIStaticText(e);
	}

	IGUIElement* IGUIEnvironment::AddEditBox(System::String* text, Core::Rect position,
		bool border, IGUIElement* parent, int id)
	{
		const wchar_t __pin* ntext = PtrToStringChars(text); 

		irr::gui::IGUIElement* e = Environment->addEditBox(
			ntext, 
			irr::NativeConverter::getNativeRect(position), 
			border, parent ? parent->get_NativeElement() : 0, id);

		if (!e)
			return 0;

		return new IGUIElement(e);
	}

}
}