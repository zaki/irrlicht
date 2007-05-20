#ifndef __C_GUI_ATTRIBUTE_EDITOR_H_INCLUDED__
#define __C_GUI_ATTRIBUTE_EDITOR_H_INCLUDED__

#include "IGUIElement.h"
#include "CGUIPanel.h"
#include "irrArray.h"
#include "IAttributes.h"

namespace irr
{
namespace gui
{

	class CGUIAttribute;


	class CGUIAttributeEditor : public CGUIPanel
	{
	public:

		//! constructor
		CGUIAttributeEditor(IGUIEnvironment* environment, s32 id, IGUIElement *parent=0);

		//! destructor
		~CGUIAttributeEditor();

		// gets the current attributes list
		virtual io::IAttributes* getAttribs();

		// update the attribute list after making a change
		void refreshAttribs();

		// save the attributes
		void updateAttribs();

		//! this shoudln't be serialized, but this is included as it's an example
		virtual const c8* getTypeName() const { return "attributeEditor"; }

	private:

		core::array<CGUIAttribute*>	AttribList;	// attributes editing controls
		io::IAttributes*			Attribs;	// current attributes
		CGUIPanel*					Panel;

	};

} // end namespace gui
} // end namespace irr

#endif // __C_GUI_ATTRIBUTE_EDITOR_H_INCLUDED__

