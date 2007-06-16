 // Copyright (C) 2002-2007 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_GUI_ELEMENT_H_INCLUDED__
#define __I_GUI_ELEMENT_H_INCLUDED__

#include "IAttributeExchangingObject.h"
#include "irrList.h"
#include "rect.h"
#include "irrString.h"
#include "IEventReceiver.h"
#include "EGUIElementTypes.h"
#include "IAttributes.h"

namespace irr
{
namespace gui
{
class IGUIEnvironment;

enum EGUI_ALIGNMENT
{
	//! Aligned to parent's top or left side (default)
	EGUIA_UPPERLEFT=0,
	//! Aligned to parent's bottom or right side
	EGUIA_LOWERRIGHT,
	//! Aligned to the center of parent
	EGUIA_CENTER,
	//! Scaled within its parent
	EGUIA_SCALE
};

//! Names for alignments
const c8* const GUIAlignmentNames[] =
{
	"upperLeft",
	"lowerRight",
	"center",
	"scale",
	0
};

//! Base class of all GUI elements.
class IGUIElement : public virtual io::IAttributeExchangingObject, public IEventReceiver
{
public:

	//! Constructor
	IGUIElement(EGUI_ELEMENT_TYPE type, IGUIEnvironment* environment, IGUIElement* parent,
		s32 id, core::rect<s32> rectangle)
		: Parent(0), RelativeRect(rectangle), AbsoluteRect(rectangle),
		AbsoluteClippingRect(rectangle), DesiredRect(rectangle),
		MaxSize(0,0), MinSize(1,1), IsVisible(true), IsEnabled(true),
		IsSubElement(false), NoClip(false), ID(id), 
		AlignLeft(EGUIA_UPPERLEFT), AlignRight(EGUIA_UPPERLEFT), AlignTop(EGUIA_UPPERLEFT), AlignBottom(EGUIA_UPPERLEFT),
		Environment(environment), Type(type)
	{
		// if we were given a parent to attach to
		if (parent)
			parent->addChild(this);

		// if we succeeded in becoming a child
		if (Parent)
		{
			LastParentRect = Parent->getAbsolutePosition();
			AbsoluteRect += LastParentRect.UpperLeftCorner;
			AbsoluteClippingRect = AbsoluteRect;
			AbsoluteClippingRect.clipAgainst(Parent->AbsoluteClippingRect);
		}
	}


	//! Destructor
	virtual ~IGUIElement()
	{
		// delete all children
		core::list<IGUIElement*>::Iterator it = Children.begin();
		for (; it != Children.end(); ++it)
		{
			(*it)->Parent = 0;
			(*it)->drop();
		}
	};


	//! Returns parent of this element.
	IGUIElement* getParent() const
	{
		return Parent;
	}


	//! Returns the relative rectangle of this element.
	core::rect<s32> getRelativePosition() const
	{
		return RelativeRect;
	}


	//! Sets the relative rectangle of this element.
	void setRelativePosition(const core::rect<s32>& r)
	{
		if (Parent)
		{
			const core::rect<s32>& r2 = Parent->getAbsolutePosition();

			core::dimension2df d((f32)(r2.getSize().Width), (f32)(r2.getSize().Height));

			if (AlignLeft   == EGUIA_SCALE)
				ScaleRect.UpperLeftCorner.X = (f32)r.UpperLeftCorner.X / d.Width;
			if (AlignRight  == EGUIA_SCALE)
				ScaleRect.LowerRightCorner.X = (f32)r.LowerRightCorner.X / d.Width;
			if (AlignTop    == EGUIA_SCALE)
				ScaleRect.UpperLeftCorner.Y = (f32)r.UpperLeftCorner.Y / d.Height;
			if (AlignBottom == EGUIA_SCALE)
				ScaleRect.LowerRightCorner.Y = (f32)r.LowerRightCorner.Y / d.Height;
		}

		DesiredRect = r;
		updateAbsolutePosition();
	}

