// Copyright (C) 2006 Michael Zeilfelder
// This file uses the licence of the Irrlicht Engine.

#ifndef __I_GUI_SPIN_BOX_H_INCLUDED__
#define __I_GUI_SPIN_BOX_H_INCLUDED__

#include "IGUIElement.h"

namespace irr
{
namespace gui
{
	class IGUIEditBox;

	//! Single line edit box + spin buttons
	class IGUISpinBox : public IGUIElement
	{
	public:

		//! constructor
		IGUISpinBox(IGUIEnvironment* environment, IGUIElement* parent,
					s32 id, core::rect<s32> rectangle)
			: IGUIElement(EGUIET_SPIN_BOX, environment, parent, id, rectangle) {}

		//! destructor
		virtual ~IGUISpinBox() {}

		//! Access the edit box used in the spin control
		/** \param enable: If set to true, the override color, which can be set
		 with IGUIEditBox::setOverrideColor is used, otherwise the
		 EGDC_BUTTON_TEXT color of the skin. */
		virtual IGUIEditBox* getEditBox() const = 0;

		//! set the current value of the spinbox
		/** \param val: value to be set in the spinbox */
		virtual void setValue(f32 val) = 0;

		//! Get the current value of the spinbox
		virtual f32 getValue() const = 0;

		//! set the range of values which can be used in the spinbox
		/** \param min: minimum value
		\param max: maximum value */
		virtual void setRange(f32 min, f32 max) = 0;

		//! get the minimum value which can be used in the spinbox
		virtual f32 getMin() const = 0;

		//! get the maximum value which can be used in the spinbox
		virtual f32 getMax() const = 0;

		//! Step size by which values are changed when pressing the spinbuttons
		/** The step size also determines the number of decimal places to display
		\param step: stepsize used for value changes when pressing spinbuttons */
		virtual void setStepSize(f32 step=1.f) = 0;

		//! Sets the number of decimal places to display.
		/** \param places: The number of decimal places to display, use -1 to reset */
		virtual void setDecimalPlaces(s32 places) = 0;

		//! get the current step size
		virtual f32 getStepSize() const = 0;
	};


} // end namespace gui
} // end namespace irr

#endif // __I_GUI_SPIN_BOX_H_INCLUDED__

