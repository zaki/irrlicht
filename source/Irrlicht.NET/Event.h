// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#pragma unmanaged
#include "..\\..\\include\\irrlicht.h"
#pragma managed

#using <mscorlib.dll>
using namespace System;

#include "IGUIElement.h"
#include "Position2D.h"
#include "KeyCodes.h"


namespace Irrlicht
{

	/// <summary>
	/// An enum for all event types
	/// </summary>
	__value public enum EventType
	{
		/// <summary> An event of the graphical user interface. </summary>
		GUIEvent = 0,

		/// <summary> A mouse input event. </summary>
		MouseInput,

		//! <summary> A key input evant. </summary>
		KeyInput,

		//! <summary> A log event </summary>
		LogText
	};

	namespace GUI
	{
		/// <summary>
		//! Enumeration for all events which are sendable by the gui system
		/// </summary>
		__value public enum GUIEvent
		{
			//! A gui element has lost its focus.
			ELEMENT_FOCUS_LOST = 0,

			//! A gui element has got the focus.
			ELEMENT_FOCUSED,

			//! A gui element was hovered.
			ELEMENT_HOVERED,

			//! A hovered gui element was left
			ELEMENT_LEFT,

			//! A button was clicked.
			BUTTON_CLICKED,

			//! A scrollbar has changed its position.
			SCROLL_BAR_CHANGED,

			//! A checkbox has changed its check state.
			CHECKBOX_CHANGED,

			//! A new item in a listbox was seleted.
			LISTBOX_CHANGED, 

			//! An item in the listbox was selected, which was already selected.
			LISTBOX_SELECTED_AGAIN, 

			//! A file has been selected in the file open dialog
			FILE_SELECTED,

			//! A file open dialog has been closed without choosing a file
			FILE_CHOOSE_DIALOG_CANCELLED,

			//! 'Yes' was clicked on a messagebox
			MESSAGEBOX_YES,

			//! 'No' was clicked on a messagebox
			MESSAGEBOX_NO,

			//! 'OK' was clicked on a messagebox
			MESSAGEBOX_OK,

			//! 'Cancel' was clicked on a messagebox
			MESSAGEBOX_CANCEL,

			//! In an editbox was pressed 'ENTER'
			EDITBOX_ENTER,

			//! The tab was changed in an tab control
			TAB_CHANGED,

			//! A menu item was selected in a (context) menu
			MENU_ITEM_SELECTED,
			
			//! The selection in a combo box has been changed
			COMBO_BOX_CHANGED
		};

	}// end namespace GUI


	//! Enumeration for all mouse input events
	__value public enum MouseInputEvent
	{
		/// <summary> Left mouse button was pressed down. </summary>
		PressedDownLeft = 0,

		/// <summary> Right mouse button was pressed down. </summary>
		PressedDownRight,

		/// <summary> Middle mouse button was pressed down. </summary>
		PressedDownMiddle,

		/// <summary> Left mouse button was left up. </summary>
		LeftUpLeft,

		/// <summary> Right mouse button was left up. </summary>
		LeftUpRight,

		/// <summary> Middle mouse button was left up. </summary>
		LeftUpMiddle,

		/// <summary> The mouse cursor changed its position. </summary>
		MouseMoved,

		/// <summary> The mouse wheel was moved. Use Wheel value in event data to find out 
		/// in what direction and how fast. </summary>
		MouseWheel
	};


