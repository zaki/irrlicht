// Copyright (C) 2002-2007 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

// 07.10.2005 - Multicolor-Listbox addet by A. Buschhüter (Acki)
//                                          A_Buschhueter@gmx.de

#include "CGUITable.h"

#ifdef _IRR_COMPILE_WITH_GUI_

#include "IGUISkin.h"
#include "IGUIEnvironment.h"
#include "IVideoDriver.h"
#include "IGUIFont.h"
#include "CGUIScrollBar.h"
#include "os.h"

#define ARROW_PAD 15

namespace irr
{
namespace gui
{

//! constructor
CGUITable::CGUITable(IGUIEnvironment* environment, IGUIElement* parent,
						 s32 id, core::rect<s32> rectangle, bool clip,
						 bool drawBack, bool moveOverSelect)
: IGUITable(environment, parent, id, rectangle), ScrollBar(0),
	ItemHeight(0), TotalItemHeight(0), Font(0), Selected(-1),
	Clip(clip), DrawBack(drawBack), Selecting(false), ActiveTab(-1),
	MoveOverSelect(moveOverSelect), CellHeightPadding(2), CellWidthPadding(5), m_CurrentOrdering(EGOM_ASCENDING)
{
	#ifdef _DEBUG
	setDebugName("CGUITable");
	#endif

	IGUISkin* skin = Environment->getSkin();
	s32 s = skin->getSize(EGDS_SCROLLBAR_SIZE);

	ScrollBar = new CGUIScrollBar(false, Environment, this, -1,
		core::rect<s32>(RelativeRect.getWidth() - s, 0, RelativeRect.getWidth(), RelativeRect.getHeight()),
		!clip);

	if (ScrollBar)
	{
		ScrollBar->setPos(0);
		ScrollBar->setAlignment(EGUIA_LOWERRIGHT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT); 
		ScrollBar->setSubElement(true);
		ScrollBar->grab();
	}

	recalculate();
}


//! destructor
CGUITable::~CGUITable()
{
	if (ScrollBar)
		ScrollBar->drop();

	if (Font)
		Font->drop();
}


void CGUITable::addColumn(const wchar_t* caption, s32 id)
{
	IGUISkin* skin = Environment->getSkin();
	if (!skin)
		return;

	Column tabHeader;
	tabHeader.name = caption;
	tabHeader.width = Font->getDimension(caption).Width + (CellWidthPadding * 2) + ARROW_PAD;
	tabHeader.TextColor = skin->getColor(EGDC_BUTTON_TEXT);
	tabHeader.useCustomOrdering = false;


	Columns.push_back(tabHeader);

	if (ActiveTab == -1)
		ActiveTab = 0;
}

s32 CGUITable::getColumncount() const
{
	return Columns.size();
}

s32 CGUITable::getRowcount() const
{
	return Rows.size();
}

bool CGUITable::setActiveColumn(s32 idx) 
{
	if (idx < 0 || idx >= (s32)Columns.size())
		return false;

	bool changed = (ActiveTab != idx);

	ActiveTab = idx;

	m_CurrentOrdering = EGOM_ASCENDING;

	if ( !Columns[idx].useCustomOrdering )
		orderRows();

	if (changed)
	{
		SEvent event;
		event.EventType = EET_GUI_EVENT;
		event.GUIEvent.Caller = this;
		event.GUIEvent.EventType = EGET_TABLE_HEADER_CHANGED;
		Parent->OnEvent(event);
	}

	return true;
}

s32 CGUITable::getActiveColumn() const
{
	return ActiveTab;
}

EGUI_ORDERING_MODE CGUITable::getActiveColumnOrdering() const
{
	return m_CurrentOrdering;
}

void CGUITable::setColumnWidth(u32 columnIndex, u32 width)
{
	if ( columnIndex < Columns.size() )
	{
		if ( width < ( Font->getDimension(Columns[columnIndex].name.c_str() ).Width + (u32(CellWidthPadding) * 2) ) )
			width = Font->getDimension(Columns[columnIndex].name.c_str() ).Width + (CellWidthPadding * 2) + ARROW_PAD;

		Columns[columnIndex].width = width;
	}
}

void CGUITable::addRow(u32 rowIndex)
{
	if (!( rowIndex < (Rows.size() + 1) ) )
		return;

	Row row;

	if ( rowIndex == Rows.size() )
		Rows.push_back(row);
	else
		Rows.insert(row, rowIndex);

	for ( u32 i = 0 ; i < Columns.size() ; ++i )
	{
		Cell cell;
		cell.data = 0;
		Rows[rowIndex].Items.push_back(cell);
	}

	recalculate();
}

void CGUITable::removeRow(u32 rowIndex)
{
	if (!( rowIndex < Rows.size() ) )
		return;

	Rows.erase( rowIndex );

	if ( !(Selected < s32(Rows.size())) )
		Selected = Rows.size() - 1;

	recalculate();
}

//! adds an list item, returns id of item
void CGUITable::setCellText(u32 rowIndex, u32 columnIndex, const wchar_t* text)
{
	if ( rowIndex < (Rows.size() + 1) && columnIndex < Columns.size() )
	{
		Rows[rowIndex].Items[columnIndex].text = text;
		breakText( Rows[rowIndex].Items[columnIndex].text, Columns[columnIndex].width );

		IGUISkin* skin = Environment->getSkin();
		if ( skin )
			Rows[rowIndex].Items[columnIndex].color = skin->getColor(EGDC_BUTTON_TEXT);
	}
}

void CGUITable::setCellText(u32 rowIndex, u32 columnIndex, const wchar_t* text, video::SColor color)
{
	if ( rowIndex < (Rows.size() + 1) && columnIndex < Columns.size() )
	{
		Rows[rowIndex].Items[columnIndex].text = text;
		breakText( Rows[rowIndex].Items[columnIndex].text, Columns[columnIndex].width );
		Rows[rowIndex].Items[columnIndex].color = color;
	}
}

void CGUITable::setCellColor(u32 rowIndex, u32 columnIndex, video::SColor color)
{
	if ( rowIndex < (Rows.size() + 1) && columnIndex < Columns.size() )
	{
		Rows[rowIndex].Items[columnIndex].color = color;
	}
}

void CGUITable::setCellData(u32 rowIndex, u32 columnIndex, void *data)
{
	if ( rowIndex < (Rows.size() + 1) && columnIndex < Columns.size() )
	{
		Rows[rowIndex].Items[columnIndex].data = data;
	}
}

const wchar_t* CGUITable::getCellText(u32 rowIndex, u32 columnIndex ) const
{
	if ( rowIndex < (Rows.size() + 1) && columnIndex < Columns.size() )
	{
		return Rows[rowIndex].Items[columnIndex].text.c_str();
	}

	return 0;
}

void* CGUITable::getCellData(u32 rowIndex, u32 columnIndex ) const
{
	if ( rowIndex < (Rows.size() + 1) && columnIndex < Columns.size() )
	{
		return Rows[rowIndex].Items[columnIndex].data;
	}

	return 0;
}

//! clears the list
void CGUITable::clear()
{
	Rows.clear();
	Columns.clear();

	if (ScrollBar)
		ScrollBar->setPos(0);

	recalculate();
}

void CGUITable::clearRows()
{
	Rows.clear();

	if (ScrollBar)
		ScrollBar->setPos(0);

	recalculate();
}

s32 CGUITable::getSelected() const
{
	return Selected;
}

void CGUITable::recalculate()
{

	IGUISkin* skin = Environment->getSkin();
	if (Font != skin->getFont())
	{
		if (Font)
			Font->drop();

		Font = skin->getFont();

		ItemHeight = 0;

		if(Font)
		{
			ItemHeight = Font->getDimension(L"A").Height + (CellHeightPadding * 2);
			Font->grab();
		}
	}

	ScrollBar->setMax((ItemHeight * Rows.size()) - AbsoluteRect.getHeight() + ItemHeight);

}

//! called if an event happened.
bool CGUITable::OnEvent(const SEvent& event)
{

	switch(event.EventType)
	{
	case EET_GUI_EVENT:
		switch(event.GUIEvent.EventType)
		{
		case gui::EGET_SCROLL_BAR_CHANGED:
			if (event.GUIEvent.Caller == ScrollBar)
			{
				s32 pos = ((gui::IGUIScrollBar*)event.GUIEvent.Caller)->getPos();
				return true;
			}
			break;
		/*case gui::EGET_ELEMENT_FOCUS_LOST:
			{
				Selecting = false;
				return true;
			}
			break;*/
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

				if (Environment->hasFocus(this) &&
					ScrollBar->getAbsolutePosition().isPointInside(p) &&
					ScrollBar->OnEvent(event))
					return true;

				if ( selectColumnHeader( event.MouseInput.X, event.MouseInput.Y ) )
					return true;

				Selecting = true;
				Environment->setFocus(this);
				return true;

			case EMIE_LMOUSE_LEFT_UP:

				if (Environment->hasFocus(this) &&
					ScrollBar->getAbsolutePosition().isPointInside(p) &&
					ScrollBar->OnEvent(event))
				{
					return true;
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
						selectNew(event.MouseInput.Y);
						return true;
					}
				}
			}
		}
		break;
	}


