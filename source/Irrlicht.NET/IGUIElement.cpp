// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "IGUIElement.h"
#include "NativeConverter.h"
#include <vcclr.h> // for PtrToStringChars

namespace Irrlicht
{
namespace GUI
{

	IGUIElement::IGUIElement(irr::gui::IGUIElement* e)
		: Element(e)
	{
		Element->grab();
	}


	IGUIElement::~IGUIElement()
	{
		Element->drop();
	}

	IGUIElement* IGUIElement::get_Parent()
	{
		irr::gui::IGUIElement* e = Element->getParent();
		if (!e)
			return 0;

		return new IGUIElement(e);
	}

	Core::Rect IGUIElement::get_RelativePosition()
	{
		return irr::NativeConverter::getNETRect(Element->getRelativePosition());
	}

	void IGUIElement::set_RelativePosition(Core::Rect r)
	{
		Element->setRelativePosition(irr::NativeConverter::getNativeRect(r));
	}

	Core::Rect IGUIElement::get_AbsolutePosition()
	{
		return irr::NativeConverter::getNETRect(Element->getAbsolutePosition());
	}

	void IGUIElement::UpdateAbsolutePosition()
	{
		Element->updateAbsolutePosition();
	}

	IGUIElement* IGUIElement::GetElementFromPoint(Core::Position2D point)
	{
		irr::gui::IGUIElement* e = Element->getElementFromPoint(
			irr::core::position2d<irr::s32>(point.X, point.Y));

		if (!e)
			return 0;

		return new IGUIElement(e);
	}

	void IGUIElement::AddChild(IGUIElement* child)
	{
		Element->addChild(child ? child->Element : 0);
	}

	void IGUIElement::RemoveChild(IGUIElement* child)
	{
		Element->removeChild(child ? child->Element : 0);
	}

	void IGUIElement::Remove()
	{
		Element->remove();
	}

	void IGUIElement::Draw()
	{
		Element->draw();
	}

	void IGUIElement::Move(Core::Position2D m)
	{
		Element->move(irr::core::position2d<irr::s32>(m.X, m.Y));
	}

	bool IGUIElement::get_Visible()
	{
		return Element->isVisible();
	}

	void IGUIElement::set_Visible(bool visible)
	{
		Element->setVisible(visible);
	}

	bool IGUIElement::get_Enabled()
	{
		return Element->isEnabled();
	}

	void IGUIElement::set_Enabled(bool enabled)
	{
		Element->setEnabled(enabled);
	}

	void IGUIElement::set_Text(System::String* text)
	{
		const wchar_t __pin* pinchars = PtrToStringChars(text); 
		Element->setText(pinchars);
	}
    
	System::String* IGUIElement::get_Text()
	{
		return new System::String(Element->getText());
	}

	int IGUIElement::get_ID()
	{
		return Element->getID();
	}

	void IGUIElement::set_ID(int id)
	{
		Element->setID(id);
	}

	irr::gui::IGUIElement* IGUIElement::get_NativeElement()
	{
		return Element;
	}

}
}