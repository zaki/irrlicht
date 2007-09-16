// Copyright (C) 2002-2007 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_DEFAULT_GUI_ELEMENT_FACTORY_H_INCLUDED__
#define __C_DEFAULT_GUI_ELEMENT_FACTORY_H_INCLUDED__

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_GUI_

#include "IGUIElementFactory.h"

namespace irr
{
namespace gui
{
	class IGUIElement;
	class IGUIEnvironment;
	
	//!	Interface making it possible to dynamicly create gui elements
	class CDefaultGUIElementFactory : public IGUIElementFactory
	{
	public:

		CDefaultGUIElementFactory(IGUIEnvironment* env);
		~CDefaultGUIElementFactory();
		//! adds an element to the gui environment based on its type id
		/** \param type: Type of the element to add.
		\param parent: Parent scene node of the new element, can be null to add to the root.
		\return Returns pointer to the new element or null if not successful. */
		virtual IGUIElement* addGUIElement(EGUI_ELEMENT_TYPE type, IGUIElement* parent=0);

		//! adds a GUI element to the GUI Environment based on its type name
		/** \param typeName: Type name of the element to add.
		\param parent: Parent scene node of the new element, can be null to add it to the root.
		\return Returns pointer to the new element or null if not successful. */
		virtual IGUIElement* addGUIElement(const c8* typeName, IGUIElement* parent=0);

		//! returns amount of GUI element types this factory is able to create
		virtual s32 getCreatableGUIElementTypeCount() const;

		//! returns type of a createable element type
		/** \param idx: Index of the element type in this factory. Must be a value between 0 and
		getCreatableGUIElementTypeCount() */
		virtual EGUI_ELEMENT_TYPE getCreateableGUIElementType(s32 idx) const;

		//! returns type name of a createable GUI element type by index
		/** \param idx: Index of the type in this factory. Must be a value between 0 and
		getCreatableGUIElementTypeCount() */
		virtual const c8* getCreateableGUIElementTypeName(s32 idx) const;

		//! returns type name of a createable GUI element
		/** \param type: Type of GUE element. 
		\return: Returns name of the type if this factory can create the type, otherwise 0. */
		virtual const c8* getCreateableGUIElementTypeName(EGUI_ELEMENT_TYPE type) const;

	private:

		EGUI_ELEMENT_TYPE getTypeFromName(const c8* name) const;

		IGUIEnvironment* Environment;
	};


} // end namespace gui
} // end namespace irr

#endif // _IRR_COMPILE_WITH_GUI_

#endif // __C_DEFAULT_GUI_ELEMENT_FACTORY_H_INCLUDED__
