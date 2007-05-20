// Copyright (C) 2002-2007 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_GUI_ENVIRONMENT_H_INCLUDED__
#define __I_GUI_ENVIRONMENT_H_INCLUDED__

#include "IUnknown.h"
#include "IGUIWindow.h"
#include "IGUISkin.h"
#include "rect.h"

namespace irr
{
	class IOSOperator;
	class IEventReceiver;

	namespace io
	{
		class IXMLWriter;
		class IReadFile;
		class IWriteFile;
		class IFileSystem;
	} // end namespace io
	namespace video
	{
		class IVideoDriver;
		class ITexture;
	} // end namespace video

namespace gui
{

class IGUIElement;
class IGUIFont;
class IGUISpriteBank;
class IGUIScrollBar;
class IGUIImage;
class IGUIMeshViewer;
class IGUICheckBox;
class IGUIListBox;
class IGUIFileOpenDialog;
class IGUIColorSelectDialog;
class IGUIInOutFader;
class IGUIStaticText;
class IGUIEditBox;
class IGUITabControl;
class IGUITab;
class IGUIContextMenu;
class IGUIComboBox;
class IGUIToolBar;
class IGUIElementFactory;

//! GUI Environment. Used as factory and manager of all other GUI elements.
class IGUIEnvironment : public virtual IUnknown
{
public:

	//! destructor
	virtual ~IGUIEnvironment() {};

	//! Draws all gui elements.
	virtual void drawAll() = 0;

	//! Sets the focus to an element.
	virtual void setFocus(IGUIElement* element) = 0;

	//! Returns the element with the focus
	virtual IGUIElement* getFocus() = 0;

	//! Removes the focus from an element.
	virtual void removeFocus(IGUIElement* element) = 0;

	//! Returns if the element has focus
	virtual bool hasFocus(IGUIElement* element) = 0;

	//! Returns the current video driver.
	virtual video::IVideoDriver* getVideoDriver() = 0;

	//! Returns the file system.
	virtual io::IFileSystem* getFileSystem() = 0;

	//! returns a pointer to the OS operator
	virtual IOSOperator* getOSOperator() = 0;

	//! removes all elements from the environment.
	virtual void clear() = 0;

	//! Posts an input event to the environment. 
	/** Usually you do not have to
	use this method, it is used by the internal engine. */
	virtual bool postEventFromUser(SEvent event) = 0;

	//! This sets a new event receiver for gui events. 
	/** Usually you do not have to
	use this method, it is used by the internal engine. */
	virtual void setUserEventReceiver(IEventReceiver* evr) = 0;

	//! Returns pointer to the current gui skin.
	virtual IGUISkin* getSkin() = 0;

	//! Sets a new GUI Skin
	/** You can used this to change the appearance of the whole GUI Environment. You
	can set one ot the built-in skins or implement your own class derived from 
	IGUISkin and set this useing this method.
	To set for example the built-in Windows classic skin, use the following code:
	\code
	gui::IGUISkin* newskin = environment->createSkin(gui::EGST_WINDOWS_CLASSIC);
	environment->setSkin(newskin);
	newskin->drop();
	\endcode
	*/
	virtual void setSkin(IGUISkin* skin) = 0;

	//! Creates a new GUI Skin based on a template.
	/** Use setSkin() to set the created skin.
	\return Returns a pointer to the created skin.
	If you no longer need the image, you should call IGUISkin::drop().
	See IUnknown::drop() for more information. */
	virtual IGUISkin* createSkin(EGUI_SKIN_TYPE type) = 0;

	//! Returns pointer to the font with the specified file name. 
	/** Loads the font if it was not loaded before. Returns 0 if the font could not be loaded.
	\return
	returns a pointer to the font.
	This pointer should not be dropped. See IUnknown::drop() for more information. */
	virtual IGUIFont* getFont(const c8* filename) = 0;

	//! Returns the default built-in font.
	virtual IGUIFont* getBuiltInFont() = 0;

	//! Returns pointer to the sprite bank with the specified file name. 
	/** Loads the bank if it was not loaded before. Returns 0 if it could not be loaded.
	\return
	returns a pointer to the sprite bank.
	This pointer should not be dropped. See IUnknown::drop() for more information. */
	virtual IGUISpriteBank* getSpriteBank(const c8* filename) = 0;

	//! adds an empty sprite bank to the manager
	virtual IGUISpriteBank* addEmptySpriteBank(const c8 *name) = 0;