	/// <summary>
	/// Struct for holding event data. An event can be a gui, mouse or keyboard event.
	/// Use the Type property to find out what type of event this is.
	/// Please note that the properties of this class are not typesafe, for example if you read 
	/// the MouseInputEvent property although the event type is KeyInput, the result 
	/// will be undefined.
	/// This may look a little bit strange, but this interface was created in
	/// this way to reflect the original C++ Irrlicht interface and to increase speed.
	/// </summary>
	public __value class Event
	{
	public:

		/// <summary>
		/// Constructs a Irrlicht.NET event from a native C++ Irrlicht event.
		/// </summary>
		Event(irr::SEvent e) 
			: NativeEvent(e)
		{
		}

		/// <summary>
		/// Returns type of event. Based of this type, you can call further methods to
		/// find out more about that event. For example if it was a mouse input event
		/// (MouseInput), use the MouseInputType property. 
		/// Please note that this system is not very typesafe, for example if you read 
		/// the MouseInputEvent property although the event type is KeyInput, the result 
		/// will be undefined.
		/// This may look a little bit strange, but this interface was created in
		/// this way to reflect the original C++ Irrlicht interface and to increase speed. 
		/// </summary>
		__property EventType get_Type() 
		{  
			return (EventType)NativeEvent.EventType;
		}

		/// <summary>
		/// Returns type of mouse event, if Type of this event is MouseInput, otherwise undefined.
		/// </summary>
		__property MouseInputEvent get_MouseInputType()
		{
			return (MouseInputEvent)NativeEvent.MouseInput.Event;
		}

		/// <summary>
		/// Returns mouse wheel delta, usually 1.0 or -1.0. Only valid if 
		// Type of this event is MouseInput and MouseInputType is
		/// MouseWheel, otherwise undefined.
		/// </summary>
		__property float get_MouseWheelDelta()
		{
			return NativeEvent.MouseInput.Wheel;
		}

		/// <summary>
		/// Returns position of mouse. Only valid if Type of this event is MouseInput,
		/// otherwise undefined.
		/// </summary>
		__property Core::Position2D get_MousePos()
		{
			return Core::Position2D(NativeEvent.MouseInput.X, NativeEvent.MouseInput.Y);
		}

		/// <summary>
		/// Returns character corresponding to the key (0, if not a character). 
		/// Only valid if Type of this event is KeyInput, otherwise undefined.
		/// </summary>
		__property __wchar_t get_KeyCharacter()
		{
			return NativeEvent.KeyInput.Char;
		}

		/// <summary>
		/// Returns key code of the pressed key.
		/// Only valid if Type of this event is KeyInput, otherwise undefined.
		/// </summary>
		__property KeyCode get_Key()
		{
			return (KeyCode)NativeEvent.KeyInput.Key;
		}

		/// <summary>
		/// Returns true if the key was pressed down, and false if it was left up.
		/// Only valid if Type of this event is KeyInput, otherwise undefined.
		/// </summary>
		__property bool get_KeyPressedDown()
		{
			return (bool)NativeEvent.KeyInput.PressedDown;
		}

		/// <summary>
		/// Returns true if shift was pressed also.
		/// Only valid if Type of this event is KeyInput, otherwise undefined.
		/// </summary>
		__property bool get_KeyShift()
		{
			return (bool)NativeEvent.KeyInput.Shift;
		}

		/// <summary>
		/// Returns true if control was pressed also.
		/// Only valid if Type of this event is KeyInput, otherwise undefined.
		/// </summary>
		__property bool get_KeyControl()
		{
			return (bool)NativeEvent.KeyInput.Control;
		}

		/// <summary>
		/// Returns the text if this was a log event.
		/// Only valid if Type of this event is LogText, otherwise undefined.
		/// </summary>
		__property System::String* get_LogText()
		{
			// for this, we'll add a type check at least.
			if (NativeEvent.EventType != irr::EET_LOG_TEXT_EVENT)
				throw new System::Exception("Irrlicht Event problem: The event is not a log text, but you're trying to get the log text. Don't do that.");

			return new System::String(NativeEvent.LogEvent.Text);
		}

		/// <summary>
		/// Returns the text if this was a log event.
		/// Only valid if Type of this event is GUIEvent, otherwise undefined.
		/// </summary>
		__property GUI::GUIEvent get_GUIEventType()
		{
			return (GUI::GUIEvent)NativeEvent.GUIEvent.EventType;
		}

		/// <summary>
		/// Returns caller of a gui event.
		/// Only valid if Type of this event is GUIEvent, otherwise undefined.
		/// </summary>
		__property GUI::IGUIElement* get_GUIEventCaller();

	protected:

		irr::SEvent NativeEvent;
	};


	/// <summary>
	/// Interface of an object wich can receive events.
	/// </summary>
	public __gc __interface IEventReceiver
	{
		/// <summary>
		/// called if an event happened. Should return true if event was processed
		/// </summary>
		bool OnEvent(Event e);
	};

}