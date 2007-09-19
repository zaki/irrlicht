#ifndef __C_GUI_EDITOR_H_INCLUDED__
#define __C_GUI_EDITOR_H_INCLUDED__


#include "IGUIWindow.h"
#include "CGUIAttributeEditor.h"
//#include "IGUIStaticText.h"
#include "IGUIButton.h"
#include "irrArray.h"
#include "IAttributes.h"

namespace irr
{
namespace gui
{
	class CGUIEditWindow : public IGUIWindow
	{
	public:

		//! constructor
		CGUIEditWindow(IGUIEnvironment* environment, core::rect<s32> rectangle, IGUIElement *parent);

		//! destructor
		~CGUIEditWindow();

		//! this part draws the window
		virtual void draw();
		//! handles events
		virtual bool OnEvent(const SEvent &event);

		//! change selection
		virtual void setSelectedElement(IGUIElement *sel);

		// not used
		virtual IGUIButton* getCloseButton() const;
		virtual IGUIButton* getMinimizeButton() const;
		virtual IGUIButton* getMaximizeButton() const;

		CGUIAttributeEditor* getAttributeEditor() const;
		CGUIAttributeEditor* getOptionEditor() const;
		CGUIAttributeEditor* getEnvironmentEditor() const;

		//! this shoudln't be serialized, but this is included as it's an example
		virtual const c8* getTypeName() const { return "GUIEditWindow"; }

	private:


		// for dragging the window
		bool                    Dragging;
		bool                    Resizing;
		core::position2d<s32>   DragStart;

		IGUIElement*            SelectedElement; // current selected element

		CGUIAttributeEditor*    AttribEditor;	// edits the current attribute
		CGUIAttributeEditor*    OptionEditor;	// edits the options for the window
		CGUIAttributeEditor*    EnvEditor;		// edits attributes for the environment
		IGUIButton*             ResizeButton;

	};

} // end namespace gui
} // end namespace irr

#endif // __C_GUI_EDITOR_H_INCLUDED__

