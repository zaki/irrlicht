// Copyright (C) 2002-2007 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CGUIEditBox.h"
#include "IGUISkin.h"
#include "IGUIEnvironment.h"
#include "IGUIFont.h"
#include "IVideoDriver.h"
#include "rect.h"
#include "os.h"
#include "Keycodes.h"

namespace irr
{
namespace gui
{

//! constructor
CGUIEditBox::CGUIEditBox(const wchar_t* text, bool border, IGUIEnvironment* environment,
			IGUIElement* parent, s32 id,
			const core::rect<s32>& rectangle)
: IGUIEditBox(environment, parent, id, rectangle), MouseMarking(false),
	Border(border), OverrideColorEnabled(false), MarkBegin(0), MarkEnd(0),
	OverrideColor(video::SColor(101,255,255,255)),
	OverrideFont(0), CursorPos(0), ScrollPos(0), Max(0)
{
	#ifdef _DEBUG
	setDebugName("CGUIEditBox");
	#endif

	Text = text;

	Operator = environment->getOSOperator();

	if (Operator)
		Operator->grab();
}


//! destructor
CGUIEditBox::~CGUIEditBox()
{
	if (OverrideFont)
		OverrideFont->drop();

	if (Operator)
		Operator->drop();
}


//! Sets another skin independent font.
void CGUIEditBox::setOverrideFont(IGUIFont* font)
{
	if (OverrideFont)
		OverrideFont->drop();

	OverrideFont = font;

	if (OverrideFont)
		OverrideFont->grab();
}


//! Sets another color for the text.
void CGUIEditBox::setOverrideColor(video::SColor color)
{
	OverrideColor = color;
	OverrideColorEnabled = true;
}


//! Sets if the text should use the overide color or the
//! color in the gui skin.
void CGUIEditBox::enableOverrideColor(bool enable)
{
	OverrideColorEnabled = enable;
}


//! called if an event happened.
bool CGUIEditBox::OnEvent(SEvent event)
{
	switch(event.EventType)
	{
	case EET_GUI_EVENT:
		if (event.GUIEvent.EventType == EGET_ELEMENT_FOCUS_LOST)
		{
			if (event.GUIEvent.Caller == (IGUIElement*)this)
			{
				MouseMarking = false;
				MarkBegin = 0;
				MarkEnd = 0;
			}
		}
		break;
	case EET_KEY_INPUT_EVENT:
		if (processKey(event))
			return true;
		break;
	case EET_MOUSE_INPUT_EVENT:
		if (processMouse(event))
			return true;
		break;
	}

	return Parent ? Parent->OnEvent(event) : false;
}


bool CGUIEditBox::processKey(const SEvent& event)
{
	if (!event.KeyInput.PressedDown)
		return false;

	// control shortcut handling

	if (event.KeyInput.Control)
	{
		switch(event.KeyInput.Key)
		{
		case KEY_KEY_C:
			// copy to clipboard
			if (Operator && MarkBegin != MarkEnd)
			{
				s32 realmbgn = MarkBegin < MarkEnd ? MarkBegin : MarkEnd;
				s32 realmend = MarkBegin < MarkEnd ? MarkEnd : MarkBegin;

				core::stringc s;
				s = Text.subString(realmbgn, realmend - realmbgn).c_str();
				Operator->copyToClipboard(s.c_str());
			}
			break;
		case KEY_KEY_X:
			if ( !this->IsEnabled )
				break;

			// cut to the clipboard
			if (Operator && MarkBegin != MarkEnd)
			{
				s32 realmbgn = MarkBegin < MarkEnd ? MarkBegin : MarkEnd;
				s32 realmend = MarkBegin < MarkEnd ? MarkEnd : MarkBegin;

				// copy
				core::stringc sc;
				sc = Text.subString(realmbgn, realmend - realmbgn).c_str();
				Operator->copyToClipboard(sc.c_str());

				// delete
				core::stringw s;
				s = Text.subString(0, realmbgn);
				s.append( Text.subString(realmend, Text.size()-realmend) );
				Text = s;

				CursorPos = realmbgn;
				MarkBegin = 0;
				MarkEnd = 0;
			}
			break;
		case KEY_KEY_V:
			if ( !this->IsEnabled )
				break;

			// paste from the clipboard
			if (Operator)
			{
				s32 realmbgn = MarkBegin < MarkEnd ? MarkBegin : MarkEnd;
				s32 realmend = MarkBegin < MarkEnd ? MarkEnd : MarkBegin;

				// add new character
				const c8* p = Operator->getTextFromClipboard();
				if (p)
				{
					if (MarkBegin == MarkEnd)
					{
						// insert text
						core::stringw s = Text.subString(0, CursorPos);
						s.append(p);
						s.append( Text.subString(CursorPos, Text.size()-CursorPos) );

						if (!Max || s.size()<=(u32)Max) // thx to Fish FH for fix
						{
							Text = s;
							s = p;
							CursorPos += s.size();
						}
					}
					else
					{
						// replace text

						core::stringw s = Text.subString(0, realmbgn);
						s.append(p);
						s.append( Text.subString(realmend, Text.size()-realmend) );

						if (!Max || s.size()<=(u32)Max)  // thx to Fish FH for fix
						{
							Text = s;
							s = p;
							CursorPos = realmbgn + s.size();
						}
					}
				}

				MarkBegin = 0;
				MarkEnd = 0;
			}
			break;
		default:
			return false;
		}
	}

	// default keyboard handling

	if (!event.KeyInput.Control)
	switch(event.KeyInput.Key)
	{
	case KEY_END:
		if (event.KeyInput.Shift)
		{
			if (MarkBegin == MarkEnd)
				MarkBegin = CursorPos;
			MarkEnd = Text.size();
		}
		else
		{
			MarkBegin = 0;
			MarkEnd = 0;
		}
		CursorPos = Text.size();
		BlinkStartTime = os::Timer::getTime();
		break;
	case KEY_HOME:
		if (event.KeyInput.Shift)
		{
			if (MarkBegin == MarkEnd)
				MarkBegin = CursorPos;
			MarkEnd = 0;
		}
		else
		{
			MarkBegin = 0;
			MarkEnd = 0;
		}
		CursorPos = 0;
		BlinkStartTime = os::Timer::getTime();
		break;
	case KEY_RETURN:
		{
			SEvent e;
			e.EventType = EET_GUI_EVENT;
			e.GUIEvent.Caller = this;
			e.GUIEvent.EventType = EGET_EDITBOX_ENTER;
			Parent->OnEvent(e);
		}
		break;
	case KEY_LEFT:

		if (event.KeyInput.Shift)
		{
			if (CursorPos > 0) 
			{ 
				if (MarkBegin == MarkEnd) 
					MarkBegin = CursorPos; 

				MarkEnd = CursorPos-1; 
			}
		}
		else
		{
			MarkBegin = 0;
			MarkEnd = 0;
		}

		if (CursorPos > 0) CursorPos--;
		BlinkStartTime = os::Timer::getTime();
		break;

	case KEY_RIGHT:

		if (event.KeyInput.Shift) 
		{ 
			if (Text.size() > (u32)CursorPos) 
			{ 
				if (MarkBegin == MarkEnd) 
					MarkBegin = CursorPos; 

				MarkEnd = CursorPos+1; 
			} 
		}
		else
		{
			MarkBegin = 0;
			MarkEnd = 0;
		}

		if (Text.size() > (u32)CursorPos) CursorPos++;
		BlinkStartTime = os::Timer::getTime();
		break;

	case KEY_BACK:
		if ( !this->IsEnabled )
			break;

		if (Text.size())
		{
			core::stringw s;
			
			if (MarkBegin != MarkEnd)
			{
				// delete marked text
				s32 realmbgn = MarkBegin < MarkEnd ? MarkBegin : MarkEnd;
				s32 realmend = MarkBegin < MarkEnd ? MarkEnd : MarkBegin;

				s = Text.subString(0, realmbgn);
				s.append( Text.subString(realmend, Text.size()-realmend) );
				Text = s;

				CursorPos = realmbgn;
			}
			else
			{
				// delete text behind cursor
				if (CursorPos>0)
					s = Text.subString(0, CursorPos-1);
				else
					s = L"";
				s.append( Text.subString(CursorPos, Text.size()-CursorPos) );
				Text = s;
				--CursorPos;
			}

			if (CursorPos < 0)
				CursorPos = 0;
			BlinkStartTime = os::Timer::getTime();
			MarkBegin = 0;
			MarkEnd = 0;
		}
		break;
	case KEY_DELETE:
		if ( !this->IsEnabled )
			break;

		if (Text.size() != 0)
		{
			core::stringw s;

			if (MarkBegin != MarkEnd)
			{
				// delete marked text
				s32 realmbgn = MarkBegin < MarkEnd ? MarkBegin : MarkEnd;
				s32 realmend = MarkBegin < MarkEnd ? MarkEnd : MarkBegin;

				s = Text.subString(0, realmbgn);
				s.append( Text.subString(realmend, Text.size()-realmend) );
				Text = s;

				CursorPos = realmbgn;
			}
			else
			{
				// delete text before cursor
				s = Text.subString(0, CursorPos);
				s.append( Text.subString(CursorPos+1, Text.size()-CursorPos-1) );
				Text = s;
			}

			if (CursorPos > (s32)Text.size())
				CursorPos = (s32)Text.size();

			BlinkStartTime = os::Timer::getTime();
			MarkBegin = 0;
			MarkEnd = 0;
		}
		break;
	default:
		if ( !this->IsEnabled )
			break;

		if (event.KeyInput.Char != 0)
		{
			if (Text.size() < (u32)Max || Max == 0)
			{
				core::stringw s;

				if (MarkBegin != MarkEnd)
				{
					// replace marked text
					s32 realmbgn = MarkBegin < MarkEnd ? MarkBegin : MarkEnd;
					s32 realmend = MarkBegin < MarkEnd ? MarkEnd : MarkBegin;

					s = Text.subString(0, realmbgn);
					s.append(event.KeyInput.Char);
					s.append( Text.subString(realmend, Text.size()-realmend) );
					Text = s;
					CursorPos = realmbgn+1;
				}
				else
				{
					// add new character
					s = Text.subString(0, CursorPos);
					s.append(event.KeyInput.Char);
					s.append( Text.subString(CursorPos, Text.size()-CursorPos) );
					Text = s;
					++CursorPos;
				}
				
				BlinkStartTime = os::Timer::getTime();
				MarkBegin = 0;
				MarkEnd = 0;
			}
		}
		break;
	}

	// calculate scrollpos

	IGUIFont* font = OverrideFont;
	IGUISkin* skin = Environment->getSkin();
	if (!OverrideFont)
		font = skin->getFont();

	s32 cursorwidth = font->getDimension(L"_ ").Width;

	s32 minwidht = cursorwidth*2;
	if (minwidht >= AbsoluteRect.getWidth())
		minwidht = AbsoluteRect.getWidth() / 2;

	s32 tries = Text.size()*2;
	if (tries < 100)
		tries = 100;

	for (s32 t=0; t<tries; ++t)
	{
		core::stringw s = Text.subString(0,CursorPos);
		s32 charcursorpos = font->getDimension(s.c_str()).Width;

		s = Text.subString(0, ScrollPos);
		s32 charscrollpos = font->getDimension(s.c_str()).Width;

		if ((charcursorpos + cursorwidth - charscrollpos) > AbsoluteRect.getWidth())
			ScrollPos++;
		else
		if ((charcursorpos + cursorwidth - charscrollpos) < minwidht)
		{
			if (ScrollPos > 0)
				ScrollPos--;
			else
				break;
		}
		else
			break;
	}

	return true;
}

//! draws the element and its children
void CGUIEditBox::draw()
{
	if (!IsVisible)
		return;

	bool focus = Environment->hasFocus(this);

	IGUISkin* skin = Environment->getSkin();
	if (!skin)
		return;

	irr::video::IVideoDriver* driver = Environment->getVideoDriver();

	core::rect<s32> frameRect(AbsoluteRect);

	// draw the border

	if (Border)
	{
		skin->draw3DSunkenPane(this, skin->getColor(EGDC_WINDOW),
			false, true, frameRect, &AbsoluteClippingRect);
		
		frameRect.UpperLeftCorner.X += skin->getSize(EGDS_TEXT_DISTANCE_X);
	}

	// draw the text

	IGUIFont* font = OverrideFont;
	if (!OverrideFont)
		font = skin->getFont();

	if (font)
	{
		// calculate cursor pos

		core::stringw s = Text.subString(0,CursorPos);
		s32 charcursorpos = font->getDimension(s.c_str()).Width;

		s = Text.subString(0, ScrollPos);
		s32 charscrollpos = font->getDimension(s.c_str()).Width;

		core::rect<s32> rct;

		// draw mark

		if (focus && MarkBegin != MarkEnd)
		{
			rct = frameRect;

			rct.LowerRightCorner.Y -= skin->getSize(EGDS_TEXT_DISTANCE_Y);
			rct.UpperLeftCorner.Y += skin->getSize(EGDS_TEXT_DISTANCE_Y);

			s32 realmbgn = MarkBegin < MarkEnd ? MarkBegin : MarkEnd;
			s32 realmend = MarkBegin < MarkEnd ? MarkEnd : MarkBegin;

			s = Text.subString(0, realmbgn);
			s32 mbegin = font->getDimension(s.c_str()).Width;

			s = Text.subString(realmbgn, realmend - realmbgn);
			s32 mend = font->getDimension(s.c_str()).Width;

			rct.UpperLeftCorner.X  += mbegin - charscrollpos;
			rct.LowerRightCorner.X = rct.UpperLeftCorner.X + mend;

			driver->draw2DRectangle(skin->getColor(EGDC_HIGH_LIGHT), rct, &AbsoluteClippingRect);
		}

		// draw cursor

		if (focus && (os::Timer::getTime() - BlinkStartTime) % 700 < 350)
		{
			rct = frameRect;
			rct.UpperLeftCorner.X += charcursorpos;
			rct.UpperLeftCorner.X -= charscrollpos;

			font->draw(L"_", rct, 
				OverrideColorEnabled ? OverrideColor : skin->getColor(EGDC_BUTTON_TEXT),
				false, true, &AbsoluteClippingRect);
		}

		// draw text

		if (Text.size())
		{
			rct = frameRect;
			rct.UpperLeftCorner.X -= charscrollpos;

			// Save the override color information.
			// Then, alter it if the edit box is disabled.
			bool prevOver = OverrideColorEnabled;
			video::SColor prevColor = OverrideColor;
			if ( !this->IsEnabled && !OverrideColorEnabled )
			{
				OverrideColorEnabled = true;
				OverrideColor = skin->getColor( EGDC_GRAY_TEXT );
			}

			if (focus && MarkBegin != MarkEnd)
			{
				// marked text

                font->draw(Text.c_str(), rct, 
					OverrideColorEnabled ? OverrideColor : skin->getColor(EGDC_BUTTON_TEXT),
					false, true, &AbsoluteClippingRect);

				s32 realmbgn = MarkBegin < MarkEnd ? MarkBegin : MarkEnd;
				s32 realmend = MarkBegin < MarkEnd ? MarkEnd : MarkBegin;

				s = Text.subString(0, realmbgn);
				s32 mbegin = font->getDimension(s.c_str()).Width;

				s = Text.subString(realmbgn, realmend - realmbgn);

				rct.UpperLeftCorner.X += mbegin;

				font->draw(s.c_str(), rct, 
					OverrideColorEnabled ? OverrideColor : skin->getColor(EGDC_HIGH_LIGHT_TEXT),
					false, true, &AbsoluteClippingRect);								
			}
			else
			{
				// normal text
				font->draw(Text.c_str(), rct, 
					OverrideColorEnabled ? OverrideColor : skin->getColor(EGDC_BUTTON_TEXT),
					false, true, &AbsoluteClippingRect);
			}

			// Return the override color information to its previous settings.
			OverrideColorEnabled = prevOver;
			OverrideColor = prevColor;
		}
	}
}

//! Sets the new caption of this element.
void CGUIEditBox::setText(const wchar_t* text)
{
	Text = text;
	CursorPos = 0;
	ScrollPos = 0;
	MarkBegin = 0;
	MarkEnd = 0;
}


//! Sets the maximum amount of characters which may be entered in the box.
//! \param max: Maximum amount of characters. If 0, the character amount is 
//! infinity.
void CGUIEditBox::setMax(s32 max)
{
	Max = max;
	if (Max < 0)
		Max = 0;

	if (Text.size() > (u32)Max && Max != 0)
		Text = Text.subString(0, Max);
}


//! Returns maximum amount of characters, previously set by setMax();
s32 CGUIEditBox::getMax()
{
	return Max;
}


bool CGUIEditBox::processMouse(const SEvent& event)
{
	switch(event.MouseInput.Event)
	{
	case irr::EMIE_LMOUSE_LEFT_UP:
		if (Environment->hasFocus(this))
		{
			CursorPos = getCursorPos(event.MouseInput.X);
			if (MouseMarking)
				MarkEnd = CursorPos;
			MouseMarking = false;			
			return true;
		}
		break;
	case irr::EMIE_MOUSE_MOVED:
		{
			if (MouseMarking)
			{
				CursorPos = getCursorPos(event.MouseInput.X);
				MarkEnd = CursorPos;
				return true;
			}
		}
		break;
	case EMIE_LMOUSE_PRESSED_DOWN:
		if (!Environment->hasFocus(this))
		{
			// get focus
			BlinkStartTime = os::Timer::getTime();
			Environment->setFocus(this);
			MouseMarking = true;
			CursorPos = getCursorPos(event.MouseInput.X);
			MarkBegin = CursorPos;
			MarkEnd = CursorPos;
			return true;
		}
		else
		{
			if (!AbsoluteClippingRect.isPointInside(
			core::position2d<s32>(event.MouseInput.X, event.MouseInput.Y)))
			{
				// remove focus
				Environment->removeFocus(this);
				return false;
			}

			// move cursor

			CursorPos = getCursorPos(event.MouseInput.X);

			if (!MouseMarking)
				MarkBegin = CursorPos;

			MouseMarking = true;
			MarkEnd = CursorPos;
			return true;
		}	
	}

	return false;
}


s32 CGUIEditBox::getCursorPos(s32 x)
{
	IGUIFont* font = OverrideFont;
	IGUISkin* skin = Environment->getSkin();
	if (!OverrideFont)
		font = skin->getFont();

	core::stringw s = Text.subString(0, ScrollPos);
	s32 charscrollpos = font->getDimension(s.c_str()).Width;

	s32 idx = font->getCharacterFromPos(Text.c_str(), x - (AbsoluteRect.UpperLeftCorner.X + 3) + charscrollpos);	
	if (idx != -1)
		return idx;		

	return Text.size();
}

//! Writes attributes of the element.
void CGUIEditBox::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options=0)
{
	IGUIEditBox::serializeAttributes(out,options);
	out->addBool	("OverrideColorEnabled",		OverrideColorEnabled );
	out->addColor	("OverrideColor",				OverrideColor);
	// out->addFont("OverrideFont",OverrideFont);
	out->addInt		("MaxChars",					Max);
}

//! Reads attributes of the element
void CGUIEditBox::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options=0)
{
	IGUIEditBox::deserializeAttributes(in,options);
	setOverrideColor(in->getAttributeAsColor("OverrideColor"));
	enableOverrideColor(in->getAttributeAsBool("OverrideColorEnabled"));
	setMax(in->getAttributeAsInt("MaxChars"));

	// setOverrideFont(in->getAttributeAsFont("OverrideFont"));
}


} // end namespace gui
} // end namespace irr

