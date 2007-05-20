// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#include "Vector3D.h"
#include "Vector2D.h"
#include "Color.h"

namespace Irrlicht
{
namespace Video
{

/// <summary>
/// Enumeration for all vertex types there are.
/// </summary>
public __value enum VertexType
{
	/// Standard vertex type used by the Irrlicht engine.
	STANDARD = 0, 

	/// Vertex with two texture coordinates. Usually used for geometry with lightmaps
	/// or other special materials.
	TWOTCOORDS,

	/// Vertex with a tangent and binormal vector.
	/// Usually used for tangent space normal mapping.
	TANGENTS
};


/// <summary>
/// Standard vertex used by the Irrlicht engine. Its size is 36 bytes. 
///</summary>
public __value class S3DVertex
{
public:

	/// <summary>
	/// default constructor
	/// </summary>
	S3DVertex() {};

	/// <summary>
	/// constructor
	/// </summary>
	S3DVertex(float x, float y, float z, float nx, float ny, float nz, Color c, float tu, float tv)
		: Pos(x,y,z), Normal(nx,ny,nz), Color(c), TCoords(tu,tv) {}

	/// <summary>
	/// constructor
	/// </summary>
	S3DVertex(Core::Vector3D pos, Core::Vector3D normal,
	Video::Color color, Core::Vector2D tcoords)
		: Pos(pos), Normal(normal), Color(color), TCoords(tcoords) {}

	/// <summary>
	/// Position
	/// </summary>
	Core::Vector3D Pos;	
	
	/// <summary>
	/// Normal vector
	/// </summary>
	Core::Vector3D Normal;

	/// <summary>
	/// Color
	/// </summary>
	Video::Color Color;				

	/// <summary>
	/// Texture coordinates
	/// </summary>
	Core::Vector2D TCoords;	

	/// <summary>
	/// Compares the vertex to another vertex.
	/// </summary>
	bool Equals(Object* rhs) 
	{
		S3DVertex* c = dynamic_cast<S3DVertex *>(rhs);

		if(!c) 
			return false;

		return c->Pos == Pos && c->Normal == Normal &&
			c->Color == Color && c->TCoords == TCoords;
	}

};



}
}