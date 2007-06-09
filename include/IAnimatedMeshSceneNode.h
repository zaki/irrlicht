// Copyright (C) 2002-2007 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_ANIMATED_MESH_SCENE_NODE_H_INCLUDED__
#define __I_ANIMATED_MESH_SCENE_NODE_H_INCLUDED__

#include "ISceneNode.h"
#include "IAnimatedMeshMD2.h"
#include "IAnimatedMeshMD3.h"
#include "IShadowVolumeSceneNode.h"

namespace irr
{
namespace scene
{
	class IAnimatedMeshSceneNode;

	//! Callback interface for catching events of ended animations.
	/** Implement this interface and use
	 IAnimatedMeshSceneNode::setAnimationEndCallback to be able to
	 be notified if an animation playback has ended.
	**/
	class IAnimationEndCallBack : public virtual IUnknown
	{
	public:

		//! Will be called when the animation playback has ended.
		//! See IAnimatedMeshSceneNode::setAnimationEndCallback for
		//! more informations.
		//! \param node: Node of which the animation has ended.
		virtual void OnAnimationEnd(IAnimatedMeshSceneNode* node) = 0;
	};

	//! Scene node capable of displaying an animated mesh and its shadow.
	/** The shadow is optional: If a shadow should be displayed too, just invoke
	the IAnimatedMeshSceneNode::createShadowVolumeSceneNode().*/
	class IAnimatedMeshSceneNode : public ISceneNode
	{
	public:

		//! Constructor
		IAnimatedMeshSceneNode(ISceneNode* parent, ISceneManager* mgr, s32 id,
			const core::vector3df& position = core::vector3df(0,0,0),
			const core::vector3df& rotation = core::vector3df(0,0,0),
			const core::vector3df& scale = core::vector3df(1.0f, 1.0f, 1.0f))
			: ISceneNode(parent, mgr, id, position, rotation, scale) {}

		//! Destructor
		virtual ~IAnimatedMeshSceneNode() {};

		//! Sets the current frame number.
		//! From now on the animation is played from this frame.
		//! \param frame: Number of the frame to let the animation be started from.
		//! The frame number must be a valid frame number of the IMesh used by this
		//! scene node. Set IAnimatedMesh::getMesh() for details.
		virtual void setCurrentFrame(s32 frame) = 0;

		//! Sets the frame numbers between the animation is looped.
		//! The default is 0 - MaximalFrameCount of the mesh.
		//! \param begin: Start frame number of the loop.
		//! \param end: End frame number of the loop.
		//! \return Returns true if successful, false if not.
		virtual bool setFrameLoop(s32 begin, s32 end) = 0;

		//! Sets the speed with witch the animation is played.
		//! \param framesPerSecond: Frames per second played.
		virtual void setAnimationSpeed(f32 framesPerSecond) = 0;

		//! Creates shadow volume scene node as child of this node
		//! and returns a pointer to it.  The shadow can be rendered using the ZPass
		//! or the zfail method. ZPass is a little bit faster because the shadow volume
		//! creation is easier, but with this method there occur ugly looking artifacs
		//! when the camera is inside the shadow volume. These error do not occur
		//! with the ZFail method.
		//! \param id: Id of the shadow scene node. This id can be used to identify
		//! the node later.
		//! \param zfailmethod: If set to true, the shadow will use the zfail method,
		//! if not, zpass is used.
		//! \param infinity: Value used by the shadow volume algorithm to scale the
		//! shadow volume.
		//! \return Returns pointer to the created shadow scene node.
		//! This pointer should not be dropped. See IUnknown::drop() for more information.
		virtual IShadowVolumeSceneNode* addShadowVolumeSceneNode(s32 id=-1,
			bool zfailmethod=true, f32 infinity=10000.0f) = 0;

		//! Returns a pointer to a child node, which has the same transformation as
		//! the corresponding joint, if the mesh in this scene node is a ms3d mesh.
		//! Otherwise 0 is returned. With this method it is possible to
		//! attach scene nodes to joints more easily. In this way, it is
		//! for example possible to attach a weapon to the left hand of an
		//! animated model. This example shows how:
		//! \code
		//! ISceneNode* hand =
		//!		yourMS3DAnimatedMeshSceneNode->getMS3DJointNode("LeftHand");
		//! hand->addChild(weaponSceneNode);
		//! \endcode
		//! Please note that the SceneNode returned by this method may not exist
		//! before this call and is created by it.
		//! \param jointName: Name of the joint.
		//! \return Returns a pointer to the scene node which represents the joint
		//! with the specified name. Returns 0 if the contained mesh is not an
		//! ms3d mesh or the name of the joint could not be found.
		virtual ISceneNode* getMS3DJointNode(const c8* jointName) = 0;

