// Copyright 2006-2012 Asger Feldthaus
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

/*
	Originally Klasker's but I've messed around with it lots - Gaz
*/

#include "CGUIPanel.h"
#include "IGUIEnvironment.h"
#include "IGUIScrollBar.h"
#include "IGUITabControl.h"
#include "IVideoDriver.h"

const int SCROLL_BAR_SIZE = 16; // Scroll bars are 16 pixels wide
const int BORDER_WIDTH = 2;

namespace irr
{
namespace gui
{

CGUIPanel::CGUIPanel(IGUIEnvironment* environment, IGUIElement* parent, s32 id, const core::rect<s32>& rectangle,
			bool border, E_SCROLL_BAR_MODE vMode, E_SCROLL_BAR_MODE hMode)
	: IGUIElement(EGUIET_ELEMENT, environment, parent, id, rectangle),
	VScrollBar(0), HScrollBar(0), ClipPane(0), InnerPane(0),
	VScrollBarMode(vMode), HScrollBarMode(hMode), NeedsUpdate(true), Border(border)
{
	#ifdef _DEBUG
	setDebugName("CGUIPanel");
	#endif

	s32 width = rectangle.getWidth();
	s32 height = rectangle.getHeight();

	core::rect<s32> rct = core::rect<s32>(width - SCROLL_BAR_SIZE,0, width, height);

	VScrollBar = environment->addScrollBar(false, rct, 0, id);
	VScrollBar->setSubElement(true);
	VScrollBar->setTabStop(false);
	VScrollBar->setAlignment(EGUIA_LOWERRIGHT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT);
	VScrollBar->grab();
	IGUIElement::addChild(VScrollBar);

	rct = core::rect<s32>(0, height - SCROLL_BAR_SIZE, width - SCROLL_BAR_SIZE,height );

	HScrollBar = environment->addScrollBar(true, rct, 0, id);
	HScrollBar->setSubElement(true);
	HScrollBar->setTabStop(false);
	HScrollBar->setAlignment(EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT, EGUIA_LOWERRIGHT, EGUIA_LOWERRIGHT);
	HScrollBar->grab();
	IGUIElement::addChild(HScrollBar);

	rct = core::rect<s32>(0,0, width - SCROLL_BAR_SIZE, height - SCROLL_BAR_SIZE);

	ClipPane = environment->addTab( rct, 0, -1);
	ClipPane->setSubElement(true);
	ClipPane->setAlignment(EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT);
	ClipPane->grab();
	IGUIElement::addChild(ClipPane);

	InnerPane = environment->addTab(rct, ClipPane, -1);
	InnerPane->setSubElement(true);
	InnerPane->grab();

	calculateClientArea();
	resizeInnerPane();
}

CGUIPanel::~CGUIPanel()
{
	// because the inner pane has the list of children, we need to remove the outer ones manually
	IGUIElement::removeChild(VScrollBar);
	IGUIElement::removeChild(HScrollBar);
	IGUIElement::removeChild(ClipPane);

	// now we can drop the others
	VScrollBar->drop();
	HScrollBar->drop();
	ClipPane->drop();
	InnerPane->drop();
}


void CGUIPanel::draw()
{
	if (NeedsUpdate)
	{
		calculateClientArea();
		resizeInnerPane();
		NeedsUpdate = false;
	}

	IGUISkin* skin = Environment->getSkin();
	if (Border && skin)
	{
		skin->draw3DSunkenPane( this, skin->getColor( EGDC_APP_WORKSPACE), false, true, AbsoluteRect, &AbsoluteClippingRect );
	}

	IGUIElement::draw();
}

void CGUIPanel::addChild(IGUIElement *child)
{
	// add the child to the inner pane
	InnerPane->addChild(child);

	NeedsUpdate = true;
}

void CGUIPanel::removeChild(IGUIElement *child)
{
	InnerPane->removeChild(child);

	NeedsUpdate = true;
}

//! returns children of the inner pane
const core::list<IGUIElement*>& CGUIPanel::getChildren()
{
	return InnerPane->getChildren();
}

bool CGUIPanel::hasBorder() const
{
	return Border;
}

void CGUIPanel::setBorder( bool enabled )
{
	Border = enabled;
}

IGUIScrollBar* CGUIPanel::getVScrollBar() const
{
	return VScrollBar;
}

IGUIScrollBar* CGUIPanel::getHScrollBar() const
{
	return HScrollBar;
}

E_SCROLL_BAR_MODE CGUIPanel::getVScrollBarMode() const
{
	return VScrollBarMode;
}

void CGUIPanel::setVScrollBarMode( E_SCROLL_BAR_MODE mode )
{
	VScrollBarMode = mode;
	NeedsUpdate = true;
}

E_SCROLL_BAR_MODE CGUIPanel::getHScrollBarMode() const
{
	return HScrollBarMode;
}

void CGUIPanel::setHScrollBarMode(E_SCROLL_BAR_MODE mode)
{
	HScrollBarMode = mode;
	NeedsUpdate = true;
}

bool CGUIPanel::OnEvent(const SEvent &event)
{
	// Redirect mouse wheel to scrollbar
	if (event.EventType == EET_MOUSE_INPUT_EVENT && event.MouseInput.Event == EMIE_MOUSE_WHEEL)
	{
		if (VScrollBar->isVisible())
		{
			Environment->setFocus(VScrollBar);
			VScrollBar->OnEvent(event);
			return true;
		}
		else if (VScrollBar->isVisible())
		{
			Environment->setFocus(HScrollBar);
			HScrollBar->OnEvent(event);
			return true;
		}
	}
	else
	{
		if (event.EventType == EET_GUI_EVENT && event.GUIEvent.EventType == EGET_SCROLL_BAR_CHANGED &&
			(event.GUIEvent.Caller == HScrollBar || event.GUIEvent.Caller == VScrollBar) )
		{
			moveInnerPane();

			return true;
		}
	}

	return IGUIElement::OnEvent(event);
}

void CGUIPanel::moveInnerPane()
{
	core::dimension2d<s32> dim = InnerPane->getAbsolutePosition().getSize();
	core::position2d<s32> newpos(HScrollBar->isVisible() ? -HScrollBar->getPos() : 0 , VScrollBar->isVisible() ? -VScrollBar->getPos() : 0);
	core::rect<s32> r(newpos, newpos + dim);
	InnerPane->setRelativePosition(r);
}


void CGUIPanel::updateAbsolutePosition()
{
	IGUIElement::updateAbsolutePosition();
	calculateClientArea();
	resizeInnerPane();
}


void CGUIPanel::resizeInnerPane()
{
	if (!HScrollBar || !VScrollBar || !InnerPane || !ClipPane)
		return;

	// get outer pane size
	core::rect<s32> outerRect = ClipPane->getRelativePosition();

	// resize flexible children depending on outer pane
	InnerPane->setRelativePosition(outerRect);

	// get desired size (total size of all children)
	core::rect<s32> totalRect(0, 0, 0, 0);

	core::list<IGUIElement*>::ConstIterator it;

	for (it  = InnerPane->getChildren().begin();
         it != InnerPane->getChildren().end(); ++it)
	{
		core::rect<s32> rct = (*it)->getRelativePosition();
		totalRect.addInternalPoint(rct.UpperLeftCorner);
		totalRect.addInternalPoint(rct.LowerRightCorner);
	}

	// move children if pane needs to grow
	core::position2di adjustedMovement(0,0);

	if (totalRect.UpperLeftCorner.X < 0)
		adjustedMovement.X = -totalRect.UpperLeftCorner.X;
	if (totalRect.UpperLeftCorner.Y < 0)
		adjustedMovement.Y = -totalRect.UpperLeftCorner.Y;

	if (adjustedMovement.X > 0 || adjustedMovement.Y > 0)
	{
		totalRect += adjustedMovement;

		for (it = InnerPane->getChildren().begin();
			it != InnerPane->getChildren().end(); ++it )
		{
			(*it)->move(adjustedMovement);
		}
	}

	// make sure the inner pane is at least as big as the outer
	if (totalRect.getWidth() < outerRect.getWidth())
	{
		totalRect.UpperLeftCorner.X = 0;
		totalRect.LowerRightCorner.X = outerRect.getWidth();
	}
	if (totalRect.getHeight() < outerRect.getHeight())
	{
		totalRect.UpperLeftCorner.Y = 0;
		totalRect.LowerRightCorner.Y = outerRect.getHeight();
	}

	InnerPane->setRelativePosition(totalRect);

	// scrollbars
	if ( HScrollBarMode != ESBM_ALWAYS_INVISIBLE &&
		(totalRect.getWidth() > outerRect.getWidth() || HScrollBarMode == ESBM_ALWAYS_VISIBLE) )
	{
		HScrollBar->setVisible(true);
		HScrollBar->setMax(totalRect.getWidth() - outerRect.getWidth());
		bringToFront(HScrollBar);
	}
	else
		HScrollBar->setVisible(false);

	if ( VScrollBarMode != ESBM_ALWAYS_INVISIBLE &&
		(totalRect.getHeight() > outerRect.getHeight() || VScrollBarMode == ESBM_ALWAYS_VISIBLE) )
	{
		VScrollBar->setVisible(true);
		VScrollBar->setMax(totalRect.getHeight() - outerRect.getHeight());
		bringToFront(VScrollBar);
	}
	else
		VScrollBar->setVisible(false);

	// move to adjust for scrollbar pos
	moveInnerPane();
}

void CGUIPanel::calculateClientArea()
{
	core::rect<s32> ClientArea(0,0, AbsoluteRect.getWidth(),AbsoluteRect.getHeight());

	if (VScrollBar->isVisible())
		ClientArea.LowerRightCorner.X -= VScrollBar->getRelativePosition().getWidth();

	if (HScrollBar->isVisible())
		ClientArea.LowerRightCorner.Y -= HScrollBar->getRelativePosition().getHeight();

	if (Border)
	{
		ClientArea.UpperLeftCorner += core::position2d<s32>( BORDER_WIDTH, BORDER_WIDTH );
		ClientArea.LowerRightCorner -= core::position2d<s32>( BORDER_WIDTH, BORDER_WIDTH );
	}

	ClipPane->setRelativePosition(ClientArea);
}

core::rect<s32> CGUIPanel::getClientArea() const
{
	return ClipPane->getRelativePosition();
}

void CGUIPanel::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options)
{
	IGUIElement::serializeAttributes(out, options);

	out->addBool("border", Border);
	out->addEnum("horizontalScrollBar", HScrollBarMode, GUIScrollBarModeNames );
	out->addEnum("verticalScrollBar", VScrollBarMode, GUIScrollBarModeNames );
}

void CGUIPanel::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options)
{
	IGUIElement::deserializeAttributes(in, options);

	setBorder(in->getAttributeAsBool("border"));
	setHScrollBarMode((E_SCROLL_BAR_MODE)in->getAttributeAsEnumeration("horizontalScrollBar", GUIScrollBarModeNames));
	setVScrollBarMode((E_SCROLL_BAR_MODE)in->getAttributeAsEnumeration("verticalScrollBar", GUIScrollBarModeNames));
}

} // namespace gui
} // namespace irr
