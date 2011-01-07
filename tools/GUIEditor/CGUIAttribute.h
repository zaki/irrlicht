/*
	This base class is used by the Attribute editor for making your own attribute types.

	The attribute editor will try and create an attribute called "AttribType_attribute",
	and if it fails, it will create a "string_attribute".

*/

#ifndef __C_GUI_ATTRIBUTE_H_INCLUDED__
#define __C_GUI_ATTRIBUTE_H_INCLUDED__

#include "IGUIElement.h"
#include "IGUIEnvironment.h"
#include "IGUIFont.h"
#include "IGUIStaticText.h"
#include "IAttributes.h"
#include "CGUIEditWorkspace.h"

namespace irr
{

namespace gui
{

	const u32 ATTRIBEDIT_ATTRIB_CHANGED=MAKE_IRR_ID('A','T','T','R');

	class CGUIAttribute : public IGUIElement
	{
	public:
		//! constructor
		CGUIAttribute(IGUIEnvironment* environment, IGUIElement *parent, s32 myParentID) :
			IGUIElement(EGUIET_ELEMENT, environment, parent, -1, core::rect<s32>(0, 0, 100, 100) ),
			AttribName(0), Attribs(0), Index(0), MyParentID(myParentID)
		{

			#ifdef _DEBUG
			setDebugName("CGUIAttribute");
			#endif

			AttribName = environment->addStaticText(0,
				core::rect<s32>(0, 0,
					100, Environment->getSkin()->getFont()->getDimension(L"A").Height),
					false, false, this, -1, false);
			AttribName->grab();
			AttribName->setSubElement(true);
			AttribName->setAlignment(EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);
		}

		virtual ~CGUIAttribute()
		{
			if (Attribs)
				Attribs->drop();
			if (AttribName)
				AttribName->drop();
		}

		virtual bool OnEvent(const SEvent &e)
		{
			if (IsEnabled)
			{
				switch (e.EventType)
				{
				case EET_GUI_EVENT:
					switch (e.GUIEvent.EventType)
					{
					case EGET_ELEMENT_FOCUSED:
						if (Parent && isMyChild(e.GUIEvent.Caller))
							Parent->bringToFront(this);
						break;
					case EGET_ELEMENT_HOVERED:
					case EGET_ELEMENT_LEFT:
						return IGUIElement::OnEvent(e);
					case EGET_ELEMENT_FOCUS_LOST:
						updateAttrib();
						return IGUIElement::OnEvent(e);
					default:
						return updateAttrib();
					}
					break;
				case EET_KEY_INPUT_EVENT:
					return true;
				default:
					break;
				}
			}

			return IGUIElement::OnEvent(e);
		}

		//! sets the attribute to use
		virtual void setAttrib(io::IAttributes *attribs, u32 attribIndex)
		{
			if (Attribs)
				Attribs->drop();
			Attribs = attribs;
			if (Attribs)
				Attribs->grab();
			Index = attribIndex;

			core::stringw name(attribs->getAttributeName(attribIndex));
			name += L" (";
			name += attribs->getAttributeTypeString(attribIndex);
			name += L")";
			AttribName->setText(name.c_str());

			core::rect<s32> r = Parent->getAbsolutePosition();
			core::rect<s32> r2(0, 5,
				r.getWidth(), Environment->getSkin()->getFont()->getDimension(L"A").Height + 10 );

			AttribName->setRelativePosition(r2);

			// get minimum height
			s32 y=0;
			core::list<IGUIElement*>::Iterator it = Children.begin();
			for (; it != Children.end(); ++it)
			{
				if (y < (*it)->getRelativePosition().LowerRightCorner.Y)
					y = (*it)->getRelativePosition().LowerRightCorner.Y;
			}
			setMinSize( core::dimension2du(0, y+5));

			updateAttrib(false);
		}

		//! sets the parent ID, for identifying where events came from
		void setParentID(s32 parentID)
		{
			MyParentID = parentID;
		}

		//! save the attribute and possibly post the event to its parent
		virtual bool updateAttrib(bool sendEvent=true)
		{
			if (Attribs && IsEnabled && sendEvent)
			{
				// build event and pass to parent
				SEvent event;
				event.EventType = (EEVENT_TYPE)ATTRIBEDIT_ATTRIB_CHANGED;
				event.UserEvent.UserData1 = MyParentID;
				event.UserEvent.UserData2 = Index;
				return Parent->OnEvent(event);
			}

			return true;
		}

		virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options=0)
		{
			IGUIElement::serializeAttributes(out, options);
		}

		virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options=0)
		{
			IGUIElement::deserializeAttributes(in, options);
			if (AttribName)
				AttribName->setText(Text.c_str());
		}

	protected:
		IGUIStaticText*		AttribName;
		io::IAttributes*	Attribs;
		u32			Index;
		s32			MyParentID;
	};

} // namespace gui
} // namespace irr

#endif
