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
	TWOTCOORDS
};


/// <summary>
/// Standard vertex used by the Irrlicht engine. Its size is 36 bytes. 
/// <summary>
///</summary>
public __value class Vertex3D
{
public:

	/// <summary>
	/// default constructor
	/// </summary>
	Vertex3D() {};

	/// <summary>
	/// constructor
	/// </summary>
	Vertex3D(float x, float y, float z, float nx, float ny, float nz, Color c, float tu, float tv)
		: Pos(x,y,z), Normal(nx,ny,nz), Color(c), TCoords(tu,tv) {}

	/// <summary>
	/// constructor
	/// </summary>
	Vertex3D(Core::Vector3D pos, Core::Vector3D normal,
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
		Vertex3D* c = dynamic_cast<Vertex3D *>(rhs);

		if(!c) 
			return false;

		return c->Pos == Pos && c->Normal == Normal &&
			c->Color == Color && c->TCoords == TCoords;
	}

};



/// <summary>
/// Vertex with two texture coordinates.
/// <summary>
///</summary>
public __value class Vertex3D2Tex
{
public:

	/// <summary>
	/// default constructor
	/// </summary>
	Vertex3D2Tex() {};

	/// <summary>
	/// constructor
	/// </summary>
	Vertex3D2Tex(float x, float y, float z, float nx, float ny, float nz, Color c, float tu, float tv)
		: Pos(x,y,z), Normal(nx,ny,nz), Color(c), TCoords(tu,tv) {}

	/// <summary>
	/// constructor
	/// </summary>
	Vertex3D2Tex(Core::Vector3D pos, Core::Vector3D normal,
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
	/// First set of texture coordinates
	/// </summary>
	Core::Vector2D TCoords;	

	/// <summary>
	/// Second set of texture coordinates
	/// </summary>
	Core::Vector2D TCoords2;	

	/// <summary>
	/// Compares the vertex to another vertex.
	/// </summary>
	bool Equals(Object* rhs) 
	{
		Vertex3D2Tex* c = dynamic_cast<Vertex3D2Tex *>(rhs);

		if(!c) 
			return false;

		return c->Pos == Pos && c->Normal == Normal &&
			c->Color == Color && c->TCoords == TCoords &&
			c->TCoords2 == TCoords;
	}

};



}
}