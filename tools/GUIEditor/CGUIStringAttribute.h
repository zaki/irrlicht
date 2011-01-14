#ifndef __C_GUI_STRING_ATTRIBUTE_H_INCLUDED__
#define __C_GUI_STRING_ATTRIBUTE_H_INCLUDED__

#include "CGUIAttribute.h"
#include "IGUIEditBox.h"
#include "EGUIEditTypes.h"

namespace irr
{
namespace gui
{

	class CGUIStringAttribute : public CGUIAttribute
	{
	public:
		//
		CGUIStringAttribute(IGUIEnvironment* environment, IGUIElement *parent, s32 myParentID) :
			CGUIAttribute(environment, parent, myParentID),
			AttribEditBox(0)
		{
			core::rect<s32> r = getAbsolutePosition();
			core::rect<s32> r2(0, Environment->getSkin()->getFont()->getDimension(L"A").Height + 10,
				r.getWidth() - 5,
				Environment->getSkin()->getFont()->getDimension(L"A").Height*2 + 15 );

			AttribEditBox = environment->addEditBox(0, r2, true, this, -1);
			AttribEditBox->grab();
			AttribEditBox->setSubElement(true);
			AttribEditBox->setAlignment(EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);

		}

		virtual ~CGUIStringAttribute()
		{
			if (AttribEditBox)
				AttribEditBox->drop();
		}

		virtual void setAttrib(io::IAttributes *attribs, u32 attribIndex)
		{
			AttribEditBox->setText(attribs->getAttributeAsStringW(attribIndex).c_str());
			CGUIAttribute::setAttrib(attribs, attribIndex);
		}

		//! save the attribute and possibly post the event to its parent
		virtual bool updateAttrib(bool sendEvent=true)
		{
			if (!Attribs)
				return true;

			Attribs->setAttribute(Index, AttribEditBox->getText());
			AttribEditBox->setText(Attribs->getAttributeAsStringW(Index).c_str());

			return CGUIAttribute::updateAttrib(sendEvent);
		}

		//! Returns the type name of the gui element.
		virtual const c8* getTypeName() const
		{
			return GUIEditElementTypeNames[EGUIEDIT_STRINGATTRIBUTE];
		}

	private:
		IGUIEditBox*		AttribEditBox;
	};

} // namespace gui
} // namespace irr

#endif