		//! Returns a pointer to a child node, which has the same transformation as
		//! the corresponding joint, if the mesh in this scene node is a x mesh.
		//! Otherwise 0 is returned. With this method it is possible to
		//! attach scene nodes to joints more easily. In this way, it is
		//! for example possible to attach a weapon to the left hand of an
		//! animated model. This example shows how:
		//! \code
		//! ISceneNode* hand =
		//!		yourMS3DAnimatedMeshSceneNode->getXJointNode("LeftHand");
		//! hand->addChild(weaponSceneNode);
		//! \endcode
		//! Please note that the SceneNode returned by this method may not exist
		//! before this call and is created by it.
		//! \param jointName: Name of the joint.
		//! \return Returns a pointer to the scene node which represents the joint
		//! with the specified name. Returns 0 if the contained mesh is not an
		//! ms3d mesh or the name of the joint could not be found.
		virtual ISceneNode* getXJointNode(const c8* jointName) = 0;

		//! Returns a pointer to a child node, which has the same transformation as
		//! the corresponding joint, if the mesh in this scene node is a b3d mesh.
		//! Otherwise 0 is returned. With this method it is possible to
		//! attach scene nodes to joints more easily. In this way, it is
		//! for example possible to attach a weapon to the left hand of an
		//! animated model. This example shows how:
		//! \code
		//! ISceneNode* hand =
		//!		yourB3DAnimatedMeshSceneNode->getB3DJointNode("LeftHand");
		//! hand->addChild(weaponSceneNode);
		//! \endcode
		//! Please note that the SceneNode returned by this method may not exist
		//! before this call and is created by it.
		//! \param jointName: Name of the joint.
		//! \return Returns a pointer to the scene node which represents the joint
		//! with the specified name. Returns 0 if the contained mesh is not an
		//! ms3d mesh or the name of the joint could not be found.
		virtual ISceneNode* getB3DJointNode(const c8* jointName) = 0;

		//! Starts a default MD2 animation.
		//! With this method it is easily possible to start a Run, Attack,
		//! Die or whatever animation, if the mesh contained in this scene
		//! node is an md2 mesh. Otherwise, nothing happens.
		//! \param anim: An MD2 animation type, which should be played, for
		//! example EMAT_STAND for the standing animation.
		//! \return Returns true if successful, and false if not, for example
		//! if the mesh in the scene node is not a md2 mesh.
		virtual bool setMD2Animation(EMD2_ANIMATION_TYPE anim) = 0;

		//! Starts a special MD2 animation.
		//! With this method it is easily possible to start a Run, Attack,
		//! Die or whatever animation, if the mesh contained in this scene
		//! node is an md2 mesh. Otherwise, nothing happens. This method uses
		//! a character string to identify the animation. If the animation is a
		//! standard md2 animation, you might want to start this animation
		//! with the EMD2_ANIMATION_TYPE enumeration instead.
		//! \param animationName: Name of the animation which should be played.
		//! \return Returns true if successful, and false if not, for example
		//! if the mesh in the scene node is not an md2 mesh, or no animation
		//! with this name could be found.
		virtual bool setMD2Animation(const c8* animationName) = 0;

		//! Returns the current displayed frame number.
		virtual s32 getFrameNr() const = 0;
		//! Returns the current start frame number.
		virtual s32 getStartFrame() const = 0;
		//! Returns the current end frame number.
		virtual s32 getEndFrame() const = 0;

		//! Sets looping mode which is on by default. If set to false,
		//! animations will not be played looped.
		virtual void setLoopMode(bool playAnimationLooped) = 0;

		//! Sets a callback interface which will be called if an animation
		//! playback has ended. Set this to 0 to disable the callback again.
		//! Please note that this will only be called when in non looped mode,
		//! see IAnimatedMeshSceneNode::setLoopMode().
		virtual void setAnimationEndCallback(IAnimationEndCallBack* callback=0) = 0;

		//! Sets if the scene node should not copy the materials of the mesh but use them in a read only style.
		/* In this way it is possible to change the materials a mesh causing all mesh scene nodes
		referencing this mesh to change too. */
		virtual void setReadOnlyMaterials(bool readonly) = 0;

		//! Returns if the scene node should not copy the materials of the mesh but use them in a read only style
		virtual bool isReadOnlyMaterials() = 0;

		//! Sets a new mesh
		virtual void setMesh(IAnimatedMesh* mesh) = 0;

		//! Returns the current mesh
		virtual IAnimatedMesh* getMesh(void) = 0;

		// returns the absolute transformation for a special MD3 Tag if the mesh is a md3 mesh,
		// or the absolutetransformation if it's a normal scenenode
		virtual const SMD3QuaterionTag& getMD3TagTransformation( const core::stringc & tagname) = 0;

	};

} // end namespace scene
} // end namespace irr

#endif

