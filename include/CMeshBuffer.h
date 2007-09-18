// Copyright (C) 2002-2007 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __T_MESH_BUFFER_H_INCLUDED__
#define __T_MESH_BUFFER_H_INCLUDED__

#include "irrArray.h"
#include "IMeshBuffer.h"

namespace irr
{
namespace scene
{
	//! Template implementation of the IMeshBuffer interface
	template <class T>
	class CMeshBuffer : public IMeshBuffer
	{
	public:
		//! constructor
		CMeshBuffer() // everything's default constructed
		{
			#ifdef _DEBUG
			setDebugName("SMeshBuffer");
			#endif
		}

		//! returns the material of this meshbuffer
		virtual const video::SMaterial& getMaterial() const
		{
			return Material;
		}

		//! returns the material of this meshbuffer
		virtual video::SMaterial& getMaterial()
		{
			return Material;
		}

		//! returns pointer to vertices
		virtual const void* getVertices() const
		{
			return Vertices.const_pointer();
		} 

		//! returns pointer to vertices
		virtual void* getVertices()
		{
			return Vertices.pointer();
		} 

		//! returns amount of vertices
		virtual u32 getVertexCount() const
		{
			return Vertices.size();
		}

		//! returns pointer to Indices
		virtual const u16* getIndices() const
		{
			return Indices.const_pointer();
		}

		//! returns pointer to Indices
		virtual u16* getIndices()
		{
			return Indices.pointer();
		}

		//! returns amount of indices
		virtual u32 getIndexCount() const
		{
			return Indices.size();
		}

		//! returns an axis aligned bounding box
		virtual const core::aabbox3d<f32>& getBoundingBox() const
		{
			return BoundingBox;
		}

		//! set user axis aligned bounding box
		virtual void setBoundingBox(const core::aabbox3df& box)
		{
			BoundingBox = box;
		}


		//! recalculates the bounding box. should be called if the mesh changed.
		virtual void recalculateBoundingBox()
		{
			if (Vertices.empty())
				BoundingBox.reset(0,0,0);
			else
			{
				BoundingBox.reset(Vertices[0].Pos);
				for (u32 i=1; i<Vertices.size(); ++i)
					BoundingBox.addInternalPoint(Vertices[i].Pos);
			}
		}

		//! returns which type of vertex data is stored.
		virtual video::E_VERTEX_TYPE getVertexType() const
		{
			return T().getType();
		}

	
		//! append the vertices and indices to the current buffer
		virtual void append(const void* const vertices, u32 numVertices, const u16* const indices, u32 numIndices)
		{
			const u32 vertexCount = getVertexCount();
			u32 i;

			Vertices.reallocate(vertexCount+numVertices);
			for (i=0; i<numVertices; ++i)
			{
				Vertices.push_back(reinterpret_cast<const T*>(vertices)[i]);
				BoundingBox.addInternalPoint(reinterpret_cast<const T*>(vertices)[i].Pos);
			}

			Indices.reallocate(getIndexCount()+numIndices);
			for (i=0; i<numIndices; ++i)
			{
				Indices.push_back(indices[i]+vertexCount);
			}
		}

		//! append the meshbuffer to the current buffer
		virtual void append(const IMeshBuffer* const other)
		{
			const u32 vertexCount = getVertexCount();
			u32 i;

			Vertices.reallocate(vertexCount+other->getVertexCount());
			for (i=0; i<other->getVertexCount(); ++i)
			{
				Vertices.push_back(reinterpret_cast<const T*>(other->getVertices())[i]);
			}

			Indices.reallocate(getIndexCount()+other->getIndexCount());
			for (i=0; i<other->getIndexCount(); ++i)
			{
				Indices.push_back(other->getIndices()[i]+vertexCount);
			}
			BoundingBox.addInternalBox(other->getBoundingBox());
		}

		//! Material for this meshbuffer.
		video::SMaterial Material;
		//! Vertices of this buffer
		core::array<T> Vertices;
		//! Indices into the vertices of this buffer.
		core::array<u16> Indices;
		//! Bounding box of this meshbuffer.
		core::aabbox3d<f32> BoundingBox;
	};

	typedef CMeshBuffer<video::S3DVertex> SMeshBuffer;
	typedef CMeshBuffer<video::S3DVertex2TCoords> SMeshBufferLightMap;
	typedef CMeshBuffer<video::S3DVertexTangents> SMeshBufferTangents;
} // end namespace scene
} // end namespace irr

#endif

