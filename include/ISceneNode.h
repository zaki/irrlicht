// Copyright (C) 2002-2007 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_SCENE_NODE_H_INCLUDED__
#define __I_SCENE_NODE_H_INCLUDED__

#include "IUnknown.h"
#include "ESceneNodeTypes.h"
#include "ECullingTypes.h"
#include "EDebugSceneTypes.h"
#include "ISceneManager.h"
#include "ISceneNodeAnimator.h"
#include "ITriangleSelector.h"
#include "SMaterial.h"
#include "irrString.h"
#include "aabbox3d.h"
#include "matrix4.h"
#include "irrList.h"
#include "IAttributes.h"
#include "IAttributeExchangingObject.h"

namespace irr
{
namespace scene
{
	//! Scene node interface.
	/** A scene node is a node in the hirachical scene graph. Every scene node may have children,
	which are other scene nodes. Children move relative the their parents position. If the parent of a node is not
	visible, its children won't be visible too. In this way, it is for example easily possible
	to attach a light to a moving car, or to place a walking character on a moving platform
	on a moving ship. */
	class ISceneNode : public io::IAttributeExchangingObject
	{
	public:

		//! Constructor
		ISceneNode(	ISceneNode* parent, ISceneManager* mgr, s32 id=-1,
					const core::vector3df& position = core::vector3df(0,0,0),
					const core::vector3df& rotation = core::vector3df(0,0,0),
					const core::vector3df& scale = core::vector3df(1.0f, 1.0f, 1.0f))
			: RelativeTranslation(position), RelativeRotation(rotation), RelativeScale(scale),
				Parent(parent), ID(id), SceneManager(mgr), TriangleSelector(0),
				AutomaticCullingState(EAC_BOX), IsVisible(true),
				DebugDataVisible(EDS_OFF), IsDebugObject(false)
		{
			if (Parent)
				Parent->addChild(this);

			updateAbsolutePosition();
		}



		//! Destructor
		virtual ~ISceneNode()
		{
			// delete all children
			removeAll();

			// delete all animators
			core::list<ISceneNodeAnimator*>::Iterator ait = Animators.begin();
			for (; ait != Animators.end(); ++ait)
				(*ait)->drop();

			if (TriangleSelector)
				TriangleSelector->drop();
		}


		//! This method is called just before the rendering process of the whole scene.
		/** Nodes may register themselves in the render pipeline during this call,
		 precalculate the geometry which should be renderered, and prevent their
		 children from being able to register them selfes if they are clipped by simply
		 not calling their OnRegisterSceneNode-Method. 
		 If you are implementing your own scene node, you should overwrite this method
		 with an implementtion code looking like this:
		 \code
		 if (IsVisible)
			SceneManager->registerNodeForRendering(this);

		 ISceneNode::OnRegisterSceneNode();
		 \endcode
	    */
		virtual void OnRegisterSceneNode()
		{
			if (IsVisible)
			{
				core::list<ISceneNode*>::Iterator it = Children.begin();
				for (; it != Children.end(); ++it)
					(*it)->OnRegisterSceneNode();
			}
		}


		//! OnAnimate() is called just before rendering the whole scene.
		//! Nodes may calculate or store animations here, and may do other useful things,
		//! dependent on what they are. Also, OnAnimate() should be called for all
		//! child scene nodes here. This method will called once per frame, independent
		//! of if the scene node is visible or not.
		//! \param timeMs: Current time in milli seconds.
		virtual void OnAnimate(u32 timeMs)
		{
			if (IsVisible)
			{
				// animate this node with all animators

				core::list<ISceneNodeAnimator*>::Iterator ait = Animators.begin();
				for (; ait != Animators.end(); ++ait)
					(*ait)->animateNode(this, timeMs);

				// update absolute position
				updateAbsolutePosition();

				// perform the post render process on all children

				core::list<ISceneNode*>::Iterator it = Children.begin();
				for (; it != Children.end(); ++it)
					(*it)->OnAnimate(timeMs);
			}
		}


		//! Renders the node.
		virtual void render() = 0;


		//! Returns the name of the node.
		//! \return Returns name as wide character string.
		virtual const c8* getName() const
		{
			return Name.c_str();
		}


