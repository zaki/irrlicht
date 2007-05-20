// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#include "ISceneNode.h"
#include "IAnimatedMeshMD2.h"

namespace Irrlicht
{
namespace Scene
{

/// <summary> 
/// Scene node capable of displaying an animated mesh and its shadow.
/// The shadow is optional: If a shadow should be displayed too, just invoke
///	the IAnimatedMeshSceneNode::createShadowVolumeSceneNode().
/// </summary>
public __gc class IAnimatedMeshSceneNode : public ISceneNode
{
public:

	/// <summary>
	/// Creates a scene node from a native C++ scene node. Don't use this, its better to create 
	/// scene nodes using the SceneManager with its addSceneNode() methods.
	/// </summary>
	IAnimatedMeshSceneNode(irr::scene::IAnimatedMeshSceneNode* realSceneNode);

	/// <summary>
	/// Sets or gets the current frame number. 
	/// From now on the animation is played from this frame. 
	/// </summary>
	/// <param name="frame"/> Number of the frame to let the animation be started from.
	/// The frame number must be a valid frame number of the IMesh used by this 
	/// scene node. Set IAnimatedMesh::getMesh() for details.</param>
	__property void set_CurrentFrame(int frame);

	/// <summary>
	/// Sets or gets the current frame number. 
	/// From now on the animation is played from this frame. 
	/// </summary>
	__property int get_CurrentFrame();

	/// <summary>
	/// Sets the frame numbers between the animation is looped.
	/// The default is 0 - MaximalFrameCount of the mesh.
	/// </summary>
	/// <param name="begin"/> Start frame number of the loop.</param>
	/// <param name="end"/> End frame number of the loop.</param>
	/// <returns> Returns true if successful, false if not.</returns>
	bool SetFrameLoop(int begin, int end);

	/// <summary>
	/// Sets the speed with witch the animation is played.
	/// </summary>
	/// <param name="framesPerSecond"/> Frames per second played.</param>
	__property void set_AnimationSpeed(float framesPerSecond);

	/// <summary>
	/// Creates shadow volume scene node as child of this node
	/// and returns a pointer to it.  The shadow can be rendered using the ZPass
	/// or the zfail method. ZPass is a little bit faster because the shadow volume
	/// creation is easier, but with this method there occur ugly looking artifacs
	/// when the camera is inside the shadow volume. These error do not occur
	/// with the ZFail method.
	/// </summary>
	/// <param name="id"/> Id of the shadow scene node. This id can be used to identify
	/// the node later.</param>
	/// <param name="zfailmethod"/> If set to true, the shadow will use the zfail method,
	/// if not, zpass is used. Default: true </param>
	/// <param name="intinity"/> Value used by the shadow volume algorithm to scale the 
	/// shadow volume. Default: 10000</param>
	/// <returns> Returns pointer to the created shadow scene node.
	/// </returns>
	ISceneNode* AddShadowVolumeSceneNode(int id, bool zfailmethod, float infinity);

	/// <summary>
	/// Creates shadow volume scene node as child of this node
	/// and returns a pointer to it.  The shadow can be rendered using the ZPass
	/// or the zfail method. ZPass is a little bit faster because the shadow volume
	/// creation is easier, but with this method there occur ugly looking artifacs
	/// when the camera is inside the shadow volume. These error do not occur
	/// with the ZFail method.
	/// <returns> Returns pointer to the created shadow scene node.
	/// </returns>
	ISceneNode* AddShadowVolumeSceneNode();

	/// <summary>
	/// Returns a pointer to a child node, wich has the same transformation as 
	/// the corrsesponding joint, if the mesh in this scene node is a ms3d mesh.
	/// Otherwise 0 is returned. With this method it is possible to
	/// attach scene nodes to joints more easily. In this way, it is
	/// for example possible to attach a weapon to the left hand of an
	/// animated model. This example shows how:
	/// \code
	/// ISceneNode* hand = 
	///		yourMS3DAnimatedMeshSceneNode->getMS3DJointNode("LeftHand");
	/// hand->addChild(weaponSceneNode);
	/// \endcode
	/// Please note that the SceneNode returned by this method may not exist
	/// before this call and is created by it.
	/// </summary>
	/// <param name="jointName"/> Name of the joint.</param>
	/// <returns>Returns a pointer to the scene node which represents the joint
	/// with the specified name. Returns 0 if the contained mesh is not an
	/// ms3d mesh or the name of the joint could not be found.</returns>
	ISceneNode* GetMS3DJointNode(System::String* jointName);

	/// <summary>
	/// Starts a default MD2 animation. 
	/// With this method it is easily possible to start a Run, Attack,
	/// Die or whatever animation, if the mesh contained in this scene
	/// node is a md2 mesh. Otherwise, nothing happenes.
	/// </summary>
	/// <param name="anim"/> An MD2 animation type, which should be played, for
	/// example EMAT_STAND for the standing animation.</param>
	/// <returns> Returns true if successful, and false if not, for example
	/// if the mesh in the scene node is not a md2 mesh.</returns>
	bool SetMD2Animation(MD2AnimationType anim);

	/// <summary>
	/// Starts a special MD2 animation. 
	/// With this method it is easily possible to start a Run, Attack,
	/// Die or whatever animation, if the mesh contained in this scene
	/// node is a md2 mesh. Otherwise, nothing happenes. This method uses
	/// a character string to identify the animation. If the animation is a
	/// standard md2 animation, you might want to start this animation
	/// with the MD2_ANIMATION_TYPE enumeraition instead.
	/// </summary>
	/// <param name="animationName"/> Name of the animation which should be played.</param>
    /// <returns> Returns true if successful, and false if not, for example
	/// if the mesh in the scene node is not a md2 mesh, or no animation
	/// with this name could be found.</returns>
	bool SetMD2Animation(System::String* animationName);

protected:

	inline irr::scene::IAnimatedMeshSceneNode* getAnimatedMeshSceneNode()
	{
		return (irr::scene::IAnimatedMeshSceneNode*)SceneNode;
	}
};

}
}
