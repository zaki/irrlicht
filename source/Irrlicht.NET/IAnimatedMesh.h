// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#using <mscorlib.dll>
using namespace System;

#pragma unmanaged
#include "..\\..\\include\\irrlicht.h"
#pragma managed

#include "Dimension2D.h"
#include "edrivertypes.h"
#include "ecolorformat.h"
#include "IMesh.h"

namespace Irrlicht
{
namespace Scene
{

/// <summary> Enumeration for all types of meshes</summary>
public __value enum AnimatedMeshType
{
	/// Unknown animated mesh type.
	Unknown = 0,

	/// Quake 2 MD2 model file
	MD2, 

	/// Milkshape 3d skeletal animation file
	MS3D,

	/// Maya .obj not animated model
	OBJ,

	/// Quake 3 .bsp Map, not animated
	BSP,

	/// 3D Studio .3ds file
	_3DS,

	/// Microsoft Direct3D .x-file. Can contain static and skeletal animated 
	/// skinned meshes. This is the standard and best supported 
	/// format of the Irrlicht Engine.
	X
};

/// <summary> 
/// Interface for an Animated Mesh.
/// </summary>
public __gc class IAnimatedMesh
{
public:

	/// <summary>
	/// You should create an IAnimatedMesh
	/// through the Irrlicht::IrrlichtDevice::SceneManager::getMesh method. Simply don't use
	/// this constructor.
	///</summary>
	///<param name="realMesh">The real, unmanaged C++ mesh</param>
	IAnimatedMesh(irr::scene::IAnimatedMesh* realMesh);

	/// destructor
	~IAnimatedMesh();

	/// <summary>
	/// Gets the frame count of the animated mesh.
	/// </summary>
	/// <returns> Returns the amount of frames. If the amount is 1, it is a static, not animated mesh.</returns>
	__property int get_FrameCount();

	/// <summary>
	/// Returns the IMesh interface for a frame.
	/// </summary>
	/// <param name="frame"> Frame number as zero based index. The maximum frame number is
	/// getFrameCount() - 1;</param>
	/// <returns>Returns the animated mesh based on a detail level. </returns>
	IMesh* GetMesh(int frame);

	/// <summary>
	/// Returns the IMesh interface for a frame.
	/// </summary>
	/// <param name="frame"> Frame number as zero based index. The maximum frame number is
	/// getFrameCount() - 1;</param>
	/// <param name="detailLevel"> Level of detail. 0 is the lowest,
	/// 255 the highest level of detail. Most meshes will ignore the detail level.</param>
	/// <param name="startFrameLoop"> Because some animated meshes (.MD2) are blended between 2
	/// static frames, and maybe animated in a loop, the startFrameLoop and the endFrameLoop
	/// have to be defined, to prevent the animation to be blended between frames which are
	/// outside of this loop. 
	/// If startFrameLoop and endFrameLoop are both -1, they are ignored.</param>
	/// <param name="endFrameLoop"> see startFrameLoop.</name>
	/// <returns>Returns the animated mesh based on a detail level. </returns>
	IMesh* GetMesh(int frame, int startFrameLoop, int endFrameLoop, int detailLevel);

	/// <summary>
	/// Returns an axis aligned bounding box of the mesh.
	/// </summary>
	/// <returns>A bounding box of this mesh is returned.</returns>
	__property Core::Box3D get_BoundingBox();

	/// <summary>
	/// Returns the type of the animated mesh.
	/// </summary>
	__property AnimatedMeshType get_MeshType();

	/// <summary>
	/// Returns the internal pointer to the native C++ irrlicht mesh.
	/// Do not use this, only needed by the internal .NET wrapper.
	///</summary>
	__property irr::scene::IAnimatedMesh* get_NativeAnimatedMesh();

protected:

	irr::scene::IAnimatedMesh* AnimatedMesh;
};


}
}