		//! Sets the name of the node.
		//! \param name: New name of the scene node.
		virtual void setName(const c8* name)
		{
			Name = name;
		}


		//! Returns the axis aligned, not transformed bounding box of this node.
		//! This means that if this node is a animated 3d character, moving in a room,
		//! the bounding box will always be around the origin. To get the box in
		//! real world coordinates, just transform it with the matrix you receive with
		//! getAbsoluteTransformation() or simply use getTransformedBoundingBox(),
		//! which does the same.
		virtual const core::aabbox3d<f32>& getBoundingBox() const = 0;


		//! Returns the axis aligned, transformed and animated absolute bounding box
		//! of this node.
		virtual const core::aabbox3d<f32> getTransformedBoundingBox() const
		{
			core::aabbox3d<f32> box = getBoundingBox();
			AbsoluteTransformation.transformBox(box);
			return box;
		}


		//! returns the absolute transformation of the node. Is recalculated every OnAnimate()-call.
		const core::matrix4& getAbsoluteTransformation() const
		{
			return AbsoluteTransformation;
		}


		//! Returns the relative transformation of the scene node.
		//! The relative transformation is stored internally as 3 vectors:
		//! translation, rotation and scale. To get the relative transformation
		//! matrix, it is calculated from these values.
		//! \return Returns the relative transformation matrix.
		virtual core::matrix4 getRelativeTransformation() const
		{
			core::matrix4 mat;
			mat.setRotationDegrees(RelativeRotation);
			mat.setTranslation(RelativeTranslation);

			if (RelativeScale != core::vector3df(1,1,1))
			{
				core::matrix4 smat;
				smat.setScale(RelativeScale);
				mat *= smat;
			}

			return mat;
		}


		//! Returns true if the node is visible. This is only an option, set by the user and has
		//! nothing to do with geometry culling
		virtual bool isVisible() const
		{
			_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
			return IsVisible;
		}


		//! Sets if the node should be visible or not. All children of this node won't be visible too.
		virtual void setVisible(bool isVisible)
		{
			IsVisible = isVisible;
		}


		//! Returns the id of the scene node. This id can be used to identify the node.
		virtual s32 getID() const
		{
			return ID;
		}


		//! sets the id of the scene node. This id can be used to identify the node.
		virtual void setID(s32 id)
		{
			ID = id;
		}


		//! Adds a child to this scene node. If the scene node already
		//! has got a parent, it is removed from there as child.
		virtual void addChild(ISceneNode* child)
		{
			if (child)
			{
				child->grab();
				child->remove(); // remove from old parent
				Children.push_back(child);
				child->Parent = this;
			}
		}


		//! Removes a child from this scene node.
		//! \return Returns true if the child could be removed, and false if not.
		virtual bool removeChild(ISceneNode* child)
		{
			core::list<ISceneNode*>::Iterator it = Children.begin();
			for (; it != Children.end(); ++it)
				if ((*it) == child)
				{
					(*it)->Parent = 0;
					(*it)->drop();
					Children.erase(it);
					return true;
				}

			_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
			return false;
		}


		//! Removes all children of this scene node
		virtual void removeAll()
		{
			core::list<ISceneNode*>::Iterator it = Children.begin();
			for (; it != Children.end(); ++it)
			{
				(*it)->Parent = 0;
				(*it)->drop();
			}

			Children.clear();
		}


		//! Removes this scene node from the scene, deleting it.
		virtual void remove()
		{
			if (Parent)
				Parent->removeChild(this);
		}


		//! Adds an animator which should animate this node.
		virtual void addAnimator(ISceneNodeAnimator* animator)
		{
			if (animator)
			{
				Animators.push_back(animator);
				animator->grab();
			}
		}


		//! Returns a const reference to the list of all scene node animators.
		const core::list<ISceneNodeAnimator*>& getAnimators() const
		{
			return Animators;
		}


		//! Removes an animator from this scene node.
		virtual void removeAnimator(ISceneNodeAnimator* animator)
		{
			core::list<ISceneNodeAnimator*>::Iterator it = Animators.begin();
			for (; it != Animators.end(); ++it)
				if ((*it) == animator)
				{
					(*it)->drop();
					Animators.erase(it);
					return;
				}
		}


