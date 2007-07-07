
#include "CGUIAttributeEditor.h"
#include "IGUIEnvironment.h"
#include "IFileSystem.h"
#include "IVideoDriver.h"
#include "IAttributes.h"
#include "IGUIFont.h"
#include "IGUIScrollBar.h"
#include "CGUIEditWorkspace.h"
#include "CGUIAttribute.h"
#include "CGUIStringAttribute.h"

namespace irr
{
namespace gui
{

using namespace core;
using namespace io;

CGUIAttributeEditor::CGUIAttributeEditor(IGUIEnvironment* environment, s32 id, IGUIElement *parent) :
	CGUIPanel(environment, parent, id, rect<s32>(0, 0, 100, 100)),
		Attribs(0)
{
	#ifdef _DEBUG
	setDebugName("CGUIAttributeEditor");
	#endif

	// create attributes
	Attribs = environment->getFileSystem()->createEmptyAttributes(Environment->getVideoDriver());

	calculateClientArea();
	resizeInnerPane();

	// refresh attrib list
	refreshAttribs();

	IGUIScrollBar* sb = getVScrollBar();
	core::rect<s32> r = sb->getRelativePosition();
	r.LowerRightCorner.Y -= 16;
	sb->setRelativePosition(r);

}

CGUIAttributeEditor::~CGUIAttributeEditor()
{
	u32 i;
	for (i=0; i<AttribList.size(); ++i)
	{
		AttribList[i]->remove();
		AttribList[i]->drop();
	}
	AttribList.clear();

	Attribs->drop();
}


IAttributes* CGUIAttributeEditor::getAttribs()
{
	return Attribs;
}

void CGUIAttributeEditor::refreshAttribs()
{
	// clear the attribute list
	u32 i;
	for (i=0; i<AttribList.size(); ++i)
	{
		AttribList[i]->remove();
		AttribList[i]->drop();
	}
	AttribList.clear();

	position2di top(10, 5);
	rect<s32> r(top.X,
				top.Y,
				getClientArea().getWidth() - 10,
				5 + Environment->getSkin()->getFont()->getDimension(L"A").Height);

	// add attribute elements
	u32 c = Attribs->getAttributeCount();
	for (i=0; i<c; ++i)
	{

		// try to create attribute
		stringc str = Attribs->getAttributeTypeString(i);
		str += "_attribute";
		CGUIAttribute* n = (CGUIAttribute*)Environment->addGUIElement(str.c_str(), this);

		if (n)
		{
			// add custom attribute editor
			AttribList.push_back(n);
			n->setParentID(getID());
			n->grab();
		}
		else
		{
			// create a generic string editor
			AttribList.push_back(new CGUIStringAttribute(Environment, this, getID()));
			// dont grab it because we created it with new
		}

		AttribList[i]->setSubElement(true);
		AttribList[i]->setRelativePosition(r);
		AttribList[i]->setAlignment(EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);
		AttribList[i]->setAttrib(Attribs, i);
		r += position2di(0, AttribList[i]->getRelativePosition().getHeight() + 5);
		
	}
}

void CGUIAttributeEditor::updateAttribs()
{
	for (u32 i=0; i<AttribList.size(); ++i)
		AttribList[i]->updateAttrib(false);
}

} // namespace gui
} // namespace irr
