// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_SKINNED_MESH_H_INCLUDED__
#define __I_SKINNED_MESH_H_INCLUDED__

#include "irrArray.h"
#include "IBoneSceneNode.h"
#include "IAnimatedMesh.h"
#include "SSkinMeshBuffer.h"

namespace irr
{
namespace scene
{

	enum E_INTERPOLATION_MODE
	{
		// constant interpolation
		EIM_CONSTANT = 0,

		// linear interpolation
		EIM_LINEAR,

		//! count of all available interpolation modes
		EIM_COUNT
	};


	//! Interface for using some special functions of Skinned meshes
	class ISkinnedMesh : public IAnimatedMesh
	{
	public:

		//! Gets joint count.
		//! \return Returns amount of joints in the skeletal animated mesh.
		virtual s32 getJointCount() const = 0;

		//! Gets the name of a joint.
		//! \param number: Zero based index of joint. The last joint has the number
		//! IAnimatedMeshB3d::getJointCount()-1;
		//! \return Returns name of joint and null if an error happened.
		virtual const c8* getJointName(s32 number) const = 0;

		//! Gets a joint number from its name
		//! \param name: Name of the joint.
		//! \return Returns the number of the joint or -1 if not found.
		virtual s32 getJointNumber(const c8* name) const = 0;

		//! uses animation from another mesh
		//! the animation is linked (not copied) based on joint names (so make sure they are unique)
		//! \return Returns true if all joints in this mesh were matched up (empty names will not be matched, and it's case sensitive)
		//! unmatched joints will not be animated
		virtual bool useAnimationFrom(ISkinnedMesh *mesh) = 0;

		//!Update Normals when Animating
		//!False= Don't (default)
		//!True= Update normals, slower
		virtual void updateNormalsWhenAnimating(bool on) = 0;

		//!Sets Interpolation Mode
		virtual void setInterpolationMode(E_INTERPOLATION_MODE mode) = 0;

		//! Animates this mesh's joints based on frame input
		virtual void animateMesh(f32 frame, f32 blend)=0;

		//! Preforms a software skin on this mesh based of joint positions
		virtual void skinMesh() = 0;

		//!Recovers the joints from the mesh
		virtual void recoverJointsFromMesh(core::array<IBoneSceneNode*> &JointChildSceneNodes) = 0;

		//!Tranfers the joint data to the mesh
		virtual void tranferJointsToMesh(core::array<IBoneSceneNode*> &JointChildSceneNodes) = 0;

		//!Creates an array of joints from this mesh
		virtual void createJoints(core::array<IBoneSceneNode*> &JointChildSceneNodes,
			IAnimatedMeshSceneNode* AnimatedMeshSceneNode, ISceneManager* SceneManager) = 0;

		virtual void convertMeshToTangents() = 0;

		//! A vertex weight
		struct SWeight
		{
			//! Index of the mesh buffer
			u16 buffer_id; //I doubt 32bits is needed

			//! Index of the vertex
			u32 vertex_id; //Store global ID here

			//! Weight Strength/Percentage (0-1)
			f32 strength;

		private:
			//! Internal members used by CSkinnedMesh
			friend class CSkinnedMesh;
			bool *Moved;
			core::vector3df StaticPos;
			core::vector3df StaticNormal;
		};


		//! Animation keyframe which describes a new position, scale or rotation
		struct SPositionKey
		{
			f32 frame;
			core::vector3df position;
		};

		struct SScaleKey
		{
			f32 frame;
			core::vector3df scale;
		};

		struct SRotationKey
		{
			f32 frame;
			core::quaternion rotation;
		};

		//! Joints
	  	struct SJoint
		{
			SJoint() :
				Name(""), LocalMatrix(),
				Children(), PositionKeys(), ScaleKeys(), RotationKeys(), Weights(),
				UseAnimationFrom(0), LocalAnimatedMatrix_Animated(false),positionHint(-1),scaleHint(-1),rotationHint(-1)
			{
			}

			//! The name of this joint
			core::stringc Name;

			//! Local matrix of this joint
			core::matrix4 LocalMatrix;

			//! List of child joints
			core::array<SJoint*> Children;

			//! Animation keys causing translation change
			core::array<SPositionKey> PositionKeys;

			//! Animation keys causing scale change
			core::array<SScaleKey> ScaleKeys;

			//! Animation keys causing rotation change
			core::array<SRotationKey> RotationKeys;

			//! Skin weights
			core::array<SWeight> Weights;

			//! Unnecessary for loaders, will be overwritten on finalize
			core::matrix4 GlobalMatrix;
			core::matrix4 GlobalAnimatedMatrix;
			core::matrix4 LocalAnimatedMatrix;
			core::vector3df Animatedposition;
			core::vector3df Animatedscale;
			core::quaternion Animatedrotation;

			core::matrix4 GlobalInversedMatrix; //the x format pre-calculates this

		private:
			//! Internal members used by CSkinnedMesh
			friend class CSkinnedMesh;

			SJoint *UseAnimationFrom;
			bool LocalAnimatedMatrix_Animated;

			s32 positionHint;
			s32 scaleHint;
			s32 rotationHint;

		};






		//Interface for the mesh loaders (finalize should lock these functions, and they should have some prefix like loader_

		//these functions will use the needed arrays, set vaules, etc to help the loaders

		//! exposed for loaders: to add mesh buffers
		virtual core::array<SSkinMeshBuffer*> &getMeshBuffers() = 0;

		//! exposed for loaders: joints list
		virtual core::array<SJoint*> &getAllJoints() = 0;

		//! loaders should call this after populating the mesh
		virtual void finalize() = 0;




		virtual SSkinMeshBuffer *createBuffer() = 0;

		virtual SJoint *createJoint(SJoint *parent=0) = 0;

		virtual SPositionKey *createPositionKey(SJoint *joint) = 0;
		virtual SScaleKey *createScaleKey(SJoint *joint) = 0;
		virtual SRotationKey *createRotationKey(SJoint *joint) = 0;

		virtual SWeight *createWeight(SJoint *joint) = 0;
	};

} // end namespace scene
} // end namespace irr

#endif