		//! Removes all animators from this scene node.
		virtual void removeAnimators()
		{
			core::list<ISceneNodeAnimator*>::Iterator it = Animators.begin();
			for (; it != Animators.end(); ++it)
				(*it)->drop();

			Animators.clear();
		}


		//! Returns the material based on the zero based index i. To get the amount
		//! of materials used by this scene node, use getMaterialCount().
		//! This function is needed for inserting the node into the scene hirachy on a
		//! optimal position for minimizing renderstate changes, but can also be used
		//! to directly modify the material of a scene node.
		//! \param num: Zero based index. The maximal value is getMaterialCount() - 1.
		//! \return Returns the material of that index.
		virtual video::SMaterial& getMaterial(u32 num)
		{
			return *((video::SMaterial*)0);
		}


		//! Returns amount of materials used by this scene node.
		//! \return Returns current count of materials used by this scene node.
		virtual u32 getMaterialCount()
		{
			return 0;
		}


		//! Sets all material flags at once to a new value. Helpful for
		//! example, if you want to be the the whole mesh to be lighted by
		//! \param flag: Which flag of all materials to be set.
		//! \param newvalue: New value of the flag.
		void setMaterialFlag(video::E_MATERIAL_FLAG flag, bool newvalue)
		{
			for (u32 i=0; i<getMaterialCount(); ++i)
				getMaterial(i).setFlag(flag, newvalue);
		}


		//! Sets the texture of the specified layer in all materials of this
		//! scene node to the new texture.
		//! \param textureLayer: Layer of texture to be set. Must be a value greater or
		//! equal than 0 and smaller than MATERIAL_MAX_TEXTURES.
		//! \param texture: Texture to be used.
		void setMaterialTexture(u32 textureLayer, video::ITexture* texture)
		{
			if (textureLayer>= video::MATERIAL_MAX_TEXTURES)
				return;

			for (u32 i=0; i<getMaterialCount(); ++i)
				getMaterial(i).Textures[textureLayer] = texture;
		}


		//! Sets the material type of all materials s32 this scene node
		//! to a new material type.
		//! \param newType: New type of material to be set.
		void setMaterialType(video::E_MATERIAL_TYPE newType)
		{
			for (u32 i=0; i<getMaterialCount(); ++i)
				getMaterial(i).MaterialType = newType;
		}


		//! Gets the scale of the scene node.
		/** \return Returns the scale of the scene node. */
		virtual core::vector3df getScale() const
		{
			return RelativeScale;
		}


		//! Sets the scale of the scene node.
		/** \param scale: New scale of the node */
		virtual void setScale(const core::vector3df& scale)
		{
			RelativeScale = scale;
		}


		//! Gets the rotation of the node.
		/** Note that this is the relative rotation of the node.
		\return Current relative rotation of the scene node. */
		virtual const core::vector3df& getRotation() const
		{
			return RelativeRotation;
		}


		//! Sets the rotation of the node.
		/** This only modifies the relative rotation of the node.
		\param rotation: New rotation of the node in degrees. */
		virtual void setRotation(const core::vector3df& rotation)
		{
			RelativeRotation = rotation;
		}


		//! Gets the position of the node.
		/** Note that the position is relative to the parent.
		\return Returns the current position of the node relative to the parent. */
		virtual const core::vector3df getPosition() const
		{
			return RelativeTranslation;
		}


		//! Sets the position of the node.
		/** Note that the position is relative to the parent.
		\param newpos: New relative postition of the scene node. */
		virtual void setPosition(const core::vector3df& newpos)
		{
			RelativeTranslation = newpos;
		}


		//! Gets the abolute position of the node.
		/** The position is absolute.
		\return Returns the current absolute position of the scene node. */
		virtual core::vector3df getAbsolutePosition() const
		{
			return AbsoluteTransformation.getTranslation();
		}


		//! Enables or disables automatic culling based on the bounding box.
		/** Automatic culling is enabled by default. Note that not
		all SceneNodes support culling (the billboard scene node for example)
		and that some nodes always cull their geometry because it is their
		only reason for existence, for example the OctreeSceneNode.
		\param state: The culling state to be used. */
		void setAutomaticCulling( E_CULLING_TYPE state)
		{
			AutomaticCullingState = state;
		}


