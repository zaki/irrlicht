// Copyright (C) 2002-2007 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h


//B3D file loader by Luke Hoschke, File format by Mark Sibly

#ifndef __C_ANIMATED_MESH_B3D_H_INCLUDED__
#define __C_ANIMATED_MESH_B3D_H_INCLUDED__

#include "IAnimatedMeshB3d.h"
#include "IMesh.h"
#include "IReadFile.h"

#include "SMeshBuffer.h"
#include "S3DVertex.h"

#include "irrString.h"
#include "matrix4.h"
#include "quaternion.h"

namespace irr
{
namespace video
{
	class IVideoDriver;
} // end namespace video
namespace scene
{
	class CAnimatedMeshB3d : public IAnimatedMeshB3d, public IMesh
	{
	public:

		//! constructor
		CAnimatedMeshB3d(video::IVideoDriver* driver);

		//! destructor
		virtual ~CAnimatedMeshB3d();

		//! loads an B3d file
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


		virtual core::matrix4* getLocalMatrixOfJoint(s32 jointNumber);

		virtual core::matrix4* getMatrixOfJointUnanimated(s32 jointNumber);

		virtual void setJointAnimation(s32 jointNumber, bool On);

		//!Update Normals when Animating
		//!False= Don't (default)
		//!True= Update normals, slower
		virtual void updateNormalsWhenAnimating(bool on);


		//!Sets Interpolation Mode
		//!0- Constant
		//!1- Linear (default)
		virtual void setInterpolationMode(s32 mode);

		//!Want should happen on when animating
		//!0-Nothing
		//!1-Update nodes only
		//!2-Update skin only
		//!3-Update both nodes and skin (default)
		virtual void setAnimateMode(s32 mode);


		//!Convert all mesh buffers to use tangent vertices
		virtual void convertToTangents();



		//New Animation System Stuff (WIP)...
		virtual void recoverJointsFromMesh(core::array<ISceneNode*> &JointChildSceneNodes);
		virtual void tranferJointsToMesh(core::array<ISceneNode*> &JointChildSceneNodes);
		virtual void createJoints(core::array<ISceneNode*> &JointChildSceneNodes, ISceneNode* AnimatedMeshSceneNode, ISceneManager* SceneManager);





private:
		struct SB3DMeshBuffer : public IMeshBuffer
		{
			SB3DMeshBuffer()
			{
				#ifdef _DEBUG
				setDebugName("SSkinMeshBuffer");
				#endif
			}

			~SB3DMeshBuffer() {};

			virtual const video::SMaterial& getMaterial() const
			{
				return Material;
			}

			virtual video::SMaterial& getMaterial()
			{
				return Material;
			}

			virtual video::S3DVertex *getVertex(u32 index)
			{
				if (VertexType==video::EVT_STANDARD) return &Vertices_Standard[index];
				if (VertexType==video::EVT_TANGENTS) return (video::S3DVertex*)&Vertices_Tangents[index];
				return (video::S3DVertex*)&Vertices_2TCoords[index];
			}

			virtual const void* getVertices() const
			{
				if (VertexType==video::EVT_STANDARD) return Vertices_Standard.const_pointer();
				if (VertexType==video::EVT_TANGENTS) return Vertices_Tangents.const_pointer();
				return Vertices_2TCoords.const_pointer();
			}

			virtual void* getVertices()
			{
				if (VertexType==video::EVT_STANDARD) return Vertices_Standard.pointer();
				if (VertexType==video::EVT_TANGENTS) return Vertices_Tangents.pointer();
				return Vertices_2TCoords.pointer();
			}

			virtual u32 getVertexCount() const
			{
				if (VertexType==video::EVT_STANDARD) return Vertices_Standard.size();
				if (VertexType==video::EVT_TANGENTS) return Vertices_Tangents.size();
				return Vertices_2TCoords.size();
			}

			virtual const u16* getIndices() const
			{
				return Indices.const_pointer();
			}

			virtual u16* getIndices()
			{
				return Indices.pointer();
			}

			virtual u32 getIndexCount() const
			{
				return Indices.size();
			}

			virtual const core::aabbox3d<f32>& getBoundingBox() const
			{
				return BoundingBox;
			}

			virtual void setBoundingBox( const core::aabbox3df& box)
			{
				BoundingBox = box;
			}