	//! Sets the relative rectangle of this element.
	void setRelativePosition(const core::rect<f32>& r)
	{
		if (!Parent)
			return;
		
		const core::dimension2di& d = Parent->getAbsolutePosition().getSize();
		
		DesiredRect = core::rect<s32>( 
						(s32)((f32)d.Width  * r.UpperLeftCorner.X),
						(s32)((f32)d.Height * r.UpperLeftCorner.Y),
						(s32)((f32)d.Width  * r.LowerRightCorner.X),
						(s32)((f32)d.Height * r.LowerRightCorner.Y));

		ScaleRect = r;

		updateAbsolutePosition();
	}


	//! Returns the absolute rectangle of element.
	core::rect<s32> getAbsolutePosition() const
	{
		return AbsoluteRect;
	}

	//! Sets whether the element will ignore its parent's clipping rectangle
	void setNotClipped(bool noClip)
	{
		NoClip = noClip;
	}

	//! Gets whether the element will ignore its parent's clipping rectangle
	bool isNotClipped()
	{
		return NoClip;
	}

	//! Sets the maximum size allowed for this element
	/** If set to 0,0, there is no maximum size */
	void setMaxSize(core::dimension2di size)
	{
		MaxSize = size;
		updateAbsolutePosition();
	}

	//! Sets the minimum size allowed for this element
	void setMinSize(core::dimension2di size)
	{
		MinSize = size;
		if (MinSize.Width < 1)
			MinSize.Width = 1;
		if (MinSize.Height < 1)
			MinSize.Height = 1;
		updateAbsolutePosition();
	}

	void setAlignment(EGUI_ALIGNMENT left, EGUI_ALIGNMENT right, EGUI_ALIGNMENT top, EGUI_ALIGNMENT bottom)
	{
		AlignLeft = left;
		AlignRight = right;
		AlignTop = top;
		AlignBottom = bottom;

		if (Parent)
		{
			core::rect<s32> r(Parent->getAbsolutePosition());
		
			core::dimension2df d((f32)r.getSize().Width, (f32)r.getSize().Height);

			if (AlignLeft   == EGUIA_SCALE)
				ScaleRect.UpperLeftCorner.X = (f32)DesiredRect.UpperLeftCorner.X / d.Width;
			if (AlignRight  == EGUIA_SCALE)
				ScaleRect.LowerRightCorner.X = (f32)DesiredRect.LowerRightCorner.X / d.Width;
			if (AlignTop    == EGUIA_SCALE)
				ScaleRect.UpperLeftCorner.Y = (f32)DesiredRect.UpperLeftCorner.Y / d.Height;
			if (AlignBottom == EGUIA_SCALE)
				ScaleRect.LowerRightCorner.Y = (f32)DesiredRect.LowerRightCorner.Y / d.Height;
		}
	}

