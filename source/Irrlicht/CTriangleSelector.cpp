// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CTriangleSelector.h"
#include "ISceneNode.h"
#include "IMeshBuffer.h"

namespace irr
{
namespace scene
{

//! constructor
CTriangleSelector::CTriangleSelector(const ISceneNode* node)
: SceneNode(node)
{
	#ifdef _DEBUG
	setDebugName("CTriangleSelector");
	#endif
}


//! constructor
CTriangleSelector::CTriangleSelector(const IMesh* mesh, const ISceneNode* node)
: SceneNode(node)
{
	#ifdef _DEBUG
	setDebugName("CTriangleSelector");
	#endif

	const u32 cnt = mesh->getMeshBufferCount();
	u32 totalFaceCount = 0;
	for (u32 j=0; j<cnt; ++j)
		totalFaceCount += mesh->getMeshBuffer(j)->getIndexCount();
	totalFaceCount /= 3;
	Triangles.reallocate(totalFaceCount);

	for (u32 i=0; i<cnt; ++i)
	{
		const IMeshBuffer* buf = mesh->getMeshBuffer(i);

		const u32 idxCnt = buf->getIndexCount();
		const u16* const indices = buf->getIndices();

		for (u32 j=0; j<idxCnt; j+=3)
		{
			Triangles.push_back(core::triangle3df(
					buf->getPosition(indices[j+0]),
					buf->getPosition(indices[j+1]),
					buf->getPosition(indices[j+2])));
		}
	}
}


//! constructor
CTriangleSelector::CTriangleSelector(const core::aabbox3d<f32>& box, const ISceneNode* node)
: SceneNode(node)
{
	#ifdef _DEBUG
	setDebugName("CTriangleSelector");
	#endif

	// TODO
}


//! Gets all triangles.
void CTriangleSelector::getTriangles(core::triangle3df* triangles,
					s32 arraySize, s32& outTriangleCount, 
					const core::matrix4* transform) const
{
	s32 cnt = Triangles.size();
	if (cnt > arraySize)
		cnt = arraySize;

	core::matrix4 mat;

	if (transform)
		mat = *transform;

	if (SceneNode)
		mat *= SceneNode->getAbsoluteTransformation();

	for (s32 i=0; i<cnt; ++i)
	{
		triangles[i] = Triangles[i];
		mat.transformVect(triangles[i].pointA);
		mat.transformVect(triangles[i].pointB);
		mat.transformVect(triangles[i].pointC);
	}

	outTriangleCount = cnt;
}



//! Gets all triangles which lie within a specific bounding box.
void CTriangleSelector::getTriangles(core::triangle3df* triangles, 
					s32 arraySize, s32& outTriangleCount, 
					const core::aabbox3d<f32>& box,
					const core::matrix4* transform) const
{
	// return all triangles
	getTriangles(triangles, arraySize, outTriangleCount, transform);
}


//! Gets all triangles which have or may have contact with a 3d line.
void CTriangleSelector::getTriangles(core::triangle3df* triangles,
					s32 arraySize, s32& outTriangleCount,
					const core::line3d<f32>& line,
					const core::matrix4* transform) const
{
	// return all triangles
	getTriangles(triangles, arraySize, outTriangleCount, transform);
}


//! Returns amount of all available triangles in this selector
s32 CTriangleSelector::getTriangleCount() const
{
	return Triangles.size();
}



} // end namespace scene
} // end namespace irr

