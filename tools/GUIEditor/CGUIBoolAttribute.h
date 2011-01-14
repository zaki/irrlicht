#ifndef __C_GUI_BOOL_ATTRIBUTE_H_INCLUDED__
#define __C_GUI_BOOL_ATTRIBUTE_H_INCLUDED__

#include "CGUIAttribute.h"
#include "IGUICheckBox.h"
#include "EGUIEditTypes.h"

namespace irr
{
namespace gui
{

	class CGUIBoolAttribute : public CGUIAttribute
	{
	public:
		//
		CGUIBoolAttribute(IGUIEnvironment* environment, IGUIElement *parent, s32 myParentID) :
		  	CGUIAttribute(environment, parent, myParentID), AttribCheckBox(0)
		{

			core::rect<s32> r = getAbsolutePosition();
			core::rect<s32> r2(0, Environment->getSkin()->getFont()->getDimension(L"A").Height + 10,
				r.getWidth() - 5,
				Environment->getSkin()->getFont()->getDimension(L"A").Height*2 + 15 );

			AttribCheckBox = environment->addCheckBox(false, r2, this);
			AttribCheckBox->setAlignment(EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);
			AttribCheckBox->setSubElement(true);
			AttribCheckBox->grab();
		}

		virtual ~CGUIBoolAttribute()
		{
			if (AttribCheckBox)
				AttribCheckBox->drop();
		}

		virtual void setAttrib(io::IAttributes *attribs, u32 attribIndex)
		{
			AttribCheckBox->setChecked(attribs->getAttributeAsBool(attribIndex));
			CGUIAttribute::setAttrib(attribs, attribIndex);
		}

		// save the attribute and possibly post the event to its parent
		virtual bool updateAttrib(bool sendEvent=true)
		{
			if (!Attribs)
				return true;

			Attribs->setAttribute(Index, AttribCheckBox->isChecked());

			return CGUIAttribute::updateAttrib(sendEvent);
		}

		//! Returns the type name of the gui element.
		virtual const c8* getTypeName() const
		{
			return GUIEditElementTypeNames[EGUIEDIT_BOOLATTRIBUTE];
		}

	private:
		IGUICheckBox*		AttribCheckBox;
	};

} // namespace gui
} // namespace irr

#endif