	//! Updates the absolute position.
	virtual void updateAbsolutePosition()
	{
		core::rect<s32> parentAbsolute(0,0,0,0);
		core::rect<s32> parentAbsoluteClip;
		s32 diffx, diffy;
		f32 fw=0.f, fh=0.f;

		if (Parent)
		{
			parentAbsolute = Parent->AbsoluteRect;

			if (NoClip)
			{
				IGUIElement* p=this;
				while (p && p->NoClip && p->Parent)
						p = p->Parent;
				if (p->Parent)
					parentAbsoluteClip = p->Parent->AbsoluteClippingRect;
				else
					parentAbsoluteClip = p->AbsoluteClippingRect;
			}
			else
				parentAbsoluteClip = Parent->AbsoluteClippingRect;
		}


		diffx = parentAbsolute.getWidth() - LastParentRect.getWidth();
		diffy = parentAbsolute.getHeight() - LastParentRect.getHeight();
		
		if (AlignLeft == EGUIA_SCALE || AlignRight == EGUIA_SCALE)
			fw = (f32)parentAbsolute.getWidth();

		if (AlignTop == EGUIA_SCALE || AlignBottom == EGUIA_SCALE)
			fh = (f32)parentAbsolute.getHeight();


		switch (AlignLeft)
		{
			case EGUIA_UPPERLEFT:
				break;
			case EGUIA_LOWERRIGHT:
				DesiredRect.UpperLeftCorner.X += diffx;
				break;
			case EGUIA_CENTER:
				DesiredRect.UpperLeftCorner.X += diffx/2;
				break;
			case EGUIA_SCALE:
				DesiredRect.UpperLeftCorner.X = (s32)(ScaleRect.UpperLeftCorner.X * fw);
				break;
		}

		switch (AlignRight)
		{
			case EGUIA_UPPERLEFT:
				break;
			case EGUIA_LOWERRIGHT:
				DesiredRect.LowerRightCorner.X += diffx;
				break;
			case EGUIA_CENTER:
				DesiredRect.LowerRightCorner.X += diffx/2;
				break;
			case EGUIA_SCALE:
				DesiredRect.LowerRightCorner.X = (s32)(ScaleRect.LowerRightCorner.X * fw);
				break;
		}

		switch (AlignTop)
		{
			case EGUIA_UPPERLEFT:
				break;
			case EGUIA_LOWERRIGHT:
				DesiredRect.UpperLeftCorner.Y += diffy;
				break;
			case EGUIA_CENTER:
				DesiredRect.UpperLeftCorner.Y += diffy/2;
				break;
			case EGUIA_SCALE:
				DesiredRect.UpperLeftCorner.Y = (s32)(ScaleRect.UpperLeftCorner.Y * fh);
				break;
		}

		switch (AlignBottom)
		{
			case EGUIA_UPPERLEFT:
				break;
			case EGUIA_LOWERRIGHT:
				DesiredRect.LowerRightCorner.Y += diffy;
				break;
			case EGUIA_CENTER:
				DesiredRect.LowerRightCorner.Y += diffy/2;
				break;
			case EGUIA_SCALE:
				DesiredRect.LowerRightCorner.Y = (s32)(ScaleRect.LowerRightCorner.Y * fh);
				break;
		}

		RelativeRect = DesiredRect;

		s32 w = RelativeRect.getWidth();
		s32 h = RelativeRect.getHeight();

		// make sure the desired rectangle is allowed
		if (w < MinSize.Width)
			RelativeRect.LowerRightCorner.X = RelativeRect.UpperLeftCorner.X + MinSize.Width;
		if (h < MinSize.Height)
			RelativeRect.LowerRightCorner.Y = RelativeRect.UpperLeftCorner.Y + MinSize.Height;
		if (MaxSize.Width && w > MaxSize.Width)
			RelativeRect.LowerRightCorner.X = RelativeRect.UpperLeftCorner.X + MaxSize.Width;
		if (MaxSize.Height && h > MaxSize.Height)
			RelativeRect.LowerRightCorner.Y = RelativeRect.UpperLeftCorner.Y + MaxSize.Height;

		RelativeRect.repair();
		
		AbsoluteRect = RelativeRect + parentAbsolute.UpperLeftCorner;

		if (!Parent)
			parentAbsoluteClip = AbsoluteRect;

		AbsoluteClippingRect = AbsoluteRect;
		AbsoluteClippingRect.clipAgainst(parentAbsoluteClip);

		LastParentRect = parentAbsolute;

		// update all children
		core::list<IGUIElement*>::Iterator it = Children.begin();
		for (; it != Children.end(); ++it)
		{
			(*it)->updateAbsolutePosition();
		}
	}


	//! Returns the child element, which is at the position of the point.
	IGUIElement* getElementFromPoint(const core::position2d<s32>& point)
	{
		IGUIElement* target = 0;

		// we have to search from back to front.

		core::list<IGUIElement*>::Iterator it = Children.getLast();

		if (IsVisible)
			while(it != Children.end())
			{
				target = (*it)->getElementFromPoint(point);
				if (target)
					return target;

				--it;
			}

		if (AbsoluteClippingRect.isPointInside(point) && IsVisible)
			target = this;
		
		return target;
	}


	//! Adds a GUI element as new child of this element.
	virtual void addChild(IGUIElement* child)
	{
		if (child)
		{
			child->grab();
			child->remove();  // remove from old parent
			child->LastParentRect = getAbsolutePosition();
			child->Parent = this;
			Children.push_back(child);			
		}
	}


	//! Removes a child.
	virtual void removeChild(IGUIElement* child)
	{
		core::list<IGUIElement*>::Iterator it = Children.begin();
		for (; it != Children.end(); ++it)
			if ((*it) == child)
			{
				(*it)->Parent = 0;
				(*it)->drop();
				Children.erase(it);
				return;
			}
	}


