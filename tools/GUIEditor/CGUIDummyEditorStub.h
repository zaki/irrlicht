/*
	This is a custom editor for stubbing problematic elements out, 
	for example elements which include modal screens
*/

#ifndef __C_GUI_DUMMY_EDITOR_STUB_H_INCLUDED__
#define __C_GUI_DUMMY_EDITOR_STUB_H_INCLUDED__

#include "IGUIElement.h"
#include "IGUIEnvironment.h"
#include "IGUIStaticText.h"

namespace irr
{

namespace gui
{
	class CGUIDummyEditorStub : public IGUIElement
	{
	public:
		//! constructor
		CGUIDummyEditorStub(IGUIEnvironment* environment, IGUIElement *parent, const char *text) :
			IGUIElement(EGUIET_ELEMENT, environment, parent, -1, core::rect<s32>(0, 0, 100, 100) ),
			TextBox(0), TypeName(text)
		{

			#ifdef _DEBUG
			setDebugName("CGUIDummyEditorStub");
			#endif

			core::dimension2du d = Environment->getSkin()->getFont()->getDimension(L"A");
			s32 h = d.Height / 2;
			s32 w = d.Width / 2;

			TextBox = environment->addStaticText(core::stringw(text).c_str(), 
				core::rect<s32>(50-w, 50-h, 50+w, 50+h), 
				false, false, this, -1, false);
			TextBox->grab();
			TextBox->setSubElement(true);
			TextBox->setAlignment(EGUIA_CENTER, EGUIA_CENTER, EGUIA_CENTER, EGUIA_CENTER);
		}

		virtual ~CGUIDummyEditorStub()
		{
			if (TextBox)
				TextBox->drop();
		}
		virtual const c8* getTypeName() const { return TypeName.c_str(); }

	protected:
		IGUIStaticText*		TextBox;
		core::stringc		TypeName;
				
	};

} // namespace gui
} // namespace irr

#endif
