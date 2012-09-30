// Copyright (C) 2002-2012 Nikolaus Gebhardt / Gaz Davidson
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CGUITextureCacheBrowser.h"
#include "IGUIEnvironment.h"
#include "IGUIButton.h"
#include "IGUISkin.h"
#include "IGUIFont.h"
#include "IVideoDriver.h"

namespace irr
{
namespace gui
{

CGUITextureCacheBrowser::CGUITextureCacheBrowser(IGUIEnvironment* environment, s32 id, IGUIElement *parent)
:	IGUIWindow(environment, parent, id, core::rect<s32>(0,0,300,200)),
	CloseButton(0), Panel(0), SelectedTexture(-1), Dragging(false), IsDraggable(true)
{
	#ifdef _DEBUG
	setDebugName("CGUITextureCacheBrowser");
	#endif

	IGUISkin* skin = 0;
	IGUISpriteBank* sprites = 0;
	video::SColor color(255,255,255,255);

	if (environment)
		skin = environment->getSkin();

	s32 buttonw = 15;
	if (skin)
	{
		buttonw = skin->getSize(EGDS_WINDOW_BUTTON_WIDTH);
		sprites = skin->getSpriteBank();
		color = skin->getColor(EGDC_WINDOW_SYMBOL);
	}
	s32 posx = RelativeRect.getWidth() - buttonw - 4;

	CloseButton = Environment->addButton(core::rect<s32>(posx, 3, posx + buttonw, 3 + buttonw), this, -1,
		L"", skin ? skin->getDefaultText(EGDT_WINDOW_CLOSE) : L"Close" );
	CloseButton->setSubElement(true);
	CloseButton->setAlignment(EGUIA_LOWERRIGHT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);
	if (sprites)
	{
		CloseButton->setSpriteBank(sprites);
		CloseButton->setSprite(EGBS_BUTTON_UP, skin->getIcon(EGDI_WINDOW_CLOSE), color);
		CloseButton->setSprite(EGBS_BUTTON_DOWN, skin->getIcon(EGDI_WINDOW_CLOSE), color);
	}

	CloseButton->grab();

	// window title
	Text = L"Texture Browser";

	// panel element
	Panel = new CGUIPanel(environment, this);
	Panel->setRelativePosition( core::rect<s32>(1, buttonw + 5, 151, RelativeRect.getHeight() - 1));
	Panel->setAlignment(EGUIA_UPPERLEFT, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT);
	Panel->setBorder(true);
	Panel->setSubElement(true);

	// some buttons


	// add images from texture cache
	updateImageList();

}

CGUITextureCacheBrowser::~CGUITextureCacheBrowser()
{
	if (CloseButton)
		CloseButton->drop();
	if (Panel)
		Panel->drop();

	// drop images
	u32 i;
	for (i=0; i<Images.size(); ++i)
	{
		Images[i]->drop();
		Images[i]->remove();
	}
	Images.clear();
}
void CGUITextureCacheBrowser::updateImageList()
{
	if (!Panel)
		return;

	video::IVideoDriver* Driver = Environment->getVideoDriver();

	// clear images
	u32 i;
	for (i=0; i<Images.size(); ++i)
	{
		Images[i]->drop();
		Images[i]->remove();
	}
	Images.clear();

	u32 count = (u32)Driver->getTextureCount();

	s32 h = Panel->getClientArea().getWidth()-10;
	s32 hw = h/2;
	core::rect<s32> pos(Panel->getClientArea().getCenter().X - Panel->getAbsolutePosition().UpperLeftCorner.X - hw, 5,
						Panel->getClientArea().getCenter().X - Panel->getAbsolutePosition().UpperLeftCorner.X + hw, h+5);

	core::position2di moveDist(0, h+5);

	for (u32 i=0; i<count; ++i)
	{
		core::stringw details;
		video::ITexture* tex = Driver->getTextureByIndex(i);
		details = L"File name: ";
		details += tex->getName();
		details += L"\nFormat: ";
		video::ECOLOR_FORMAT cf = tex->getColorFormat();

		bool alpha = false;

		switch (cf)
		{
		case video::ECF_A1R5G5B5:
			details += L"A1R5G5B5 (16-bit with 1-bit alpha channel)\n";
			alpha = true;
			break;
		case video::ECF_R5G6B5:
			details += L"R5G6B5 (16-bit, no alpha channel)\n";
			break;
		case video::ECF_R8G8B8:
			details += L"R8G8B8 (16-bit, no alpha channel)\n";
			break;
		case video::ECF_A8R8G8B8:
			details += L"R8G8B8 (32-bit with 8-bit alpha channel)\n";
			alpha = true;
			break;
		default:
			details += L"Unknown\n";
		}

		core::dimension2du osize = tex->getOriginalSize();
		core::dimension2du size = tex->getOriginalSize();

		details += "Size: ";
		details += size.Width;
		details += "x";
		details += size.Height;

		if (osize != size)
		{
			details += "\nOriginal Size: ";
			details += osize.Width;
			details += "x";
			details += osize.Height;
		}

		details += L"\nMip-maps: ";

		if (tex->hasMipMaps())
			details += L"Yes\n";
		else
			details += L"No\n";

		IGUIImage* img = Environment->addImage(tex, core::position2di(1,1), alpha, Panel, i);
		img->grab();
		Images.push_back(img);
		img->setRelativePosition(pos);
		img->setToolTipText(details.c_str());
		img->setScaleImage(true);
		img->setColor( SelectedTexture == (s32)i ? video::SColor(255,255,255,255) : video::SColor(128,128,128,128) );

		pos = pos + moveDist;
	}
}

void CGUITextureCacheBrowser::updateAbsolutePosition()
{
	IGUIWindow::updateAbsolutePosition();
	updateImageList();
}

//! called if an event happened.
bool CGUITextureCacheBrowser::OnEvent(const SEvent &event)
{
	switch(event.EventType)
	{
	case EET_GUI_EVENT:
		if (event.GUIEvent.EventType == EGET_ELEMENT_FOCUS_LOST)
		{
			if (event.GUIEvent.Caller == (IGUIElement*)this)
				Dragging = false;
			return true;
		}
		else
		if (event.GUIEvent.EventType == EGET_BUTTON_CLICKED)
		{
			if (event.GUIEvent.Caller == CloseButton)
			{
				remove();
				return true;
			}
		}
		break;
	case EET_MOUSE_INPUT_EVENT:
		switch(event.MouseInput.Event)
		{
		case EMIE_LMOUSE_PRESSED_DOWN:
			DragStart.X = event.MouseInput.X;
			DragStart.Y = event.MouseInput.Y;

			if (getElementFromPoint(DragStart) == this)
			{
				if (!Environment->hasFocus(this))
				{
					Dragging = IsDraggable;
					//Environment->setFocus(this);
					if (Parent)
						Parent->bringToFront(this);
				}
				return true;
			}
			else
			{
				if (Panel->getAbsolutePosition().isPointInside(DragStart))
				{
					// select an image
					IGUIElement* el = Panel->getElementFromPoint(DragStart);
					if (el && el != Panel)
					{
						if (el->getType() == EGUIET_IMAGE)
						{
							setSelected(el->getID());
						}
					}
					else
					{
						setSelected();
					}
				}
			}
			break;
		case EMIE_LMOUSE_LEFT_UP:
			Dragging = false;
			//Environment->removeFocus(this);
			return true;
		case EMIE_MOUSE_MOVED:
			if (Dragging)
			{
				// gui window should not be dragged outside its parent
				if (Parent)
					if (event.MouseInput.X < Parent->getAbsolutePosition().UpperLeftCorner.X +1 ||
						event.MouseInput.Y < Parent->getAbsolutePosition().UpperLeftCorner.Y +1 ||
						event.MouseInput.X > Parent->getAbsolutePosition().LowerRightCorner.X -1 ||
						event.MouseInput.Y > Parent->getAbsolutePosition().LowerRightCorner.Y -1)

						return true;


				move(core::position2d<s32>(event.MouseInput.X - DragStart.X, event.MouseInput.Y - DragStart.Y));
				DragStart.X = event.MouseInput.X;
				DragStart.Y = event.MouseInput.Y;
				return true;
			}
			break;
		default:
			break;
		}
	default:
		break;
	}

	return Parent ? Parent->OnEvent(event) : false;
}

void CGUITextureCacheBrowser::setSelected(s32 index)
{
	SelectedTexture = index;
	updateImageList();
	printf("Texture %d selected\n", index);
}

void CGUITextureCacheBrowser::draw()
{
	if (!IsVisible)
		return;

	IGUISkin* skin = Environment->getSkin();

	core::rect<s32> rect = AbsoluteRect;
	core::rect<s32> *cl = &AbsoluteClippingRect;

	// draw body fast
	rect = skin->draw3DWindowBackground(this, true, skin->getColor(EGDC_ACTIVE_BORDER),
		AbsoluteRect, &AbsoluteClippingRect);

	// draw window text
	if (Text.size())
	{
		rect.UpperLeftCorner.X += skin->getSize(EGDS_TEXT_DISTANCE_X);
		rect.UpperLeftCorner.Y += skin->getSize(EGDS_TEXT_DISTANCE_Y);
		rect.LowerRightCorner.X -= skin->getSize(EGDS_WINDOW_BUTTON_WIDTH) + 5;

		IGUIFont* font = skin->getFont();
		if (font)
			font->draw(Text.c_str(), rect, skin->getColor(EGDC_ACTIVE_CAPTION), false, true, cl);
	}

	IGUIElement::draw();
}


bool CGUITextureCacheBrowser::isDraggable() const
{
	return IsDraggable;
}

void CGUITextureCacheBrowser::setDraggable(bool draggable)
{
	IsDraggable = draggable;

	if (Dragging && !IsDraggable)
		Dragging = false;
}


//! Returns the rectangle of the drawable area (without border, without titlebar and without scrollbars)
core::rect<s32> CGUITextureCacheBrowser::getClientRect() const
{
	return core::recti();
}

} // namespace gui
} // namespace irr