	//! Returns the root gui element. 
	/** This is the first gui element, parent of all other
	gui elements. You'll never need to use this method, unless you are not creating 
	your own gui elements, trying to add them to the gui elements without a parent.
	The returned pointer should not be dropped. See IUnknown::drop() for more information. */
	virtual IGUIElement* getRootGUIElement() = 0;

	//! Adds an button element. 
	/** \return
	 Returns a pointer to the created button. Returns 0 if an error occured.
	 This pointer should not be dropped. See IUnknown::drop() for more information. */
	virtual IGUIButton* addButton(const core::rect<s32>& rectangle,
		IGUIElement* parent=0, s32 id=-1, const wchar_t* text=0, const wchar_t* tooltiptext = 0) = 0;

	//! Adds an empty window element. 
	/** \param modal: Defines if the dialog is modal. This means, that all other
	gui elements which were created before the message box cannot be used
	until this messagebox is removed.
	\return
	Returns a pointer to the created window. Returns 0 if an error occured.
	This pointer should not be dropped. See IUnknown::drop() for more information. */
	virtual IGUIWindow* addWindow(const core::rect<s32>& rectangle, bool modal = false, 
		const wchar_t* text=0, IGUIElement* parent=0, s32 id=-1) = 0;

	//! Adds a modal screen. This control stops its parent's members from being 
	//! able to recieve input until its last child is removed, it then deletes its self.
	/** \return
	Returns a pointer to the created window. Returns 0 if an error occured.
	This pointer should not be dropped. See IUnknown::drop() for more information. */
	virtual IGUIElement* addModalScreen(IGUIElement* parent) = 0;

	//! Adds a message box.
	/** \param caption: Text to be displayed the title of the message box.
	\param text: Text to be displayed in the body of the message box.
	 \param modal: Defines if the dialog is modal. This means, that all other
	 gui elements which were created before the message box cannot be used
	 until this messagebox is removed.
	 \param flags: Flags specifying the layout of the message box. For example
	 to create a message box with an OK and a CANCEL button on it, set this
	 to (EMBF_OK | EMBF_CANCEL).
	 \param parent: Parent gui element of the message box.
	 \param id: Id with which the gui element can be identified.
	 \return
	 Returns a pointer to the created message box. Returns 0 if an error occured.
	 This pointer should not be dropped. See IUnknown::drop() for more information. */
	virtual IGUIWindow* addMessageBox(const wchar_t* caption, const wchar_t* text=0,
		bool modal = true, s32 flags = EMBF_OK, IGUIElement* parent=0, s32 id=-1) = 0;

	//! Adds a scrollbar. 
	/** \return
	Returns a pointer to the created scrollbar. Returns 0 if an error occured.
	This pointer should not be dropped. See IUnknown::drop() for more information. */
	virtual IGUIScrollBar* addScrollBar(bool horizontal, const core::rect<s32>& rectangle,
		IGUIElement* parent=0, s32 id=-1) = 0;

	//! Adds an image element. 
	/** \param image: Image to be displayed.
	 \param pos: Position of the image. The width and height of the image is taken
	 from the image.
	 \param useAlphaChannel: Sets if the image should use the alpha channel of the texture
	 to draw itself.
	 \return
	 Returns a pointer to the created image element. Returns 0 if an error occured.
	 This pointer should not be dropped. See IUnknown::drop() for more information. */
	virtual IGUIImage* addImage(video::ITexture* image, core::position2d<s32> pos,
		bool useAlphaChannel=true, IGUIElement* parent=0, s32 id=-1, const wchar_t* text=0) = 0;

	//! Adds an image element. 
	/** Use IGUIImage::setImage later to set the image to be displayed. 
	\return
	 Returns a pointer to the created image element. Returns 0 if an error occured.
	 This pointer should not be dropped. See IUnknown::drop() for more information. */
	virtual IGUIImage* addImage(const core::rect<s32>& rectangle,
		IGUIElement* parent=0, s32 id=-1, const wchar_t* text=0) = 0;

	//! Adds a checkbox element.
	/** \return
	 Returns a pointer to the created check box. Returns 0 if an error occured.
	 This pointer should not be dropped. See IUnknown::drop() for more information. */
	virtual IGUICheckBox* addCheckBox(bool checked, const core::rect<s32>& rectangle,
		IGUIElement* parent=0, s32 id=-1, const wchar_t* text=0) = 0;

	//! Adds a list box element.
	/** \return
	 Returns a pointer to the created list box. Returns 0 if an error occured.
	 This pointer should not be dropped. See IUnknown::drop() for more information. */
	virtual IGUIListBox* addListBox(const core::rect<s32>& rectangle, 
		IGUIElement* parent=0, s32 id=-1, bool drawBackground=false) = 0;

