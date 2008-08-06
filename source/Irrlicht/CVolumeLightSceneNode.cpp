// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h
//
// created by Dean Wadsworth aka Varmint Dec 31 2007

#include "CVolumeLightSceneNode.h"
#include "IVideoDriver.h"
#include "ISceneManager.h"
#include "S3DVertex.h"
#include "os.h"

namespace irr
{
namespace scene
{

//! constructor
CVolumeLightSceneNode::CVolumeLightSceneNode(ISceneNode* parent, ISceneManager* mgr,
		s32 id, const u32 subdivU, const u32 subdivV,
		const video::SColor foot,
		const video::SColor tail,
		const core::vector3df& position,
		const core::vector3df& rotation, const core::vector3df& scale)
	: IVolumeLightSceneNode(parent, mgr, id, position, rotation, scale),
		LPDistance(8.0f), SubdivideU(subdivU), SubdivideV(subdivV),
		FootColour(foot), TailColour(tail),
		LightDimensions(core::vector3df(1.0f, 1.2f, 1.0f))
{
	#ifdef _DEBUG
	setDebugName("CVolumeLightSceneNode");
	#endif

	Buffer = new SMeshBuffer();
	if (Buffer)
	{
		Buffer->setHardwareMappingHint(EHM_STATIC);
		constructLight();
	}
}


CVolumeLightSceneNode::~CVolumeLightSceneNode()
{
	if (Buffer)
		Buffer->drop();
}


void CVolumeLightSceneNode::addToBuffer(const video::S3DVertex& v)
{
	const s32 tnidx = Buffer->Vertices.linear_reverse_search(v);
	const bool alreadyIn = (tnidx != -1);
	u16 nidx = (u16)tnidx;
	if (!alreadyIn) {
		nidx = Buffer->Vertices.size();
		Buffer->Indices.push_back(nidx);
		Buffer->Vertices.push_back(v);
	} else
		Buffer->Indices.push_back(nidx);
}


void CVolumeLightSceneNode::constructLight()
{
	const core::vector3df lightPoint(0, -(LPDistance*LightDimensions.Y), 0);
	const f32 ax = LightDimensions.X * 0.5f; // X Axis
	const f32 az = LightDimensions.Z * 0.5f; // Z Axis

	Buffer->Vertices.clear();
	Buffer->Vertices.reallocate(6+12*(SubdivideU+SubdivideV));
	Buffer->Indices.clear();
	Buffer->Indices.reallocate(6+12*(SubdivideU+SubdivideV));
	//draw the bottom foot.. the glowing region
	addToBuffer(video::S3DVertex(-ax, 0, az,  0,0,0, FootColour, 0, 1));
	addToBuffer(video::S3DVertex(ax , 0, az,  0,0,0, FootColour, 1, 1));
	addToBuffer(video::S3DVertex(ax , 0,-az,  0,0,0, FootColour, 1, 0));

	addToBuffer(video::S3DVertex(ax , 0,-az,  0,0,0, FootColour, 1, 0));
	addToBuffer(video::S3DVertex(-ax, 0,-az,  0,0,0, FootColour, 0, 0));
	addToBuffer(video::S3DVertex(-ax, 0, az,  0,0,0, FootColour, 0, 1));

	f32 tu = 0.f;
	const f32 tuStep = 1.f/SubdivideU;
	f32 bx = -ax;
	const f32 bxStep = LightDimensions.X * tuStep;
	// Slices in X/U space
	for (u32 i = 0; i <= SubdivideU; ++i)
	{
		// These are the two endpoints for a slice at the foot
		core::vector3df end1(bx, 0.0f, -az);
		core::vector3df end2(bx, 0.0f, az);

		end1 -= lightPoint;		// get a vector from point to lightsource
		end1.normalize();		// normalize vector
		end1 *= LightDimensions.Y;	// multiply it out by shootlength

		end1.X += bx;			// Add the original point location to the vector
		end1.Z -= az;

		// Do it again for the other point.
		end2 -= lightPoint;
		end2.normalize();
		end2 *= LightDimensions.Y;

		end2.X += bx;
		end2.Z += az;

		addToBuffer(video::S3DVertex(bx , 0,  az,  0,0,0, FootColour, tu, 1));
		addToBuffer(video::S3DVertex(bx , 0, -az,  0,0,0, FootColour, tu, 0));
		addToBuffer(video::S3DVertex(end2.X , end2.Y, end2.Z,  0,0,0, TailColour, tu, 1));

		addToBuffer(video::S3DVertex(bx , 0, -az,  0,0,0, FootColour, tu, 0));
		addToBuffer(video::S3DVertex(end1.X , end1.Y, end1.Z,  0,0,0, TailColour, tu, 0));
		addToBuffer(video::S3DVertex(end2.X , end2.Y, end2.Z,  0,0,0, TailColour, tu, 1));

		//back side
		addToBuffer(video::S3DVertex(-end2.X , end2.Y, -end2.Z,  0,0,0, TailColour, tu, 1));
		addToBuffer(video::S3DVertex(-bx , 0,  -az,  0,0,0, FootColour, tu, 1));
		addToBuffer(video::S3DVertex(-bx , 0, az,  0,0,0, FootColour, tu, 0));

		addToBuffer(video::S3DVertex(-bx , 0, az,  0,0,0, FootColour, tu, 0));
		addToBuffer(video::S3DVertex(-end1.X , end1.Y, -end1.Z,  0,0,0, TailColour, tu, 0));
		addToBuffer(video::S3DVertex(-end2.X , end2.Y, -end2.Z,  0,0,0, TailColour, tu, 1));
		tu += tuStep;
		bx += bxStep;
	}

	f32 tv = 0.f;
	const f32 tvStep = 1.f/SubdivideV;
	f32 bz = -az;
	const f32 bzStep = LightDimensions.Z * tvStep;
	// Slices in Z/V space
	for(u32 i = 0; i <= SubdivideV; ++i)
	{
		// These are the two endpoints for a slice at the foot
		core::vector3df end1(-ax, 0.0f, bz);
		core::vector3df end2(ax, 0.0f, bz);

		end1 -= lightPoint;		// get a vector from point to lightsource
		end1.normalize();		// normalize vector
		end1 *= LightDimensions.Y;	// multiply it out by shootlength

		end1.X -= ax;			// Add the original point location to the vector
		end1.Z += bz;

		// Do it again for the other point.
		end2 -= lightPoint;
		end2.normalize();
		end2 *= LightDimensions.Y;

		end2.X += ax;
		end2.Z += bz;

		addToBuffer(video::S3DVertex(-ax , 0, bz,  0,0,0, FootColour, 0, tv));
		addToBuffer(video::S3DVertex(ax , 0,  bz,  0,0,0, FootColour, 1, tv));
		addToBuffer(video::S3DVertex(end2.X , end2.Y, end2.Z,  0,0,0, TailColour, 1, tv));

		addToBuffer(video::S3DVertex(end2.X , end2.Y, end2.Z,  0,0,0, TailColour, 1, tv));
		addToBuffer(video::S3DVertex(end1.X , end1.Y, end1.Z,  0,0,0, TailColour, 0, tv));
		addToBuffer(video::S3DVertex(-ax , 0, bz,  0,0,0, FootColour, 0, tv));

		//back side
		addToBuffer(video::S3DVertex(ax , 0, -bz,  0,0,0, FootColour, 0, tv));
		addToBuffer(video::S3DVertex(-ax , 0,  -bz,  0,0,0, FootColour, 1, tv));
		addToBuffer(video::S3DVertex(-end2.X , end2.Y, -end2.Z,  0,0,0, TailColour, 1, tv));

		addToBuffer(video::S3DVertex(-end2.X , end2.Y, -end2.Z,  0,0,0, TailColour, 1, tv));
		addToBuffer(video::S3DVertex(-end1.X , end1.Y, -end1.Z,  0,0,0, TailColour, 0, tv));
		addToBuffer(video::S3DVertex(ax , 0, -bz,  0,0,0, FootColour, 0, tv));
		tv += tvStep;
		bz += bzStep;
	}

	Buffer->recalculateBoundingBox();

	Buffer->Material.MaterialType = video::EMT_ONETEXTURE_BLEND;
	Buffer->Material.MaterialTypeParam = pack_texureBlendFunc( video::EBF_SRC_COLOR, video::EBF_SRC_ALPHA, video::EMFN_MODULATE_1X );

	Buffer->Material.Lighting = false;
	Buffer->Material.ZWriteEnable = false;

	Buffer->setDirty(EBT_VERTEX_AND_INDEX);
}


//! renders the node.
void CVolumeLightSceneNode::render()
{
	if (!Buffer)
		return;

	video::IVideoDriver* driver = SceneManager->getVideoDriver();
	driver->setTransform(video::ETS_WORLD, AbsoluteTransformation);

	driver->setMaterial(Buffer->Material);
	driver->drawMeshBuffer(Buffer);
}


//! returns the axis aligned bounding box of this node
const core::aabbox3d<f32>& CVolumeLightSceneNode::getBoundingBox() const
{
	return Buffer->BoundingBox;
}


void CVolumeLightSceneNode::OnRegisterSceneNode()
{
	if (IsVisible)
	{
		SceneManager->registerNodeForRendering(this, ESNRP_TRANSPARENT);
	}
	ISceneNode::OnRegisterSceneNode();
}


//! returns the material based on the zero based index i. To get the amount
//! of materials used by this scene node, use getMaterialCount().
//! This function is needed for inserting the node into the scene hirachy on a
//! optimal position for minimizing renderstate changes, but can also be used
//! to directly modify the material of a scene node.
video::SMaterial& CVolumeLightSceneNode::getMaterial(u32 i)
{
	return Buffer->Material;
}


//! returns amount of materials used by this scene node.
u32 CVolumeLightSceneNode::getMaterialCount() const
{
	return 1;
}


void CVolumeLightSceneNode::setSubDivideU (const u32 inU)
{
	SubdivideU = inU;
	constructLight();
}


void CVolumeLightSceneNode::setSubDivideV (const u32 inV)
{
	SubdivideV = inV;
	constructLight();
}


void CVolumeLightSceneNode::setFootColour(const video::SColor inColouf)
{
	FootColour = inColouf;
	constructLight();
}


void CVolumeLightSceneNode::setTailColour(const video::SColor inColouf)
{
	TailColour = inColouf;
	constructLight();
}


//! Writes attributes of the scene node.
void CVolumeLightSceneNode::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options) const
{
	ISceneNode::serializeAttributes(out, options);

	out->addFloat("lpDistance", LPDistance);
	out->addInt("subDivideU", SubdivideU);
	out->addInt("subDivideV", SubdivideV);

	out->addColor("footColour", FootColour);
	out->addColor("tailColour", TailColour);

	out->addVector3d("lightDimension", LightDimensions);
}


//! Reads attributes of the scene node.
void CVolumeLightSceneNode::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options)
{
	LPDistance = in->getAttributeAsFloat("lpDistance");
	LPDistance = core::max_(LPDistance, 8.0f);

	SubdivideU = in->getAttributeAsInt("subDivideU");
	SubdivideU = core::max_(SubdivideU, 1u);

	SubdivideV = in->getAttributeAsInt("subDivideV");
	SubdivideV = core::max_(SubdivideV, 1u);

	FootColour = in->getAttributeAsColor("footColour");
	TailColour = in->getAttributeAsColor("tailColour");

	LightDimensions = in->getAttributeAsVector3d("lightDimension");

	constructLight();

	ISceneNode::deserializeAttributes(in, options);
}


//! Creates a clone of this scene node and its children.
ISceneNode* CVolumeLightSceneNode::clone(ISceneNode* newParent, ISceneManager* newManager)
{
	if (!newParent)
		newParent = Parent;
	if (!newManager)
		newManager = SceneManager;

	CVolumeLightSceneNode* nb = new CVolumeLightSceneNode(newParent,
		newManager, ID, SubdivideU, SubdivideV, FootColour, TailColour, RelativeTranslation);

	nb->cloneMembers(this, newManager);
	nb->Buffer->Material = Buffer->Material;

	nb->drop();
	return nb;
}


} // end namespace scene
} // end namespace irr

