// Copyright (C) 2002-2007 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_ANIMATED_MESH_MS3D_H_INCLUDED__
#define __C_ANIMATED_MESH_MS3D_H_INCLUDED__

#include "IAnimatedMeshMS3D.h"
#include "IMesh.h"
#include "IReadFile.h"
#include "S3DVertex.h"
#include "irrString.h"
#include "SMeshBuffer.h"

namespace irr
{
namespace video
{
	class IVideoDriver;
} // end namespace video
namespace scene
{

	class CAnimatedMeshMS3D : public IAnimatedMeshMS3D, public IMesh
	{
	public:

		//! constructor
		CAnimatedMeshMS3D(video::IVideoDriver* driver);

		//! destructor
		virtual ~CAnimatedMeshMS3D();

		//! loads an md2 file
		virtual bool loadFile(io::IReadFile* file);

		//! returns the amount of frames in milliseconds. If the amount is 1, it is a static (=non animated) mesh.
		virtual s32 getFrameCount();

		//! returns the animated mesh based on a detail level. 0 is the lowest, 255 the highest detail. Note, that some Meshes will ignore the detail level.
		virtual IMesh* getMesh(s32 frame, s32 detailLevel=255, s32 startFrameLoop=-1, s32 endFrameLoop=-1);

		//! returns amount of mesh buffers.
		virtual u32 getMeshBufferCount() const;

		//! returns pointer to a mesh buffer
		virtual IMeshBuffer* getMeshBuffer(u32 nr) const;

		//! Returns pointer to a mesh buffer which fits a material
 		/** \param material: material to search for
		\return Returns the pointer to the mesh buffer or 
		NULL if there is no such mesh buffer. */
		virtual IMeshBuffer* getMeshBuffer( const video::SMaterial &material) const;

		//! returns an axis aligned bounding box
		virtual const core::aabbox3d<f32>& getBoundingBox() const;

		//! set user axis aligned bounding box
		virtual void setBoundingBox( const core::aabbox3df& box);

		//! sets a flag of all contained materials to a new value
		virtual void setMaterialFlag(video::E_MATERIAL_FLAG flag, bool newvalue);

		//! Returns the type of the animated mesh.
		virtual E_ANIMATED_MESH_TYPE getMeshType() const;

		//! Returns a pointer to a transformation matrix of a part of the 
		//! mesh based on a frame time.
		virtual core::matrix4* getMatrixOfJoint(s32 jointNumber, s32 frame);

		//! Gets joint count.
		virtual s32 getJointCount() const;

		//! Gets the name of a joint.
		virtual const c8* getJointName(s32 number) const;

		//! Gets a joint number from its name
		virtual s32 getJointNumber(const c8* name) const;

	private:
	
		struct SKeyframe
		{
			f32 timeindex;
			core::vector3df data; // translation or rotation
		};
	
		struct SJoint
		{
			core::stringc Name;
			s32 Index;
			core::vector3df Rotation;
			core::vector3df Translation;
			
			core::matrix4 RelativeTransformation;
			core::matrix4 AbsoluteTransformation;
			core::matrix4 AbsoluteTransformationAnimated;
			
			core::array<SKeyframe> TranslationKeys;
			core::array<SKeyframe> RotationKeys;
			core::array<u16> VertexIds;
			
			s32 Parent;
			core::stringc ParentName;
		};

		struct SGroup 
		{ 
			core::stringc Name; 
			core::array<u16> VertexIds; 

			u16 MaterialIdx; 
		}; 
		  
		//! Simple implementation of the IMeshBuffer interface with S3DVertex vertices. 
		struct SMS3DMeshBuffer : public IMeshBuffer 
		{ 
			//! constructor 
			SMS3DMeshBuffer();

			//! destructor 
			~SMS3DMeshBuffer();

			//! returns the material of this meshbuffer 
			virtual const video::SMaterial& getMaterial() const;

			//! returns the material of this meshbuffer 
			virtual video::SMaterial& getMaterial();

			//! returns pointer to vertices 
			virtual const void* getVertices() const;

			//! returns pointer to vertices 
			virtual void* getVertices();

			//! returns amount of vertices 
			virtual u32 getVertexCount() const;

			//! returns pointer to Indices 
			virtual const u16* getIndices() const;

			//! returns pointer to Indices 
			virtual u16* getIndices();

			//! returns amount of indices 
			virtual u32 getIndexCount() const;

			//! returns an axis aligned bounding box 
			virtual const core::aabbox3d<f32>& getBoundingBox() const;

			//! set user axis aligned bounding box
			virtual void setBoundingBox( const core::aabbox3df& box);

			//! returns which type of vertex data is stored. 
			virtual video::E_VERTEX_TYPE getVertexType() const;

			//! returns the byte size (stride, pitch) of the vertex
			virtual u32 getVertexPitch() const;


			video::SMaterial Material;                  //! material for this meshBuffer. 
			core::array<video::S3DVertex> *Vertices;      //! Array of vertices 
			core::array<u16> Indices;   //! Array of the Indices. 
			core::aabbox3d<f32> *BoundingBox; 
		}; 

		void animate(s32 frame);
		void getKeyframeData(const core::array<SKeyframe>& keys, f32 time, core::vector3df& outdata) const;
		void getKeyframeRotation(const core::array<SKeyframe>& keys, f32 time, core::vector3df& outdata) const;

		core::aabbox3d<f32> BoundingBox;

		core::array<video::S3DVertex> Vertices;
		core::array<video::S3DVertex> AnimatedVertices;
		core::array<u16> Indices;
		
		core::array<SJoint> Joints;
		core::array<SGroup> Groups;
		core::array<SMS3DMeshBuffer> Buffers;

		f32 totalTime;
		bool HasAnimation;
		s32 lastCalculatedFrame;

		video::IVideoDriver* Driver;
	};

} // end namespace scene
} // end namespace irr

#endif

