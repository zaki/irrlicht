// Copyright (C) 2002-2007 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __S_3D_VERTEX_H_INCLUDED__
#define __S_3D_VERTEX_H_INCLUDED__

#include "vector3d.h"
#include "vector2d.h"
#include "SColor.h"

namespace irr
{
namespace video
{

//! Enumeration for all vertex types there are.
enum E_VERTEX_TYPE
{
	//! Standard vertex type used by the Irrlicht engine, video::S3DVertex.
	EVT_STANDARD = 0,

	//! Vertex with two texture coordinates, video::S3DVertex2TCoords. Usually used
	//! for geometry with lightmaps or other special materials.
	EVT_2TCOORDS,

	//! Vertex with a tangent and binormal vector, video::S3DVertexTangents. Usually
	//! used for tangent space normal mapping.
	EVT_TANGENTS
};

//! standard vertex used by the Irrlicht engine.
struct S3DVertex
{
	//! default constructor
	S3DVertex() {};

	//! constructor
	S3DVertex(f32 x, f32 y, f32 z, f32 nx, f32 ny, f32 nz, SColor c, f32 tu, f32 tv)
		: Pos(x,y,z), Normal(nx,ny,nz), Color(c), TCoords(tu,tv) {}

	//! constructor
	S3DVertex(const core::vector3df& pos, const core::vector3df& normal,
		SColor color, const core::vector2d<f32>& tcoords)
		: Pos(pos), Normal(normal), Color(color), TCoords(tcoords) {}

	//! Position
	core::vector3df Pos;
	
	//! Normal vector
	core::vector3df Normal;

	//! Color
	SColor Color;

	//! Texture coordinates
	core::vector2d<f32> TCoords;

	bool operator == (const S3DVertex& other) const
	{
		return (Pos == other.Pos && Normal == other.Normal &&
			Color == other.Color && TCoords == other.TCoords);
	}

	bool operator != (const S3DVertex& other) const
	{
		return (Pos != other.Pos || Normal != other.Normal ||
			Color != other.Color || TCoords != other.TCoords);
	}

	E_VERTEX_TYPE getType() const
	{
		return EVT_STANDARD;
	}
};


//! Vertex with two texture coordinates.
/** Usually used for geometry with lightmaps
or other special materials.
*/
struct S3DVertex2TCoords
{
	//! default constructor
	S3DVertex2TCoords() {};

	//! constructor with two different texture coords
	S3DVertex2TCoords(f32 x, f32 y, f32 z, SColor c, f32 tu, f32 tv, f32 tu2, f32 tv2)
		: Pos(x,y,z), Color(c), TCoords(tu,tv), TCoords2(tu2,tv2) {}

	//! constructor with two different texture coords
	S3DVertex2TCoords(const core::vector3df& pos, SColor color,
		const core::vector2d<f32>& tcoords, const core::vector2d<f32>& tcoords2)
		: Pos(pos), Color(color), TCoords(tcoords), TCoords2(tcoords2) {}

	//! constructor 
	S3DVertex2TCoords(const core::vector3df& pos, const core::vector3df& normal, const SColor& color,
		const core::vector2d<f32>& tcoords, const core::vector2d<f32>& tcoords2)
		: Pos(pos), Normal(normal), Color(color), TCoords(tcoords), TCoords2(tcoords2) {}

	//! constructor with the same texture coords and normal
	S3DVertex2TCoords(f32 x, f32 y, f32 z, f32 nx, f32 ny, f32 nz, SColor c, f32 tu, f32 tv)
		: Pos(x,y,z), Normal(nx,ny,nz), Color(c), TCoords(tu,tv), TCoords2(tu,tv) {}

	//! constructor with the same texture coords and normal
	S3DVertex2TCoords(const core::vector3df& pos, const core::vector3df& normal,
		SColor color, const core::vector2d<f32>& tcoords)
		: Pos(pos), Normal(normal), Color(color), TCoords(tcoords), TCoords2(tcoords) {}

	//! constructor from S3DVertex
	S3DVertex2TCoords(S3DVertex& o)
		: Pos(o.Pos), Normal(o.Normal), Color(o.Color), TCoords(o.TCoords) {}

	//! Position
	core::vector3df Pos;

	//! Normal
	core::vector3df Normal;

	//! Color
	SColor Color;

	//! First set of texture coordinates
	core::vector2d<f32> TCoords;

	//! Second set of texture coordinates
	core::vector2d<f32> TCoords2;

	//! Equality operator
	bool operator == (const S3DVertex2TCoords& other) const
	{
		return (Pos == other.Pos && Normal == other.Normal &&
			Color == other.Color && TCoords == other.TCoords &&
			TCoords2 == other.TCoords2);
	}

	//! Inequality operator
	bool operator != (const S3DVertex2TCoords& other) const
	{
		return (Pos != other.Pos || Normal != other.Normal ||
			Color != other.Color || TCoords != other.TCoords ||
			TCoords2 != other.TCoords2);
	}

	E_VERTEX_TYPE getType() const
	{
		return EVT_2TCOORDS;
	}
};


//! Vertex with a tangent and binormal vector.
/**  Usually used for tangent space normal mapping.
*/
struct S3DVertexTangents
{
	//! default constructor
	S3DVertexTangents() { };

	//! constructor
	S3DVertexTangents(f32 x, f32 y, f32 z)
	: Pos(x,y,z) { }

	//! constructor
	S3DVertexTangents(const core::vector3df& pos,
		const core::vector2df& tcoords,	SColor c)
		: Pos(pos), Color(c), TCoords(tcoords) { }

	//! Position
	core::vector3df Pos;

	//! Normal vector
	core::vector3df Normal;

	//! Color
	SColor Color;

	//! Texture coordinates
	core::vector2d<f32> TCoords;

	//! Tangent vector along the x-axis of the texture
	core::vector3df Tangent;

	//! Binormal vector (tangent x normal)
	core::vector3df Binormal;

	bool operator == (const S3DVertexTangents& other) const
	{
		return (Pos == other.Pos && Normal == other.Normal &&
			Color == other.Color && TCoords == other.TCoords &&
			Tangent == other.Tangent && Binormal == other.Binormal);
	}

	bool operator != (const S3DVertexTangents& other) const
	{
		return (Pos != other.Pos || Normal != other.Normal ||
			Color != other.Color || TCoords != other.TCoords ||
			Tangent != other.Tangent || Binormal != other.Binormal);
	}

	E_VERTEX_TYPE getType() const
	{
		return EVT_TANGENTS;
	}
};



} // end namespace video
} // end namespace irr

#endif

