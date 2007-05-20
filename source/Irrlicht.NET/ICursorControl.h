// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#pragma unmanaged
#include "..\\..\\include\\irrlicht.h"
#pragma managed

#include "Position2D.h"

namespace Irrlicht
{
namespace GUI
{

/// <summary> 
/// Interface to manipulate the mouse cursor.
///
/// This class has been ported directly from the native C++ Irrlicht Engine, so it may not 
/// be 100% complete yet and the design may not be 100% .NET like.
/// </summary> 
public __gc class ICursorControl
{
public:

	/// <summary>
	/// You should access ICursorControl
	/// through the Irrlicht::IrrlichtDevice::getCursorControl() method. Simply don't use
	/// this constructor.
	///</summary>
	///<param name="driver">The real, unmanaged C++ ICursorControl</param>
	ICursorControl(irr::gui::ICursorControl* realControl);

	//! destructor
	~ICursorControl();

	/// <summary>
	/// Changes the visible state of the mouse cursor.
	/// </summary>
	/// <param name="visible"> The new visible state. If true, the cursor will be visible,
	/// if false, it will be invisible.</param>
	__property void set_Visible(bool visible);

	/// <summary>
	/// Returns if the cursor is currently visible.
	/// </summary>
	/// <returns> Returns true if the cursor is visible, false if not.</returns>
    __property bool get_Visible();

	/// <summary>
	/// Sets/Gets the new position of the cursor as float values.
	/// The position must be between (0.0f, 0.0f) and (1.0f, 1.0f), where (0.0f, 0.0f) is 
	/// the top left corner and (1.0f, 1.0f) is the bottom right corner of the
	/// render window.
	/// </summary>
	__property void set_Positionf(Core::Position2Df pos);

	/// <summary>
	/// Sets/Gets the new position of the cursor as float values.
	/// The position must be between (0.0f, 0.0f) and (1.0f, 1.0f), where (0.0f, 0.0f) is 
	/// the top left corner and (1.0f, 1.0f) is the bottom right corner of the
	/// render window.
	/// </summary>
	__property Core::Position2Df get_Positionf();

	/// <summary>
	/// Sets/Gets the new position of the cursor. The coordinates are pixel units.
	/// </summary>
	__property void set_Position(Core::Position2D pos);

	/// <summary>
	/// Sets/Gets the new position of the cursor. The coordinates are pixel units.
	/// </summary>
	__property Core::Position2D get_Position();

	/// <summary>
	/// Returns the internal pointer to the native C++ irrlicht texture.
	/// Do not use this, only needed by the internal .NET wrapper.
	///</summary>
	__property irr::gui::ICursorControl* get_NativeCursorControl();

private:

	irr::gui::ICursorControl* CursorControl;
};


}
}