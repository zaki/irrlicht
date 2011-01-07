#ifndef __C_GUI_EDITOR_H_INCLUDED__
#define __C_GUI_EDITOR_H_INCLUDED__


#include "IGUIWindow.h"
#include "CGUIAttributeEditor.h"
//#include "IGUIStaticText.h"
#include "IGUIButton.h"
#include "IGUITreeView.h"
#include "irrArray.h"
#include "IAttributes.h"
#include "EGUIEditTypes.h"

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

		//! get draggable
		virtual bool isDraggable() const;

		//! get draggable
		virtual void setDraggable(bool draggable);

		// not used
		virtual core::rect<s32> getClientRect() const;
		virtual IGUIButton* getCloseButton() const;
		virtual IGUIButton* getMinimizeButton() const;
		virtual IGUIButton* getMaximizeButton() const;
        virtual void setDrawBackground(bool draw)  { }
		virtual bool getDrawBackground() const { return true; }
		virtual void setDrawTitlebar(bool draw) { }
		virtual bool getDrawTitlebar() const { return true; }

        IGUITreeView* getTreeView() const;
		CGUIAttributeEditor* getAttributeEditor() const;
		CGUIAttributeEditor* getOptionEditor() const;
		CGUIAttributeEditor* getEnvironmentEditor() const;

		//! Returns the type name of the gui element.
		virtual const c8* getTypeName() const
		{
			return GUIEditElementTypeNames[EGUIEDIT_GUIEDITWINDOW];
		}

        void updateTree();
	private:

        void addChildrenToTree(IGUIElement* parentElement, IGUITreeViewNode* treenode);
        IGUITreeViewNode* getTreeNode(IGUIElement* element, IGUITreeViewNode* searchnode);
		// for dragging the window
		bool                    Dragging;
		bool                    IsDraggable;
		bool                    Resizing;
		core::position2d<s32>   DragStart;

		IGUIElement*            SelectedElement; // current selected element

		CGUIAttributeEditor*    AttribEditor;	// edits the current attribute
		CGUIAttributeEditor*    OptionEditor;	// edits the options for the window
		CGUIAttributeEditor*    EnvEditor;		// edits attributes for the environment
		IGUITreeView*           TreeView;       // tree view of all elements in scene
		IGUIButton*             ResizeButton;

	};

} // end namespace gui
} // end namespace irr

#endif // __C_GUI_EDITOR_H_INCLUDED__

