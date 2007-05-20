// Copyright (C) 2002-2007 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CSphereSceneNode.h"
#include "IVideoDriver.h"
#include "ISceneManager.h"
#include "S3DVertex.h"
#include "os.h"

namespace irr
{
namespace scene
{

//! constructor
CSphereSceneNode::CSphereSceneNode(f32 Radius, s32 polyCount, ISceneNode* parent, ISceneManager* mgr, s32 id,
			const core::vector3df& position, const core::vector3df& rotation, const core::vector3df& scale)
: ISceneNode(parent, mgr, id, position, rotation, scale), Radius(Radius),
	PolyCount(polyCount)
{
	#ifdef _DEBUG
	setDebugName("CSphereSceneNode");
	#endif

	setSizeAndPolys();
}



//! destructor
CSphereSceneNode::~CSphereSceneNode()
{
}


void CSphereSceneNode::setSizeAndPolys()
{
	// thanks to Alfaz93 who made his code available for Irrlicht on which
	// this one is based!

	// we are creating the sphere mesh here.

	if (PolyCount < 2)
		PolyCount = 2;
	else
	if (PolyCount > 181) // prevent u16 overflow
		PolyCount = 181;

	Buffer.Vertices.set_used((PolyCount * PolyCount) + 2);
	Buffer.Indices.set_used((PolyCount * PolyCount) * 6);

	video::SColor clr(100, 255,255,255);

	int i=0;
	int level = 0;

	for (int p1=0; p1<PolyCount-1; ++p1)
	{
		level = p1 * PolyCount;
		int p2;

		for (p2 = 0; p2 < PolyCount - 1; ++p2)
		{
			Buffer.Indices[i] = level + p2 + PolyCount;
			Buffer.Indices[++i] = level + p2;
			Buffer.Indices[++i] = level + p2 + 1;
			++i;
		}

		Buffer.Indices[i] = level + PolyCount - 1 + PolyCount;
		Buffer.Indices[++i] = level + PolyCount - 1;
		Buffer.Indices[++i] = level;
		++i;

		Buffer.Indices[i] = level + PolyCount - 1 + PolyCount;
		Buffer.Indices[++i] = level;
		Buffer.Indices[++i] = level + PolyCount;
		++i;

		for (p2 = 1; p2 <= PolyCount - 1; ++p2)
		{
			Buffer.Indices[i] = level + p2 - 1 + PolyCount;
			Buffer.Indices[++i] = level + p2;
			Buffer.Indices[++i] = level + p2 + PolyCount;
			++i;
		}
	}

	int PolyCountSq = PolyCount * PolyCount;
	int PolyCountSq1 = PolyCountSq + 1;
	int PolyCountSqM1 = (PolyCount - 1) * PolyCount;

	for (int p2 = 0; p2 < PolyCount - 1; ++p2)
	{
		// create triangles which are at the top of the sphere

		Buffer.Indices[i] = PolyCountSq;
		Buffer.Indices[++i] = p2 + 1;
		Buffer.Indices[++i] = p2;
		++i;

		// create triangles which are at the bottom of the sphere

		Buffer.Indices[i] = PolyCountSqM1 + p2;
		Buffer.Indices[++i] = PolyCountSqM1 + p2 + 1;
		Buffer.Indices[++i] = PolyCountSq1;
		++i;
	}

	// create a triangle which is at the top of the sphere

	Buffer.Indices[i] = PolyCountSq;
	Buffer.Indices[++i] = 0;
	Buffer.Indices[++i] = PolyCount - 1;
	++i;

	// create a triangle which is at the bottom of the sphere

	Buffer.Indices[i] = PolyCountSqM1 + PolyCount - 1;
	Buffer.Indices[++i] = PolyCountSqM1;
	Buffer.Indices[++i] = PolyCountSq1;

	// calculate the angle which separates all points in a circle

	f32 Angle = 2 * core::PI / (f32)PolyCount;
	f32 sinay;
	f32 cosay;
	f32 sinaxz;
	f32 cosaxz;

	i = 0;
	f32 axz;

	// we don't start at 0.

	f32 ay = -Angle / 4;

	for (int y = 0; y < PolyCount; ++y)
	{
		ay += Angle / 2;
		axz = 0;

		for (int xz = 0;xz < PolyCount; ++xz)
		{
			// calculate points position

			axz += Angle;
			sinay = sin(ay) * Radius;
			cosay = cos(ay) * Radius;
			cosaxz = cos(axz);
			sinaxz = sin(axz);

			core::vector3df pos(cosaxz * sinay, cosay, sinaxz * sinay);
			core::vector3df normal(pos);
			normal.normalize();

			Buffer.Vertices[i] = video::S3DVertex(pos.X, pos.Y, pos.Z,
						normal.X, normal.Y, normal.Z,
						clr, 
						(f32)(asin(normal.X)/core::PI*2) + 0.5f,
						(f32)(acos(normal.Y)/core::PI*2) + 0.5f);

			++i;
		}
	}

	// the vertex at the top of the sphere
	Buffer.Vertices[i] = video::S3DVertex(0.0f,Radius,0.0f, 0.0f,1.0f,0.0f, clr, 0.5f, 0.5f);

	// the vertex at the bottom of the sphere
	++i;
	Buffer.Vertices[i] = video::S3DVertex(0.0f,-Radius,0.0f, 0.0f,-1.0f,0.0f, clr, 0.5f, 0.5f);

	// recalculate bounding box

	Buffer.BoundingBox.reset(Buffer.Vertices[i].Pos);
	Buffer.BoundingBox.addInternalPoint(Buffer.Vertices[i-1].Pos);
	Buffer.BoundingBox.addInternalPoint(Radius,0.0f,0.0f);
	Buffer.BoundingBox.addInternalPoint(-Radius,0.0f,0.0f);
	Buffer.BoundingBox.addInternalPoint(0.0f,0.0f,Radius);
	Buffer.BoundingBox.addInternalPoint(0.0f,0.0f,-Radius);
}



//! renders the node.
void CSphereSceneNode::render()
{
	video::IVideoDriver* driver = SceneManager->getVideoDriver();

	if (Buffer.Vertices.size() && Buffer.Indices.size())
	{
		driver->setMaterial(Buffer.Material);
		driver->setTransform(video::ETS_WORLD, AbsoluteTransformation);
		driver->drawMeshBuffer(&Buffer);
		if ( DebugDataVisible & scene::EDS_BBOX )
		{
			video::SMaterial m;
			m.Lighting = false;
			driver->setMaterial(m);
			driver->draw3DBox(Buffer.BoundingBox, video::SColor(255,255,255,255));
		}
	}
}



//! returns the axis aligned bounding box of this node
const core::aabbox3d<f32>& CSphereSceneNode::getBoundingBox() const
{
	return Buffer.BoundingBox;
}


void CSphereSceneNode::OnRegisterSceneNode()
{
	if (IsVisible)
		SceneManager->registerNodeForRendering(this);

	ISceneNode::OnRegisterSceneNode();
}


//! returns the material based on the zero based index i. To get the amount
//! of materials used by this scene node, use getMaterialCount().
//! This function is needed for inserting the node into the scene hirachy on a
//! optimal position for minimizing renderstate changes, but can also be used
//! to directly modify the material of a scene node.
video::SMaterial& CSphereSceneNode::getMaterial(u32 i)
{
	return Buffer.Material;
}


//! returns amount of materials used by this scene node.
u32 CSphereSceneNode::getMaterialCount()
{
	return 1;
}


//! Writes attributes of the scene node.
void CSphereSceneNode::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options)
{
	ISceneNode::serializeAttributes(out, options);

	out->addFloat("Radius", Radius);
	out->addInt("PolyCount", PolyCount);
}


//! Reads attributes of the scene node.
void CSphereSceneNode::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options)
{
	f32 oldRadius = Radius;
	s32 oldPolyCount = PolyCount;

	Radius = in->getAttributeAsFloat("Radius");
	PolyCount = in->getAttributeAsInt("PolyCount");

	Radius = irr::core::max_(Radius, 0.0001f);

	if ( !core::equals(Radius, oldRadius) || PolyCount != oldPolyCount)
		setSizeAndPolys();

	ISceneNode::deserializeAttributes(in, options);
}

//! Creates a clone of this scene node and its children.
ISceneNode* CSphereSceneNode::clone(ISceneNode* newParent, ISceneManager* newManager)
{
	if (!newParent) newParent = Parent;
	if (!newManager) newManager = SceneManager;

	CSphereSceneNode* nb = new CSphereSceneNode(Radius, PolyCount, newParent, 
		newManager, ID, RelativeTranslation);

	nb->cloneMembers(this, newManager);
	nb->Buffer.Material = Buffer.Material;

	nb->drop();
	return nb;
}

} // end namespace scene
} // end namespace irr