			virtual void recalculateBoundingBox()
			{
				if (VertexType==video::EVT_STANDARD)
				{
					if (Vertices_Standard.empty())
						BoundingBox.reset(0,0,0);
					else
					{
						BoundingBox.reset(Vertices_Standard[0].Pos);
						for (u32 i=1; i<Vertices_Standard.size(); ++i)
							BoundingBox.addInternalPoint(Vertices_Standard[i].Pos);
					}
				}
				else if (VertexType==video::EVT_2TCOORDS)
				{
					if (Vertices_2TCoords.empty())
						BoundingBox.reset(0,0,0);
					else
					{
						BoundingBox.reset(Vertices_2TCoords[0].Pos);
						for (u32 i=1; i<Vertices_2TCoords.size(); ++i)
							BoundingBox.addInternalPoint(Vertices_2TCoords[i].Pos);
					}
				}
				else
				{
					if (Vertices_Tangents.empty())
						BoundingBox.reset(0,0,0);
					else
					{
						BoundingBox.reset(Vertices_Tangents[0].Pos);
						for (u32 i=1; i<Vertices_Tangents.size(); ++i)
							BoundingBox.addInternalPoint(Vertices_Tangents[i].Pos);
					}
				}

			}

			virtual video::E_VERTEX_TYPE getVertexType() const
			{
				return VertexType;
			}

			//! returns the byte size (stride, pitch) of the vertex
			virtual u32 getVertexPitch() const
			{
				if (VertexType==video::EVT_STANDARD) return sizeof ( video::S3DVertex );
				if (VertexType==video::EVT_TANGENTS) return sizeof ( video::S3DVertexTangents );
				return sizeof ( video::S3DVertex2TCoords );
			}

			virtual void MoveTo_2TCoords()
			{
				if (VertexType==video::EVT_STANDARD)
				{

					for(u32 n=0;n<Vertices_Standard.size();++n)
					{
						video::S3DVertex2TCoords Vertex;
						Vertex.Color=Vertices_Standard[n].Color;
						Vertex.Pos=Vertices_Standard[n].Pos;
						Vertex.Normal=Vertices_Standard[n].Normal;
						Vertex.TCoords=Vertices_Standard[n].TCoords;
						Vertices_2TCoords.push_back(Vertex);
					}
					Vertices_Standard.clear();

					VertexType=video::EVT_2TCOORDS;
				}
			}

			virtual void MoveTo_Tangents()
			{
				if (VertexType==video::EVT_STANDARD)
				{

					for(u32 n=0;n<Vertices_Standard.size();++n)
					{
						video::S3DVertexTangents Vertex;
						Vertex.Color=Vertices_Standard[n].Color;
						Vertex.Pos=Vertices_Standard[n].Pos;
						Vertex.Normal=Vertices_Standard[n].Normal;
						Vertex.TCoords=Vertices_Standard[n].TCoords;
						Vertices_Tangents.push_back(Vertex);
					}

					Vertices_Standard.clear();

					VertexType=video::EVT_TANGENTS;
				}
				else if (VertexType==video::EVT_2TCOORDS)
				{

					for(u32 n=0;n<Vertices_2TCoords.size();++n)
					{
						video::S3DVertexTangents Vertex;
						Vertex.Color=Vertices_2TCoords[n].Color;
						Vertex.Pos=Vertices_2TCoords[n].Pos;
						Vertex.Normal=Vertices_2TCoords[n].Normal;
						Vertex.TCoords=Vertices_2TCoords[n].TCoords;
						//Vertex.TCoords2=Vertices_2TCoords[n].TCoords2;
						Vertices_Tangents.push_back(Vertex);

					}

					Vertices_2TCoords.clear();

					VertexType=video::EVT_TANGENTS;
				}


			}





			video::SMaterial Material;
			video::E_VERTEX_TYPE VertexType;

			core::array<video::S3DVertexTangents> Vertices_Tangents;
			core::array<video::S3DVertex2TCoords> Vertices_2TCoords;
			core::array<video::S3DVertex> Vertices_Standard;
			core::array<u16> Indices;
			core::aabbox3d<f32> BoundingBox;
		};

		struct B3dChunk
		{
			c8 name[4];
			s32 length;
			s32 startposition;
		};

		s32 AnimFlags;
		s32 AnimFrames; //how many frames in anim
		f32 AnimFPS;

		struct SB3dBone
		{
			s32 vertex_id;
			f32 weight;
			core::vector3df pos;
			core::vector3df normal;
			video::S3DVertex *vertex;
		};

		struct SB3dPositionKey
		{
			s32 frame;
			core::vector3df position;
		};

		struct SB3dScaleKey
		{
			s32 frame;
			core::vector3df scale;
		};

		struct SB3dRotationKey
		{
			s32 frame;
			core::quaternion rotation;
		};

	  	struct SB3dNode
		{
			core::stringc Name;

			core::vector3df position;
			core::vector3df scale;
			core::quaternion rotation;

			core::vector3df Animatedposition;
			core::vector3df Animatedscale;
			core::quaternion Animatedrotation;