		//! Gets the automatic culling state.
		/** \return The node is culled based on its bounding box if this method
		 returns true, otherwise no culling is performed. */
		E_CULLING_TYPE getAutomaticCulling() const
		{
			_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
			return AutomaticCullingState;
		}


		//! Sets if debug data like bounding boxes should be drawn.
		/** Please note that not all scene nodes support this feature. */
		virtual void setDebugDataVisible(E_DEBUG_SCENE_TYPE visible)
		{
			DebugDataVisible = visible;
		}

		//! Returns if debug data like bounding boxes are drawed.
		E_DEBUG_SCENE_TYPE isDebugDataVisible() const
		{
			_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
			return DebugDataVisible;
		}


		//! Sets if this scene node is a debug object.
		/** Debug objects have some special properties, for example they can be easily
		excluded from collision detection or from serialization, etc. */
		void setIsDebugObject(bool debugObject)
		{
			IsDebugObject = debugObject;
		}

		//! Returns if this scene node is a debug object.
		/** Debug objects have some special properties, for example they can be easily
		excluded from collision detection or from serialization, etc. */
		bool isDebugObject()
		{
			_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
			return IsDebugObject;
		}


		//! Returns a const reference to the list of all children.
		const core::list<ISceneNode*>& getChildren() const
		{
			return Children;
		}


		//! Changes the parent of the scene node.
		virtual void setParent(ISceneNode* newParent)
		{
			grab();
			remove();

			Parent = newParent;

			if (Parent)
				Parent->addChild(this);

			drop();
		}


		//! Returns the triangle selector attached to this scene node.
		//! The Selector can be used by the engine for doing collision
		//! detection. You can create a TriangleSelector with
		//! ISceneManager::createTriangleSelector() or
		//! ISceneManager::createOctTreeTriangleSelector and set it with
		//! ISceneNode::setTriangleSelector(). If a scene node got no triangle
		//! selector, but collision tests should be done with it, a triangle
		//! selector is created using the bounding box of the scene node.
		//! \return Returns a pointer to the TriangleSelector or NULL, if there
		//! is none.
		virtual ITriangleSelector* getTriangleSelector() const
		{
			return TriangleSelector;
		}


		//! Sets the triangle selector of the scene node. The Selector can be
		//! used by the engine for doing collision detection. You can create a
		//! TriangleSelector with ISceneManager::createTriangleSelector() or
		//! ISceneManager::createOctTreeTriangleSelector(). Some nodes may
		//! create their own selector by default, so it would be good to
		//! check if there is already a selector in this node by calling
		//! ISceneNode::getTriangleSelector().
		//! \param selector: New triangle selector for this scene node.
		virtual void setTriangleSelector(ITriangleSelector* selector)
		{
			if (TriangleSelector)
				TriangleSelector->drop();

			TriangleSelector = selector;
			if (TriangleSelector)
				TriangleSelector->grab();
		}

		//! updates the absolute position based on the relative and the parents position
		virtual void updateAbsolutePosition()
		{
			if (Parent )
			{
				AbsoluteTransformation =
					Parent->getAbsoluteTransformation() * getRelativeTransformation();
			}
			else
				AbsoluteTransformation = getRelativeTransformation();
		}

		//! Returns the parent of this scene node
		scene::ISceneNode* getParent() const
		{
			return Parent;
		}

		//! Returns type of the scene node
		virtual ESCENE_NODE_TYPE getType() const
		{
			return ESNT_UNKNOWN;
		}

		//! Writes attributes of the scene node.
		//! Implement this to expose the attributes of your scene node for
		//! scripting languages, editors, debuggers or xml serialization purposes.
		virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options=0)
		{
			out->addString	("Name", Name.c_str());
			out->addInt	("Id", ID );

			out->addVector3d("Position", getPosition() );
			out->addVector3d("Rotation", getRotation() );
			out->addVector3d("Scale", getScale() );

			out->addBool	("Visible", IsVisible );
			out->addEnum	("AutomaticCulling", AutomaticCullingState, AutomaticCullingNames);
			out->addInt	("DebugDataVisible", DebugDataVisible );
			out->addBool	("IsDebugObject", IsDebugObject );
		}

