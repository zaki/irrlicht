// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#using <mscorlib.dll>
using namespace System;

#pragma unmanaged
#include "..\\..\\include\\irrlicht.h"
#pragma managed

#include "Rect.h"
#include "Color.h"

namespace Irrlicht
{
namespace GUI
{
	/// <summary>
	/// Base class of all GUI elements.
	/// </summary>
	public __gc class IGUIElement
	{
	public:

		/// <summary>
		/// You should access the IGUIElement 
		/// through the methods in IGUIEnvironment. Simply don't use
		/// this constructor.
		///</summary>
		///<param name="font">The real, unmanaged C++ element</param>
		IGUIElement(irr::gui::IGUIElement* element);

		~IGUIElement();

		/// <summary>
		/// Returns the parent of this element.
		/// </summary>
		__property IGUIElement* get_Parent();

		/// <summary>
		/// Sets or gets the relative rectangle of this element.
		/// </summary>
		__property Core::Rect get_RelativePosition();

		/// <summary>
		/// Sets or gets the relative rectangle of this element.
		/// </summary>
		__property void set_RelativePosition(Core::Rect r);

		/// <summary>
		/// Gets the absolute rectangle of this element.
		/// </summary>
		__property Core::Rect get_AbsolutePosition();

		/// <summary>
		/// Updates the absolute position of this element.
		/// </summary>
		void UpdateAbsolutePosition();

		/// <summary>
		/// Returns the child element, which is at the position of the point.
		/// </summary>
		IGUIElement* GetElementFromPoint(Core::Position2D point);

		/// <summary>
		/// Adds a GUI element as new child of this element.
		/// </summary>
		void AddChild(IGUIElement* child);

		/// <summary>
		/// Removes a child element.
		/// </summary>
		void RemoveChild(IGUIElement* child);

		/// <summary>
		/// Removes this element from its parent.
		/// <summary>
		void Remove();

		/// <summary>
		/// Draws this element
		/// </summary>
		void Draw();

		/// <summary>
		/// Moves this element.
		/// </summary>
		void Move(Core::Position2D absoluteMovement);

		/// <summary>
		/// Sets or gets if the element is visible.
		/// </summary>
		__property bool get_Visible();

		/// <summary>
		/// Sets or gets if the element is visible.
		/// </summary>
		__property void set_Visible(bool visible);

		/// <summary>
		/// Sets or gets if the element is enabled.
		/// </summary>
		__property bool get_Enabled();

		/// <summary>
		/// Sets or gets if the element is enabled.
		/// </summary>
		__property void set_Enabled(bool enabled);

		/// <summary>
		/// Sets or gets the text the element is displaying
		/// </summary>
		__property void set_Text(System::String* text); 

		/// <summary>
		/// Sets or gets the text the element is displaying
		/// </summary>
		__property System::String* get_Text(); 

		/// <summary>
		/// Sets or gets the ID of this element
		/// </summary>
		__property int get_ID();

		/// <summary>
		/// Sets or gets the ID of this element
		/// </summary>
		__property void set_ID(int id);

		/// <summary>
		/// Returns the internal pointer to the native C++ irrlicht element.
		/// Do not use this, only needed by the internal .NET wrapper.
		///</summary>
		__property irr::gui::IGUIElement* get_NativeElement();

	protected:

		irr::gui::IGUIElement* Element;
	};

}
}
