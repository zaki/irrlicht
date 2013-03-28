// Copyright (C) 2002-2012 Nikolaus Gebhardt / Gaz Davidson
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_GUIEDIT_WORKSPACE_H_INCLUDED__
#define __C_GUIEDIT_WORKSPACE_H_INCLUDED__

#include "IGUIElement.h"
#include "CGUIEditWindow.h"
#include "EGUIEditTypes.h"

namespace irr
{
namespace gui
{

	//! Adding the GUI Editor Workspace to an element allows you
	/** to create, edit, load and save any elements supported
		by any loaded factories.
		When you add it without a parent (to the root element)
		it will also allow you to edit, load and save settings in
		the current skin.
	*/

	// custom events
	enum EGUIEDIT_CUSTOM_EVENTS
	{
		EGUIEDCE_ATTRIB_EDITOR = MAKE_IRR_ID('g','A','t','t'),
		EGUIEDCE_OPTION_EDITOR = MAKE_IRR_ID('g','O','p','t'),
		EGUIEDCE_ENV_EDITOR = MAKE_IRR_ID('g','E','n','v')
	};

	class CGUIEditWorkspace : public IGUIElement
	{
	public:

		//! constructor
		CGUIEditWorkspace(IGUIEnvironment* environment, s32 id=-1, IGUIElement *parent=0);

		//! destructor
		~CGUIEditWorkspace();

		//! called if an event happened.
		virtual bool OnEvent(const SEvent &event);

		//! Removes a child.
		virtual void removeChild(IGUIElement* child);

		//! draws the element and its children
		virtual void draw();

		//! Updates the absolute position.
		virtual void updateAbsolutePosition();

		//! Sets the menu command id's
		/** The GUI editor defaults to command ID's from 0xED17 to 0xED17+EGUIEDMC_COUNT
		In the rare case that these are already in use and you wish to use menus
		while the editor is present you can set a new offset here.
		*/
		virtual void setMenuCommandIDStart(s32 id);

		//! grid drawing...
		virtual void setDrawGrid(bool drawGrid);
		virtual void setGridSize(const core::dimension2di& gridSize);
		virtual void setUseGrid(bool useGrid);

		//! returns the first editable element under the mouse
		virtual IGUIElement* getEditableElementFromPoint(IGUIElement *start, const core::position2di &point, s32 index=0 );

		//! selecting elements
		virtual void setSelectedElement(IGUIElement *sel);
		virtual void selectNextSibling();
		virtual void selectPreviousSibling();

		//! returns the selected element
		virtual IGUIElement* getSelectedElement();

		//! copies the xml of the selected element and all children to the clipboard
		virtual void CopySelectedElementXML();

		//! copies the xml of the selected element and all children to the clipboard
		virtual void PasteXMLToSelectedElement();

		virtual const c8* getTypeName() const
		{
			return GUIEditElementTypeNames[EGUIEDIT_GUIEDIT];
		}

		virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options=0);
		virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options=0);

	private:

		enum EGUIEDIT_MODE
		{
			// when we are currently selecting an element
			EGUIEDM_SELECT=0,
			// selecting a new parent for the selected element
			EGUIEDM_SELECT_NEW_PARENT,

			// moving the selected element
			EGUIEDM_MOVE,
			// resizing the selected element
			EGUIEDM_RESIZE_TL,
			EGUIEDM_RESIZE_T,
			EGUIEDM_RESIZE_TR,
			EGUIEDM_RESIZE_R,
			EGUIEDM_RESIZE_BR,
			EGUIEDM_RESIZE_B,
			EGUIEDM_RESIZE_BL,
			EGUIEDM_RESIZE_L
		};

		enum EGUIEDIT_MENUCOMMANDS
		{
			//! file commands
			EGUIEDMC_FILE_NEW,
			EGUIEDMC_FILE_LOAD,
			EGUIEDMC_FILE_SAVE,
			//! edit menu
			EGUIEDMC_CUT_ELEMENT,
			EGUIEDMC_COPY_ELEMENT,
			EGUIEDMC_PASTE_ELEMENT,
			EGUIEDMC_DELETE_ELEMENT,
			EGUIEDMC_SET_PARENT,
			EGUIEDMC_BRING_TO_FRONT,
			EGUIEDMC_SAVE_ELEMENT,
			//! grid
			EGUIEDMC_TOGGLE_EDITOR,

			EGUIEDMC_INSERT_XML,

			//! number of menu options
			EGUIEDMC_COUNT
		};

		EGUIEDIT_MODE getModeFromPos(core::position2di p);

		EGUIEDIT_MODE		CurrentMode;
		EGUIEDIT_MODE		MouseOverMode;
		core::position2di	DragStart;
		core::position2di	StartMovePos;
		core::rect<s32>		SelectedArea;

		core::dimension2di	GridSize;
		s32			MenuCommandStart;
		bool			DrawGrid, UseGrid;

		IGUIElement		*MouseOverElement,
					*SelectedElement;
		CGUIEditWindow		*EditorWindow;

		core::rect<s32> TLRect;
		core::rect<s32> TRRect;
		core::rect<s32> TopRect;
		core::rect<s32> BLRect;
		core::rect<s32> LRect;
		core::rect<s32> RRect;
		core::rect<s32> BRRect;
		core::rect<s32> BRect;
	};


} // end namespace gui
} // end namespace irr

#endif