	//! Removes this element from its parent.
	virtual void remove()
	{
		if (Parent)
			Parent->removeChild(this);
	}


	//! Draws the element and its children.
	virtual void draw()
	{
		if (!IsVisible)
			return;

		core::list<IGUIElement*>::Iterator it = Children.begin();
		for (; it != Children.end(); ++it)
			(*it)->draw();
	}

	//! animate the element and its children.
	virtual void OnPostRender(u32 timeMs)
	{
		if (!IsVisible)
			return;

		core::list<IGUIElement*>::Iterator it = Children.begin();
		for (; it != Children.end(); ++it)
			(*it)->OnPostRender( timeMs );
	}


	//! Moves this element.
	virtual void move(core::position2d<s32> absoluteMovement)
	{
		setRelativePosition(DesiredRect + absoluteMovement);
	}


	//! Returns true if element is visible.
	virtual bool isVisible()
	{
		_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
		return IsVisible;
	}


	//! Sets the visible state of this element.
	virtual void setVisible(bool visible)
	{
		IsVisible = visible;
	}


	//! Returns true if this element was created as part of its parent control
	virtual bool isSubElement()
	{
		_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
		return IsSubElement;
	}

	//! Sets whether this control was created as part of its parent, 
	//! for example when a scrollbar is part of a listbox. 
	//! SubElements are not saved to disk when calling guiEnvironment->saveGUI()
	virtual void setSubElement(bool subElement)
	{
		IsSubElement = subElement;
	}

	//! Returns true if element is enabled.
	virtual bool isEnabled()
	{
		_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
		return IsEnabled;
	}


	//! Sets the enabled state of this element.
	virtual void setEnabled(bool enabled)
	{
		IsEnabled = enabled;
	}


	//! Sets the new caption of this element.
	virtual void setText(const wchar_t* text)
	{
		Text = text;
	}


	//! Returns caption of this element.
	virtual const wchar_t* getText()
	{
		return Text.c_str();
	}

	//! Sets the new caption of this element.
	virtual void setToolTipText(const wchar_t* text)
	{
		ToolTipText = text;
	}


	//! Returns caption of this element.
	virtual core::stringw &getToolTipText()
	{
		return ToolTipText;
	}


	//! Returns id. Can be used to identify the element.
	virtual s32 getID()
	{
		return ID;
	}

	//! Sets the id of this element
	virtual void setID(s32 id)
	{
		ID = id;
	}


	//! Called if an event happened.
	virtual bool OnEvent(SEvent event)
	{
		if (Parent)
			Parent->OnEvent(event);

		return true;
	}


	//! Brings a child to front
	/** \return Returns true if successful, false if not. */
	virtual bool bringToFront(IGUIElement* element)
	{
		core::list<IGUIElement*>::Iterator it = Children.begin();
		for (; it != Children.end(); ++it)
		{
			if (element == (*it))
			{
				Children.erase(it);
				Children.push_back(element);
				return true;
			}
		}

		_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
		return false;
	}

	//! Returns list with children of this element
	virtual const core::list<IGUIElement*>& getChildren() const
	{
		return Children;
	}

	//! Finds the first element with the given id.
	/** \param id: Id to search for.
	 \param searchchildren: Set this to true, if also children of this 
	 element may contain the element with the searched id and they 
	 should be searched too.
	 \return Returns the first element with the given id. If no element
	 with this id was found, 0 is returned. */
	virtual IGUIElement* getElementFromId(s32 id, bool searchchildren=false) const
	{
		IGUIElement* e = 0;

		core::list<IGUIElement*>::Iterator it = Children.begin();
		for (; it != Children.end(); ++it)
		{
			if ((*it)->getID() == id)
				return (*it);

			if (searchchildren)
				e = (*it)->getElementFromId(id, true);

			if (e)
				return e;
		}		

		return e;
	}

	//! Returns the type of the gui element. 
	/** This is needed for the .NET wrapper but will be used
	later for serializing and deserializing.
	If you wrote your own GUIElements, you need to set the type for your element as first parameter
	in the constructor of IGUIElement. For own (=unknown) elements, simply use EGUIET_ELEMENT as type */
	EGUI_ELEMENT_TYPE getType() const
	{
		return Type;
	}