	return Parent ? Parent->OnEvent(event) : false;
}

void CGUITable::setColumnCustomOrdering(u32 columnIndex, bool state)
{
	if ( columnIndex < Columns.size() )
		Columns[columnIndex].useCustomOrdering = state;
}

void CGUITable::swapRows(u32 rowIndexA, u32 rowIndexB)
{
	if ( rowIndexA >= Rows.size() )
		return;

	if ( rowIndexB >= Rows.size() )
		return;

	Row swap = Rows[rowIndexA];
	Rows[rowIndexA] = Rows[rowIndexB];
	Rows[rowIndexB] = swap;

	if ( Selected == s32(rowIndexA) )
		Selected = rowIndexB;
	else if( Selected == s32(rowIndexB) )
		Selected = rowIndexA;

}

bool CGUITable::selectColumnHeader(s32 xpos, s32 ypos)
{
	if ( ypos > ( AbsoluteRect.UpperLeftCorner.Y + ItemHeight ) )
		return false;


	core::rect<s32> frameRect(AbsoluteRect);

	s32 pos = frameRect.UpperLeftCorner.X;;
	u32 colWidth;

	for ( u32 i = 0 ; i < Columns.size() ; ++i )
	{
		colWidth = Columns[i].width;

		if ( xpos >= pos && xpos < ( pos + s32(colWidth) ) )
		{
			if ( ActiveTab == s32(i) )
			{
				if ( m_CurrentOrdering == EGOM_ASCENDING )
					m_CurrentOrdering = EGOM_DESCENDING;
				else
					m_CurrentOrdering = EGOM_ASCENDING;
			}
			else
			{
				ActiveTab = i;
				m_CurrentOrdering = EGOM_ASCENDING;
			}

			if ( !Columns[i].useCustomOrdering )
				orderRows();
			else
			{
				if (Parent)
				{
					SEvent event;
					event.EventType = EET_GUI_EVENT;
					event.GUIEvent.Caller = this;
					event.GUIEvent.EventType = EGET_TABLE_HEADER_CHANGED;
					Parent->OnEvent(event);
				}
			}

			break;
		}

		pos += colWidth;
	}

	return false;
}

