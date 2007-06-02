// Copyright (C) 2002-2007 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_EVENT_RECEIVER_H_INCLUDED__
#define __I_EVENT_RECEIVER_H_INCLUDED__

#include "ILogger.h"
#include "position2d.h"
#include "Keycodes.h"

namespace irr
{
	//! Enumeration for all event types there are.
	enum EEVENT_TYPE
	{
		//! An event of the graphical user interface.
		EET_GUI_EVENT = 0,

		//! A mouse input event.
		EET_MOUSE_INPUT_EVENT,

		//! A key input evant.
		EET_KEY_INPUT_EVENT,

		//! A log event
		EET_LOG_TEXT_EVENT,

		//! A user event with user data. This is not used by Irrlicht and can be used
		//! to send user specific data though the system.
		EET_USER_EVENT
	};

	//! Enumeration for all mouse input events
	enum EMOUSE_INPUT_EVENT
	{
		//! Left mouse button was pressed down.
		EMIE_LMOUSE_PRESSED_DOWN = 0,

		//! Right mouse button was pressed down.
		EMIE_RMOUSE_PRESSED_DOWN,

		//! Middle mouse button was pressed down.
		EMIE_MMOUSE_PRESSED_DOWN,

		//! Left mouse button was left up.
		EMIE_LMOUSE_LEFT_UP,

		//! Right mouse button was left up.
		EMIE_RMOUSE_LEFT_UP,

		//! Middle mouse button was left up.
		EMIE_MMOUSE_LEFT_UP,

		//! The mouse cursor changed its position.
		EMIE_MOUSE_MOVED,

		//! The mouse wheel was moved. Use Wheel value in event data to find out 
		//! in what direction and how fast.
		EMIE_MOUSE_WHEEL,

		//! No real event. Just for convenience to get number of events
		EMIE_COUNT
	};

	namespace gui
	{

		class IGUIElement;

		//! Enumeration for all events which are sendable by the gui system
		enum EGUI_EVENT_TYPE
		{
			//! A gui element has lost its focus.
			EGET_ELEMENT_FOCUS_LOST = 0,

			//! A gui element has got the focus.
			EGET_ELEMENT_FOCUSED,

			//! A gui element was hovered.
			EGET_ELEMENT_HOVERED,

			//! A hovered gui element was left
			EGET_ELEMENT_LEFT,

			//! A button was clicked.
			EGET_BUTTON_CLICKED,

			//! A scrollbar has changed its position.
			EGET_SCROLL_BAR_CHANGED,

			//! A checkbox has changed its check state.
			EGET_CHECKBOX_CHANGED,

			//! A new item in a listbox was seleted.
			EGET_LISTBOX_CHANGED, 

			//! An item in the listbox was selected, which was already selected.
			EGET_LISTBOX_SELECTED_AGAIN, 

			//! A file has been selected in the file dialog
			EGET_FILE_SELECTED,

			//! A file open dialog has been closed without choosing a file
			EGET_FILE_CHOOSE_DIALOG_CANCELLED,

			//! 'Yes' was clicked on a messagebox
			EGET_MESSAGEBOX_YES,

			//! 'No' was clicked on a messagebox
			EGET_MESSAGEBOX_NO,

			//! 'OK' was clicked on a messagebox
			EGET_MESSAGEBOX_OK,

			//! 'Cancel' was clicked on a messagebox
			EGET_MESSAGEBOX_CANCEL,

			//! In an editbox was pressed 'ENTER'
			EGET_EDITBOX_ENTER,

			//! The tab was changed in an tab control
			EGET_TAB_CHANGED,

			//! A menu item was selected in a (context) menu
			EGET_MENU_ITEM_SELECTED,

			//! The selection in a combo box has been changed
			EGET_COMBO_BOX_CHANGED
		};
	} // end namespace gui


//! Struct for holding event data. An event can be a gui, mouse or keyboard event.
struct SEvent
{
	EEVENT_TYPE EventType;

	union
	{
		struct 
		{
			//! IGUIElement who called the event
			gui::IGUIElement* Caller;

			//! Type of GUI Event
			gui::EGUI_EVENT_TYPE EventType;

		} GUIEvent;

		struct
		{
			//! X position of mouse cursor
			s32 X;

			//! Y position of mouse cursor
			s32 Y;

			//! mouse wheel delta, usually 1.0 or -1.0.
			/** Only valid if event was EMIE_MOUSE_WHEEL */
			f32 Wheel; 

			//! type of mouse event
			EMOUSE_INPUT_EVENT Event;
		} MouseInput;

		struct
		{
			//! Character corresponding to the key (0, if not a character)
			wchar_t Char; 

			//! Key which has been pressed or released
			EKEY_CODE Key; 

			//! if not pressed, then the key was left up
			bool PressedDown; 

			//! true if shift was also pressed
			bool Shift; 

			//! true if ctrl was also pressed
			bool Control; 
		} KeyInput;

		struct
		{
			//! pointer to text which has been logged
			const c8* Text;

			//! log level in which the text has been logged
			ELOG_LEVEL Level;
		} LogEvent;

		struct
		{
			//! Some user specified data as int
			s32 UserData1; 

			//! Another user specified data as int
			s32 UserData2; 

			//! Some user specified data as float
			f32 UserData3; 
		} UserEvent;

	};

};

//! Interface of an object which can receive events.
class IEventReceiver
{
public:

	virtual ~IEventReceiver() {};

	//! called if an event happened. returns true if event was processed
	virtual bool OnEvent(SEvent event) = 0;
};


} // end namespace irr

#endif

