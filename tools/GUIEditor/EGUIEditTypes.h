#ifndef __C_GUIEDIT_TYPES_H_INCLUDED__
#define __C_GUIEDIT_TYPES_H_INCLUDED__

#include "irrTypes.h"

namespace irr {
namespace gui {

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

} // gui
} // irr

#endif