		//! Reads attributes of the scene node.
		//! Implement this to set the attributes of your scene node for
		//! scripting languages, editors, debuggers or xml deserialization purposes.
		virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options=0)
		{
			Name = in->getAttributeAsString("Name");
			ID = in->getAttributeAsInt("Id");

			setPosition(in->getAttributeAsVector3d("Position"));
			setRotation(in->getAttributeAsVector3d("Rotation"));
			setScale(in->getAttributeAsVector3d("Scale"));

			IsVisible = in->getAttributeAsBool("Visible");
			AutomaticCullingState = (scene::E_CULLING_TYPE ) in->getAttributeAsEnumeration("AutomaticCulling", scene::AutomaticCullingNames);

			DebugDataVisible = (scene::E_DEBUG_SCENE_TYPE ) in->getAttributeAsInt("DebugDataVisible");
			IsDebugObject = in->getAttributeAsBool("IsDebugObject");

			updateAbsolutePosition();
		}

		//! Creates a clone of this scene node and its children.
		virtual ISceneNode* clone(ISceneNode* newParent=0, ISceneManager* newManager=0) 
		{ 
			return 0; // to be implemented by derived classes
		}

	protected:

		//! this method can be used by clone() implementations of derived classes
		void cloneMembers(ISceneNode* toCopyFrom, ISceneManager* newManager)
		{
			Name = toCopyFrom->Name;
			AbsoluteTransformation = toCopyFrom->AbsoluteTransformation;
			RelativeTranslation = toCopyFrom->RelativeTranslation;
			RelativeRotation = toCopyFrom->RelativeRotation;
			RelativeScale = toCopyFrom->RelativeScale;			
			ID = toCopyFrom->ID;
			setTriangleSelector(toCopyFrom->TriangleSelector);
			AutomaticCullingState = toCopyFrom->AutomaticCullingState;
			DebugDataVisible = toCopyFrom->DebugDataVisible;
			IsVisible = toCopyFrom->IsVisible;
			IsDebugObject = toCopyFrom->IsDebugObject;

			if (newManager)
				SceneManager = newManager;
			else
				SceneManager = toCopyFrom->SceneManager;

			// clone children

			core::list<ISceneNode*>::Iterator it = toCopyFrom->Children.begin();
			for (; it != toCopyFrom->Children.end(); ++it)
				(*it)->clone(this, newManager);

			// clone animators

			core::list<ISceneNodeAnimator*>::Iterator ait = toCopyFrom->Animators.begin();
			for (; ait != toCopyFrom->Animators.end(); ++ait)
			{
				ISceneNodeAnimator* anim = (*ait)->createClone(this, SceneManager);
				if (anim)
				{
					addAnimator(anim);
					anim->drop();
				}
			}
		}

		//! name of the scene node.
		core::stringc Name;

		//! absolute transformation of the node.
		core::matrix4 AbsoluteTransformation;

		//! relative translation of the scene node.
		core::vector3df RelativeTranslation;

		//! relative rotation of the scene node.
		core::vector3df RelativeRotation;

		//! relative scale of the scene node.
		core::vector3df RelativeScale;

		//! Pointer to the parent
		ISceneNode* Parent;

		//! List of all children of this node
		core::list<ISceneNode*> Children;

		//! List of all animator nodes
		core::list<ISceneNodeAnimator*> Animators;

		//! id of the node.
		s32 ID;

		//! pointer to the scene manager
		ISceneManager* SceneManager;

		//! pointer to the triangle selector
		ITriangleSelector* TriangleSelector;

		//! automatic culling
		E_CULLING_TYPE AutomaticCullingState;

		//! is the node visible?
		bool IsVisible;

		//! flag if debug data should be drawn, such as Bounding Boxes.
		E_DEBUG_SCENE_TYPE DebugDataVisible;

		//! is debug object?
		bool IsDebugObject;
	};

} // end namespace scene
} // end namespace irr

#endif

