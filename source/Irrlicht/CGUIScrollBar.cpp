// Copyright (C) 2002-2007 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CGUIScrollBar.h"
#include "IGUISkin.h"
#include "IGUIEnvironment.h"
#include "IVideoDriver.h"
#include "CGUIButton.h"
#include "IGUIFont.h"
#include "IGUIFontBitmap.h"

namespace irr
{
namespace gui
{


//! constructor
CGUIScrollBar::CGUIScrollBar(bool horizontal, IGUIEnvironment* environment,
				IGUIElement* parent, s32 id,
				core::rect<s32> rectangle, bool noclip)
: IGUIScrollBar(environment, parent, id, rectangle), UpButton(0), DownButton(0),
	Dragging(false), Horizontal(horizontal), Pos(0), DrawPos(0),
	DrawHeight(0), Max(100), SmallStep(10)
{
	#ifdef _DEBUG
	setDebugName("CGUIScrollBar");
	#endif

	refreshControls();

	setNotClipped(noclip);

	setPos(0);
}


//! destructor
CGUIScrollBar::~CGUIScrollBar()
{
	if (UpButton)
		UpButton->drop();

	if (DownButton)
		DownButton->drop();
}


//! called if an event happened.
bool CGUIScrollBar::OnEvent(SEvent event)
{
	switch(event.EventType)
	{
	case EET_GUI_EVENT:
		if (event.GUIEvent.EventType == EGET_BUTTON_CLICKED)
		{
			if (event.GUIEvent.Caller == UpButton)
				setPos(Pos-SmallStep);
			else
			if (event.GUIEvent.Caller == DownButton)
				setPos(Pos+SmallStep);

			SEvent newEvent;
			newEvent.EventType = EET_GUI_EVENT;
			newEvent.GUIEvent.Caller = this;
			newEvent.GUIEvent.EventType = EGET_SCROLL_BAR_CHANGED;
			Parent->OnEvent(newEvent);

			return true;
		}
		else
		if (event.GUIEvent.EventType == EGET_ELEMENT_FOCUS_LOST)
		{
			if (event.GUIEvent.Caller == (IGUIElement*)this)
				Dragging = false;
		}
		break;
	case EET_MOUSE_INPUT_EVENT:
		switch(event.MouseInput.Event)
		{
		case EMIE_MOUSE_WHEEL:
			if (Environment->getFocus() == this)
			{ // thanks to a bug report by REAPER
				setPos(getPos() + (s32)event.MouseInput.Wheel* -SmallStep);
				SEvent newEvent;
				newEvent.EventType = EET_GUI_EVENT;
				newEvent.GUIEvent.Caller = this;
				newEvent.GUIEvent.EventType = EGET_SCROLL_BAR_CHANGED;
				Parent->OnEvent(newEvent);
				return true;
			}
			break;
		case EMIE_LMOUSE_PRESSED_DOWN:
		{

			if (AbsoluteClippingRect.isPointInside(core::position2di(event.MouseInput.X, event.MouseInput.Y)))
			{
				Dragging = true;
				Environment->setFocus(this);
				return true;
			}
			else
			{
				Environment->removeFocus(this);
			}
			break;
		}
		case EMIE_LMOUSE_LEFT_UP:
			Dragging = false;
			return true;
			
			break;
		case EMIE_MOUSE_MOVED:
			if (Dragging)
			{
				s32 oldPos = Pos;
				setPosFromMousePos(event.MouseInput.X, event.MouseInput.Y);
				if (Pos != oldPos && Parent)
				{
					SEvent newEvent;
					newEvent.EventType = EET_GUI_EVENT;
					newEvent.GUIEvent.Caller = this;
					newEvent.GUIEvent.EventType = EGET_SCROLL_BAR_CHANGED;
					Parent->OnEvent(newEvent);
				}
				return true;
			}
		}
		break;
	}

	return Parent ? Parent->OnEvent(event) : false;
}

//! draws the element and its children
void CGUIScrollBar::draw()
{
	if (!IsVisible)
		return;

	IGUISkin* skin = Environment->getSkin();
	if (!skin)
		return;

	irr::video::IVideoDriver* driver = Environment->getVideoDriver();

	core::rect<s32> rect = AbsoluteRect;

	// draws the background
	driver->draw2DRectangle(skin->getColor(EGDC_SCROLLBAR), rect, &AbsoluteClippingRect);

	if (Max!=0)
	{
		// draw thumb
		if (Horizontal)
		{
			rect.UpperLeftCorner.X = AbsoluteRect.UpperLeftCorner.X + DrawPos + RelativeRect.getHeight() - DrawHeight/2;
			rect.LowerRightCorner.X = rect.UpperLeftCorner.X + DrawHeight;
		}
		else
		{
			rect.UpperLeftCorner.Y = AbsoluteRect.UpperLeftCorner.Y + DrawPos + RelativeRect.getWidth() - DrawHeight/2;
			rect.LowerRightCorner.Y = rect.UpperLeftCorner.Y + DrawHeight;
		}

		skin->draw3DButtonPaneStandard(this, rect, &AbsoluteClippingRect);
	}

	// draw buttons
	IGUIElement::draw();
}

void CGUIScrollBar::updateAbsolutePosition()
{
	IGUIElement::updateAbsolutePosition();
	// todo: properly resize
	refreshControls();

	if (Horizontal)
	{
		f32 f = (RelativeRect.getWidth() - ((f32)RelativeRect.getHeight()*3.0f)) / (f32)Max;
		DrawPos = (s32)((Pos * f) + ((f32)RelativeRect.getHeight() * 0.5f));
		DrawHeight = RelativeRect.getHeight();
	}
	else
	{
		f32 f = 0.0f;
		if (Max != 0)
			f = (RelativeRect.getHeight() - ((f32)RelativeRect.getWidth()*3.0f)) / (f32)Max;

		DrawPos = (s32)((Pos * f) + ((f32)RelativeRect.getWidth() * 0.5f));
		DrawHeight = RelativeRect.getWidth();
	}
}

void CGUIScrollBar::setPosFromMousePos(s32 x, s32 y)
{
	if (Horizontal)
	{
		f32 f = (RelativeRect.getWidth() - ((f32)RelativeRect.getHeight()*3.0f)) / (f32)Max;
		setPos((s32)(((f32)(x - AbsoluteRect.UpperLeftCorner.X - RelativeRect.getHeight())) / f));
	}
	else
	{
		f32 f = (RelativeRect.getHeight() - ((f32)RelativeRect.getWidth()*3.0f)) / (f32)Max;
		setPos((s32)(((f32)y - AbsoluteRect.UpperLeftCorner.Y - RelativeRect.getWidth()) / f));
	}
}



//! sets the position of the scrollbar
void CGUIScrollBar::setPos(s32 pos)
{
	Pos = pos;
	if (Pos < 0)
		Pos = 0;
	if (Pos > Max)
		Pos = Max;

	if (Horizontal)
	{
		f32 f = (RelativeRect.getWidth() - ((f32)RelativeRect.getHeight()*3.0f)) / (f32)Max;
		DrawPos = (s32)((Pos * f) + ((f32)RelativeRect.getHeight() * 0.5f));
		DrawHeight = RelativeRect.getHeight();
	}
	else
	{
		f32 f = 0.0f;
		if (Max != 0)
			f = (RelativeRect.getHeight() - ((f32)RelativeRect.getWidth()*3.0f)) / (f32)Max;

		DrawPos = (s32)((Pos * f) + ((f32)RelativeRect.getWidth() * 0.5f));
		DrawHeight = RelativeRect.getWidth();
	}
}


//! gets the small step value
s32 CGUIScrollBar::getSmallStep()
{
	return SmallStep;
}


//! sets the small step value
void CGUIScrollBar::setSmallStep(s32 step)
{
	if (step > 0)
		SmallStep = step;
	else
		SmallStep = 10;

}


//! gets the maximum value of the scrollbar.
s32 CGUIScrollBar::getMax()
{
	return Max;
}


//! sets the maximum value of the scrollbar.
void CGUIScrollBar::setMax(s32 max)
{
	if (max > 0)
		Max = max;
	else
		Max = 0;

	bool enable = (Max != 0);
	UpButton->setEnabled(enable);
	DownButton->setEnabled(enable);
	setPos(Pos);
}


//! gets the current position of the scrollbar
s32 CGUIScrollBar::getPos()
{
	return Pos;
}


//! refreshes the position and text on child buttons
void CGUIScrollBar::refreshControls()
{
	video::SColor color(255,255,255,255);

	IGUISkin* skin = Environment->getSkin();
	IGUISpriteBank* sprites = 0;

	if (skin)
	{
		sprites = skin->getSpriteBank();
		color = skin->getColor(EGDC_WINDOW_SYMBOL);
	}

	if (Horizontal)
	{
		s32 h = RelativeRect.getHeight();
		if (!UpButton)
		{
			UpButton = new CGUIButton(Environment, this, -1, core::rect<s32>(0,0, h, h), NoClip);
			UpButton->setSubElement(true);
		}
		if (sprites)
		{
			UpButton->setSpriteBank(sprites);
			UpButton->setSprite(EGBS_BUTTON_UP, skin->getIcon(EGDI_CURSOR_LEFT), color);
			UpButton->setSprite(EGBS_BUTTON_DOWN, skin->getIcon(EGDI_CURSOR_LEFT), color);
		}
		UpButton->setRelativePosition(core::rect<s32>(0,0, h, h));
		UpButton->setAlignment(EGUIA_UPPERLEFT, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT);
		if (!DownButton)
		{
			DownButton = new CGUIButton(Environment, this, -1, core::rect<s32>(RelativeRect.getWidth()-h, 0, RelativeRect.getWidth(), h), NoClip);
			DownButton->setSubElement(true);
		}
		if (sprites)
		{
			DownButton->setSpriteBank(sprites);
			DownButton->setSprite(EGBS_BUTTON_UP, skin->getIcon(EGDI_CURSOR_RIGHT), color);
			DownButton->setSprite(EGBS_BUTTON_DOWN, skin->getIcon(EGDI_CURSOR_RIGHT), color);
		}
		DownButton->setRelativePosition(core::rect<s32>(RelativeRect.getWidth()-h, 0, RelativeRect.getWidth(), h));
		DownButton->setAlignment(EGUIA_LOWERRIGHT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT);
	}
	else
	{
		s32 w = RelativeRect.getWidth();
		if (!UpButton)
		{
			UpButton = new CGUIButton(Environment, this, -1, core::rect<s32>(0,0, w, w), NoClip);
			UpButton->setSubElement(true);
		}
		if (sprites)
		{
			UpButton->setSpriteBank(sprites);
			UpButton->setSprite(EGBS_BUTTON_UP, skin->getIcon(EGDI_CURSOR_UP), color);
			UpButton->setSprite(EGBS_BUTTON_DOWN, skin->getIcon(EGDI_CURSOR_UP), color);
		}
		UpButton->setRelativePosition(core::rect<s32>(0,0, w, w));
		UpButton->setAlignment(EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);
		if (!DownButton)
		{
			DownButton = new CGUIButton(Environment, this, -1, core::rect<s32>(0,RelativeRect.getHeight()-w, w, RelativeRect.getHeight()), NoClip);
			DownButton->setSubElement(true);
		}
		if (sprites)
		{
			DownButton->setSpriteBank(sprites);
			DownButton->setSprite(EGBS_BUTTON_UP, skin->getIcon(EGDI_CURSOR_DOWN), color);
			DownButton->setSprite(EGBS_BUTTON_DOWN, skin->getIcon(EGDI_CURSOR_DOWN), color);
		}
		DownButton->setRelativePosition(core::rect<s32>(0,RelativeRect.getHeight()-w, w, RelativeRect.getHeight()));
		DownButton->setAlignment(EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT, EGUIA_LOWERRIGHT, EGUIA_LOWERRIGHT);
	}
}

//! Writes attributes of the element.
void CGUIScrollBar::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options=0)
{
	IGUIScrollBar::serializeAttributes(out,options);

	out->addBool("Horizontal",	Horizontal);
	out->addInt	("Value",		Pos);
	out->addInt	("Max",			Max);
	out->addInt	("SmallStep",	SmallStep);
}

//! Reads attributes of the element
void CGUIScrollBar::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options=0)
{
	IGUIScrollBar::deserializeAttributes(in,options);

	Horizontal = in->getAttributeAsBool("Horizontal");
	setMax(in->getAttributeAsInt("Max"));
	setPos(in->getAttributeAsInt("Value"));
	setSmallStep(in->getAttributeAsInt("SmallStep"));
	NoClip = in->getAttributeAsBool("NoClip");

	refreshControls();
}


} // end namespace gui
} // end namespace irr