void CGUITable::orderRows()
{
	Row swap;
	s32 columnIndex = getActiveColumn();

	if ( m_CurrentOrdering == EGOM_ASCENDING )
	{
		for ( s32 i = 0 ; i < s32(Rows.size()) - 1 ; ++i )
		{
			for ( s32 j = 0 ; j < s32(Rows.size()) - i - 1 ; ++j )
			{
				if ( Rows[j+1].Items[columnIndex].text < Rows[j].Items[columnIndex].text )
				{
					swap = Rows[j];
					Rows[j] = Rows[j+1];
					Rows[j+1] = swap;

					if ( Selected == j )
						Selected = j+1;
					else if( Selected == j+1 )
						Selected = j;
				}
			}
		}
	}
	else
	{
		for ( s32 i = 0 ; i < s32(Rows.size()) - 1 ; ++i )
		{
			for ( s32 j = 0 ; j < s32(Rows.size()) - i - 1 ; ++j )
			{
				if ( Rows[j].Items[columnIndex].text < Rows[j+1].Items[columnIndex].text)
				{
					swap = Rows[j];
					Rows[j] = Rows[j+1];
					Rows[j+1] = swap;

					if ( Selected == j )
						Selected = j+1;
					else if( Selected == j+1 )
						Selected = j;
				}
			}
		}
	}
}


