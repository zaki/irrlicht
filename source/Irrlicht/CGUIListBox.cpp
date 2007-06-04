// Copyright (C) 2002-2007 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CGUIListBox.h"
#include "IGUISkin.h"
#include "IGUIEnvironment.h"
#include "IVideoDriver.h"
#include "IGUIFont.h"
#include "IGUISpriteBank.h"
#include "CGUIScrollBar.h"
#include "os.h"

namespace irr
{
namespace gui
{

//! constructor
CGUIListBox::CGUIListBox(IGUIEnvironment* environment, IGUIElement* parent, 
						 s32 id, core::rect<s32> rectangle, bool clip,
						 bool drawBack, bool moveOverSelect)
: IGUIListBox(environment, parent, id, rectangle), Selected(-1), ItemHeight(0),
	TotalItemHeight(0), ItemsIconWidth(0), Font(0), IconBank(0),
	ScrollBar(0), Selecting(false), DrawBack(drawBack),
	MoveOverSelect(moveOverSelect), selectTime(0)
{
	#ifdef _DEBUG
	setDebugName("CGUIListBox");
	#endif

	IGUISkin* skin = Environment->getSkin();
	s32 s = skin->getSize(EGDS_SCROLLBAR_SIZE);

	ScrollBar = new CGUIScrollBar(false, Environment, this, 0,
		core::rect<s32>(RelativeRect.getWidth() - s, 0, RelativeRect.getWidth(), RelativeRect.getHeight()),
		!clip);
	ScrollBar->setSubElement(true);
	ScrollBar->setAlignment(EGUIA_LOWERRIGHT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT); 
	ScrollBar->drop();

	ScrollBar->setPos(0);
	ScrollBar->grab();

	setNotClipped(!clip);
	
	updateAbsolutePosition();
}


//! destructor
CGUIListBox::~CGUIListBox()
{
	if (ScrollBar)
		ScrollBar->drop();

	if (Font)
		Font->drop();

	if (IconBank)
		IconBank->drop();
}



//! returns amount of list items
s32 CGUIListBox::getItemCount()
{
	return Items.size();
}



//! returns string of a list item. the may be a value from 0 to itemCount-1
const wchar_t* CGUIListBox::getListItem(s32 id)
{
	if (id<0 || id>((s32)Items.size())-1)
		return 0;

	return Items[id].text.c_str();
}

//! Returns the icon of an item
s32 CGUIListBox::getIcon(s32 id) const
{
	if (id<0 || id>((s32)Items.size())-1)
		return -1;

	return Items[id].icon;
}


//! adds an list item, returns id of item
s32 CGUIListBox::addItem(const wchar_t* text)
{
	ListItem i;
	i.text = text;

	Items.push_back(i);
	recalculateItemHeight();
	return Items.size() - 1;
}

//! adds an list item, returns id of item
void CGUIListBox::removeItem(s32 id)
{
	if (id < 0 || id >= (s32)Items.size())
		return;

	if (Selected==id)
	{
		Selected = -1;
	}
	else if (Selected > id)
	{
		Selected -= 1;
		selectTime = os::Timer::getTime();
	}

	Items.erase(id);

	recalculateItemHeight();
}




//! clears the list
void CGUIListBox::clear()
{
	Items.clear();
	ItemsIconWidth = 0;
	Selected = -1;

	if (ScrollBar)
		ScrollBar->setPos(0);

	recalculateItemHeight();
}



void CGUIListBox::recalculateItemHeight()
{
	IGUISkin* skin = Environment->getSkin();

	if (Font != skin->getFont())
	{
		if (Font)
			Font->drop();

		Font = skin->getFont();
		ItemHeight = 0;

		if (Font)
		{
			ItemHeight = Font->getDimension(L"A").Height + 4;
			Font->grab();
		}
	}

	TotalItemHeight = ItemHeight * Items.size();
	ScrollBar->setMax(TotalItemHeight - AbsoluteRect.getHeight());
}



//! returns id of selected item. returns -1 if no item is selected.
s32 CGUIListBox::getSelected()
{
	return Selected;
}



//! sets the selected item. Set this to -1 if no item should be selected
void CGUIListBox::setSelected(s32 id)
{
	if (id<0 || id>((s32)Items.size())-1)
		Selected = -1;
	else
		Selected = id;

	selectTime = os::Timer::getTime();
}



//! called if an event happened.
bool CGUIListBox::OnEvent(SEvent event)
{
	switch(event.EventType)
	{
	case EET_GUI_EVENT:
		switch(event.GUIEvent.EventType)
		{
		case gui::EGET_SCROLL_BAR_CHANGED:
			if (event.GUIEvent.Caller == ScrollBar)
			{
//				s32 pos = ((gui::IGUIScrollBar*)event.GUIEvent.Caller)->getPos();
				return true;
			}
			break;
		case gui::EGET_ELEMENT_FOCUS_LOST:
			{
				if (event.GUIEvent.Caller == (IGUIElement*)this)
					Selecting = false;
			}
		}
		break;
	case EET_MOUSE_INPUT_EVENT:
		{
			core::position2d<s32> p(event.MouseInput.X, event.MouseInput.Y);

			switch(event.MouseInput.Event)
			{
			case EMIE_MOUSE_WHEEL:
				ScrollBar->setPos(ScrollBar->getPos() + (s32)event.MouseInput.Wheel*-10);
				return true;

			case EMIE_LMOUSE_PRESSED_DOWN:
			{
				IGUIElement *el = Environment->getRootGUIElement()->getElementFromPoint(
					core::position2di(event.MouseInput.X, event.MouseInput.Y));

				if (Environment->hasFocus(this) &&	
					ScrollBar == el &&
					ScrollBar->OnEvent(event))
					return true;
			}

				Selecting = true;
				Environment->setFocus(this);
				return true;

			case EMIE_LMOUSE_LEFT_UP:
				if (Environment->hasFocus(this) &&	
					ScrollBar->getAbsolutePosition().isPointInside(p) &&
					ScrollBar->OnEvent(event))
					return true;

				if (!AbsoluteRect.isPointInside(p))
				{
					Selecting = false;
					Environment->removeFocus(this);
					break;
				}

				Selecting = false;
				Environment->removeFocus(this);			
				selectNew(event.MouseInput.Y);
				return true;

			case EMIE_MOUSE_MOVED:
				if (Selecting || MoveOverSelect)
				{
					if (getAbsolutePosition().isPointInside(p))
					{
						selectNew(event.MouseInput.Y, true);
						return true;
					}
				}
			}
		}
		break;
	}


	return Parent ? Parent->OnEvent(event) : false;
}


void CGUIListBox::selectNew(s32 ypos, bool onlyHover)
{
	s32 oldSelected = Selected;

	// find new selected item.
	if (ItemHeight!=0)
		Selected = ((ypos - AbsoluteRect.UpperLeftCorner.Y - 1) + ScrollBar->getPos()) / ItemHeight;

	if (Selected >= (s32)Items.size())
		Selected = Items.size() - 1;
	else 
	if (Selected<0)
		Selected = 0;

	// post the news

	if (Parent && !onlyHover)
	{
		SEvent event;
		event.EventType = EET_GUI_EVENT;
		event.GUIEvent.Caller = this;
		event.GUIEvent.EventType = (Selected != oldSelected) ? EGET_LISTBOX_CHANGED : EGET_LISTBOX_SELECTED_AGAIN;
		Parent->OnEvent(event);
	}
}

//! Update the position and size of the listbox, and update the scrollbar
void CGUIListBox::updateAbsolutePosition()
{
	recalculateItemHeight();

	IGUIElement::updateAbsolutePosition();
}

//! draws the element and its children
void CGUIListBox::draw()
{
	if (!IsVisible)
		return;

	recalculateItemHeight(); // if the font changed

	IGUISkin* skin = Environment->getSkin();
	irr::video::IVideoDriver* driver = Environment->getVideoDriver();

	core::rect<s32>* clipRect = 0;

	// draw background
	core::rect<s32> frameRect(AbsoluteRect);
	
	// draw items

	core::rect<s32> clientClip(AbsoluteRect);
	clientClip.UpperLeftCorner.Y += 1;
	clientClip.UpperLeftCorner.X += 1;
	clientClip.LowerRightCorner.X = AbsoluteRect.LowerRightCorner.X - skin->getSize(EGDS_SCROLLBAR_SIZE);
	clientClip.LowerRightCorner.Y -= 1;
	clientClip.clipAgainst(AbsoluteClippingRect);

	skin->draw3DSunkenPane(this, skin->getColor(EGDC_3D_HIGH_LIGHT), true,
		DrawBack, frameRect, &clientClip);

	if (clipRect)
		clientClip.clipAgainst(*clipRect);

	frameRect = AbsoluteRect;
	frameRect.UpperLeftCorner.X += 1;
	frameRect.LowerRightCorner.X = AbsoluteRect.LowerRightCorner.X - skin->getSize(EGDS_SCROLLBAR_SIZE);
	frameRect.LowerRightCorner.Y = AbsoluteRect.UpperLeftCorner.Y + ItemHeight;

	frameRect.UpperLeftCorner.Y -= ScrollBar->getPos();
	frameRect.LowerRightCorner.Y -= ScrollBar->getPos();

	for (s32 i=0; i<(s32)Items.size(); ++i)
	{
		if (frameRect.LowerRightCorner.Y >= AbsoluteRect.UpperLeftCorner.Y &&
			frameRect.UpperLeftCorner.Y <= AbsoluteRect.LowerRightCorner.Y)
		{
			if (i == Selected)
				driver->draw2DRectangle(skin->getColor(EGDC_HIGH_LIGHT), frameRect, &clientClip);

			core::rect<s32> textRect = frameRect;
			textRect.UpperLeftCorner.X += 3;

			if (Font)
			{
				if (IconBank && (Items[i].icon > -1))
				{
					core::position2di iconPos = textRect.UpperLeftCorner;
					iconPos.Y += textRect.getHeight() / 2;
					iconPos.X += ItemsIconWidth/2; 
					IconBank->draw2DSprite( (u32)Items[i].icon, iconPos, &clientClip, 
						skin->getColor((i==Selected) ? EGDC_ICON_HIGH_LIGHT : EGDC_ICON),
						(i==Selected) ? selectTime : 0 , (i==Selected) ? os::Timer::getTime() : 0, false, true);
				}

				textRect.UpperLeftCorner.X += ItemsIconWidth+3;

				Font->draw(Items[i].text.c_str(), textRect, skin->getColor((i==Selected) ? EGDC_HIGH_LIGHT_TEXT : EGDC_BUTTON_TEXT), false, true, &clientClip);

				textRect.UpperLeftCorner.X -= ItemsIconWidth+3;
			}
		}

		frameRect.UpperLeftCorner.Y += ItemHeight;
		frameRect.LowerRightCorner.Y += ItemHeight;
	}

	IGUIElement::draw();
}



//! adds an list item with an icon
s32 CGUIListBox::addItem(const wchar_t* text, s32 icon)
{
	ListItem i;
	i.text = text;
	i.icon = icon;

	Items.push_back(i);
	recalculateItemHeight();

	if (IconBank && icon > -1 && 
		IconBank->getSprites().size() > (u32)icon &&
		IconBank->getSprites()[(u32)icon].Frames.size())
	{
		u32 rno = IconBank->getSprites()[(u32)icon].Frames[0].rectNumber;
		if (IconBank->getPositions().size() > rno)
		{
			s32 w = IconBank->getPositions()[rno].getWidth();
			if (w > ItemsIconWidth)
				ItemsIconWidth = w;
		}
	}

    return Items.size() - 1;
}


void CGUIListBox::setSpriteBank(IGUISpriteBank* bank)
{
	if (IconBank)
		IconBank->drop();

	IconBank = bank;
	if (IconBank)
		IconBank->grab();
}


//! Writes attributes of the element.
void CGUIListBox::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options=0)
{
	IGUIListBox::serializeAttributes(out,options);

	// todo: out->addString	("IconBank",		IconBank->getName?);
	out->addBool	("DrawBack",		DrawBack);
	out->addBool	("MoveOverSelect",	MoveOverSelect);

	// todo: save list of items and icons.
	/*core::array<core::stringw> tmpText;
	core::array<core::stringw> tmpIcons;
	u32 i;
	for (i=0;i<Items.size(); ++i)
	{
		tmpText.push_back(Items[i].text);
		tmpIcons.push_back(Items[i].icon);
	}

	out->addArray	("ItemText",		tmpText);
	out->addArray	("ItemIcons",		tmpIcons);

	out->addInt		("Selected",		Selected);
	*/

}

//! Reads attributes of the element
void CGUIListBox::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options=0)
{
	DrawBack		= in->getAttributeAsBool("DrawBack");
	MoveOverSelect	= in->getAttributeAsBool("MoveOverSelect");

	IGUIListBox::deserializeAttributes(in,options);

	// read arrays
	/*
	core::array<core::stringw> tmpText;
	core::array<core::stringw> tmpIcons;

	tmpText			= in->getAttributeAsArray("ItemText");
	tmpIcons		= in->getAttributeAsArray("ItemIcons");
	u32 i;
	for (i=0; i<Items.size(); ++i)
		addItem(tmpText[i].c_str(), tmpIcons[i].c_str());
	

	this->setSelected(in->getAttributeAsInt("Selected"));
	*/

}



} // end namespace gui
} // end namespace irr
