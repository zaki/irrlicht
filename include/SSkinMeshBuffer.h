// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_SKIN_MESH_BUFFER_H_INCLUDED__
#define __I_SKIN_MESH_BUFFER_H_INCLUDED__

#include "IMeshBuffer.h"
#include "S3DVertex.h"


namespace irr
{
namespace scene
{


//! A mesh buffer able to choose between
//! S3DVertex2TCoords, S3DVertex and S3DVertexTangents at runtime
struct SSkinMeshBuffer : public IMeshBuffer
{
	SSkinMeshBuffer(video::E_VERTEX_TYPE vt=video::EVT_STANDARD) : VertexType(vt)
	{
		#ifdef _DEBUG
		setDebugName("SSkinMeshBuffer");
		#endif
	}

	virtual ~SSkinMeshBuffer() {}

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
		switch (VertexType)
		{
			case video::EVT_2TCOORDS:	return (video::S3DVertex*)&Vertices_2TCoords[index];
			case video::EVT_TANGENTS:	return (video::S3DVertex*)&Vertices_Tangents[index];
			default:			return &Vertices_Standard[index];
		}
	}

	virtual const void* getVertices() const
	{
		switch (VertexType)
		{
			case video::EVT_2TCOORDS:	return Vertices_2TCoords.const_pointer();
			case video::EVT_TANGENTS:	return Vertices_Tangents.const_pointer();
			default:			return Vertices_Standard.const_pointer();
		}
	}

	virtual void* getVertices()
	{
		switch (VertexType)
		{
			case video::EVT_2TCOORDS:	return Vertices_2TCoords.pointer();
			case video::EVT_TANGENTS:	return Vertices_Tangents.pointer();
			default:			return Vertices_Standard.pointer();
		}
	}

	virtual u32 getVertexCount() const
	{
		switch (VertexType)
		{
			case video::EVT_2TCOORDS:	return Vertices_2TCoords.size();
			case video::EVT_TANGENTS:	return Vertices_Tangents.size();
			default:			return Vertices_Standard.size();
		}
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
		switch (VertexType)
		{
			case video::EVT_STANDARD:
			{
				if (Vertices_Standard.empty())
					BoundingBox.reset(0,0,0);
				else
				{
					BoundingBox.reset(Vertices_Standard[0].Pos);
					for (u32 i=1; i<Vertices_Standard.size(); ++i)
						BoundingBox.addInternalPoint(Vertices_Standard[i].Pos);
				}
				break;
			}
			case video::EVT_2TCOORDS:
			{
				if (Vertices_2TCoords.empty())
					BoundingBox.reset(0,0,0);
				else
				{
					BoundingBox.reset(Vertices_2TCoords[0].Pos);
					for (u32 i=1; i<Vertices_2TCoords.size(); ++i)
						BoundingBox.addInternalPoint(Vertices_2TCoords[i].Pos);
				}
				break;
			}
			case video::EVT_TANGENTS:
			{
				if (Vertices_Tangents.empty())
					BoundingBox.reset(0,0,0);
				else
				{
					BoundingBox.reset(Vertices_Tangents[0].Pos);
					for (u32 i=1; i<Vertices_Tangents.size(); ++i)
						BoundingBox.addInternalPoint(Vertices_Tangents[i].Pos);
				}
				break;
			}
		}
	}

	virtual video::E_VERTEX_TYPE getVertexType() const
	{
		return VertexType;
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
				Vertices_Tangents.push_back(Vertex);
			}
			Vertices_2TCoords.clear();
			VertexType=video::EVT_TANGENTS;
		}
	}

	//! append the vertices and indices to the current buffer
	virtual void append(const void* const vertices, u32 numVertices, const u16* const indices, u32 numIndices) {}

	//! append the meshbuffer to the current buffer
	virtual void append(const IMeshBuffer* const other) {}


	//ISkinnedMesh::SJoint *AttachedJoint;
	core::matrix4 Transformation;

	video::SMaterial Material;
	video::E_VERTEX_TYPE VertexType;
	core::array<video::S3DVertexTangents> Vertices_Tangents;
	core::array<video::S3DVertex2TCoords> Vertices_2TCoords;
	core::array<video::S3DVertex> Vertices_Standard;
	core::array<u16> Indices;
	core::aabbox3d<f32> BoundingBox;
};


} // end namespace scene
} // end namespace irr

#endif

