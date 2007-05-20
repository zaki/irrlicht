// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "ICursorControl.h"

namespace Irrlicht
{
namespace GUI
{

ICursorControl::ICursorControl(irr::gui::ICursorControl* realControl)
: CursorControl(realControl)
{
	if (!CursorControl)
		throw new System::Exception(new System::String("Null pointer set in ICursorControl."));

	CursorControl->grab();
}

//! destructor
ICursorControl::~ICursorControl()
{
	if (CursorControl)
		CursorControl->drop();
}

void ICursorControl::set_Visible(bool visible)
{
	CursorControl->setVisible(visible);
}

bool ICursorControl::get_Visible()
{
	return CursorControl->isVisible();
}

void ICursorControl::set_Positionf(Core::Position2Df pos)
{
	CursorControl->setPosition(irr::core::position2d<irr::f32>(pos.X, pos.Y));
}

Core::Position2Df ICursorControl::get_Positionf()
{
	irr::core::position2d<irr::f32> p = CursorControl->getRelativePosition();
	return Core::Position2Df(p.X, p.Y);
}

void ICursorControl::set_Position(Core::Position2D pos)
{
	CursorControl->setPosition(irr::core::position2d<irr::s32>(pos.X, pos.Y));
}

Core::Position2D ICursorControl::get_Position()
{
	irr::core::position2d<irr::s32> p = CursorControl->getPosition();
	return Core::Position2D(p.X, p.Y);
}


irr::gui::ICursorControl* ICursorControl::get_NativeCursorControl()
{
	return CursorControl;
}


} // end namespace 
}// end namespace 