// Copyright (C) 2002-2012 Nikolaus Gebhardt / Gaz Davidson
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

// Thanks to Midnight for all his testing, bug fixes and patches :)

#include "CGUIEditWorkspace.h"
#include "IGUIEnvironment.h"
#include "IVideoDriver.h"
#include "IOSOperator.h"
#include "IReadFile.h"
#include "IFileSystem.h"
#include "IXMLWriter.h"
#include "IGUISkin.h"
#include "IGUIElementFactory.h"
#include "CGUIEditWindow.h"
#include "IGUIContextMenu.h"
#include "IGUIFileOpenDialog.h"
#include "IGUITreeView.h"
#include "CGUIAttribute.h"
#include "CMemoryReadWriteFile.h"

namespace irr
{
namespace gui
{

//! constructor
CGUIEditWorkspace::CGUIEditWorkspace(IGUIEnvironment* environment, s32 id, IGUIElement *parent)
: IGUIElement(EGUIET_ELEMENT, environment, parent ? parent : environment->getRootGUIElement(), id, environment->getRootGUIElement()->getAbsolutePosition()),
	CurrentMode(EGUIEDM_SELECT), MouseOverMode(EGUIEDM_SELECT),
	GridSize(10,10), MenuCommandStart(0x3D17), DrawGrid(false), UseGrid(true),
	MouseOverElement(0), SelectedElement(0), EditorWindow(0)
{
	#ifdef _DEBUG
	setDebugName("CGUIEditWorkspace");
	#endif

	// this element is never saved.
	setSubElement(true);

	// it resizes to fit a resizing window
	setAlignment(EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT);

	EditorWindow = (CGUIEditWindow*) Environment->addGUIElement("GUIEditWindow", this);
	if (EditorWindow)
	{
		EditorWindow->grab();
		EditorWindow->setSubElement(true);

		Environment->setFocus(EditorWindow);
		serializeAttributes(EditorWindow->getOptionEditor()->getAttribs());
		EditorWindow->getOptionEditor()->refreshAttribs();

		if (EditorWindow->getEnvironmentEditor())
		{
			Environment->serializeAttributes(EditorWindow->getEnvironmentEditor()->getAttribs());
			EditorWindow->getEnvironmentEditor()->refreshAttribs();
		}
	}
}


//! destructor
CGUIEditWorkspace::~CGUIEditWorkspace()
{
	if (EditorWindow)
		EditorWindow->drop();
}


void CGUIEditWorkspace::setMenuCommandIDStart(s32 id)
{
	MenuCommandStart = id;
}

CGUIEditWorkspace::EGUIEDIT_MODE CGUIEditWorkspace::getModeFromPos(core::position2di p)
{
	if (SelectedElement)
	{
		core::rect<s32>		r = SelectedElement->getAbsolutePosition();

		if		(TLRect.isPointInside(p))
			return EGUIEDM_RESIZE_TL;

		else if (TRRect.isPointInside(p))
			return EGUIEDM_RESIZE_TR;

		else if (BLRect.isPointInside(p) )
			return EGUIEDM_RESIZE_BL;

		else if (BRRect.isPointInside(p))
			return EGUIEDM_RESIZE_BR;

		else if (TopRect.isPointInside(p))
			return EGUIEDM_RESIZE_T;

		else if (BRect.isPointInside(p))
			return EGUIEDM_RESIZE_B;

		else if (LRect.isPointInside(p))
			return EGUIEDM_RESIZE_L;

		else if (RRect.isPointInside(p))
			return EGUIEDM_RESIZE_R;

		else if (getEditableElementFromPoint(SelectedElement, p) == SelectedElement)
			return EGUIEDM_MOVE;

		else
			return EGUIEDM_SELECT;
	}

	return EGUIEDM_SELECT;

}

IGUIElement* CGUIEditWorkspace::getEditableElementFromPoint(IGUIElement *start, const core::position2di &point, s32 index )
{
	IGUIElement* target = 0;

	// we have to search from back to front.

	core::list<IGUIElement*>::ConstIterator it = start->getChildren().getLast();
	s32 count=0;
	while(it != start->getChildren().end())
	{
		target = getEditableElementFromPoint((*it),point);
		if (target)
		{
			if (!target->isSubElement() && !isMyChild(target) && target != this)
			{
				if (index == count)
					return target;
				else
					count++;
			}
			else
				target = 0;
		}
		--it;
	}

	if (start->getAbsolutePosition().isPointInside(point))
		target = start;

	return target;
}

void CGUIEditWorkspace::setSelectedElement(IGUIElement *sel)
{
	IGUIElement* focus = Environment->getFocus();
	// we only give focus back to children
	if (!isMyChild(focus))
		focus = 0;

	if (SelectedElement != Parent)
	{
		if (SelectedElement != sel && EditorWindow)
		{
			EditorWindow->setSelectedElement(sel);
			SelectedElement = sel;
		}
	}
	else
		SelectedElement = 0;

	if (focus)
		Environment->setFocus(focus);
	else
		Environment->setFocus(this);
}

IGUIElement* CGUIEditWorkspace::getSelectedElement()
{
	return SelectedElement;
}
void CGUIEditWorkspace::selectNextSibling()
{
	IGUIElement* p=0;

	if (SelectedElement && SelectedElement->getParent())
		p = SelectedElement->getParent();
	else
		p = Parent;

	core::list<IGUIElement*>::ConstIterator it = p->getChildren().begin();
	// find selected element
	if (SelectedElement)
		while (*it != SelectedElement)
			++it;
	if (it !=p->getChildren().end())
		++it;
	// find next non sub-element
	while (it != p->getChildren().end() && (*it)->isSubElement())
		++it;

	if (it != p->getChildren().end())
		setSelectedElement(*it);
}
void CGUIEditWorkspace::selectPreviousSibling()
{
	IGUIElement* p=0;

	if (SelectedElement && SelectedElement->getParent())
		p = SelectedElement->getParent();
	else
		p = Parent;

	core::list<IGUIElement*>::ConstIterator it = p->getChildren().getLast();
	// find selected element
	if (SelectedElement)
		while (*it != SelectedElement)
			--it;
	if (it != p->getChildren().end())
		--it;
	// find next non sub-element
	while (it != p->getChildren().end() && (*it)->isSubElement())
		--it;

	if (it != p->getChildren().end())
		setSelectedElement(*it);
}

//! called if an event happened.
bool CGUIEditWorkspace::OnEvent(const SEvent &e)
{
	IGUIFileOpenDialog* dialog=0;
	switch(e.EventType)
	{
	case ATTRIBEDIT_ATTRIB_CHANGED:
		{
			switch (e.UserEvent.UserData1)
			{
			case EGUIEDCE_ATTRIB_EDITOR:
				{
					// update selected items attributes
					if (SelectedElement)
					{
						SelectedElement->deserializeAttributes(EditorWindow->getAttributeEditor()->getAttribs());
						EditorWindow->updateTree();
					}
					return true;
				}
			case EGUIEDCE_OPTION_EDITOR:
				{
					// update editor options
					deserializeAttributes(EditorWindow->getOptionEditor()->getAttribs());
					return true;
				}
			case EGUIEDCE_ENV_EDITOR:
				{
					// update environment
					Environment->deserializeAttributes(EditorWindow->getEnvironmentEditor()->getAttribs());
					return true;
				}
			}
		}
		break;

	case EET_KEY_INPUT_EVENT:
		if (!e.KeyInput.PressedDown)
		{
			switch (e.KeyInput.Key)
			{
			case KEY_DELETE:
				if (SelectedElement)
				{
					IGUIElement* el = SelectedElement;
					setSelectedElement(0);
					MouseOverElement = 0;
					el->remove();
					EditorWindow->updateTree();
				}
				break;
			case KEY_KEY_X:
				if (e.KeyInput.Control && SelectedElement)
				{
					// cut
					CopySelectedElementXML();
					// delete element
					IGUIElement *el = SelectedElement;
					setSelectedElement(0);
					MouseOverElement = 0;
					el->remove();
				}
				break;
			case KEY_KEY_C:
				// copy
				if (e.KeyInput.Control && SelectedElement)
				{
					CopySelectedElementXML();
				}
				break;
			case KEY_KEY_V:
				// paste
				if (e.KeyInput.Control)
				{
					PasteXMLToSelectedElement();
				}
				break;
			default:
				break;
			}

			return true;
		}
		break;

	case EET_MOUSE_INPUT_EVENT:

		switch(e.MouseInput.Event)
		{
		case EMIE_MOUSE_WHEEL:
			{
				f32 wheel = e.MouseInput.Wheel;

				if (wheel > 0)
					selectPreviousSibling();
				else
					selectNextSibling();
			}
			break;
		case EMIE_LMOUSE_PRESSED_DOWN:
		{
			core::position2di p = core::position2di(e.MouseInput.X,e.MouseInput.Y);

			IGUIElement* newSelection = getElementFromPoint(p);

			if (newSelection != this && isMyChild(newSelection) ) // redirect event
			{
				Environment->setFocus(newSelection);
				return true;
			}

			// hide the gui editor
			if (EditorWindow)
				EditorWindow->setVisible(false);

			if (CurrentMode == EGUIEDM_SELECT)
			{
				if (SelectedElement)
				{
					// start moving or dragging
					CurrentMode = getModeFromPos(p);

					if (CurrentMode == EGUIEDM_MOVE)
						StartMovePos = SelectedElement->getAbsolutePosition().UpperLeftCorner;

					DragStart = p;
					SelectedArea = SelectedElement->getAbsolutePosition();
				}

				if (CurrentMode < EGUIEDM_MOVE)
				{
					// selecting an element...
					MouseOverElement = getEditableElementFromPoint(Parent, p);

					if (MouseOverElement == Parent)
						MouseOverElement = 0;

					setSelectedElement(MouseOverElement);
				}
			}

			break;
		}
		case EMIE_RMOUSE_PRESSED_DOWN:
			if (CurrentMode == EGUIEDM_SELECT_NEW_PARENT || CurrentMode >= EGUIEDM_MOVE)
			{
				// cancel dragging
				CurrentMode = EGUIEDM_SELECT;
			}
			else
			{
				DragStart = core::position2di(e.MouseInput.X,e.MouseInput.Y);
				// root menu
				IGUIContextMenu* mnu = Environment->addContextMenu(
					core::rect<s32>(e.MouseInput.X, e.MouseInput.Y, e.MouseInput.Y+100, e.MouseInput.Y+100),this);
				mnu->addItem(L"File",-1,true,true);
				mnu->addItem(L"Edit",-1,true,true);
				mnu->addItem(L"View",-1,true,true);
				mnu->addItem(SelectedElement ? L"Add child" : L"Add" ,-1,true,true);

				// file menu
				IGUIContextMenu* sub = mnu->getSubMenu(0);
				IGUIContextMenu* sub2 =0;

				sub->addItem(L"New",	MenuCommandStart + EGUIEDMC_FILE_NEW );
				sub->addItem(L"Load...",MenuCommandStart + EGUIEDMC_FILE_LOAD);
				sub->addItem(L"Save...",MenuCommandStart + EGUIEDMC_FILE_SAVE);

				// edit menu
				sub = mnu->getSubMenu(1);
				sub->addItem(L"Cut (ctrl+x)", MenuCommandStart + EGUIEDMC_CUT_ELEMENT,	(SelectedElement != 0));
				sub->addItem(L"Copy (ctrl+c)", MenuCommandStart + EGUIEDMC_COPY_ELEMENT,	(SelectedElement != 0));
				sub->addItem(L"Paste (ctrl+v)", MenuCommandStart + EGUIEDMC_PASTE_ELEMENT,
					(core::stringc(Environment->getOSOperator()->getTextFromClipboard()) != ""));
				sub->addItem(L"Delete (del)", MenuCommandStart + EGUIEDMC_DELETE_ELEMENT, (SelectedElement != 0));
				sub->addSeparator();
				sub->addItem(L"Set parent",		MenuCommandStart + EGUIEDMC_SET_PARENT,		(SelectedElement != 0));
				sub->addItem(L"Bring to front", MenuCommandStart + EGUIEDMC_BRING_TO_FRONT, (SelectedElement != 0));
				sub->addSeparator();
				sub->addItem(L"Save to XML...", MenuCommandStart + EGUIEDMC_SAVE_ELEMENT,	(SelectedElement != 0));

				sub = mnu->getSubMenu(2);
				// view menu
				if (EditorWindow)
					sub->addItem(EditorWindow->isVisible() ? L"Hide window" : L"Show window", MenuCommandStart + EGUIEDMC_TOGGLE_EDITOR);

				sub = mnu->getSubMenu(3);

				s32 i,j,c=0;
				sub->addItem(L"Default factory",-1,true, true);

				// add elements from each factory
				for (i=0; u32(i) < Environment->getRegisteredGUIElementFactoryCount(); ++i)
				{
					sub2 = sub->getSubMenu(i);

					IGUIElementFactory *f = Environment->getGUIElementFactory(i);

					for (j=0; j< f->getCreatableGUIElementTypeCount(); ++j)
					{
						sub2->addItem(core::stringw(f->getCreateableGUIElementTypeName(j)).c_str(), MenuCommandStart + EGUIEDMC_COUNT + c);
						c++;
					}

					if (u32(i+1) < Environment->getRegisteredGUIElementFactoryCount())
					{
						core::stringw strFact;
						strFact = L"Factory ";
						strFact += i+1;
						sub->addItem(strFact.c_str(),-1, true, true);
					}
				}
				sub->addSeparator();
				sub->addItem(L"From XML...", MenuCommandStart + EGUIEDMC_INSERT_XML);

				// set focus to menu
				Environment->setFocus(mnu);

			}
			break;
		case EMIE_LMOUSE_LEFT_UP:

			// make window visible again
			if (EditorWindow)
				EditorWindow->setVisible(true);
			if (CurrentMode == EGUIEDM_SELECT_NEW_PARENT)
			{
				if (SelectedElement)
				{
					MouseOverElement = getEditableElementFromPoint(Parent,
						core::position2di(e.MouseInput.X,e.MouseInput.Y));
					if (MouseOverElement)
					{
						MouseOverElement->addChild(SelectedElement);
						setSelectedElement(0);
						setSelectedElement(SelectedElement);
					}
				}
				CurrentMode = EGUIEDM_SELECT;
			}
			else if (CurrentMode >= EGUIEDM_MOVE)
			{
				IGUIElement *sel = SelectedElement;
				// unselect
				setSelectedElement(0);

				// move
				core::position2d<s32> p(0,0);
				if (sel->getParent())
					p = sel->getParent()->getAbsolutePosition().UpperLeftCorner;

				sel->setRelativePosition(SelectedArea - p);

				// select
				setSelectedElement(sel);

				// reset selection mode...
				CurrentMode = EGUIEDM_SELECT;
			}
			break;
		case EMIE_MOUSE_MOVED:
			// always on top
			Parent->bringToFront(this);

			// if selecting
			if (CurrentMode == EGUIEDM_SELECT || CurrentMode == EGUIEDM_SELECT_NEW_PARENT)
			{

				core::position2di p = core::position2di(e.MouseInput.X,e.MouseInput.Y);

				// highlight the element that the mouse is over
				MouseOverElement = getEditableElementFromPoint(Parent, p);
				if (MouseOverElement == Parent)
				{
					MouseOverElement = 0;
				}

				if (CurrentMode == EGUIEDM_SELECT)
				{
					MouseOverMode = getModeFromPos(p);
					if (MouseOverMode > EGUIEDM_MOVE)
					{
						MouseOverElement = SelectedElement;
					}
				}
			}
			else if (CurrentMode == EGUIEDM_MOVE)
			{
				// get difference
				core::position2di p = core::position2di(e.MouseInput.X,e.MouseInput.Y);
				p -= DragStart;

				// apply to top corner
				p = StartMovePos + p;
				if (UseGrid)
				{
					p.X = (p.X/GridSize.Width)*GridSize.Width;
					p.Y = (p.Y/GridSize.Height)*GridSize.Height;
				}

				SelectedArea += p - SelectedArea.UpperLeftCorner;
			}
			else if (CurrentMode > EGUIEDM_MOVE)
			{
				// get difference from start position
				core::position2di p = core::position2di(e.MouseInput.X,e.MouseInput.Y);
				if (UseGrid)
				{
					p.X = (p.X/GridSize.Width)*GridSize.Width;
					p.Y = (p.Y/GridSize.Height)*GridSize.Height;
				}

				switch(CurrentMode)
				{
					case EGUIEDM_RESIZE_T:
						SelectedArea.UpperLeftCorner.Y = p.Y;
						break;
					case EGUIEDM_RESIZE_B:
						SelectedArea.LowerRightCorner.Y = p.Y;
						break;
					case EGUIEDM_RESIZE_L:
						SelectedArea.UpperLeftCorner.X = p.X;
						break;
					case EGUIEDM_RESIZE_R:
						SelectedArea.LowerRightCorner.X = p.X;
						break;
					case EGUIEDM_RESIZE_TL:
						SelectedArea.UpperLeftCorner = p;
						break;
					case EGUIEDM_RESIZE_TR:
						SelectedArea.UpperLeftCorner.Y = p.Y;
						SelectedArea.LowerRightCorner.X = p.X;
						break;
					case EGUIEDM_RESIZE_BL:
						SelectedArea.UpperLeftCorner.X = p.X;
						SelectedArea.LowerRightCorner.Y = p.Y;
						break;
					case EGUIEDM_RESIZE_BR:
						SelectedArea.LowerRightCorner = p;
						break;
					default:
						break;
				}
			}

			break;
		default:
			break;
		}
		break;

	case EET_GUI_EVENT:
		switch(e.GUIEvent.EventType)
		{
        case EGET_TREEVIEW_NODE_SELECT:
        {
            IGUITreeViewNode* eventnode = ((IGUITreeView*)e.GUIEvent.Caller)->getLastEventNode();
            if(!eventnode->isRoot())
                setSelectedElement((IGUIElement*)(eventnode->getData()));
            break;
        }
		// load a gui file
		case EGET_FILE_SELECTED:
			dialog = (IGUIFileOpenDialog*)e.GUIEvent.Caller;
			Environment->loadGUI(core::stringc(dialog->getFileName()).c_str());
			break;

		case EGET_MENU_ITEM_SELECTED:
		{
			IGUIContextMenu *menu = (IGUIContextMenu*)e.GUIEvent.Caller;
			s32 cmdID = menu->getItemCommandId(menu->getSelectedItem()) - MenuCommandStart;

			IGUIElement* el;

			switch(cmdID)
			{

				//! file commands
				case EGUIEDMC_FILE_NEW:
					// clear all elements belonging to our parent
					setSelectedElement(0);
					MouseOverElement = 0;
					el = Parent;
					grab();
					// remove all children
					while(Children.end() != el->getChildren().begin())
						el->removeChild(*(el->getChildren().begin()));
					// attach to parent again
					el->addChild(this);
					drop();

					break;
				case EGUIEDMC_FILE_LOAD:
					Environment->addFileOpenDialog(L"Please select a GUI file to open", false, this);
					break;
				case EGUIEDMC_FILE_SAVE:
					Environment->saveGUI("guiTest.xml");
					break;

				//! edit menu
				case EGUIEDMC_CUT_ELEMENT:
				{
					CopySelectedElementXML();
					// delete element
					el = SelectedElement;
					setSelectedElement(0);
					MouseOverElement = 0;
					el->remove();
					break;
				}
				case EGUIEDMC_COPY_ELEMENT:
					CopySelectedElementXML();
					break;
				case EGUIEDMC_PASTE_ELEMENT:
					PasteXMLToSelectedElement();
					break;
				case EGUIEDMC_DELETE_ELEMENT:
					el = SelectedElement;
					setSelectedElement(0);
					MouseOverElement = 0;
					el->remove();
					break;
				case EGUIEDMC_SET_PARENT:
					CurrentMode = EGUIEDM_SELECT_NEW_PARENT;
					break;
				case EGUIEDMC_BRING_TO_FRONT:
					if (SelectedElement->getParent())
						SelectedElement->getParent()->bringToFront(SelectedElement);
					break;

				case EGUIEDMC_SAVE_ELEMENT:
                    //TODO: add 'save' dialog.
					Environment->saveGUI("guiTest.xml", SelectedElement ? SelectedElement : Environment->getRootGUIElement() );
					break;

				//! toggle edit window
				case EGUIEDMC_TOGGLE_EDITOR:
					break;

				case EGUIEDMC_INSERT_XML:
					Environment->loadGUI("guiTest.xml", SelectedElement ? SelectedElement : Environment->getRootGUIElement() );
					break;

				default:
					// create element from factory?
					if (cmdID >= EGUIEDMC_COUNT)
					{

						s32 num = cmdID - EGUIEDMC_COUNT; // get index
						// loop through all factories
						s32 i, c=Environment->getRegisteredGUIElementFactoryCount();
						for (i=0; i<c && num > Environment->getGUIElementFactory(i)->getCreatableGUIElementTypeCount(); ++i)
						{
							num -= Environment->getGUIElementFactory(i)->getCreatableGUIElementTypeCount();
						}
						if (num < Environment->getGUIElementFactory(i)->getCreatableGUIElementTypeCount() )
						{
							core::stringc name = Environment->getGUIElementFactory(i)->getCreateableGUIElementTypeName(num);
							IGUIElement *parentElement = SelectedElement ? SelectedElement : Environment->getRootGUIElement();
							// add it
							IGUIElement *newElement = Environment->getGUIElementFactory(i)->addGUIElement(name.c_str(),parentElement);
							if (newElement)
							{
								core::position2di p = DragStart - parentElement->getAbsolutePosition().UpperLeftCorner;
								newElement->setRelativePosition(core::rect<s32>(p,p+core::position2di(100,100)));
								//Environment->removeFocus(newElement);
							}
						}
					}
					break;
				}
				EditorWindow->updateTree();
			}
			return true;
		default:
			break;
		}
		break;

	default:
		break;
	}

	// even if we didn't absorb the event,
	// we never pass events back to the GUI we're editing!
	return false;
}


//! draws the element and its children
void CGUIEditWorkspace::draw()
{
	video::IVideoDriver *driver = Environment->getVideoDriver();

	if (DrawGrid)
	{
		// draw the grid

		core::rect<s32> r = getAbsolutePosition();

		s32 cy = r.UpperLeftCorner.Y;
		while (cy < r.LowerRightCorner.Y)
		{
			s32 cx = r.UpperLeftCorner.X;
			while (cx < r.LowerRightCorner.X)
			{
				driver->draw2DRectangle(video::SColor(40,0,0,90),core::rect<s32>(cx+1,cy+1,GridSize.Width+cx,GridSize.Height+cy));
				cx += GridSize.Width;
			}
			cy += GridSize.Height;
		}
	}
	if (MouseOverElement &&
		MouseOverElement != SelectedElement &&
		MouseOverElement != Parent)
	{
		core::rect<s32> r = MouseOverElement->getAbsolutePosition();
		driver->draw2DRectangle(video::SColor(100,0,0,255), r);
	}
	if (SelectedElement && CurrentMode == EGUIEDM_SELECT)
	{
		driver->draw2DRectangle(video::SColor(100,0,255,0),SelectedElement->getAbsolutePosition());
	}
	if (CurrentMode >= EGUIEDM_MOVE)
	{
		driver->draw2DRectangle(video::SColor(100,255,0,0),SelectedArea);
	}

	if ( (SelectedElement && CurrentMode >= EGUIEDM_MOVE) ||
		(SelectedElement && MouseOverElement == SelectedElement && MouseOverMode >= EGUIEDM_MOVE) )
	{
		// draw handles for moving
		EGUIEDIT_MODE m = CurrentMode;
		core::rect<s32> r = SelectedArea;
		if (m < EGUIEDM_MOVE)
		{
			m = MouseOverMode;
			r = SelectedElement->getAbsolutePosition();
		}

		core::position2di d = core::position2di(4,4);

		TLRect = core::rect<s32>(r.UpperLeftCorner, r.UpperLeftCorner + d );
		TRRect = core::rect<s32>(r.LowerRightCorner.X-4, r.UpperLeftCorner.Y, r.LowerRightCorner.X, r.UpperLeftCorner.Y+4);
		TopRect = core::rect<s32>(r.getCenter().X-2, r.UpperLeftCorner.Y,r.getCenter().X+2, r.UpperLeftCorner.Y+4 );
		BLRect = core::rect<s32>(r.UpperLeftCorner.X, r.LowerRightCorner.Y-4, r.UpperLeftCorner.X+4, r.LowerRightCorner.Y);
		LRect = core::rect<s32>(r.UpperLeftCorner.X,r.getCenter().Y-2, r.UpperLeftCorner.X+4, r.getCenter().Y+2 );
		RRect = core::rect<s32>(r.LowerRightCorner.X-4,r.getCenter().Y-2, r.LowerRightCorner.X, r.getCenter().Y+2 );
		BRRect = core::rect<s32>(r.LowerRightCorner-d, r.LowerRightCorner);
		BRect = core::rect<s32>(r.getCenter().X-2, r.LowerRightCorner.Y-4,r.getCenter().X+2, r.LowerRightCorner.Y );

		// top left
		if (m == EGUIEDM_RESIZE_T || m == EGUIEDM_RESIZE_L || m == EGUIEDM_RESIZE_TL || m == EGUIEDM_MOVE )
			driver->draw2DRectangle(video::SColor(100,255,255,255), TLRect);

		if (m == EGUIEDM_RESIZE_T || m == EGUIEDM_RESIZE_R || m == EGUIEDM_RESIZE_TR || m == EGUIEDM_MOVE )
			driver->draw2DRectangle(video::SColor(100,255,255,255), TRRect);

		if (m == EGUIEDM_RESIZE_T || m == EGUIEDM_MOVE )
			driver->draw2DRectangle(video::SColor(100,255,255,255), TopRect);

		if (m == EGUIEDM_RESIZE_L || m == EGUIEDM_RESIZE_BL || m == EGUIEDM_RESIZE_B || m == EGUIEDM_MOVE )
			driver->draw2DRectangle(video::SColor(100,255,255,255), BLRect);

		if (m == EGUIEDM_RESIZE_L || m == EGUIEDM_MOVE )
			driver->draw2DRectangle(video::SColor(100,255,255,255), LRect);

		if (m == EGUIEDM_RESIZE_R || m == EGUIEDM_MOVE )
			driver->draw2DRectangle(video::SColor(100,255,255,255), RRect);

		if (m == EGUIEDM_RESIZE_R || m == EGUIEDM_RESIZE_BR || m == EGUIEDM_RESIZE_B || m == EGUIEDM_MOVE )
			driver->draw2DRectangle(video::SColor(100,255,255,255), BRRect );

		if (m == EGUIEDM_RESIZE_B || m == EGUIEDM_MOVE )
			driver->draw2DRectangle(video::SColor(100,255,255,255), BRect);


	}

	IGUIElement::draw();
}


void CGUIEditWorkspace::setDrawGrid(bool drawGrid)
{
	DrawGrid = drawGrid;
}

void CGUIEditWorkspace::setGridSize(const core::dimension2di& gridSize)
{
	GridSize = gridSize;
	if (GridSize.Width < 2)
		GridSize.Width = 2;
	if (GridSize.Height < 2)
		GridSize.Height = 2;
}

void CGUIEditWorkspace::setUseGrid(bool useGrid)
{
	UseGrid = useGrid;
}


//! Removes a child.
void CGUIEditWorkspace::removeChild(IGUIElement* child)
{
	IGUIElement::removeChild(child);

	if (Children.empty())
		remove();
}


void CGUIEditWorkspace::updateAbsolutePosition()
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

void CGUIEditWorkspace::CopySelectedElementXML()
{
	core::stringc XMLText;
	core::stringw wXMLText;
	// create memory write file
	io::CMemoryReadWriteFile* memWrite = new io::CMemoryReadWriteFile("#Clipboard#");
	// save gui to mem file
	io::IXMLWriter* xml = Environment->getFileSystem()->createXMLWriter(memWrite);
	Environment->writeGUIElement(xml, SelectedElement);

	// copy to clipboard- wide chars not supported yet :(
	wXMLText = (wchar_t*)&memWrite->getData()[0];
	u32 i = memWrite->getData().size()/sizeof(wchar_t);
	if (wXMLText.size() > i)
		wXMLText[i] = L'\0';
	XMLText = wXMLText.c_str();
	memWrite->drop();
	xml->drop();
	Environment->getOSOperator()->copyToClipboard(XMLText.c_str());
}

void CGUIEditWorkspace::PasteXMLToSelectedElement()
{
	// get clipboard data
	const char * p = Environment->getOSOperator()->getTextFromClipboard();

	// convert to stringw
	// TODO: we should have such a function in core::string
	size_t lenOld = strlen(p);
	wchar_t *ws = new wchar_t[lenOld + 1];
	size_t len = mbstowcs(ws,p,lenOld);
	ws[len] = 0;
	irr::core::stringw wXMLText(ws);
	delete[] ws;

	io::CMemoryReadWriteFile* memWrite = new io::CMemoryReadWriteFile("#Clipboard#");

	io::IXMLWriter* xmlw = Environment->getFileSystem()->createXMLWriter(memWrite);
	xmlw->writeXMLHeader(); // it needs one of those
	xmlw->drop();

	// write clipboard data
	memWrite->write((void*)&wXMLText[0], wXMLText.size() * sizeof(wchar_t));

	// rewind file
	memWrite->seek(0, false);

	// read xml
	Environment->loadGUI(memWrite, SelectedElement);

	// reset focus
	Environment->setFocus(this);

	// drop the read file
	memWrite->drop();
}

void CGUIEditWorkspace::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options)
{
	out->addBool("DrawGrid", DrawGrid);
	out->addBool("UseGrid", UseGrid);
	out->addPosition2d("GridSize", core::position2di(GridSize.Width, GridSize.Height));
	out->addInt("MenuCommandStart", MenuCommandStart);
}

void CGUIEditWorkspace::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options)
{
	setDrawGrid(in->getAttributeAsBool("DrawGrid"));
	setUseGrid(in->getAttributeAsBool("UseGrid"));

	core::position2di tmpp = in->getAttributeAsPosition2d("GridSize");
	core::dimension2di tmpd(tmpp.X, tmpp.Y);
	setGridSize(tmpd);
	setMenuCommandIDStart(in->getAttributeAsInt("MenuCommandStart"));
}


} // end namespace gui
} // end namespace irr