	//! Adds an mesh viewer. Not 100% implemented yet.
	/** \return
	 Returns a pointer to the created mesh viewer. Returns 0 if an error occured.
	 This pointer should not be dropped. See IUnknown::drop() for more information. */
	virtual IGUIMeshViewer* addMeshViewer(const core::rect<s32>& rectangle, IGUIElement* parent=0, s32 id=-1, const wchar_t* text=0) = 0;

	//! Adds a file open dialog.
	/** \param modal: Defines if the dialog is modal. This means, that all other
	 gui elements which were created before the message box cannot be used
	 until this messagebox is removed.
	 \return
	 Returns a pointer to the created file open dialog. Returns 0 if an error occured.
	 This pointer should not be dropped. See IUnknown::drop() for more information. */
	virtual IGUIFileOpenDialog* addFileOpenDialog(const wchar_t* title = 0, 
		bool modal=true, IGUIElement* parent=0, s32 id=-1) = 0;

	//! Adds a color select dialog.
	/** \param modal: Defines if the dialog is modal. This means, that all other
	 gui elements which were created before the message box cannot be used
	 until this messagebox is removed.
	 \return
	 Returns a pointer to the created file open dialog. Returns 0 if an error occured.
	 This pointer should not be dropped. See IUnknown::drop() for more information. */
	virtual IGUIColorSelectDialog* addColorSelectDialog(const wchar_t* title = 0, 
		bool modal=true, IGUIElement* parent=0, s32 id=-1) = 0;

	//! Adds a static text. 
	/** The returned pointer must not be dropped.
	 \param text is the text to be displayed. Can be altered after creation with SetText().
	 \param rectangle is the position of the static text.
	 \param border has to be set to true if the static text should have a 3d border.
	 \param wordWrap specifies, if the text should be wrapped into multiple lines.
	 \param parent is the parent item of the element. E.g. a window. Set it to 0 to place the fader directly in the environment.
	 \param id is a s32 to identify the static text element.
	 \param fillBackground specifies if the background will be filled. Default: false.
	 \return
	 Returns a pointer to the created static text. Returns 0 if an error occured.
	 This pointer should not be dropped. See IUnknown::drop() for more information. */
	virtual IGUIStaticText* addStaticText(const wchar_t* text, const core::rect<s32>& rectangle, 
		bool border=false, bool wordWrap=true, IGUIElement* parent=0, s32 id=-1, 
		bool fillBackground = false) = 0;

	//! Adds an edit box.  
	/**  Supports unicode input from every keyboard around the world,
	 scrolling, copying and pasting (exchanging data with the clipboard directly), maximum 
	 character amount, marking and all shortcuts like ctrl+X, ctrl+V, ctrg+C, 
	 shift+Left, shift+Right, Home, End, and so on. 
	 \param text is the text to be displayed. Can be altered after creation with SetText().
	 \param rectangle is the position of the edit box.
	 \param border has to be set to true if the edit box should have a 3d border.
	 \param parent is the parent item of the element. E.g. a window. Set it to 0 to place the edit box directly in the environment.
	 \param id is a s32 to identify the edit box.
	 \return
	 Returns a pointer to the created static text. Returns 0 if an error occured.
	 This pointer should not be dropped. See IUnknown::drop() for more information. */
	virtual IGUIEditBox* addEditBox(const wchar_t* text, const core::rect<s32>& rectangle, 
		bool border=true, IGUIElement* parent=0, s32 id=-1) = 0;

	//! Adds an element for fading in or out.
	/* \param rectangle: Pointer to rectangle specifing the borders of the element.
	 If the pointer is NULL, the whole screen is used.
	 \param parent: Parent item of the element. E.g. a window. Set it to 0 to place the static text directly in the environment.
	 \param id: A s32 to identify the text.
	 \return
	 Returns a pointer to the created in-out-fader. Returns 0 if an error occured.
	 This pointer should not be dropped. See IUnknown::drop() for more information. */
	virtual IGUIInOutFader* addInOutFader(const core::rect<s32>* rectangle=0, IGUIElement* parent=0, s32 id=-1) = 0;

	//! Adds a tab control to the environment.
	/** \param rectangle is the position of the tab control.
	 \param parent is the parent item of the element. E.g. a window. Set it to 0 to place the tab control directly in the environment.
	 \param fillbackground specifies if the background of the tab control should be drawn to.
	 \param border specifiys if a flat 3d border should be drawn.
	 This is usually not necesarry unless you don't place the control directly into the environment without a window as parent.
	 \param id is a s32 to identify the tab control.
	 \return
	 Returns a pointer to the created tab control element. Returns 0 if an error occured.
	 This pointer should not be dropped. See IUnknown::drop() for more information. */
	virtual IGUITabControl* addTabControl(const core::rect<s32>& rectangle,
		IGUIElement* parent=0, bool fillbackground=false,
		bool border=true, s32 id=-1) = 0;