	//! Returns the type name of the gui element. 
	/** This is needed serializing elements. For serializing your own elements, override this function 
	and return your own type name which is created by your IGUIElementFactory */
	virtual const c8* getTypeName() const
	{
		return GUIElementTypeNames[Type];
	}

	//! Writes attributes of the scene node.
	//! Implement this to expose the attributes of your scene node for
	//! scripting languages, editors, debuggers or xml serialization purposes.
	virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options=0)
	{
		out->addInt("Id", ID );
		out->addString("Caption", Text.c_str());
		out->addRect("Rect", DesiredRect);
		out->addPosition2d("MinSize", core::position2di(MinSize.Width, MinSize.Height));
		out->addPosition2d("MaxSize", core::position2di(MaxSize.Width, MaxSize.Height));
		out->addBool("NoClip", NoClip);
		out->addEnum("LeftAlign", AlignLeft, GUIAlignmentNames);
		out->addEnum("RightAlign", AlignRight, GUIAlignmentNames);
		out->addEnum("TopAlign", AlignTop, GUIAlignmentNames);
		out->addEnum("BottomAlign", AlignBottom, GUIAlignmentNames);
		out->addBool("Visible", IsVisible );
		out->addBool("Enabled", IsEnabled );
	}

	//! Reads attributes of the scene node.
	//! Implement this to set the attributes of your scene node for
	//! scripting languages, editors, debuggers or xml deserialization purposes.
	virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options=0)
	{
		//! relative rect of element
		ID = in->getAttributeAsInt("Id");
		Text = in->getAttributeAsStringW("Caption").c_str();
		IsVisible = in->getAttributeAsBool("Visible");
		IsEnabled = in->getAttributeAsBool("Enabled");
		core::position2di p = in->getAttributeAsPosition2d("MaxSize");
		MaxSize = core::dimension2di(p.X,p.Y);
		p = in->getAttributeAsPosition2d("MinSize");
		MinSize = core::dimension2di(p.X,p.Y);
		NoClip = in->getAttributeAsBool("NoClip");
		AlignLeft = (EGUI_ALIGNMENT) in->getAttributeAsEnumeration("LeftAlign", GUIAlignmentNames);
		AlignRight = (EGUI_ALIGNMENT)in->getAttributeAsEnumeration("RightAlign", GUIAlignmentNames);
		AlignTop = (EGUI_ALIGNMENT)in->getAttributeAsEnumeration("TopAlign", GUIAlignmentNames);
		AlignBottom = (EGUI_ALIGNMENT)in->getAttributeAsEnumeration("BottomAlign", GUIAlignmentNames);

		setRelativePosition(in->getAttributeAsRect("Rect"));
	}

protected:

	//! List of all children of this element
	core::list<IGUIElement*> Children;

	//! Pointer to the parent
	IGUIElement* Parent;

	//! relative rect of element
	core::rect<s32> RelativeRect;

	//! absolute rect of element
	core::rect<s32> AbsoluteRect;

	//! absolute clipping rect of element
	core::rect<s32> AbsoluteClippingRect;

	//! the rectangle the element would prefer to be, 
	//! if it was not constrained by parent or max/min size
	core::rect<s32> DesiredRect;

	//! for calculating the difference when resizing parent
	core::rect<s32> LastParentRect;

	//! relative scale of the element inside its parent
	core::rect<f32> ScaleRect;

	//! maximum and minimum size of the element
	core::dimension2di MaxSize, MinSize;

	//! is visible?
	bool IsVisible;

	//! is enabled?
	bool IsEnabled;

	//! is a part of a larger whole and should not be serialized?
	bool IsSubElement;

	//! does this element ignore its parent's clipping rectangle?
	bool NoClip;

	//! caption
	core::stringw Text;

	//! tooltip
	core::stringw ToolTipText;

	//! id
	s32 ID;

	//! tells the element how to act when its parent is resized
	EGUI_ALIGNMENT AlignLeft, AlignRight, AlignTop, AlignBottom;

	//! GUI Environment
	IGUIEnvironment* Environment;

	//! type of element
	EGUI_ELEMENT_TYPE Type;
};


} // end namespace gui
} // end namespace irr

#endif

