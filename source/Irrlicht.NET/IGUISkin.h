// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#using <mscorlib.dll>
using namespace System;

#pragma unmanaged
#include "..\\..\\include\\irrlicht.h"
#pragma managed

#include "Rect.h"
#include "Color.h"

namespace Irrlicht
{
namespace GUI
{
	public __gc class IGUIFont;

	/// <summary>
	/// Enumeration of available default skins.
	/// </summary>
	__value public enum SkinType
	{
		/// Default windows look and feel
		WINDOWS_CLASSIC,

		/// Like EGST_WINDOWS_CLASSIC, but with metallic shaded windows and buttons
		WINDOWS_METALLIC,
	};

	/// <summary>
	/// Enumeration of available default skin colors.
	/// </summary>
	__value public enum SkinColor
	{
		//! Dark shadow for three-dimensional display elements. 
		E3D_DARK_SHADOW = 0,

		//! Shadow color for three-dimensional display elements (for edges facing away from the light source). 
		E3D_SHADOW,			

		//! Face color for three-dimensional display elements and for dialog box backgrounds. 
		E3D_FACE,			

		//! Highlight color for three-dimensional display elements (for edges facing the light source.) 
		E3D_HIGH_LIGHT,		

		//! Light color for three-dimensional display elements (for edges facing the light source.) 
		E3D_LIGHT,			
		
		//! Active window border. 
        ACTIVE_BORDER,		

		//! Active window title bar text.
		ACTIVE_CAPTION,	

		//! Background color of multiple document interface (MDI) applications.
		APP_WORKSPACE,		

		//! Text on a button
		BUTTON_TEXT,		

		//! Grayed (disabled) text. 
		GRAY_TEXT,			

		//! Item(s) selected in a control. 
		HIGH_LIGHT,		

		//! Text of item(s) selected in a control. 
		HIGH_LIGHT_TEXT,	

		//! Inactive window border. 
		INACTIVE_BORDER,	

		//! Inactive window caption. 
		INACTIVE_CAPTION,	

		//! Tool tip color
		TOOLTIP,			

		//! Scrollbar gray area
		SCROLLBAR,		

		//! Window background
		WINDOW,			

		COUNT
	};


	/// <summary>
	/// Enumeration for default sizes.
	/// </summary>
	__value public enum SkinSize
	{
		//! default with / height of scrollbar
		SCROLLBAR_SIZE = 0,	

		//! height of menu
		MENU_HEIGHT,			

		//! width of a window button
		WINDOW_BUTTON_WIDTH,	

		//! width of a checkbox check
		CHECK_BOX_WIDTH,		

		//! width of a messagebox
		MESSAGE_BOX_WIDTH,

		//! height of a messagebox
		MESSAGE_BOX_HEIGHT,

		//! width of a default button
		BUTTON_WIDTH,

		//! height of a default button
		BUTTON_HEIGHT,
		
		//! this value is not used, it only specifies the amount of default sizes
		//! available.
		COUNT
	};

	/// <summary>
	/// A skin modifies the look of the GUI elements.
	/// </summary>
	public __gc class IGUISkin
	{
	public:

		/// <summary>
		/// You should access the IGUISkin 
		/// through the IGUIEnvironment::getSkin() property. Simply don't use
		/// this constructor.
		/// </summary>
		/// <param name="font">The real, unmanaged C++ skin</param>
		IGUISkin(irr::gui::IGUISkin* skin);

		~IGUISkin();

		/// <summary>
		/// returns default color
		/// </summary>
		Video::Color GetColor(SkinColor color);

		/// <summary>
		/// sets a default color
		/// </summary>
		void SetColor(SkinColor which, Video::Color newColor);

		/// <summary>
		/// returns default color
		/// </summary>
		int GetSize(SkinSize size);

		/// <summary>
		/// Returns or sets the font of this skin.
		/// </summary>
		__property IGUIFont* get_Font();

		/// <summary>
		/// Returns or sets the font of this skin.
		/// </summary>
		__property void set_Font(IGUIFont* font);

		/// <summary>
		/// Returns the internal pointer to the native C++ irrlicht skin.
		/// Do not use this, only needed by the internal .NET wrapper.
		///</summary>
		__property irr::gui::IGUISkin* get_NativeSkin();

	private:

		irr::gui::IGUISkin* Skin;
	};

}
}
