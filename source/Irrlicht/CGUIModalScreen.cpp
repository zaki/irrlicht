// Copyright (C) 2002-2007 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CGUIModalScreen.h"
#include "IGUIEnvironment.h"
#include "os.h"
#include "IVideoDriver.h"
#include "IGUISkin.h"

namespace irr
{
namespace gui
{

//! constructor
CGUIModalScreen::CGUIModalScreen(IGUIEnvironment* environment, IGUIElement* parent, s32 id)
: IGUIElement(EGUIET_MODAL_SCREEN, environment, parent, id, parent->getAbsolutePosition()),
	MouseDownTime(0)
{
	#ifdef _DEBUG
	setDebugName("CGUIModalScreen");
	#endif
	setAlignment(EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT);
}


//! destructor
CGUIModalScreen::~CGUIModalScreen()
{
}


//! called if an event happened.
bool CGUIModalScreen::OnEvent(SEvent event)
{
    switch(event.EventType)
	{
	case EET_MOUSE_INPUT_EVENT:
		switch(event.MouseInput.Event)
		{
		case EMIE_LMOUSE_PRESSED_DOWN:
			MouseDownTime = os::Timer::getTime();
		}
	}
	
	if (Parent)
		Parent->OnEvent(event);

	return true;	
}


//! draws the element and its children
void CGUIModalScreen::draw()
{
	u32 now = os::Timer::getTime();
	if (now - MouseDownTime < 300 && (now / 70)%2)
	{
		core::list<IGUIElement*>::Iterator it = Children.begin();
		core::rect<s32> r;
		video::IVideoDriver* driver = Environment->getVideoDriver();
		video::SColor c = Environment->getSkin()->getColor(gui::EGDC_3D_HIGH_LIGHT);

		for (; it != Children.end(); ++it)
		{
			r = (*it)->getAbsolutePosition();
			r.LowerRightCorner.X += 1;
			r.LowerRightCorner.Y += 1;
			r.UpperLeftCorner.X -= 1;
			r.UpperLeftCorner.Y -= 1;

			driver->draw2DRectangle(c, r, &AbsoluteClippingRect);
		}
	}

	IGUIElement::draw();
}



//! Removes a child.
void CGUIModalScreen::removeChild(IGUIElement* child)
{
	IGUIElement::removeChild(child);

	if (Children.empty())
		remove();
}


void CGUIModalScreen::updateAbsolutePosition()
{
	core::rect<s32> parentRect(0,0,0,0);

	if (Parent)
	{
		parentRect = Parent->getAbsolutePosition();
		RelativeRect.UpperLeftCorner.X = 0;
		RelativeRect.UpperLeftCorner.Y = 0;
		RelativeRect.LowerRightCorner.X = parentRect.getWidth();
		RelativeRect.LowerRightCorner.Y = parentRect.getHeight();
	}

	IGUIElement::updateAbsolutePosition();
}

//! Writes attributes of the element.
void CGUIModalScreen::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options=0)
{
	// these don't get serialized, their status is added to their children.
}

//! Reads attributes of the element
void CGUIModalScreen::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options=0)
{
	// these don't get deserialized. children create them if required
}


} // end namespace gui
} // end namespace irr

