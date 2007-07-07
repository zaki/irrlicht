// Copyright (C) 2002-2007 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_GUI_MESSAGE_BOX_H_INCLUDED__
#define __C_GUI_MESSAGE_BOX_H_INCLUDED__

#include "CGUIWindow.h"
#include "IGUIStaticText.h"
#include "irrArray.h"

namespace irr
{
namespace gui
{
	class CGUIMessageBox : public CGUIWindow
	{
	public:

		//! constructor
		CGUIMessageBox(IGUIEnvironment* environment, const wchar_t* caption,
			const wchar_t* text, s32 flag,
			IGUIElement* parent, s32 id, core::rect<s32> rectangle);

		//! destructor
		~CGUIMessageBox();

		//! called if an event happened.
		virtual bool OnEvent(SEvent event);

		//! Writes attributes of the element.
		virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options);

		//! Reads attributes of the element
		virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options);

	private:

		void refreshControls();

		IGUIButton* OkButton;
		IGUIButton* CancelButton;
		IGUIButton* YesButton;
		IGUIButton* NoButton;
		IGUIStaticText* StaticText;

		s32 Flags;
		core::stringw MessageText;
		bool Pressed;
	};

} // end namespace gui
} // end namespace irr

#endif