void CGUITable::selectNew(s32 ypos, bool onlyHover)
{
	IGUISkin* skin = Environment->getSkin();
	if (!skin)
		return;

	s32 oldSelected = Selected;

	if ( ypos < ( AbsoluteRect.UpperLeftCorner.Y + ItemHeight ) )
		return;

	// find new selected item.
	if (ItemHeight!=0)
		Selected = ((ypos - AbsoluteRect.UpperLeftCorner.Y - ItemHeight - 1) + ScrollBar->getPos()) / ItemHeight;

	if (Selected >= (s32)Rows.size())
		Selected = Rows.size() - 1;
	else if (Selected<0)
		Selected = 0;

	// post the news
	if (Parent && !onlyHover)
	{
		SEvent event;
		event.EventType = EET_GUI_EVENT;
		event.GUIEvent.Caller = this;
		event.GUIEvent.EventType = (Selected != oldSelected) ? EGET_TABLE_CHANGED : EGET_TABLE_SELECTED_AGAIN;
		Parent->OnEvent(event);
	}
}

//! draws the element and its children
void CGUITable::draw()
{
	if (!IsVisible)
		return;

	irr::video::IVideoDriver* driver = Environment->getVideoDriver();

	core::rect<s32> frameRect(AbsoluteRect);
	core::rect<s32> columnSeparator(AbsoluteRect);

	IGUISkin* skin = Environment->getSkin();
	if (!skin)
		return;

	IGUIFont* font = skin->getFont();
	if (!font)
		return;

	s32 headerFinalPosition = frameRect.UpperLeftCorner.Y + ItemHeight;


	frameRect = AbsoluteRect;
	frameRect.UpperLeftCorner.X += 1;
	frameRect.UpperLeftCorner.Y = headerFinalPosition;
	frameRect.LowerRightCorner.X = AbsoluteRect.LowerRightCorner.X - skin->getSize(EGDS_SCROLLBAR_SIZE);
	frameRect.LowerRightCorner.Y = headerFinalPosition + ItemHeight;

	frameRect.UpperLeftCorner.Y -= ScrollBar->getPos();
	frameRect.LowerRightCorner.Y -= ScrollBar->getPos();

	core::rect<s32> clientClip(AbsoluteRect);
	clientClip.UpperLeftCorner.Y = headerFinalPosition + 1;
	clientClip.UpperLeftCorner.X += 1;
	clientClip.LowerRightCorner.X = AbsoluteRect.LowerRightCorner.X - skin->getSize(EGDS_SCROLLBAR_SIZE);
	clientClip.LowerRightCorner.Y -= 1;

	u32 pos;

	for ( u32 i = 0 ; i < Rows.size() ; ++i )
	{

		if (frameRect.LowerRightCorner.Y >= AbsoluteRect.UpperLeftCorner.Y &&
			  frameRect.UpperLeftCorner.Y <= AbsoluteRect.LowerRightCorner.Y)
		{

			core::rect<s32> textRect = frameRect;

			textRect.UpperLeftCorner.Y = textRect.LowerRightCorner.Y - 1;
			driver->draw2DRectangle(skin->getColor(EGDC_3D_SHADOW), textRect, &clientClip);

			textRect = frameRect;

			pos = frameRect.UpperLeftCorner.X;

			if (s32(i) == Selected)
				driver->draw2DRectangle(skin->getColor(EGDC_HIGH_LIGHT), frameRect, &clientClip);

			for ( u32 j = 0 ; j < Columns.size() ; ++j )
			{
				textRect.UpperLeftCorner.X = pos + CellWidthPadding;
				textRect.LowerRightCorner.X = pos + Columns[j].width - CellWidthPadding;

				s32 test = font->getDimension(Rows[i].Items[j].text.c_str()).Width;

				if (s32(i) == Selected)
				{
					font->draw(Rows[i].Items[j].text.c_str(), textRect, skin->getColor(EGDC_HIGH_LIGHT_TEXT), false, true, &clientClip);
				}
				else
				{
					font->draw(Rows[i].Items[j].text.c_str(), textRect, Rows[i].Items[j].color, false, true, &clientClip);
				}

				pos += Columns[j].width;
			}
		}

		frameRect.UpperLeftCorner.Y += ItemHeight;
		frameRect.LowerRightCorner.Y += ItemHeight;
	}


	frameRect = AbsoluteRect;
	columnSeparator.UpperLeftCorner.Y = headerFinalPosition;

	core::rect<s32>* clipRect = 0;
	if (Clip)
		clipRect = &AbsoluteClippingRect;

	const wchar_t* text = 0;
	pos = frameRect.UpperLeftCorner.X;
	u32 colWidth;

	skin->draw3DSunkenPane(this, skin->getColor(EGDC_3D_HIGH_LIGHT), true, DrawBack, frameRect, clipRect);

	for (u32 i = 0 ; i < Columns.size() ; ++i )
	{
		text = Columns[i].name.c_str();
		colWidth = Columns[i].width;

		core::dimension2d<s32 > dim = font->getDimension(text);

		core::rect<s32> columnrect(pos, frameRect.UpperLeftCorner.Y, pos + colWidth, headerFinalPosition);

		skin->draw3DButtonPaneStandard(this, columnrect, &AbsoluteClippingRect);

		columnSeparator.UpperLeftCorner.X = columnrect.LowerRightCorner.X;
		columnSeparator.LowerRightCorner.X = columnrect.LowerRightCorner.X + 1;
		driver->draw2DRectangle(skin->getColor(EGDC_3D_SHADOW), columnSeparator, &AbsoluteClippingRect);

		columnrect.UpperLeftCorner.X += CellWidthPadding;
		font->draw(text, columnrect, skin->getColor(EGDC_BUTTON_TEXT), false, true, &AbsoluteClippingRect);

		if ( s32(i) == ActiveTab )
		{
			if ( m_CurrentOrdering == EGOM_ASCENDING ) 
			{
				columnrect.UpperLeftCorner.X = columnrect.LowerRightCorner.X - CellWidthPadding - ARROW_PAD / 2 + 2;
				columnrect.UpperLeftCorner.Y += 7;
				skin->drawIcon(this,EGDI_CURSOR_UP,columnrect.UpperLeftCorner);
			}
			else 
			{					
				columnrect.UpperLeftCorner.X = columnrect.LowerRightCorner.X - CellWidthPadding - ARROW_PAD / 2 + 2;
				columnrect.UpperLeftCorner.Y += 7;
				skin->drawIcon(this,EGDI_CURSOR_DOWN,columnrect.UpperLeftCorner);
			}
		}

		pos += colWidth;
	}

	core::rect<s32> columnrect(pos, frameRect.UpperLeftCorner.Y, frameRect.LowerRightCorner.X - skin->getSize(EGDS_SCROLLBAR_SIZE), headerFinalPosition);
	skin->draw3DButtonPaneStandard(this, columnrect, &AbsoluteClippingRect);

	IGUIElement::draw();

}

void CGUITable::breakText(core::stringw &text, u32 cellWidth )
{
	IGUISkin* skin = Environment->getSkin();

	if (!skin)
		return;

	if (!Font)
		return;

	IGUIFont* font = skin->getFont();
	if (!font)
		return;


	core::stringw line, lineDots, character;
	wchar_t c[2];
	c[1] = L'\0';

	u32 maxLength = cellWidth - (CellWidthPadding * 2);
	u32 maxLengthDots = cellWidth - (CellWidthPadding * 2) - font->getDimension(L"...").Width;
	u32 size = text.size();
	u32 pos = 0;

	u32 i;

	for (i=0; i<size; ++i)
	{
		c[0] = text[i];

		if (c[0] == L'\n')
			break;

		pos += font->getDimension( c ).Width;
		if ( pos > maxLength )
			break;

		if ( font->getDimension( (line + c[0]).c_str() ).Width > s32(maxLengthDots) )
			lineDots = line;

		line += c[0];
	}

	if ( i < size )
		line = lineDots + L"...";


	text = line;

}

} // end namespace gui
} // end namespace irr

#endif // _IRR_COMPILE_WITH_GUI_

