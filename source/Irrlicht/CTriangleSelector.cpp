// Copyright (C) 2002-2011 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CTriangleSelector.h"
#include "ISceneNode.h"
#include "IMeshBuffer.h"
#include "IAnimatedMeshSceneNode.h"

namespace irr
{
namespace scene
{

//! constructor
CTriangleSelector::CTriangleSelector(ISceneNode* node)
: SceneNode(node), AnimatedNode(0), LastMeshFrame(-1)
{
	#ifdef _DEBUG
	setDebugName("CTriangleSelector");
	#endif
}


//! constructor
CTriangleSelector::CTriangleSelector(const IMesh* mesh, ISceneNode* node)
: SceneNode(node), AnimatedNode(0)
{
	#ifdef _DEBUG
	setDebugName("CTriangleSelector");
	#endif

	createFromMesh(mesh);
}

CTriangleSelector::CTriangleSelector(IAnimatedMeshSceneNode* node)
: SceneNode(reinterpret_cast<ISceneNode*>(node)), AnimatedNode(node)
{
	#ifdef _DEBUG
	setDebugName("CTriangleSelector");
	#endif

	if (!AnimatedNode)
		return;

	IAnimatedMesh * animatedMesh = AnimatedNode->getMesh();
	if (!animatedMesh)
		return;

	IMesh * mesh = animatedMesh->getMesh((s32)AnimatedNode->getFrameNr());

	if (mesh)
		createFromMesh(mesh);
}

void CTriangleSelector::createFromMesh(const IMesh * mesh)
{
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

void CTriangleSelector::updateFromMesh(const IMesh* mesh) const
{
	if (!mesh)
		return;

	u32 meshBuffers = mesh->getMeshBufferCount();
	u32 triangleCount = 0;

	for (u32 i = 0; i < meshBuffers; ++i)
	{
		IMeshBuffer* buf = mesh->getMeshBuffer(i);
		u32 idxCnt = buf->getIndexCount();
		const u16* indices = buf->getIndices();

		switch (buf->getVertexType())
		{
		case video::EVT_STANDARD:
			{
				video::S3DVertex* vtx = (video::S3DVertex*)buf->getVertices();
				for (u32 index = 0; index < idxCnt; index += 3)
				{
					core::triangle3df & tri = Triangles[triangleCount++];
					tri.pointA = vtx[indices[index + 0]].Pos;
					tri.pointB = vtx[indices[index + 1]].Pos;
					tri.pointC = vtx[indices[index + 2]].Pos;
				}
			}
			break;
		case video::EVT_2TCOORDS:
			{
				video::S3DVertex2TCoords* vtx = (video::S3DVertex2TCoords*)buf->getVertices();
				for (u32 index = 0; index < idxCnt; index += 3)
				{
					core::triangle3df & tri = Triangles[triangleCount++];
					tri.pointA = vtx[indices[index + 0]].Pos;
					tri.pointB = vtx[indices[index + 1]].Pos;
					tri.pointC = vtx[indices[index + 2]].Pos;
				}
			}
			break;
		case video::EVT_TANGENTS:
			{
				video::S3DVertexTangents* vtx = (video::S3DVertexTangents*)buf->getVertices();
				for (u32 index = 0; index < idxCnt; index += 3)
				{
					core::triangle3df & tri = Triangles[triangleCount++];
					tri.pointA = vtx[indices[index + 0]].Pos;
					tri.pointB = vtx[indices[index + 1]].Pos;
					tri.pointC = vtx[indices[index + 2]].Pos;
				}
			}
			break;
		}
	}
}


//! constructor
CTriangleSelector::CTriangleSelector(const core::aabbox3d<f32>& box, ISceneNode* node)
: SceneNode(node)
{
	#ifdef _DEBUG
	setDebugName("CTriangleSelector");
	#endif

	// TODO
}



void CTriangleSelector::update(void) const
{
	if (!AnimatedNode)
		return; //< harmless no-op

	s32 currentFrame = (s32)AnimatedNode->getFrameNr();
	if (currentFrame == LastMeshFrame)
		return; //< Nothing to do

	LastMeshFrame = currentFrame;
	IAnimatedMesh * animatedMesh = AnimatedNode->getMesh();

	if (animatedMesh)
	{
		IMesh * mesh = animatedMesh->getMesh(LastMeshFrame);

		if (mesh)
			updateFromMesh(mesh);
	}
}

//! Gets all triangles.
void CTriangleSelector::getTriangles(core::triangle3df* triangles,
					s32 arraySize, s32& outTriangleCount,
					const core::matrix4* transform) const
{
	// Update my triangles if necessary
	update();

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
		mat.transformVect( triangles[i].pointA, Triangles[i].pointA );
		mat.transformVect( triangles[i].pointB, Triangles[i].pointB );
		mat.transformVect( triangles[i].pointC, Triangles[i].pointC );
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
	return getTriangles(triangles, arraySize, outTriangleCount, transform);
}


//! Gets all triangles which have or may have contact with a 3d line.
void CTriangleSelector::getTriangles(core::triangle3df* triangles,
					s32 arraySize, s32& outTriangleCount,
					const core::line3d<f32>& line,
					const core::matrix4* transform) const
{
	// return all triangles
	return getTriangles(triangles, arraySize, outTriangleCount, transform);
}


//! Returns amount of all available triangles in this selector
s32 CTriangleSelector::getTriangleCount() const
{
	return Triangles.size();
}


/* Get the number of TriangleSelectors that are part of this one.
Only useful for MetaTriangleSelector others return 1
*/
u32 CTriangleSelector::getSelectorCount() const
{
	return 1;
}


/* Get the TriangleSelector based on index based on getSelectorCount.
Only useful for MetaTriangleSelector others return 'this' or 0
*/
ITriangleSelector* CTriangleSelector::getSelector(u32 index)
{
	if (index)
		return 0;
	else
		return this;
}


/* Get the TriangleSelector based on index based on getSelectorCount.
Only useful for MetaTriangleSelector others return 'this' or 0
*/
const ITriangleSelector* CTriangleSelector::getSelector(u32 index) const
{
	if (index)
		return 0;
	else
		return this;
}


} // end namespace scene
} // end namespace irr

