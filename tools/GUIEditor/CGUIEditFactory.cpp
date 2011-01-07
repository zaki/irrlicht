#include "CGUIEditFactory.h"
#include "IGUIEnvironment.h"
#include "irrString.h"

#include "EGUIEditTypes.h"

#include "CGUIEditWorkspace.h"
#include "CGUIEditWindow.h"
#include "CGUIPanel.h"
#include "CGUITextureCacheBrowser.h"
#include "CGUIAttributeEditor.h"
#include "CGUIStringAttribute.h"
#include "CGUIBoolAttribute.h"
#include "CGUIEnumAttribute.h"
#include "CGUIColorAttribute.h"
#include "CGUITextureAttribute.h"
#include "CGUIDummyEditorStub.h"

namespace irr
{
namespace gui
{

CGUIEditFactory::CGUIEditFactory(IGUIEnvironment* env)
: Environment(env)
{
	#ifdef _DEBUG
	setDebugName("CGUIEditFactory");
	#endif

	// don't grab the gui environment here to prevent cyclic references
}


CGUIEditFactory::~CGUIEditFactory()
{
}


//! adds an element to the environment based on its type name
IGUIElement* CGUIEditFactory::addGUIElement(const c8* typeName, IGUIElement* parent)
{
	/*
		here we create elements, add them to the manager, and then drop them
	*/

	core::stringc elementType(typeName);
	IGUIElement* ret=0;
	if (!parent)
		parent = Environment->getRootGUIElement();

	// editor workspace
	if (elementType == core::stringc(GUIEditElementTypeNames[EGUIEDIT_GUIEDIT]))
		ret = new CGUIEditWorkspace(Environment, -1, 0);
	// editor window
	else if (elementType == core::stringc(GUIEditElementTypeNames[EGUIEDIT_GUIEDITWINDOW]))
		ret = new CGUIEditWindow(Environment, core::rect<s32>(0,0,100,100), 0);
	// Klasker's GUI Panel
	else if (elementType == core::stringc(GUIEditElementTypeNames[EGUIEDIT_GUIPANEL]))
		ret = new CGUIPanel(Environment, 0);
	// texture cache browser
	else if (elementType == core::stringc(GUIEditElementTypeNames[EGUIEDIT_TEXTUREBROWSER]))
		ret = new CGUITextureCacheBrowser(Environment, -1, 0);
	// block of attribute editors
	else if (elementType == core::stringc(GUIEditElementTypeNames[EGUIEDIT_ATTRIBUTEEDITOR]))
		ret = new CGUIAttributeEditor(Environment, -1, 0);
	//! single attribute editors
	else if (elementType == core::stringc(GUIEditElementTypeNames[EGUIEDIT_STRINGATTRIBUTE]))
		ret = new CGUIStringAttribute(Environment, 0, -1);
	else if (elementType == core::stringc(GUIEditElementTypeNames[EGUIEDIT_BOOLATTRIBUTE]))
		ret = new CGUIBoolAttribute(Environment, 0, -1);
	else if (elementType == core::stringc(GUIEditElementTypeNames[EGUIEDIT_ENUMATTRIBUTE]))
		ret = new CGUIEnumAttribute(Environment, 0, -1);
	else if (elementType == core::stringc(GUIEditElementTypeNames[EGUIEDIT_COLORATTRIBUTE]))
		ret = new CGUIColorAttribute(Environment, 0, -1);
	else if (elementType == core::stringc(GUIEditElementTypeNames[EGUIEDIT_COLORFATTRIBUTE]))
		ret = new CGUIColorAttribute(Environment, 0, -1);
	else if (elementType == core::stringc(GUIEditElementTypeNames[EGUIEDIT_TEXTUREATTRIBUTE]))
		ret = new CGUITextureAttribute(Environment, 0, -1);
	// stubs and custom editors
	else if (elementType == core::stringc(GUIEditElementTypeNames[EGUIEDIT_CONTEXTMENUEDITOR]) ||
			 elementType == core::stringc(GUIEditElementTypeNames[EGUIEDIT_MENUEDITOR])        ||
			 elementType == core::stringc(GUIEditElementTypeNames[EGUIEDIT_FILEDIALOGEDITOR])  ||
			 elementType == core::stringc(GUIEditElementTypeNames[EGUIEDIT_COLORDIALOGEDITOR]) ||
			 elementType == core::stringc(GUIEditElementTypeNames[EGUIEDIT_MODALSCREENEDITOR]) )
		ret = new CGUIDummyEditorStub(Environment, 0, typeName);

    // add the element to its parent
    if (ret)
        parent->addChild(ret);

	// the environment now has the reference, so we can drop the element
	if (ret)
		ret->drop();

	return ret;
}


//! returns amount of element types this factory is able to create
s32 CGUIEditFactory::getCreatableGUIElementTypeCount() const
{
	return EGUIEDIT_COUNT;
}


//! returns type name of a createable element type
const c8* CGUIEditFactory::getCreateableGUIElementTypeName(s32 idx) const
{
	if (idx>=0 && idx<EGUIEDIT_COUNT)
		return GUIEditElementTypeNames[idx];

	return 0;
}



} // end namespace gui
} // end namespace irr

