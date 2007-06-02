// Copyright (C) 2002-2007 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_ANIMATED_MESH_B3D_H_INCLUDED__
#define __I_ANIMATED_MESH_B3D_H_INCLUDED__

#include "IAnimatedMesh.h"
#include "irrArray.h"
#include "matrix4.h"

namespace irr
{
namespace scene
{
	class ISceneNode;
	class ISceneManager;

	//! Interface for using some special functions of B3d meshes
	/** Please note that the B3d Mesh's frame numbers are scaled by 100 */
	class IAnimatedMeshB3d : public IAnimatedMesh
	{
	public:

		//! Returns a pointer to a transformation matrix of a part of the
		//! mesh based on a frame time. This is used for being able to attach
		//! objects to parts of animated meshes. For example a weapon to an animated
		//! hand.
		//! \param jointNumber: Zero based index of joint. The last joint has the number
		//! IAnimatedMeshB3d::getJointCount()-1;
		//! \param frame: Frame of the animation.
		//! \return Returns a pointer to the matrix of the mesh part or
		//! null if an error occured.
		virtual core::matrix4* getMatrixOfJoint(s32 jointNumber, s32 frame) = 0;


		//! Returns a pointer to a local matrix of a Joint, can be used to control the animation
		virtual core::matrix4* getLocalMatrixOfJoint(s32 jointNumber) = 0;

		//! Returns a pointer to a matrix of a part of the mesh unanimated
		virtual core::matrix4* getMatrixOfJointUnanimated(s32 jointNumber) = 0;


		//! Move this Joint's local matrix when animating
		//! \param jointNumber: Zero based index of joint. The last joint has the number
		//! IAnimatedMeshB3d::getJointCount()-1;
		//! \param On: False= Leave joint's local matrix, True= Animate
		//! (not used yet)
		virtual void setJointAnimation(s32 jointNumber, bool On) = 0;


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

		//!Update Normals when Animating
		//!False= Don't (default)
		//!True= Update normals, slower
		virtual void updateNormalsWhenAnimating(bool on) = 0;


		//!Sets Interpolation Mode
		//!0- Constant
		//!1- Linear (default)
		virtual void setInterpolationMode(s32 mode) = 0;

		//!Want should happen on when animating
		//!0-Nothing
		//!1-Update nodes only
		//!2-Update skin only
		//!3-Update both nodes and skin (default)
		virtual void setAnimateMode(s32 mode) = 0;

		//!Convert all mesh buffers to use tangent vertices
		virtual void convertToTangents() =0;


		virtual void recoverJointsFromMesh(core::array<ISceneNode*> &JointChildSceneNodes)=0;
		virtual void tranferJointsToMesh(core::array<ISceneNode*> &JointChildSceneNodes)=0;
		virtual void createJoints(core::array<ISceneNode*> &JointChildSceneNodes, ISceneNode* AnimatedMeshSceneNode, ISceneManager* SceneManager)=0;

	};

} // end namespace scene
} // end namespace irr

#endif


