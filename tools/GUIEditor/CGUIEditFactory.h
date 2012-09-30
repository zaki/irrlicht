// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_GUIEDIT_FACTORY_H_INCLUDED__
#define __C_GUIEDIT_FACTORY_H_INCLUDED__

#include "IGUIElementFactory.h"

namespace irr
{
namespace gui
{
	class IGUIElement;
	class IGUIEnvironment;

	//!	Interface making it possible to dynamicly create gui elements
	class CGUIEditFactory : public IGUIElementFactory
	{
	public:

		CGUIEditFactory(IGUIEnvironment* env);
		~CGUIEditFactory();

		//! adds a GUI element to the GUI Environment based on its type name
		/** \param typeName: Type name of the element to add.
		\param parent: Parent scene node of the new element, can be null to add it to the root.
		\return Returns pointer to the new element or null if not successful. */
		virtual IGUIElement* addGUIElement(const c8* typeName, IGUIElement* parent=0);

		//! returns amount of GUI element types this factory is able to create
		virtual s32 getCreatableGUIElementTypeCount() const;

		//! returns type name of a createable GUI element type by index
		/** \param idx: Index of the type in this factory. Must be a value between 0 and
		getCreatableGUIElementTypeCount() */
		virtual const c8* getCreateableGUIElementTypeName(s32 idx) const;

		// not used:
		virtual const c8* getCreateableGUIElementTypeName(EGUI_ELEMENT_TYPE type) const {return 0;} ;
		virtual EGUI_ELEMENT_TYPE getCreateableGUIElementType(s32 idx) const { return EGUIET_ELEMENT;};
		virtual IGUIElement* addGUIElement(EGUI_ELEMENT_TYPE type, IGUIElement* parent=0) {return 0;};
	private:

		IGUIEnvironment* Environment;
	};


} // end namespace scene
} // end namespace irr

#endif

