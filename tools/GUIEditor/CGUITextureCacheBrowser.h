// Copyright (C) 2002-2012 Nikolaus Gebhardt / Gaz Davidson
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_GUI_TEXTURE_CACHE_BROWSER_H_INCLUDED__
#define __C_GUI_TEXTURE_CACHE_BROWSER_H_INCLUDED__

#include "IGUIWindow.h"
#include "CGUIPanel.h"
#include "IGUIImage.h"
#include "EGUIEditTypes.h"

namespace irr
{
namespace gui
{

	//! Texture cache browser

	const u32 TEXTURE_BROWSER_TEXTURE_SELECTED = 0x5E1EC7ED; // custom event number for texture selected

	class CGUITextureCacheBrowser : public IGUIWindow
	{
	public:

		//! constructor
		CGUITextureCacheBrowser(IGUIEnvironment* environment, s32 id=-1, IGUIElement *parent=0);

		//! destructor
		~CGUITextureCacheBrowser();

		//! event handler
		virtual bool OnEvent(const SEvent &event);

		//! draws the element
		virtual void draw();

		//! update absolute position
		virtual void updateAbsolutePosition();

		//! this shoudln't be serialized, but this is included as it's an example
		virtual const c8* getTypeName() const { return "textureCacheBrowser"; }

		//! Returns pointer to the close button
		virtual IGUIButton* getCloseButton() const { return CloseButton; }

		//! Returns pointer to the minimize button
		virtual IGUIButton* getMinimizeButton() const { return 0;}

		//! Returns pointer to the maximize button
		virtual IGUIButton* getMaximizeButton() const { return 0;}

		//! get draggable
		virtual bool isDraggable() const;

		//! get draggable
		virtual void setDraggable(bool draggable);

		//! not used
		virtual core::rect<s32> getClientRect() const;
        virtual void setDrawBackground(bool draw)  { }
		virtual bool getDrawBackground() const { return true; }
		virtual void setDrawTitlebar(bool draw) { }
		virtual bool getDrawTitlebar() const { return true; }


		void setSelected(s32 index=-1);

	private:

		void updateImageList();

		core::array<IGUIImage*> Images;
		core::position2d<s32> DragStart;

		IGUIButton* CloseButton;
		CGUIPanel*	Panel;
		s32 SelectedTexture;
		bool Dragging;
		bool IsDraggable;
	};


} // end namespace gui
} // end namespace irr

#endif