	//! Adds tab to the environment. 
	/**  You can use this element to group other elements. This is not used for creating tabs on tab controls,
	 please use IGUITabControl::addTab() for this instead.
	 \param rectangle is the position of the tab.
	 \param parent is the parent item of the element. E.g. a window. Set it to 0 to place the tab directly in the environment.
	 \param id is a s32 to identify the tab. */
	virtual IGUITab* addTab(const core::rect<s32>& rectangle,
		IGUIElement* parent=0, s32 id=-1) = 0;

	//! Adds a context menu to the environment.
	/** \param rectangle is the position of the menu. Note that the menu is 
	 resizing itself based on what items you add.
	 \param parent is the parent item of the element. E.g. a window. Set it to 0 to place the menu directly in the environment.
	 \param id is a s32 to identify the menu. */
	virtual IGUIContextMenu* addContextMenu(const core::rect<s32>& rectangle,
		IGUIElement* parent=0, s32 id=-1) = 0;

	//! Adds a menu to the environment.
	/*  This is like the menu you can find on top of most windows in modern graphical user interfaces.
	 \param parent is the parent item of the element. E.g. a window. Set it to 0 to place the menu directly in the environment.
	 \param id is a s32 to identify the menu. */
	virtual IGUIContextMenu* addMenu(IGUIElement* parent=0, s32 id=-1) = 0;

	//! Adds a toolbar to the environment.
	/** It is like a menu is always placed on top
	 in its parent, and contains buttons.
	 \param parent is the parent item of the element. E.g. a window. Set it to 0 to place the tool bar directly in the environment.
	 \param id is a s32 to identify the tool bar. */
	virtual IGUIToolBar* addToolBar(IGUIElement* parent=0, s32 id=-1) = 0;

	//! Adds a combo box to the environment.
	/** \param parent is the parent item of the element. E.g. a window. Set it to 0 to place the combo box directly in the environment.
	 \param id is a s32 to identify the combo box. */
	virtual IGUIComboBox* addComboBox(const core::rect<s32>& rectangle,
		IGUIElement* parent=0, s32 id=-1) = 0;

	//! Returns the default element factory which can create all built in elements
	virtual IGUIElementFactory* getDefaultGUIElementFactory() = 0;

	//! Adds an element factory to the gui environment.
	/** Use this to extend the gui environment with new element types which it should be
	able to create automaticly, for example when loading data from xml files. */
	virtual void registerGUIElementFactory(IGUIElementFactory* factoryToAdd) = 0;

	//! Returns amount of registered scene node factories.
	virtual s32 getRegisteredGUIElementFactoryCount() = 0;

	//! Returns a scene node factory by index
	virtual IGUIElementFactory* getGUIElementFactory(s32 index) = 0;

	//! Adds a GUI Element by its name
	virtual IGUIElement* addGUIElement(const c8* elementName, IGUIElement* parent=0) = 0;

	//! Saves the current gui into a file.
	//! \param filename: Name of the file.
	//! \param start: The GUIElement to start with. Root if 0.
	virtual bool saveGUI(const c8* filename, IGUIElement* start=0) = 0;

	//! Saves the current gui into a file.
	//! \param file: The file to write to.
	//! \param start: The GUIElement to start with. Root if 0.
	virtual bool saveGUI(io::IWriteFile* file, IGUIElement* start=0) = 0;

	//! Loads the gui. Note that the current gui is not cleared before.
	//! \param filename: Name of the file .
	//! \param parent: Parent for the loaded GUI, root if 0.
	virtual bool loadGUI(const c8* filename, IGUIElement* parent=0) = 0;

	//! Loads the gui. Note that the current gui is not cleared before.
	//! \param file: The file to load from.
	//! \param parent: Parent for the loaded GUI, root if 0.
	virtual bool loadGUI(io::IReadFile* file, IGUIElement* parent=0) = 0;	

	//! Writes attributes of the gui environment
	virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options=0)=0;

	//! Reads attributes of the gui environment
	virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options=0)=0;

	//! writes an element
	virtual void writeGUIElement(io::IXMLWriter* writer, IGUIElement* node) =0;

	//! reads an element
	virtual void readGUIElement(io::IXMLReader* reader, IGUIElement* parent) =0;

};


} // end namespace gui
} // end namespace irr

#endif

