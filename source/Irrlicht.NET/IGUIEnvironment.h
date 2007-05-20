// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#using <mscorlib.dll>
using namespace System;

#pragma unmanaged
#include "..\\..\\include\\irrlicht.h"
#pragma managed

#include "IGUISkin.h"

namespace Irrlicht
{
public __gc class IrrlichtDevice;

namespace Video
{
	public __gc class ITexture;
}

namespace GUI
{
	public __gc class IGUIFont;
	public __gc class IGUIElement;
	public __gc class IGUISkin;
	public __gc class IGUIListBox;
	public __gc class IGUIStaticText;

	//! enumeration for message box layout flags
	__value public enum MessageBoxFlag
	{
		//! Flag for the ok button
		OK = 0x1,

		//! Flag for the cancel button
		CANCEL = 0x2,

		//! Flag for the yes button
		YES = 0x4,

		//! Flag for the no button
		NO = 0x8,

		//! This value is not used. It only forces this enumeration to compile in 32 bit. 
		FORCE_32BIT = 0x7fffffff	
	};

	public __gc class IGUIEnvironment
	{
	public:

		/// <summary>
		/// You should access the IGUIEnvironment 
		/// through the Irrlicht::IrrlichtDevice.GUIEnvironment property. Simply don't use
		/// this constructor.
		///</summary>
		///<param name="env">The real, unmanaged C++ GUI Environment</param>
		IGUIEnvironment(irr::gui::IGUIEnvironment* env);

		~IGUIEnvironment();

		/// <summary>
		/// Returns pointer to the font with the specified file name. 
		/// Loads the font if it was not loaded before. Returns 0 if the font could not be loaded.
		/// </summary>
		/// <returns> returns a pointer to the font. </returns>
		IGUIFont* GetFont(System::String* filename);

		/// <summary>
		/// Returns the default built-in font.
		/// </summary>
		__property IGUIFont* get_BuiltInFont();

		/// <summary>
		/// Sets the focus to an element.
		/// </summary>
		void SetFocus(IGUIElement* element);

		/// <summary>
		/// Removes the focus from an element.
		/// </summary>
		void RemoveFocus(IGUIElement* element);

		/// <summary>
		/// Returns if the element has focus
		/// </summary>
		bool HasFocus(IGUIElement* element);

		/// <summary>
		/// Creates a new GUI Skin based on a template.
		/// Use setSkin() to set the created skin.
		/// </summary>
		IGUISkin* CreateSkin(SkinType type);

		/// <summary>
		/// Sets or gets the currently used skin. Skins influence how GUIElements
		/// draw themselves. See createSkin() for how to set a new skin.
		/// </summary>
		__property void set_Skin(IGUISkin* skin);

		/// <summary>
		/// Sets or gets the currently used skin. Skins influence how GUIElements
		/// draw themselves. See createSkin() for how to set a new skin.
		/// </summary>
		__property IGUISkin* get_Skin();

		/// <summary>
		/// Returns the root gui element. This is the first gui element, parent of all other
		/// gui elements. You'll never need to use this method, unless you are not creating 
		/// your own gui elements, trying to add them to the gui elements without a parent.
		/// </summary>
		__property IGUIElement* get_RootGUIElement();

		/// <summary>
		/// Draws all gui elements. Call this in your drawing loops if you want
		/// GUI Elements to be visible.
		/// </summary>
		void DrawAll();

		/// <summary>
		/// Adds an button element. 
		/// </summary>
		/// <param name="parent"> Parent gui element of the message box, can be null</param>
		/// <param name="id"> Id with which the gui element can be identified.</param>
		IGUIElement* AddButton(Core::Rect position, IGUIElement* parent, int id, 
			System::String* text);

		/// <summary>
		/// Adds an empty window element.
		/// </summary>
		/// <param name="modal">Defines if the dialog is modal. This means, that all other
		/// gui elements which were created before the message box cannot be used
		/// until this messagebox is removed.</param>
		/// <param name="parent"> Parent gui element of the message box, can be null.</param>
		/// <param name="id"> Id with which the gui element can be identified.</param>
		IGUIElement* AddWindow(Core::Rect position, bool modal, System::String* text,
			IGUIElement* parent, int id);

		/// <summary>
		/// Adds a message box.
		/// </summary>
		/// <param name="text"> Text to be displayed in the body of the message box.</param>
		/// <param name="modal"> Defines if the dialog is modal. This means, that all other
		/// gui elements which were created before the message box cannot be used
		/// until this messagebox is removed.</param>
		/// <param name="flags"> Flags specifying the layout of the message box. For example
		/// to create a message box with an OK and a CANCEL button on it, set this
		/// to (MessageBoxFlag.OK | MessageBoxFlag.CANCEL)</param>.
		/// <param name="parent"> Parent gui element of the message box, can be null</param>
		/// <param name="id"> Id with which the gui element can be identified.</param>
		IGUIElement* AddMessageBox(System::String* caption, System::String* text, 
			bool modal, MessageBoxFlag flags, IGUIElement* parent, int id);

		/// <summary>
		/// Adds a scrollbar. 
		/// </summary>
		/// <param name="parent"> Parent gui element of the message box, can be null</param>
		/// <param name="id"> Id with which the gui element can be identified.</param>
		IGUIElement* AddScrollBar(bool horizontal, Core::Rect position, IGUIElement* Parent,
			int id);

		/// <summary>
		/// Adds an image.
		/// </summary>
		/// <param name="pos">Position of the image. The width and height of the image is taken
		/// from the image.</param>
		/// <param name="useAlphaChannel"> Sets if the image should use the alpha channel 
		/// of the texture to draw itself.</param>
		/// <param name="parent"> Parent gui element of the message box, can be null</param>
		/// <param name="id"> Id with which the gui element can be identified.</param>
		IGUIElement* AddImage(Video::ITexture* image, Core::Position2D pos, 
			bool useAlphaChannel, IGUIElement* parent, int id, System::String* text);

		/// <summary>
		/// Adds a checkbox element.
		/// </summary>
		/// <param name="parent"> Parent gui element of the message box, can be null</param>
		/// <param name="id"> Id with which the gui element can be identified.</param>
		IGUIElement* AddCheckBox(bool checked, Core::Rect position, IGUIElement* parent,
			int id, System::String* text);

        /// <summary>
		/// Adds a list box element.
		/// </summary>
		/// <param name="parent"> Parent gui element of the message box, can be null</param>
		/// <param name="id"> Id with which the gui element can be identified.</param>
		IGUIListBox* AddListBox(Core::Rect position, IGUIElement* parent, 
			int id, bool drawBackGround);

		/// <summary>
		/// Adds a file open dialog.
		/// </summary>
		/// <param name="parent"> Parent gui element of the message box, can be null</param>
		/// <param name="id"> Id with which the gui element can be identified.</param>
		IGUIElement* AddFileOpenDialog(System::String* text, bool modal, IGUIElement* parent,
			int id);

		/// <summary>
		/// Adds a static text. 
		/// </summary>
		/// <param name="parent"> Parent gui element of the message box, can be null</param>
		/// <param name="id"> Id with which the gui element can be identified.</param>
		IGUIStaticText* AddStaticText(System::String* text, Core::Rect position, bool border,
			bool wordWrap, IGUIElement* parent, int id);

		/// <summary>
		/// Adds an edit box.  
		/// </summary>
		/// <param name="parent"> Parent gui element of the message box, can be null</param>
		/// <param name="id"> Id with which the gui element can be identified.</param>
		IGUIElement* AddEditBox(System::String* text, Core::Rect position, bool border,
			IGUIElement* parent, int id);

	private:

		irr::gui::IGUIEnvironment* Environment;
		IGUIFont* BuildInFont;
	};

}
}
