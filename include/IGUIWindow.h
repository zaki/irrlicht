// Copyright (C) 2002-2007 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_GUI_WINDOW_H_INCLUDED__
#define __I_GUI_WINDOW_H_INCLUDED__

#include "IGUIElement.h"

namespace irr
{
namespace gui
{
	class IGUIButton;

	//! enumeration for message box layout flags
	enum EMESSAGE_BOX_FLAG
	{
		//! Flag for the ok button
		EMBF_OK = 0x1,

		//! Flag for the cancel button
		EMBF_CANCEL = 0x2,

		//! Flag for the yes button
		EMBF_YES = 0x4,

		//! Flag for the no button
		EMBF_NO = 0x8,

		//! This value is not used. It only forces this enumeration to compile in 32 bit. 
		EMBF_FORCE_32BIT = 0x7fffffff	
	};

	//! Default moveable window GUI element with border, caption and close icons.
	class IGUIWindow : public IGUIElement
	{
	public:

		//! constructor
		IGUIWindow(IGUIEnvironment* environment, IGUIElement* parent, s32 id, core::rect<s32> rectangle)
			: IGUIElement(EGUIET_WINDOW, environment, parent, id, rectangle) {}

		//! destructor
		virtual ~IGUIWindow() {};

		//! Returns pointer to the close button
		virtual IGUIButton* getCloseButton() = 0;

		//! Returns pointer to the minimize button
		virtual IGUIButton* getMinimizeButton() = 0;

		//! Returns pointer to the maximize button
		virtual IGUIButton* getMaximizeButton() = 0;
	};


} // end namespace gui
} // end namespace irr

#endif

