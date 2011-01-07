#ifndef __C_GUI_TEXTURE_ATTRIBUTE_H_INCLUDED__
#define __C_GUI_TEXTURE_ATTRIBUTE_H_INCLUDED__

#include "CGUIAttribute.h"
#include "IGUIEditBox.h"
#include "IGUIImage.h"
#include "IGUIButton.h"
#include "EGUIEditTypes.h"

namespace irr
{
namespace gui
{

	class CGUITextureAttribute : public CGUIAttribute
	{
	public:
		//
		CGUITextureAttribute(IGUIEnvironment* environment, IGUIElement *parent, s32 myParentID) :
			CGUIAttribute(environment, parent, myParentID),
			AttribEditBox(0), AttribImage(0), AttribButton(0)
		{
			IGUISkin* skin = Environment->getSkin();

			core::rect<s32> r = getAbsolutePosition();
			s32 topy = skin->getFont()->getDimension(L"A").Height + 10;
			s32 h = skin->getFont()->getDimension(L"A").Height + 5;

			AttribImage = environment->addImage(0, core::position2di(0, topy), false, this);
			AttribImage->setRelativePosition( core::rect<s32>(0,topy, r.getWidth() - 5, 100+topy));
			AttribImage->grab();
			AttribImage->setSubElement(true);
			AttribImage->setScaleImage(true);
			AttribImage->setAlignment(EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);

			topy += 105;

			core::rect<s32> r2(0, topy, r.getWidth() - 15 - skin->getSize(EGDS_CHECK_BOX_WIDTH), topy + h);
			core::rect<s32> br(r.getWidth() - 10 - skin->getSize(EGDS_CHECK_BOX_WIDTH), topy, r.getWidth(), topy + h);

			AttribEditBox = environment->addEditBox(0, r2, true, this, -1);
			AttribEditBox->grab();
			AttribEditBox->setSubElement(true);
			AttribEditBox->setAlignment(EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);

			AttribButton = environment->addButton(br, this, -1, L"...");
			AttribButton->grab();
			AttribButton->setSubElement(true);
			AttribButton->setAlignment(EGUIA_LOWERRIGHT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);
			//AttribButton->setSpriteBank(skin->getSpriteBank());
			//AttribButton->setSprite(EGBS_BUTTON_UP, skin->getIcon(EGDI_FILE), skin->getColor(EGDC_WINDOW_SYMBOL));
			//AttribButton->setSprite(EGBS_BUTTON_DOWN, skin->getIcon(EGDI_FILE), skin->getColor(EGDC_WINDOW_SYMBOL), true);
		}

		virtual ~CGUITextureAttribute()
		{
			if (AttribEditBox)
				AttribEditBox->drop();
			if (AttribImage)
				AttribImage->drop();
			if (AttribButton)
				AttribButton->drop();
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
					case EGET_BUTTON_CLICKED:
						// button click: open file dialog
						if (e.GUIEvent.Caller == AttribButton)
						{
							//Environment->addGUIElement("textureBrowser", Environment->getRootGUIElement());
							return true;
						}
						break;
					case EGET_FILE_SELECTED:
						// file selected: change editbox value and set event

						return true;
					case EGET_FILE_CHOOSE_DIALOG_CANCELLED:

						return true;
					default:
						break;
					}
					break;
				case EET_KEY_INPUT_EVENT:
					return true;
				default:
					break;
				}
			}
			return CGUIAttribute::OnEvent(e);
		}

		virtual void setAttrib(io::IAttributes *attribs, u32 attribIndex)
		{
			AttribEditBox->setText(attribs->getAttributeAsStringW(attribIndex).c_str());
			AttribImage->setImage(attribs->getAttributeAsTexture(Index));

			CGUIAttribute::setAttrib(attribs, attribIndex);
		}

		//! save the attribute and possibly post the event to its parent
		virtual bool updateAttrib(bool sendEvent=true)
		{
			if (!Attribs)
				return true;

			Attribs->setAttribute(Index, AttribEditBox->getText());
			core::stringw tmp = Attribs->getAttributeAsStringW(Index);
			AttribEditBox->setText(Attribs->getAttributeAsStringW(Index).c_str());
			AttribImage->setImage(Attribs->getAttributeAsTexture(Index));

			return CGUIAttribute::updateAttrib(sendEvent);
		}

		//! Returns the type name of the gui element.
		virtual const c8* getTypeName() const
		{
			return GUIEditElementTypeNames[EGUIEDIT_TEXTUREATTRIBUTE];
		}

	private:
		IGUIEditBox*		AttribEditBox;
		IGUIImage*			AttribImage;
		IGUIButton*			AttribButton;
	};

} // namespace gui
} // namespace irr

#endif
