// Copyright (C) 2002-2011 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CShadowVolumeSceneNode.h"
#include "ISceneManager.h"
#include "IMesh.h"
#include "IVideoDriver.h"
#include "SLight.h"

namespace irr
{
namespace scene
{


//! constructor
CShadowVolumeSceneNode::CShadowVolumeSceneNode(const IMesh* shadowMesh, ISceneNode* parent,
		ISceneManager* mgr, s32 id, bool zfailmethod, f32 infinity)
: IShadowVolumeSceneNode(parent, mgr, id),
	ShadowMesh(0), IndexCount(0), VertexCount(0), ShadowVolumesUsed(0),
	Infinity(infinity), UseZFailMethod(zfailmethod)
{
	#ifdef _DEBUG
	setDebugName("CShadowVolumeSceneNode");
	#endif
	setShadowMesh(shadowMesh);
	setAutomaticCulling(scene::EAC_OFF);
}


//! destructor
CShadowVolumeSceneNode::~CShadowVolumeSceneNode()
{
	if (ShadowMesh)
		ShadowMesh->drop();
}


void CShadowVolumeSceneNode::createShadowVolume(const core::vector3df& light, bool isDirectional)
{
	SShadowVolume* svp = 0;

	// builds the shadow volume and adds it to the shadow volume list.

	if (ShadowVolumes.size() > ShadowVolumesUsed)
	{
		// get the next unused buffer
		svp = &ShadowVolumes[ShadowVolumesUsed];
		svp->set_used(0);
	}
	else
	{
		ShadowVolumes.push_back(SShadowVolume());
		svp = &ShadowVolumes.getLast();
	}
	svp->reallocate(IndexCount*5);
	++ShadowVolumesUsed;

	// We use triangle lists
	const u32 faceCount = IndexCount / 3;

	if (faceCount * 6 > Edges.size())
		Edges.set_used(faceCount*6);

	u32 numEdges = 0;

	//if (UseZFailMethod)
	//	numEdges=createZFailVolume(faceCount, light, svp);
	//else
	//	numEdges=createZPassVolume(faceCount, light, svp, false);

	// the createZFailVolume does currently not work 100% correctly,
	// so we create createZPassVolume with caps if the zfail method
	// is used
	numEdges=createZPassVolume(faceCount, light, svp, UseZFailMethod);

	const core::vector3df ls = light * Infinity; // light scaled
	// for all edges add the near->far quads
	for (u32 i=0; i<numEdges; ++i)
	{
		const core::vector3df &v1 = Vertices[Edges[2*i+0]];
		const core::vector3df &v2 = Vertices[Edges[2*i+1]];
		const core::vector3df v3(v1 - ls);
		const core::vector3df v4(v2 - ls);

		// Add a quad (two triangles) to the vertex list
		if (svp->size() < svp->allocated_size()-5)
		{
			svp->push_back(v1);
			svp->push_back(v2);
			svp->push_back(v3);

			svp->push_back(v2);
			svp->push_back(v4);
			svp->push_back(v3);
		}
	}
}


u32 CShadowVolumeSceneNode::createZFailVolume(u32 faceCount,
						const core::vector3df& light,
						SShadowVolume* svp)
{
	u32 i;
	u32 numEdges=0;
	const core::vector3df ls = light * Infinity;

	// Check every face if it is front or back facing the light.
	for (i=0; i<faceCount; ++i)
	{
		const core::vector3df v0 = Vertices[Indices[3*i+0]];
		const core::vector3df v1 = Vertices[Indices[3*i+1]];
		const core::vector3df v2 = Vertices[Indices[3*i+2]];

		FaceData[i]=core::triangle3df(v0,v1,v2).isFrontFacing(light);
		if (FaceData[i])
		{
			if (svp->size() < svp->allocated_size()-5)
			{
				// add front cap from light-facing faces
				svp->push_back(v0);
				svp->push_back(v2);
				svp->push_back(v1);

				// add back cap
				svp->push_back(v0-ls);
				svp->push_back(v1-ls);
				svp->push_back(v2-ls);
			}
		}
	}

	// Create edges
	for(i=0; i<faceCount; ++i)
	{
		// check all front facing faces
		if (FaceData[i] == true)
		{
			const u16 wFace0 = Indices[3*i+0];
			const u16 wFace1 = Indices[3*i+1];
			const u16 wFace2 = Indices[3*i+2];

			const u16 adj0 = Adjacency[3*i+0];
			const u16 adj1 = Adjacency[3*i+1];
			const u16 adj2 = Adjacency[3*i+2];

			if (adj0 == i || FaceData[adj0] == false)
			{
				// add edge v0-v1
				Edges[2*numEdges+0] = wFace0;
				Edges[2*numEdges+1] = wFace1;
				++numEdges;
			}

			if (adj1 == i || FaceData[adj1] == false)
			{
				// add edge v1-v2
				Edges[2*numEdges+0] = wFace1;
				Edges[2*numEdges+1] = wFace2;
				++numEdges;
			}

			if (adj2 == i || FaceData[adj2] == false)
			{
				// add edge v2-v0
				Edges[2*numEdges+0] = wFace2;
				Edges[2*numEdges+1] = wFace0;
				++numEdges;
			}
		}
	}
	return numEdges;
}


u32 CShadowVolumeSceneNode::createZPassVolume(u32 faceCount,
						core::vector3df light,
						SShadowVolume* svp, bool caps)
{
	u32 numEdges=0;
	light *= Infinity;
	if (light == core::vector3df(0,0,0))
		light = core::vector3df(0.0001f,0.0001f,0.0001f);

	for (u32 i=0; i<faceCount; ++i)
	{
		const u16 wFace0 = Indices[3*i+0];
		const u16 wFace1 = Indices[3*i+1];
		const u16 wFace2 = Indices[3*i+2];

		if (core::triangle3df(Vertices[wFace0],Vertices[wFace1],Vertices[wFace2]).isFrontFacing(light))
		{
			Edges[2*numEdges+0] = wFace0;
			Edges[2*numEdges+1] = wFace1;
			++numEdges;

			Edges[2*numEdges+0] = wFace1;
			Edges[2*numEdges+1] = wFace2;
			++numEdges;

			Edges[2*numEdges+0] = wFace2;
			Edges[2*numEdges+1] = wFace0;
			++numEdges;

			if (caps && svp->size() < svp->allocated_size()-5)
			{
				svp->push_back(Vertices[wFace0]);
				svp->push_back(Vertices[wFace2]);
				svp->push_back(Vertices[wFace1]);

				svp->push_back(Vertices[wFace0] - light);
				svp->push_back(Vertices[wFace1] - light);
				svp->push_back(Vertices[wFace2] - light);
			}
		}
	}
	return numEdges;
}


void CShadowVolumeSceneNode::setShadowMesh(const IMesh* mesh)
{
    if (ShadowMesh == mesh)
		return;
	if (ShadowMesh)
		ShadowMesh->drop();
	ShadowMesh = mesh;
	if (ShadowMesh)
	{
		ShadowMesh->grab();
		Box = ShadowMesh->getBoundingBox();
	}
}


void CShadowVolumeSceneNode::updateShadowVolumes()
{
	const u32 oldIndexCount = IndexCount;
	const u32 oldVertexCount = VertexCount;

	const IMesh* const mesh = ShadowMesh;
	if (!mesh)
		return;

	// create as much shadow volumes as there are lights but
	// do not ignore the max light settings.
	const u32 lights = SceneManager->getVideoDriver()->getDynamicLightCount();
	if (!lights)
		return;

	// calculate total amount of vertices and indices

	VertexCount = 0;
	IndexCount = 0;
	ShadowVolumesUsed = 0;

	u32 i;
	u32 totalVertices = 0;
	u32 totalIndices = 0;
	const u32 bufcnt = mesh->getMeshBufferCount();

	for (i=0; i<bufcnt; ++i)
	{
		const IMeshBuffer* buf = mesh->getMeshBuffer(i);
		totalIndices += buf->getIndexCount();
		totalVertices += buf->getVertexCount();
	}

	// allocate memory if necessary

	Vertices.set_used(totalVertices);
	Indices.set_used(totalIndices);

	if (UseZFailMethod)
		FaceData.set_used(totalIndices / 3);

	// copy mesh
	for (i=0; i<bufcnt; ++i)
	{
		const IMeshBuffer* buf = mesh->getMeshBuffer(i);

		const u16* idxp = buf->getIndices();
		const u16* idxpend = idxp + buf->getIndexCount();
		for (; idxp!=idxpend; ++idxp)
			Indices[IndexCount++] = *idxp + VertexCount;

		const u32 vtxcnt = buf->getVertexCount();
		for (u32 j=0; j<vtxcnt; ++j)
			Vertices[VertexCount++] = buf->getPosition(j);
	}

	// recalculate adjacency if necessary
	if ((oldVertexCount != VertexCount || oldIndexCount != IndexCount) && UseZFailMethod)
		calculateAdjacency();

	core::matrix4 mat = Parent->getAbsoluteTransformation();
	mat.makeInverse();
	const core::vector3df parentpos = Parent->getAbsolutePosition();

	// TODO: Only correct for point lights.
	for (i=0; i<lights; ++i)
	{
		const video::SLight& dl = SceneManager->getVideoDriver()->getDynamicLight(i);
		core::vector3df lpos = dl.Position;
		if (dl.CastShadows &&
			fabs((lpos - parentpos).getLengthSQ()) <= (dl.Radius*dl.Radius*4.0f))
		{
			mat.transformVect(lpos);
			createShadowVolume(lpos);
		}
	}
}


//! pre render method
void CShadowVolumeSceneNode::OnRegisterSceneNode()
{
	if (IsVisible)
	{
		SceneManager->registerNodeForRendering(this, scene::ESNRP_SHADOW);
		ISceneNode::OnRegisterSceneNode();
	}
}


//! renders the node.
void CShadowVolumeSceneNode::render()
{
	video::IVideoDriver* driver = SceneManager->getVideoDriver();

	if (!ShadowVolumesUsed || !driver)
		return;

	driver->setTransform(video::ETS_WORLD, Parent->getAbsoluteTransformation());

	for (u32 i=0; i<ShadowVolumesUsed; ++i)
	{
		driver->drawStencilShadowVolume(ShadowVolumes[i].pointer(),ShadowVolumes[i].size(), UseZFailMethod);
	}
}


//! returns the axis aligned bounding box of this node
const core::aabbox3d<f32>& CShadowVolumeSceneNode::getBoundingBox() const
{
	return Box;
}


//! Generates adjacency information based on mesh indices.
void CShadowVolumeSceneNode::calculateAdjacency()
{
	Adjacency.set_used(IndexCount);

	// go through all faces and fetch their three neighbours
	for (u32 f=0; f<IndexCount; f+=3)
	{
		for (u32 edge = 0; edge<3; ++edge)
		{
			const core::vector3df& v1 = Vertices[Indices[f+edge]];
			const core::vector3df& v2 = Vertices[Indices[f+((edge+1)%3)]];

			// now we search an_O_ther _F_ace with these two
			// vertices, which is not the current face.
			u32 of;

			for (of=0; of<IndexCount; of+=3)
			{
				// only other faces
				if (of != f)
				{
					bool cnt1 = false;
					bool cnt2 = false;

					for (s32 e=0; e<3; ++e)
					{
						if (v1.equals(Vertices[Indices[of+e]]))
							cnt1=true;

						if (v2.equals(Vertices[Indices[of+e]]))
							cnt2=true;
					}
					// exactly one match for each vertex, i.e. edge is the same
					if (cnt1 && cnt2)
						break;
				}
			}

			// no adjacent edges -> store face number, else store adjacent face
			if (of >= IndexCount)
				Adjacency[f + edge] = f/3;
			else
				Adjacency[f + edge] = of/3;
		}
	}
}


} // end namespace scene
} // end namespace irr