			core::matrix4 GlobalAnimatedMatrix;
			core::matrix4 LocalAnimatedMatrix;

			core::matrix4 LocalMatrix;
			core::matrix4 GlobalMatrix;
			core::matrix4 GlobalInversedMatrix;

			bool Animate; //Move this nodes local matrix when animating?
			bool AnimatingPositionKeys;
			bool AnimatingScaleKeys;
			bool AnimatingRotationKeys;

			bool HasScaleAnimation;

			core::array<SB3dPositionKey> PositionKeys;
			core::array<SB3dScaleKey> ScaleKeys;
			core::array<SB3dRotationKey> RotationKeys;

			core::array<SB3dBone> Bones;

			core::array<SB3dNode*> Nodes;

		};

		core::array<SB3dNode*> Nodes;

		core::array<SB3dNode*> RootNodes;

		struct SB3dTexture
		{
			irr::video::ITexture* Texture;
			s32 Flags;
			s32 Blend;
			f32 Xpos;
			f32 Ypos;
			f32 Xscale;
			f32 Yscale;
			f32 Angle;
		};

		struct SB3dMaterial
		{
			irr::video::SMaterial* Material;
			f32 red, green, blue, alpha;
			f32 shininess;
			s32 blend,fx;
			SB3dTexture *Textures[2];

		};

		bool ReadChunkTEXS(io::IReadFile* file);
		bool ReadChunkBRUS(io::IReadFile* file);
		bool ReadChunkMESH(io::IReadFile* file, SB3dNode *InNode);
		bool ReadChunkVRTS(io::IReadFile* file, SB3dNode *InNode, SB3DMeshBuffer *MeshBuffer, s32 Vertices_Start);
		bool ReadChunkTRIS(io::IReadFile* file, SB3dNode *InNode, SB3DMeshBuffer *MeshBuffer, s32 Vertices_Start);
		bool ReadChunkNODE(io::IReadFile* file, SB3dNode *InNode);
		bool ReadChunkBONE(io::IReadFile* file, SB3dNode *InNode);
		bool ReadChunkKEYS(io::IReadFile* file, SB3dNode *InNode);
		bool ReadChunkANIM(io::IReadFile* file, SB3dNode *InNode);

		void normalizeWeights();
		void animate(s32 frame,s32 startFrameLoop, s32 endFrameLoop);
		void CalculateGlobalMatrixes(SB3dNode *Node,SB3dNode *ParentNode);
		void animateSkin(f32 frame,f32 startFrame, f32 endFrame,SB3dNode *InNode,SB3dNode *ParentNode);
		void getNodeAnimation(f32 frame,SB3dNode *Node,core::vector3df &position, core::vector3df &scale, core::quaternion &rotation);
		void animateNodes(f32 frame,f32 startFrame, f32 endFrame);
		void slerp(core::quaternion A,core::quaternion B,core::quaternion &C,f32 t);

		void calculateTangents(core::vector3df& normal,
	core::vector3df& tangent, core::vector3df& binormal,
	core::vector3df& vt1, core::vector3df& vt2, core::vector3df& vt3,
	core::vector2df& tc1, core::vector2df& tc2, core::vector2df& tc3);


		void createSkelton_Helper(ISceneManager* SceneManager, core::array<ISceneNode*> &JointChildSceneNodes, ISceneNode *AnimatedMeshSceneNode, ISceneNode* ParentNode, SB3dNode *ParentB3dNode, SB3dNode *B3dNode);



		core::array<B3dChunk> B3dStack;

		f32 totalTime;
		bool HasAnimation;
		bool HasBones;

		s32 lastCalculatedFrame;
		s32 lastAnimateMode;

		bool NormalsInFile;
		bool AnimateNormals;

		//0- Constant 1- Linear
		s32 InterpolationMode;

		//0-None 1-Update nodes only 2-Update skin only 3-Update both nodes and skin
		s32 AnimateMode;

		core::stringc readString(io::IReadFile* file);
		core::stringc stripPathString(core::stringc oldstring, bool keepPath);
		void readFloats(io::IReadFile* file, f32* vec, u32 count);

		core::aabbox3d<f32> BoundingBox;
		core::array<SB3dMaterial> Materials;
		core::array<SB3dTexture> Textures;

		core::array<video::S3DVertex2TCoords*> BaseVertices;

		core::array<bool> Vertices_Moved;
		core::array<f32> Vertices_Alpha;
		core::array<s32> AnimatedVertices_VertexID;
		core::array<SB3DMeshBuffer*> AnimatedVertices_MeshBuffer;
		core::array<SB3DMeshBuffer*> Buffers;

		video::IVideoDriver* Driver;
	};

} // end namespace scene
} // end namespace irr

#endif


