// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_MESH_MANIPULATOR_H_INCLUDED__
#define __C_MESH_MANIPULATOR_H_INCLUDED__

#include "IMeshManipulator.h"

namespace irr
{
namespace scene
{

//! An interface for easy manipulation of meshes.
/** Scale, set alpha value, flip surfaces, and so on. This exists for fixing
problems with wrong imported or exported meshes quickly after loading. It is
not intended for doing mesh modifications and/or animations during runtime.
*/
class CMeshManipulator : public IMeshManipulator
{
public:
	//! Flips the direction of surfaces.
	/** Changes backfacing triangles to frontfacing triangles and vice versa.
	\param mesh: Mesh on which the operation is performed. */
	virtual void flipSurfaces(scene::IMesh* mesh) const;

	//! Sets the alpha vertex color value of the whole mesh to a new value
	/** \param mesh: Mesh on which the operation is performed.
	\param alpha: New alpha for the vertex color. */
	virtual void setVertexColorAlpha(scene::IMesh* mesh, s32 alpha) const;

	//! Sets the colors of all vertices to one color
	virtual void setVertexColors(IMesh* mesh, video::SColor color) const;

	//! Recalculates all normals of the mesh.
	/** \param mesh: Mesh on which the operation is performed.
	    \param smooth: Whether to use smoothed normals. */
	virtual void recalculateNormals(scene::IMesh* mesh, bool smooth = false, bool angleWeighted = false) const;

	//! Recalculates all normals of the mesh buffer.
	/** \param buffer: Mesh buffer on which the operation is performed.
	    \param smooth: Whether to use smoothed normals. */
	virtual void recalculateNormals(IMeshBuffer* buffer, bool smooth = false, bool angleWeighted = false) const;

	//! Scales the actual mesh, not the scene node.
	/** \param mesh Mesh on which the operation is performed.
	\param factor Vector which defines the scale for each axis. */
	virtual void scale(scene::IMesh* mesh, const core::vector3df& factor) const;

	//! Scales the actual meshbuffer, not the scene node.
	/** \param buffer MeshBuffer on which the operation is performed.
	\param factor Vector which defines the scale for each axis. */
	virtual void scale(scene::IMeshBuffer* buffer, const core::vector3df& factor) const;

	//! Scale the texture coords of a mesh.
	/** \param mesh Mesh on which the operation is performed.
	\param factor Vector which defines the scale for each axis.
	\param level Number of texture coord, starting from 1. Support for level 2 exists for LightMap buffers. */
	virtual void scaleTCoords(scene::IMesh* mesh, const core::vector2df& factor, u32 layer=1) const;

	//! Scale the texture coords of a meshbuffer.
	/** \param mesh Mesh on which the operation is performed.
	\param factor Vector which defines the scale for each axis.
	\param level Number of texture coord, starting from 1. Support for level 2 exists for LightMap buffers. */
	virtual void scaleTCoords(scene::IMeshBuffer* buffer, const core::vector2df& factor, u32 level=1) const;

	//! Applies a transformation to a meshbuffer
	/** \param buffer: Meshbuffer on which the operation is performed.
		\param m: matrix. */
	void transform(scene::IMeshBuffer* buffer, const core::matrix4& m) const;

	//! Applies a transformation to a mesh
	/** \param mesh: Mesh on which the operation is performed.
		\param m: transformation matrix. */
	virtual void transform(scene::IMesh* mesh, const core::matrix4& m) const;

	//! Clones a static IMesh into a modifiable SMesh.
	virtual SMesh* createMeshCopy(scene::IMesh* mesh) const;

	//! Creates a planar texture mapping on the mesh
	/** \param mesh: Mesh on which the operation is performed.
	\param resolution: resolution of the planar mapping. This is the value
	specifying which is the relation between world space and 
	texture coordinate space. */
	virtual void makePlanarTextureMapping(scene::IMesh* mesh, f32 resolution) const;

	//! Creates a copy of the mesh, which will only consist of S3DVertexTangents vertices.
	virtual IMesh* createMeshWithTangents(IMesh* mesh, bool recalculateNormals=false, bool smooth=false, bool angleWeighted=false) const;

	//! Creates a copy of the mesh, which will only consist of S3D2TCoords vertices.
	virtual IMesh* createMeshWith2TCoords(IMesh* mesh) const;

	//! Creates a copy of the mesh, which will only consist of unique triangles, i.e. no vertices are shared.
	virtual IMesh* createMeshUniquePrimitives(IMesh* mesh) const;

	//! Creates a copy of the mesh, which will have all duplicated vertices removed, i.e. maximal amount of vertices are shared via indexing.
	virtual IMesh* createMeshWelded(IMesh *mesh, f32 tolerance=core::ROUNDING_ERROR_32) const;

	//! Returns amount of polygons in mesh.
	virtual s32 getPolyCount(scene::IMesh* mesh) const;

	//! Returns amount of polygons in mesh.
	virtual s32 getPolyCount(scene::IAnimatedMesh* mesh) const;

	//! create a new AnimatedMesh and adds the mesh to it
	virtual IAnimatedMesh * createAnimatedMesh(scene::IMesh* mesh,scene::E_ANIMATED_MESH_TYPE type) const;

private:

	static void calculateTangents(core::vector3df& normal, 
		core::vector3df& tangent, 
		core::vector3df& binormal, 
		const core::vector3df& vt1, const core::vector3df& vt2, const core::vector3df& vt3,
		const core::vector2df& tc1, const core::vector2df& tc2, const core::vector2df& tc3);
};

} // end namespace scene
} // end namespace irr


#endif

