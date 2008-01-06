// Copyright (C) 2002-2007 Nikolaus Gebhardt
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
		s32 id, const s32 subdivU, const s32 subdivV,
		const video::SColor foot,
		const video::SColor tail,
		const core::vector3df& position,
		const core::vector3df& rotation, const core::vector3df& scale)
	: ISceneNode(parent, mgr, id, position, rotation, scale)
{
	#ifdef _DEBUG
	setDebugName("CVolumeLightSceneNode");
	#endif

	video::IVideoDriver* driver = SceneManager->getVideoDriver();
	
	lightDimensions = core::vector3df(1.0f, 1.2f, 1.0f);

	mlpDistance = 8.0f;

	mSubdivideU = subdivU;
	mSubdivideV = subdivV;

	//test light
	mfootColour = foot;
	mtailColour = tail;
	
	Buffer = NULL;
	
	constructLight();
}


//! destructor
CVolumeLightSceneNode::~CVolumeLightSceneNode()
{
	if (Buffer)
		Buffer->drop();
}

void CVolumeLightSceneNode::addToBuffer(video::S3DVertex v)
{
	s32 tnidx = Buffer->Vertices.linear_reverse_search(v);
	bool alreadyIn = (tnidx != -1);
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
	core::vector3df lightPoint = core::vector3df(0, -(mlpDistance*lightDimensions.Y), 0);
	f32 ax = lightDimensions.X / 2.0f; // X Axis
	f32 az = lightDimensions.Z / 2.0f; // Z Axis
	
	if (Buffer)
		Buffer->drop();
	Buffer = new SMeshBuffer();
	
	//draw the bottom foot.. the glowing region
	addToBuffer(video::S3DVertex(-ax, 0, az,  0,0,0, mfootColour, 0, 1));
	addToBuffer(video::S3DVertex(ax , 0, az,  0,0,0, mfootColour, 1, 1));
	addToBuffer(video::S3DVertex(ax , 0,-az,  0,0,0, mfootColour, 1, 0));

	addToBuffer(video::S3DVertex(ax , 0,-az,  0,0,0, mfootColour, 1, 0));
	addToBuffer(video::S3DVertex(-ax, 0,-az,  0,0,0, mfootColour, 0, 0));
	addToBuffer(video::S3DVertex(-ax, 0, az,  0,0,0, mfootColour, 0, 1));

	// Slices in X/U space
	for(s32 i = 0; i <= mSubdivideU; i++)  {
		f32 k = ((f32)i) / mSubdivideU;				// use for the texture coord
		f32	bx = ((lightDimensions.X / (f32)mSubdivideU) * i) - ax;
		//printf("bx: %f\n", bx);
		
		// These are the two endpoints for a slice at the foot
		core::vector3df end1(bx, 0.0f, -az);
		core::vector3df end2(bx, 0.0f, az);
		
		end1 -= lightPoint;		// get a vector from point to lightsource
		end1.normalize();		// normalize vector
		end1 *= lightDimensions.Y;	// multiply it out by shootlength
		
		end1.X += bx;			// Add the original point location to the vector
		end1.Z -= az;
		
		// Do it again for the other point.
		end2 -= lightPoint;
		end2.normalize();
		end2 *= lightDimensions.Y;

		end2.X += bx;
		end2.Z += az;
		
		
		addToBuffer(video::S3DVertex(bx , 0,  az,  0,0,0, mfootColour, k, 1));
		addToBuffer(video::S3DVertex(bx , 0, -az,  0,0,0, mfootColour, k, 0));
		addToBuffer(video::S3DVertex(end2.X , end2.Y, end2.Z,  0,0,0, mtailColour, k, 1));
		
		addToBuffer(video::S3DVertex(bx , 0, -az,  0,0,0, mfootColour, k, 0));
		addToBuffer(video::S3DVertex(end1.X , end1.Y, end1.Z,  0,0,0, mtailColour, k, 0));
		addToBuffer(video::S3DVertex(end2.X , end2.Y, end2.Z,  0,0,0, mtailColour, k, 1));
		
		//back side
		addToBuffer(video::S3DVertex(-end2.X , end2.Y, -end2.Z,  0,0,0, mtailColour, k, 1));
		addToBuffer(video::S3DVertex(-bx , 0,  -az,  0,0,0, mfootColour, k, 1));
		addToBuffer(video::S3DVertex(-bx , 0, az,  0,0,0, mfootColour, k, 0));
		
		addToBuffer(video::S3DVertex(-bx , 0, az,  0,0,0, mfootColour, k, 0));
		addToBuffer(video::S3DVertex(-end1.X , end1.Y, -end1.Z,  0,0,0, mtailColour, k, 0));
		addToBuffer(video::S3DVertex(-end2.X , end2.Y, -end2.Z,  0,0,0, mtailColour, k, 1));
		
	}

	// Slices in Z/V space
	for(s32 i = 0; i <= mSubdivideV; i++)  {
		f32 k = ((f32)i) / mSubdivideV;				// use for the texture coord
		f32	bz = ((lightDimensions.Z / (f32)mSubdivideV) * i) - az;
		
		// These are the two endpoints for a slice at the foot
		core::vector3df end1(-ax, 0.0f, bz);
		core::vector3df end2(ax, 0.0f, bz);
		
		end1 -= lightPoint;		// get a vector from point to lightsource
		end1.normalize();		// normalize vector
		end1 *= lightDimensions.Y;	// multiply it out by shootlength
		
		end1.X -= ax;			// Add the original point location to the vector
		end1.Z += bz;
		
		// Do it again for the other point.
		end2 -= lightPoint;
		end2.normalize();
		end2 *= lightDimensions.Y;

		end2.X += ax;
		end2.Z += bz;
		
		addToBuffer(video::S3DVertex(-ax , 0, bz,  0,0,0, mfootColour, 0, k));
		addToBuffer(video::S3DVertex(ax , 0,  bz,  0,0,0, mfootColour, 1, k));
		addToBuffer(video::S3DVertex(end2.X , end2.Y, end2.Z,  0,0,0, mtailColour, 1, k));
		
		addToBuffer(video::S3DVertex(end2.X , end2.Y, end2.Z,  0,0,0, mtailColour, 1, k));
		addToBuffer(video::S3DVertex(end1.X , end1.Y, end1.Z,  0,0,0, mtailColour, 0, k));
		addToBuffer(video::S3DVertex(-ax , 0, bz,  0,0,0, mfootColour, 0, k));
		
		//back side
		addToBuffer(video::S3DVertex(ax , 0, -bz,  0,0,0, mfootColour, 0, k));
		addToBuffer(video::S3DVertex(-ax , 0,  -bz,  0,0,0, mfootColour, 1, k));
		addToBuffer(video::S3DVertex(-end2.X , end2.Y, -end2.Z,  0,0,0, mtailColour, 1, k));
		
		addToBuffer(video::S3DVertex(-end2.X , end2.Y, -end2.Z,  0,0,0, mtailColour, 1, k));
		addToBuffer(video::S3DVertex(-end1.X , end1.Y, -end1.Z,  0,0,0, mtailColour, 0, k));
		addToBuffer(video::S3DVertex(ax , 0, -bz,  0,0,0, mfootColour, 0, k));
		
	}

	Buffer->BoundingBox.reset(0,0,0); 

	Buffer->recalculateBoundingBox();
	
	Buffer->Material.MaterialType = video::EMT_ONETEXTURE_BLEND;
	Buffer->Material.MaterialTypeParam = pack_texureBlendFunc( video::EBF_SRC_COLOR, video::EBF_SRC_ALPHA, video::EMFN_MODULATE_1X );

	Buffer->Material.Lighting = false;
	Buffer->Material.ZWriteEnable = false;
}

