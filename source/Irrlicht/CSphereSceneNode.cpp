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
CSphereSceneNode::CSphereSceneNode(f32 Radius, u32 polyCountX, u32 polyCountY, ISceneNode* parent, ISceneManager* mgr, s32 id,
			const core::vector3df& position, const core::vector3df& rotation, const core::vector3df& scale)
: ISceneNode(parent, mgr, id, position, rotation, scale), Radius(Radius),
	PolyCountX(polyCountX), PolyCountY(polyCountY)
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

	if (PolyCountX < 2)
		PolyCountX = 2;
	if (PolyCountY < 2)
		PolyCountY = 2;
	if (PolyCountX * PolyCountY > 32767) // prevent u16 overflow
		if (PolyCountX > PolyCountY) // prevent u16 overflow
			PolyCountX = 32767/PolyCountY-1;
		else
			PolyCountY = 32767/(PolyCountX+1);

	u32 PolyCountXPitch = PolyCountX+1; // get to same vertex on next level
	Buffer.Vertices.set_used((PolyCountXPitch * PolyCountY) + 2);
	Buffer.Indices.set_used((PolyCountX * PolyCountY) * 6);

	video::SColor clr(100, 255,255,255);

	u32 i=0;
	u32 level = 0;

	for (u32 p1 = 0; p1 < PolyCountY-1; ++p1)
	{
		//main quads, top to bottom
		for (u32 p2 = 0; p2 < PolyCountX - 1; ++p2)
		{
			const u32 curr = level + p2;
			Buffer.Indices[i] = curr + PolyCountXPitch;
			Buffer.Indices[++i] = curr;
			Buffer.Indices[++i] = curr + 1;
			Buffer.Indices[++i] = curr + PolyCountXPitch;
			Buffer.Indices[++i] = curr+1;
			Buffer.Indices[++i] = curr + 1 + PolyCountXPitch;
			++i;
		}

		// the connectors from front to end
		Buffer.Indices[i] = level + PolyCountX - 1 + PolyCountXPitch;
		Buffer.Indices[++i] = level + PolyCountX - 1;
		Buffer.Indices[++i] = level + PolyCountX;
		++i;

		Buffer.Indices[i] = level + PolyCountX - 1 + PolyCountXPitch;
		Buffer.Indices[++i] = level + PolyCountX;
		Buffer.Indices[++i] = level + PolyCountX + PolyCountXPitch;
		++i;
		level += PolyCountXPitch;
	}

	const u32 PolyCountSq = PolyCountXPitch * PolyCountY; // top point
	const u32 PolyCountSq1 = PolyCountSq + 1; // bottom point
	const u32 PolyCountSqM1 = (PolyCountY - 1) * PolyCountXPitch; // last row's first vertex

	for (u32 p2 = 0; p2 < PolyCountX - 1; ++p2)
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

	// create final triangle which is at the top of the sphere

	Buffer.Indices[i] = PolyCountSq;
	Buffer.Indices[++i] = PolyCountX;
	Buffer.Indices[++i] = PolyCountX-1;
	++i;

	// create final triangle which is at the bottom of the sphere

	Buffer.Indices[i] = PolyCountSqM1 + PolyCountX - 1;
	Buffer.Indices[++i] = PolyCountSqM1;
	Buffer.Indices[++i] = PolyCountSq1;

	// calculate the angle which separates all points in a circle
	const f64 AngleX = 2 * core::PI / PolyCountX;
	const f64 AngleY = core::PI / PolyCountY;

	i = 0;
	f64 axz;

	// we don't start at 0.

	f64 ay = 0;//AngleY / 2;

	for (u32 y = 0; y < PolyCountY; ++y)
	{
		ay += AngleY;
		const f64 sinay = sin(ay);
		axz = 0;

		// calculate the necessary vertices without the doubled one
		for (u32 xz = 0;xz < PolyCountX; ++xz)
		{
			// calculate points position

			const core::vector3df pos(Radius * cos(axz) * sinay,
						Radius * cos(ay),
						Radius * sin(axz) * sinay);
			// for spheres the normal is the position
			core::vector3df normal(pos);
			normal.normalize();

			// calculate texture coordinates via sphere mapping
			// tu is the same on each level, so only calculate once
			f32 tu = 0.5f;
			if (y==0)
			{
				if (normal.Y != -1.0f && normal.Y != 1.0f)
					tu = (f32)(acos(core::clamp(normal.X/sinay, -1.0, 1.0)) * 0.5 *core::RECIPROCAL_PI64);
				if (normal.Z < 0.0f)
					tu=1-tu;
			}
			else
				tu = Buffer.Vertices[i-PolyCountXPitch].TCoords.X;
			Buffer.Vertices[i] = video::S3DVertex(pos.X, pos.Y, pos.Z,
						normal.X, normal.Y, normal.Z,
						clr, tu,
						(f32)(ay*core::RECIPROCAL_PI64));
			++i;
			axz += AngleX;
		}
		// This is the doubled vertex on the initial position
		Buffer.Vertices[i] = video::S3DVertex(Buffer.Vertices[i-PolyCountX]);
		Buffer.Vertices[i].TCoords.X=1.0f;
		++i;
	}

	// the vertex at the top of the sphere
	Buffer.Vertices[i] = video::S3DVertex(0.0f,Radius,0.0f, 0.0f,1.0f,0.0f, clr, 0.5f, 0.0f);

	// the vertex at the bottom of the sphere
	++i;
	Buffer.Vertices[i] = video::S3DVertex(0.0f,-Radius,0.0f, 0.0f,-1.0f,0.0f, clr, 0.5f, 1.0f);

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
	out->addInt("PolyCountX", PolyCountX);
	out->addInt("PolyCountY", PolyCountY);
}


//! Reads attributes of the scene node.
void CSphereSceneNode::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options)
{
	f32 oldRadius = Radius;
	u32 oldPolyCountX = PolyCountX;
	u32 oldPolyCountY = PolyCountY;

	Radius = in->getAttributeAsFloat("Radius");
	PolyCountX = in->getAttributeAsInt("PolyCountX");
	PolyCountY = in->getAttributeAsInt("PolyCountY");
	// legacy values read for compatibility with older versions
	u32 polyCount = in->getAttributeAsInt("PolyCount");
	if (PolyCountX ==0 && PolyCountY == 0)
		PolyCountX = PolyCountY = polyCount;

	Radius = irr::core::max_(Radius, 0.0001f);

	if ( !core::equals(Radius, oldRadius) || PolyCountX != oldPolyCountX || PolyCountY != oldPolyCountY)
		setSizeAndPolys();

	ISceneNode::deserializeAttributes(in, options);
}

//! Creates a clone of this scene node and its children.
ISceneNode* CSphereSceneNode::clone(ISceneNode* newParent, ISceneManager* newManager)
{
	if (!newParent) newParent = Parent;
	if (!newManager) newManager = SceneManager;

	CSphereSceneNode* nb = new CSphereSceneNode(Radius, PolyCountX, PolyCountY, newParent, 
		newManager, ID, RelativeTranslation);

	nb->cloneMembers(this, newManager);
	nb->Buffer.Material = Buffer.Material;

	nb->drop();
	return nb;
}

} // end namespace scene
} // end namespace irr

