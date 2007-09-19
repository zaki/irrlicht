#include "CGUIEditFactory.h"
#include "IGUIEnvironment.h"
#include "irrString.h"

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

enum EGUIEDIT_ELEMENT_TYPES
{
	// GUI Editor
	EGUIEDIT_GUIEDIT=0,
	EGUIEDIT_GUIEDITWINDOW,
	// Generic
	EGUIEDIT_GUIPANEL,
	EGUIEDIT_TEXTUREBROWSER,
	// Attribute editors
	EGUIEDIT_ATTRIBUTEEDITOR,
	EGUIEDIT_STRINGATTRIBUTE,
	EGUIEDIT_BOOLATTRIBUTE,
	EGUIEDIT_ENUMATTRIBUTE,
	EGUIEDIT_COLORATTRIBUTE,
	EGUIEDIT_COLORFATTRIBUTE,
	EGUIEDIT_TEXTUREATTRIBUTE,
	// Dummy editor stubs
	EGUIEDIT_CONTEXTMENUEDITOR,
	EGUIEDIT_MENUEDITOR,
	EGUIEDIT_FILEDIALOGEDITOR,
	EGUIEDIT_COLORDIALOGEDITOR,
	EGUIEDIT_MODALSCREENEDITOR,
	// Count
	EGUIEDIT_COUNT
};

const c8* const GUIEditElementTypeNames[] =
{
	"GUIEditor",
	"GUIEditWindow",
	"panel",
	"textureCacheBrowser",
	"attributeEditor",
	"string_attribute",
	"bool_attribute",
	"enum_attribute",
	"color_attribute",
	"colorf_attribute",
	"texture_attribute",
	// dummy editors
	"contextMenu_editor",
	"menu_editor",
	"fileOpenDialog_editor",
	"colorSelectDialog_editor",
	"modalScreen_editor",
	0
};



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
	if (parent == 0)
	{
		parent = Environment->getRootGUIElement();
	}

	// editor workspace
	if (elementType == core::stringc(GUIEditElementTypeNames[EGUIEDIT_GUIEDIT]))
		ret = new CGUIEditWorkspace(Environment, -1, parent);
	// editor window
	else if (elementType == core::stringc(GUIEditElementTypeNames[EGUIEDIT_GUIEDITWINDOW]))
		ret = new CGUIEditWindow(Environment, core::rect<s32>(0,0,100,100), parent);
	// Klasker's GUI Panel
	else if (elementType == core::stringc(GUIEditElementTypeNames[EGUIEDIT_GUIPANEL]))
		ret = new CGUIPanel(Environment, parent);
	// texture cache browser
	else if (elementType == core::stringc(GUIEditElementTypeNames[EGUIEDIT_TEXTUREBROWSER]))
		ret = new CGUITextureCacheBrowser(Environment, -1, parent);
	// block of attribute editors
	else if (elementType == core::stringc(GUIEditElementTypeNames[EGUIEDIT_ATTRIBUTEEDITOR]))
		ret = new CGUIAttributeEditor(Environment, -1, parent);
	//! single attribute editors
	else if (elementType == core::stringc(GUIEditElementTypeNames[EGUIEDIT_STRINGATTRIBUTE]))
		ret = new CGUIStringAttribute(Environment, parent, -1);
	else if (elementType == core::stringc(GUIEditElementTypeNames[EGUIEDIT_BOOLATTRIBUTE]))
		ret = new CGUIBoolAttribute(Environment, parent, -1);
	else if (elementType == core::stringc(GUIEditElementTypeNames[EGUIEDIT_ENUMATTRIBUTE]))
		ret = new CGUIEnumAttribute(Environment, parent, -1);
	else if (elementType == core::stringc(GUIEditElementTypeNames[EGUIEDIT_COLORATTRIBUTE]))
		ret = new CGUIColorAttribute(Environment, parent, -1);
	else if (elementType == core::stringc(GUIEditElementTypeNames[EGUIEDIT_COLORFATTRIBUTE]))
		ret = new CGUIColorAttribute(Environment, parent, -1);
	else if (elementType == core::stringc(GUIEditElementTypeNames[EGUIEDIT_TEXTUREATTRIBUTE]))
		ret = new CGUITextureAttribute(Environment, parent, -1);
	// stubs and custom editors
	else if (elementType == core::stringc(GUIEditElementTypeNames[EGUIEDIT_CONTEXTMENUEDITOR]) || 
			 elementType == core::stringc(GUIEditElementTypeNames[EGUIEDIT_MENUEDITOR]) || 
			 elementType == core::stringc(GUIEditElementTypeNames[EGUIEDIT_FILEDIALOGEDITOR]) || 
			 elementType == core::stringc(GUIEditElementTypeNames[EGUIEDIT_COLORDIALOGEDITOR]) || 
			 elementType == core::stringc(GUIEditElementTypeNames[EGUIEDIT_MODALSCREENEDITOR]) )
		ret = new CGUIDummyEditorStub(Environment, parent, typeName);


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