//! renders the node.
void CVolumeLightSceneNode::render()
{
	video::IVideoDriver* driver = SceneManager->getVideoDriver();
	driver->setTransform(video::ETS_WORLD, AbsoluteTransformation);

	driver->setMaterial(Buffer->Material);
	driver->drawVertexPrimitiveList(
			Buffer->getVertices(), Buffer->getVertexCount(),
			Buffer->getIndices(), Buffer->getIndexCount() / 3 ,
			Buffer->getVertexType(), EPT_TRIANGLES);
}


//! returns the axis aligned bounding box of this node
const core::aabbox3d<f32>& CVolumeLightSceneNode::getBoundingBox() const
{
	return Buffer->BoundingBox;
}


void CVolumeLightSceneNode::OnRegisterSceneNode()
{
	if (IsVisible) {
		//lie to sceneManager
		Buffer->Material.MaterialType = video::EMT_TRANSPARENT_ADD_COLOR;
		Buffer->Material.MaterialTypeParam = 0.01f;
		SceneManager->registerNodeForRendering(this, ESNRP_AUTOMATIC);
		
		//restore state
		Buffer->Material.MaterialType = video::EMT_ONETEXTURE_BLEND;
		Buffer->Material.MaterialTypeParam = pack_texureBlendFunc( video::EBF_SRC_COLOR, video::EBF_SRC_ALPHA, video::EMFN_MODULATE_1X );
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


//! Writes attributes of the scene node.
void CVolumeLightSceneNode::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options) const
{
	ISceneNode::serializeAttributes(out, options);

	out->addFloat("lpDistance", mlpDistance);
	out->addInt("subDivideU", mSubdivideU);
	out->addInt("subDivideV", mSubdivideV);
	
	out->addColor("footColour", mfootColour);
	out->addColor("tailColour", mtailColour);
	
	out->addVector3d("lightDimension", lightDimensions);
}


//! Reads attributes of the scene node.
void CVolumeLightSceneNode::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options)
{
	mlpDistance =	in->getAttributeAsFloat("lpDistance");
	mlpDistance = core::max_(mlpDistance, 8.0f);
	
	mSubdivideU =	in->getAttributeAsInt("subDivideU");
	mSubdivideU = core::max_(mSubdivideU, 1);
	
	mSubdivideV =	in->getAttributeAsInt("subDivideV");
	mSubdivideV = core::max_(mSubdivideV, 1);

	mfootColour =	in->getAttributeAsColor("footColour");
	mtailColour =	in->getAttributeAsColor("tailColour");
	
	lightDimensions = in->getAttributeAsVector3d("lightDimension");
	
	constructLight();

	ISceneNode::deserializeAttributes(in, options);
}


//! Creates a clone of this scene node and its children.
ISceneNode* CVolumeLightSceneNode::clone(ISceneNode* newParent, ISceneManager* newManager)
{
	if (!newParent) newParent = Parent;
	if (!newManager) newManager = SceneManager;

	CVolumeLightSceneNode* nb = new CVolumeLightSceneNode(newParent, 
		newManager, ID, mSubdivideU, mSubdivideV, mfootColour, mtailColour, RelativeTranslation);

	nb->cloneMembers(this, newManager);
	nb->Buffer->Material = Buffer->Material;

	nb->drop();
	return nb;
}


} // end namespace scene
} // end namespace irr
