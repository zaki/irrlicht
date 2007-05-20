#ifndef __C_GUI_COLOR_ATTRIBUTE_H_INCLUDED__
#define __C_GUI_COLOR_ATTRIBUTE_H_INCLUDED__

#include "CGUIAttribute.h"
#include "IGUIStaticText.h"
#include "IGUIScrollBar.h"

namespace irr
{
namespace gui
{
	class CGUIColorAttribute : public CGUIAttribute
	{
	public:
		//
		CGUIColorAttribute(IGUIEnvironment* environment, IGUIElement *parent, s32 myParentID) :
		  	CGUIAttribute(environment, parent, myParentID),
			AttribEditBox(0), AttribSliderA(0), AttribSliderR(0), AttribSliderG(0), AttribSliderB(0)
		{

			core::rect<s32> r = getAbsolutePosition();
			core::rect<s32> r2(0, Environment->getSkin()->getFont()->getDimension(L"A").Height + 5, 
				r.getWidth() - 5, 
				Environment->getSkin()->getFont()->getDimension(L"A").Height*2 + 10 );
			core::rect<s32> r3 = r2;

			s32 h=5;
			r2 += core::position2di(0, h*4 + Environment->getSkin()->getSize(EGDS_WINDOW_BUTTON_WIDTH)*4);
			r3.LowerRightCorner.Y = r3.UpperLeftCorner.Y + Environment->getSkin()->getSize(EGDS_WINDOW_BUTTON_WIDTH);

			AttribSliderA = environment->addScrollBar(true, r3, this, -1);
			AttribSliderA->setMax(255);
			AttribSliderA->grab();
			AttribSliderA->setSubElement(true);
			AttribSliderA->setAlignment(EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);
			r3 += core::position2di(0, r3.getHeight()+h);
			AttribSliderR = environment->addScrollBar(true, r3, this, -1);
			AttribSliderR->setMax(255);
			AttribSliderR->grab();
			AttribSliderR->setSubElement(true);
			AttribSliderR->setAlignment(EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);
			r3 += core::position2di(0, r3.getHeight()+h);
			AttribSliderG = environment->addScrollBar(true, r3, this, -1);
			AttribSliderG->setMax(255);
			AttribSliderG->grab();
			AttribSliderG->setSubElement(true);
			AttribSliderG->setAlignment(EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);
			r3 += core::position2di(0, r3.getHeight()+h);
			AttribSliderB = environment->addScrollBar(true, r3, this, -1);
			AttribSliderB->setMax(255);
			AttribSliderB->grab();
			AttribSliderB->setSubElement(true);
			AttribSliderB->setAlignment(EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);

			// add editbox
			AttribEditBox = environment->addEditBox(
					L"",
					r2,
					true, this, -1);
			AttribEditBox->grab();
			AttribEditBox->setSubElement(true);
			AttribEditBox->setAlignment(EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);
		}

		virtual ~CGUIColorAttribute()
		{
			if (AttribSliderA)
				AttribSliderA->drop();
			if (AttribSliderR)
				AttribSliderR->drop();
			if (AttribSliderG)
				AttribSliderG->drop();
			if (AttribSliderB)
				AttribSliderB->drop();
			if (AttribEditBox)
				AttribEditBox->drop();
		}

		virtual void setAttrib(io::IAttributes *attribs, u32 attribIndex)
		{
			video::SColor col = attribs->getAttributeAsColor(attribIndex);

			AttribSliderA->setPos(col.getAlpha());
			AttribSliderR->setPos(col.getRed());
			AttribSliderG->setPos(col.getGreen());
			AttribSliderB->setPos(col.getBlue());
			AttribEditBox->setText( attribs->getAttributeAsStringW(attribIndex).c_str() );

			CGUIAttribute::setAttrib(attribs, attribIndex);
		}

		virtual bool OnEvent(SEvent e)
		{
			switch (e.EventType)
			{

			case EET_GUI_EVENT:
				switch (e.GUIEvent.EventType)
				{
				case EGET_EDITBOX_ENTER:
				case EGET_ELEMENT_FOCUS_LOST:
					if (e.GUIEvent.Caller == AttribEditBox)
					{
						// update scrollbars from textbox
						Attribs->setAttribute(Index, AttribEditBox->getText());
						video::SColor col = Attribs->getAttributeAsColor(Index);
						AttribSliderA->setPos(col.getAlpha());
						AttribSliderR->setPos(col.getRed()); 
						AttribSliderG->setPos(col.getGreen());
						AttribSliderB->setPos(col.getBlue());

						return updateAttrib();
					}
				case EGET_SCROLL_BAR_CHANGED:
					{
						// update editbox from scrollbars
						video::SColor col( AttribSliderA->getPos(), AttribSliderR->getPos(), 
							AttribSliderG->getPos(), AttribSliderB->getPos());

						Attribs->setAttribute(Index, col);
						AttribEditBox->setText( Attribs->getAttributeAsStringW(Index).c_str());
					}
					return updateAttrib();
				}
				break;
			}
			return CGUIAttribute::OnEvent(e);
		}

		// save the attribute and possibly post the event to its parent
		virtual bool updateAttrib(bool sendEvent=true)
		{
			if (!Attribs)
				return true;

			Attribs->setAttribute(Index, AttribEditBox->getText());
			AttribEditBox->setText(Attribs->getAttributeAsStringW(Index).c_str());
			return CGUIAttribute::updateAttrib(sendEvent);
		}

		//! this shoudln't be serialized, but this is included as it's an example
		virtual const c8* getTypeName() const
		{ 
			return "color_attribute"; 
		}

	private:
		IGUIScrollBar*		AttribSliderA;
		IGUIScrollBar*		AttribSliderR;
		IGUIScrollBar*		AttribSliderG;
		IGUIScrollBar*		AttribSliderB;
		IGUIEditBox*		AttribEditBox;
	};

} // namespace gui
} // namespace irr

#endif

