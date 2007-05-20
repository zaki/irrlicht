// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#using <mscorlib.dll>
using namespace System;

#pragma unmanaged
#include "..\\..\\include\\irrlicht.h"
#pragma managed

#include "Box3D.h"
#include "Material.h"

namespace Irrlicht
{
namespace Scene
{

/// <summary> 
/// Interface for an static Mesh.
/// </summary>
public __gc class IMesh
{
public:

	/// <summary>
	/// You should create an IMesh
	/// through the Irrlicht::IrrlichtDevice::SceneManager::getMesh method. Simply don't use
	/// this constructor.
	///</summary>
	///<param name="realMesh">The real, unmanaged C++ mesh</param>
	IMesh(irr::scene::IMesh* realMesh);

	/// destructor
	~IMesh();

	/// <summary>
	/// Returns the amount of mesh buffers in this mesh
	/// </summary>
	__property int get_MeshBufferCount();

	/// Returns pointer to a mesh buffer.
	/// \param Zero based index of the mesh buffer. The maximum value is
	/// getMeshBufferCount() - 1;
	/// \return Returns the pointer to the mesh buffer or 
	/// NULL if there is no such mesh buffer.
	// virtual IMeshBuffer* getMeshBuffer(s32 nr) = 0;

	/// <summary>
	/// Gets or sets the axis aligned bounding box of the mesh.
	/// </summary>
	__property Core::Box3D get_BoundingBox();

	/// <summary>
	/// Gets or sets the axis aligned bounding box of the mesh.
	/// </summary>
	__property void set_BoundingBox(Core::Box3D box);

	/// <summary>
	/// Sets a flag of all contained materials to a new value.
	/// </summary>
	/// <param name="flag"> Flag to set in all materials.</param>
	/// <param name="newvalue"> New value to set in all materials.</param>
	void setMaterialFlag(Video::MaterialFlag flag, bool newvalue);

	/// <summary>
	/// Returns the internal pointer to the native C++ irrlicht mesh.
	/// Do not use this, only needed by the internal .NET wrapper.
	///</summary>
	__property irr::scene::IMesh* get_NativeMesh();

private:

	irr::scene::IMesh* Mesh;
};


}
}