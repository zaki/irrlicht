// Copyright 2006-2012 Asger Feldthaus
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef _C_GUI_PANEL_H_
#define _C_GUI_PANEL_H_

#include "IGUIElement.h"
#include "EGUIEditTypes.h"

namespace irr
{
namespace gui
{

class IGUIScrollBar;
class IGUITab;

enum E_SCROLL_BAR_MODE
{
	//! The scrollbar will only show up when needed.
	ESBM_AUTOMATIC = 0,

	//! The scrollbar will never be visible.
	ESBM_ALWAYS_INVISIBLE,

	//! The scrollbar will always the visible.
	ESBM_ALWAYS_VISIBLE,

	//! just a count of how many are in this enum
	ESBM_COUNT
};

const c8* const GUIScrollBarModeNames[] =
{
	"automatic",
	"alwaysInvisible",
	"alwaysVisible",
	0
};

class CGUIPanel : public IGUIElement
{
public:
	CGUIPanel( IGUIEnvironment* environment, IGUIElement* parent, s32 id=-1,
			const core::rect<s32>& rectangle = core::rect<s32>(0,0,100,100),
			bool border=false,
			E_SCROLL_BAR_MODE vMode=ESBM_AUTOMATIC,
			E_SCROLL_BAR_MODE hMode=ESBM_ALWAYS_INVISIBLE );

	virtual ~CGUIPanel();

	//! draws the panel and its children
	virtual void draw();

	//! returns true if it has a border, false if not
	bool hasBorder() const;

	//! sets whether the element draws a border
	void setBorder(bool enabled);

	//! returns a pointer to the vertical scrollbar
	IGUIScrollBar* getVScrollBar() const;

	//! returns a pointer to the horizontal scrollbar
	IGUIScrollBar* getHScrollBar() const;

	//! returns the vertical scrollbar visibility rule
	E_SCROLL_BAR_MODE getVScrollBarMode() const;

	//! sets the vertical scrollbar visibility rule
	void setVScrollBarMode(E_SCROLL_BAR_MODE mode);

	//! returns the horizontal scrollbar visibility rule
	E_SCROLL_BAR_MODE getHScrollBarMode() const;

	//! sets the horizontal scrollbar visibility rule
	void setHScrollBarMode(E_SCROLL_BAR_MODE mode);

	//! returns the visible area inside the panel, excluding scrollbar and border
	core::rect<s32> getClientArea() const;

	virtual bool OnEvent(const SEvent &event);

	//! adds a child to the panel
	virtual void addChild(IGUIElement* child);

	//! removes a child from the panel
	virtual void removeChild(IGUIElement* child);

	//! updates the absolute position
	virtual void updateAbsolutePosition();

	//! returns children of the inner pane
	virtual const core::list<IGUIElement*>& getChildren();

	//! Returns the type name of the gui element.
	virtual const c8* getTypeName() const
	{
		return GUIEditElementTypeNames[EGUIEDIT_GUIPANEL];
	}

	virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options=0);
	virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options=0);

protected:
	void moveInnerPane();
	void resizeInnerPane();
	void calculateClientArea();

private:

	IGUIScrollBar*	VScrollBar;
	IGUIScrollBar*	HScrollBar;
	IGUITab*	ClipPane;
	IGUITab*	InnerPane;

	E_SCROLL_BAR_MODE VScrollBarMode;
	E_SCROLL_BAR_MODE HScrollBarMode;

	bool NeedsUpdate;
	bool Border;
};

} // namespace gui
} // namespace irr

#endif
