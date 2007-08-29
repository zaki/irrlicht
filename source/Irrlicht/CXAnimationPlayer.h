// Copyright (C) 2002-2007 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_X_ANIMATION_PLAYER_H_INCLUDED__
#define __C_X_ANIMATION_PLAYER_H_INCLUDED__

#include "IAnimatedMeshX.h"
#include "CXFileReader.h"
#include "SMesh.h"
#include "SMeshBuffer.h"

namespace irr
{
namespace video
{
	class IVideoDriver;
}
namespace scene
{
	class IMeshManipulator;

	class CXAnimationPlayer : public IAnimatedMeshX
	{
	public:

		//! constructor
		CXAnimationPlayer(CXFileReader* reader, 
			video::IVideoDriver* driver, 
			IMeshManipulator* manip,
			const c8* filename);

		//! destructor
		virtual ~CXAnimationPlayer();

		//! Gets the frame count of the animated mesh.
		virtual s32 getFrameCount();

		//! Returns the IMesh interface for a frame.
		virtual IMesh* getMesh(s32 frame, s32 detailLevel=255, s32 startFrameLoop=-1, s32 endFrameLoop=-1);

		//! Returns an axis aligned bounding box of the mesh.
		virtual const core::aabbox3d<f32>& getBoundingBox() const;

		//! Returns the type of the animated mesh.
		virtual E_ANIMATED_MESH_TYPE getMeshType() const;

		//! Returns a pointer to a transformation matrix
		virtual core::matrix4* getMatrixOfJoint(s32 jointNumber, s32 frame);

		//! Gets joint count.
		virtual s32 getJointCount() const;

		//! Gets the name of a joint.
		virtual const c8* getJointName(s32 number) const;

		//! Gets a joint number from its name
		virtual s32 getJointNumber(const c8* name) const;

		//! Returns a pointer to list of points containing the skeleton.
		virtual const core::array<core::vector3df>* getDrawableSkeleton(s32 frame);

		//! Returns amount of animations in .X-file.
		virtual s32 getAnimationCount() const;

		//! Returns the name of an animation. 
		virtual const c8* getAnimationName(s32 idx) const;

		//! Sets an animation as animation to play back.
		virtual void setCurrentAnimation(s32 idx);

		//! Sets an animation as animation to play back.
		virtual bool setCurrentAnimation(const c8* name);

	private:

		struct SWeightData
		{
			SWeightData() {};

			SWeightData(const SWeightData& other)
				: buffer(other.buffer), vertex(other.vertex), weight(other.weight) 
			{}

			SWeightData(s32 b, s32 v, f32 w)
				: buffer(b), vertex(v), weight(w)
			{}

			s32 buffer;
			s32 vertex;
			f32 weight;

			bool operator <(const SWeightData& other) const
			{
				return (buffer < other.buffer ||
						(buffer == other.buffer && 
						vertex < other.vertex) ||
						(buffer == other.buffer && 
						vertex == other.vertex &&
						weight < other.weight));
			}
		};

		struct SVertexWeight
		{
			SVertexWeight() : weightCount(0) {}

			// weight per vertex, at maximum, 4 joints per vertex
			f32 weight[4];
			s32 joint[4];

			s32 weightCount;

			void add(f32 _weight, s32 _joint)
			{
				if (weightCount == 4)
					return;

				weight[weightCount] = _weight;
				joint[weightCount] = _joint;
				++weightCount;
			}
		};

		struct SJoint
		{
			s32 Parent; // index of parent
			core::array<SWeightData> Weights;
			core::matrix4 MatrixOffset;
			core::matrix4 LocalMatrix;
			core::matrix4 GlobalMatrix;
			core::matrix4 AnimatedMatrix;
			core::matrix4 LocalAnimatedMatrix;
			core::matrix4 CombinedAnimationMatrix; // for faster computing
			core::stringc Name;
			bool IsVirtualJoint; // for in .x file not weighted vertices
			bool WasAnimatedThisFrame; // used by animateSkeleton()
		};

		void createAnimationData();
		void createJointData(const CXFileReader::SXFrame& f, s32 JointParent);
		void createMeshData();
		void addFrameToMesh(CXFileReader::SXFrame& frame);
		video::SMaterial getMaterialFromXMaterial(const CXFileReader::SXMaterial& xmat);
		void addFacesToBuffer(s32 meshbuffernr, CXFileReader::SXMesh& mesh, s32 matnr, const CXFileReader::SXFrame& frame);
		//! use .x file path as prefix for texture
		core::stringc getTextureFileName(const core::stringc& texture);

		s32 getJointNumberFromName(const core::stringc& name) const;

		//! prepares animation data which was read in from the .x file
		void prepareAnimationData();

		//! animates the skeleton based on the animation data
		void animateSkeleton();

		//! modifies the skin based on the animated skeleton
		void modifySkin();

		void updateBoundingBoxFromAnimation();

		void addVirtualWeight(s32 meshbuffernr, s32 vtxidx, CXFileReader::SXMesh& mesh,
			const CXFileReader::SXFrame& frame);

		CXFileReader* Reader;
		video::IVideoDriver* Driver;
		scene::SMesh *OriginalMesh;
		scene::SMesh *AnimatedMesh;
		core::aabbox3df Box;
		core::stringc FileName;
		IMeshManipulator* Manipulator;
		core::array<core::vector3df> DebugSkeleton;
		bool IsAnimatedSkinnedMesh;
		
		core::array<SJoint> Joints;

		f32 CurrentAnimationTime;
		f32 LastAnimationTime;
		s32 CurrentAnimationSet;
		f32 DebugSkeletonCrossSize;

		// one array of weights per mesh buffer
		core::array< core::array<SVertexWeight> > Weights; 

		// data for animations

		struct SXAnimationTrack
		{
			s32 jointNr;
			s32 keyType; // 0=rotation, 1=scale, 2=position, 3=matrix

			core::array<core::quaternion> Quaternions;
			core::array<core::vector3df> Vectors;
			core::array<core::matrix4> Matrices;
			core::array<f32> Times;

			bool operator <(SXAnimationTrack& other) const
			{
				if (jointNr != other.jointNr)
					return (jointNr < other.jointNr);

				return keyType > other.keyType;
			}
		};

		struct SXAnimationSet
		{
			core::stringc AnimationName;
			core::array<SXAnimationTrack> Animations;
		};

		core::array<SXAnimationSet> AnimationSets;
	};

} // end namespace scene
} // end namespace irr

#endif

