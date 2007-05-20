// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#using <mscorlib.dll>
using namespace System;

#pragma unmanaged
#include "..\\..\\include\\irrlicht.h"
#pragma managed

#include "escenenodetype.h"
#include "Box3D.h"
#include "Material.h"
#include "Matrix4.h"

namespace Irrlicht
{
namespace Scene
{

public __gc class ISceneNodeAnimator;

/// <summary> 
/// Scene node interface.
/// A scene node is a node in the hirachical scene graph. Every scene node may have children,
/// which are other scene nodes. Children move relative the their parents position. If the parent of a node is not
/// visible, its children won't be visible too. In this way, it is for example easily possible
/// to attach a light to a moving car, or to place a walking character on a moving platform
/// on a moving ship. 
/// </summary>
public __gc class ISceneNode
{
public:

	/// <summary>
	/// Creates a scene node from a native C++ scene node. Don't use this, its better to create 
	/// scene nodes using the SceneManager with its addSceneNode() methods.
	/// </summary>
	ISceneNode(irr::scene::ISceneNode* realSceneNode);

	/// Destructor
	~ISceneNode();

	/// <summary>
	/// Determines whether two SceneNodes instances are equal. Simply compares the Irrlicht C++ instance pointers.
	/// </summary>
	bool Equals( System::Object __gc* obj)
	{
		ISceneNode* node = dynamic_cast<ISceneNode*>( obj );
		return node && node->SceneNode == SceneNode;
	}

	/// <summary>
	/// Renders the node.
	/// </summary>
	void Render();

	/// <summary>
	/// Returns or sets the name of the node.
	/// </summary>
	__property System::String* get_Name();

	/// <summary>
	/// Returns or sets the name of the node.
	/// </summary>
	__property void set_Name(System::String* name);

	/// <summary>
	/// Returns the axis aligned, not transformed bounding box of this node.
	/// This means that if this node is a animated 3d character, moving in a room,
	/// the bounding box will always be around the origin. To get the box in
	/// real world coordinates, just transform it with the matrix you receive with
	/// getAbsoluteTransformation() or simply use getTransformedBoundingBox(),
	/// which does the same.
	/// </summary>
	__property Core::Box3D get_BoundingBox();

	/// <summary>
	/// Returns the axis aligned, transformed and animated absolute bounding box
	/// of this node.
	/// </summary>
	__property Core::Box3D get_TransformedBoundingBox();

	/// <summary>
	/// Returns the absolute transformation of the node. Is recalculated every OnPostRender()-call.
	/// </summary>
	__property Core::Matrix4 get_AbsoluteTransformation();

	/// <summary>
	/// Returns the relative transformation of the scene node.
	/// The relative transformation is stored internally as 3 vectors:
	/// translation, rotation and scale. To get the relative transformation
	/// matrix, it is calculated from these values.
	/// </summary>
	__property Core::Matrix4 get_RelativeTransformation();

	/// <summary>
	/// Sets or gets if the node is visible. This is only an option, set by the user and has
	/// nothing to do with geometry culling
	/// </summary>
	__property bool get_Visible();

	/// <summary>
	/// Sets or gets if the node is visible. This is only an option, set by the user and has
	/// nothing to do with geometry culling
	/// </summary>
	__property void set_Visible(bool visible);

	/// <summary>
	/// Sets or gets the id of the scene node. This id can be used to identify the node.
	/// </summary>
	__property int get_ID();

	/// <summary>
	/// Sets or gets the id of the scene node. This id can be used to identify the node.
	/// </summary>
	__property void set_ID(int id);

	/// <summary>
	/// Adds a child to this scene node. If the scene node already
	/// has got a parent, it is removed from there as child.
	/// </summary>
	void AddChild(ISceneNode* child);

	/// <summary>
	/// Removes a child from this scene node.
	/// </summary>
	/// <returns>Returns true if the child could be removed, and false if not.</returns>
	bool RemoveChild(ISceneNode* child);

	/// <summary>
	/// Removes all children of this scene node
	/// </summary>
	void RemoveAll();
	
	/// <summary>
	/// Returns list of children. Note: This is slow because it will create wrapper objects every call.
	/// </summary>
	__property ISceneNode* get_Children() [];

	/// <summary>
	/// Removes this scene node from the scene, deleting it.
	/// </summary>
	void Remove();

	/// <summary>
	/// Adds an animator which should animate this node.
	/// </summary>
	void AddAnimator(ISceneNodeAnimator* animator);

	/// <summary>
	/// Removes an animator from this scene node.
	/// </summary>
	void RemoveAnimator(ISceneNodeAnimator* animator);

	/// <summary>
	/// Removes all animators from this scene node.
	/// </summary>
	void RemoveAnimators();
	
	/// <summary>
	/// Returns the material based on the zero based index i. To get the amount
	/// of materials used by this scene node, use getMaterialCount().
	/// This function is needed for inserting the node into the scene hirachy on a
	/// optimal position for minimizing renderstate changes, but can also be used
	/// to directly modify the material of a scene node.
	/// </summary>
	/// <param name="i"> Zero based index i. The maximal value for this may be MaterialCount - 1.</param>
	/// <returns> Returns the material of that index.</returns>
	Video::Material GetMaterial(int i);

	/// <summary>
	/// Returns the material based on the zero based index i. To get the amount
	/// of materials used by this scene node, use getMaterialCount().
	/// This function is needed for inserting the node into the scene hirachy on a
	/// optimal position for minimizing renderstate changes, but can also be used
	/// to directly modify the material of a scene node.
	/// </summary>
	/// <param name="i"> Zero based index i. The maximal value for this may be MaterialCount - 1.</summary>
	/// <returns> Returns the material of that index.</returns>
	void SetMaterial(int i, Video::Material);

	/// <summary>
	/// Returns amount of materials used by this scene node.
	/// </summary>
	/// <returns> Returns current count of materials used by this scene node.</returns>
	__property int get_MaterialCount();


	/// <summary>
	/// Sets all material flags at once to a new value. Helpful for
	/// example, if you want to be the the whole mesh to be lighted by 
	/// </summary>
	/// <param name="flag"> Which flag of all materials to be set.</param>
	/// <param name="newvalue"> New value of the flag.</param>
	void SetMaterialFlag(Video::MaterialFlag flag, bool newvalue);	

	/// <summary>
	/// Sets the texture of the specified layer in all materials of this
	/// scene node to the new texture.
	/// </summary>
	/// <param name="textureLayer"> Layer of texture to be set. Must be a value greater or
	/// equal than 0 and smaller than MATERIAL_MAX_TEXTURES.</param>
	/// <param name="texture"> Texture to be used.</param>
	void SetMaterialTexture(int textureLayer, Video::ITexture* texture);

	/// <summary>
	/// Sets the material type of all materials s32 this scene node
	/// to a new material type.
	/// </summary>
	/// <param name="newType"> New type of material to be set.</param>
	void SetMaterialType(Video::MaterialType newType);

	/// <summary>
	/// Sets or gets the scale of the scene node. 
	/// </summary>
	__property Core::Vector3D get_Scale();

	/// <summary>
	/// Sets or gets the scale of the scene node. 
	/// </summary>
	__property void set_Scale(Core::Vector3D scale);

	/// <summary>
	/// Sets or gets the rotation of the node. Note that this is
	/// the relative rotation of the node.
	/// </summary>
	__property Core::Vector3D get_Rotation();

	/// <summary>
	/// Sets or gets the rotation of the node. Note that this is
	/// the relative rotation of the node.
	/// </summary>
	__property void set_Rotation(Core::Vector3D s);

	/// <summary>
	/// Sets or gets the position of the node. Note that the position is
	/// relative to the parent.
	/// </summary>
	__property Core::Vector3D get_Position();

	/// <summary>
	/// Sets or gets the position of the node. Note that the position is
	/// relative to the parent.
	/// </summary>
	__property void set_Position(Core::Vector3D p);

	/// <summary>
	/// Gets the abolute position of the node. The position is absolute.
	/// </summary>
	__property Core::Vector3D get_AbsolutePosition();

	/// <summary>
	/// Sets or gets if automatic culling based on the bounding
	/// box is enabled. Automatic culling is enabled by default. Note that not
	/// all SceneNodes support culling (the billboard scene node for example)
	/// and that some nodes always cull their geometry because it is their
	/// only reason for existance, for example the OctreeSceneNode.
	/// </summary>
	__property void set_AutomaticCulling(bool enabled);

	/// <summary>
	/// Sets or gets if automatic culling based on the bounding
	/// box is enabled. Automatic culling is enabled by default. Note that not
	/// all SceneNodes support culling (the billboard scene node for example)
	/// and that some nodes always cull their geometry because it is their
	/// only reason for existance, for example the OctreeSceneNode.
	/// </summary>
	__property bool get_AutomaticCulling();

	/// <summary>
	/// Gets or sets if debug data like bounding boxes should be drawn.
	/// Please note that not all scene nodes support this feature.
	/// </summary>
	__property void set_DebugDataVisible(bool visible);

	/// <summary>
	/// Gets or sets if debug data like bounding boxes should be drawn.
	/// Please note that not all scene nodes support this feature.
	/// </summary>
	__property bool get_DebugDataVisible();

	/// <summary>
	/// Sets or gets if this scene node is a debug object. Debug objects have some special properties,
	/// for example they can be easily excluded from collision detection or from serialization, etc.
	/// </summary>
	__property void set_IsDebugObject(bool debugObject);

	/// <summary>
	/// Sets or gets if this scene node is a debug object. Debug objects have some special properties,
	/// for example they can be easily excluded from collision detection or from serialization, etc.
	/// </summary>
	__property bool get_IsDebugObject();

	/// <summary>
	/// Returns a const reference to the list of all children.
	/// </summary>
	//const core::list<ISceneNode*>& getChildren() const

	/// <summary>
	/// Changes the parent of the scene node.
	/// </summary>
	void SetParent(ISceneNode* newParent);

	/// <summary>
	/// Gets or sets the triangle selector attached to this scene node.
	/// The Selector can be used by the engine for doing collision
	/// detection. You can create a TriangleSelector with 
	/// ISceneManager::createTriangleSelector() or 
	/// ISceneManager::createOctTreeTriangleSelector and set it with
	/// ISceneNode::setTriangleSelector(). If a scene node got no triangle
	/// selector, but collision tests should be done with it, a triangle
	/// selector is created using the bounding box of the scene node.
	/// </summary>
	//__property ITriangleSelector* get_TriangleSelector();

	/// <summary>
	/// Gets or sets the triangle selector attached to this scene node.
	/// The Selector can be used by the engine for doing collision
	/// detection. You can create a TriangleSelector with 
	/// ISceneManager::createTriangleSelector() or 
	/// ISceneManager::createOctTreeTriangleSelector and set it with
	/// ISceneNode::setTriangleSelector(). If a scene node got no triangle
	/// selector, but collision tests should be done with it, a triangle
	/// selector is created using the bounding box of the scene node.
	/// </summary>
	//__property void set_TriangleSelector(ITriangleSelector* tri);

	/// <summary>
	/// updates the absolute position based on the relative and the parents position
	/// </summary>
	void UpdateAbsolutePosition();

	/// <summary>
	/// Returns the internal pointer to the native C++ irrlicht scene node.
	/// Do not use this, only needed by the internal .NET wrapper.
	///</summary>
	__property irr::scene::ISceneNode* get_NativeSceneNode();

	/// <summary>
	/// Returns the type of the scene node
	///</summary>
	__property SceneNodeType get_SceneNodeType();
	
protected:

	irr::scene::ISceneNode* SceneNode;
};


}
}