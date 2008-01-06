// Copyright (C) 2002-2007 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h
//
// created by Dean Wadsworth aka Varmint Dec 31 2007

#ifndef __VOLUME_LIGHT_SCENE_NODE_H_INCLUDED__
#define __VOLUME_LIGHT_SCENE_NODE_H_INCLUDED__

#include "ISceneNode.h"
#include "SMeshBuffer.h"

namespace irr
{
namespace scene
{
	class CVolumeLightSceneNode : public ISceneNode
	{
	public:

		//! constructor
		CVolumeLightSceneNode(ISceneNode* parent, ISceneManager* mgr, s32 id,
			const s32 subdivU = 32, const s32 subdivV = 32,
			const video::SColor foot = video::SColor(51, 0, 230, 180),
			const video::SColor tail = video::SColor(0, 0, 0, 0),
			const core::vector3df& position = core::vector3df(0,0,0),
			const core::vector3df& rotation = core::vector3df(0,0,0),
			const core::vector3df& scale = core::vector3df(1.0f, 1.0f, 1.0f));

		//! destructor
		virtual ~CVolumeLightSceneNode();

		virtual void OnRegisterSceneNode();

		//! renders the node.
		virtual void render();

		//! returns the axis aligned bounding box of this node
		virtual const core::aabbox3d<f32>& getBoundingBox() const;

		//! returns the material based on the zero based index i. To get the amount
		//! of materials used by this scene node, use getMaterialCount().
		//! This function is needed for inserting the node into the scene hirachy on a
		//! optimal position for minimizing renderstate changes, but can also be used
		//! to directly modify the material of a scene node.
		virtual video::SMaterial& getMaterial(u32 i);

		//! returns amount of materials used by this scene node.
		virtual u32 getMaterialCount() const;

		//! Returns type of the scene node
		virtual ESCENE_NODE_TYPE getType() const { return ESNT_CUBE; }

		//! Writes attributes of the scene node.
		virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options=0) const;

		//! Reads attributes of the scene node.
		virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options=0);

		//! Creates a clone of this scene node and its children.
		virtual ISceneNode* clone(ISceneNode* newParent=0, ISceneManager* newManager=0);
		
		void setSubDivideU (const s32 inU) { mSubdivideU = inU; }
		void setSubDivideV (const s32 inV) { mSubdivideV = inV; }
		
		s32 getSubDivideU () const { return mSubdivideU; }
		s32 getSubDivideV () const { return mSubdivideV; }
		
		void setFootColour(const video::SColor inColouf) { mfootColour = inColouf; }
		void setTailColour(const video::SColor inColouf) { mtailColour = inColouf; }
		
		video::SColor getFootColour () const { return mfootColour; }
		video::SColor getTailColour () const { return mtailColour; }		

	private:
		void addToBuffer(video::S3DVertex v);
		void constructLight();

		SMeshBuffer * Buffer;
		
		f32  mlpDistance;		// Distance to hypothetical lightsource point -- affects fov angle

		s32  mSubdivideU;		// Number of subdivisions in U and V space.
		s32  mSubdivideV;		// Controls the number of "slices" in the volume.
		// NOTE : Total number of polygons = 2 + ((mSubdiveU + 1) + (mSubdivideV + 1)) * 2
		// Each slice being a quad plus the rectangular plane at the bottom.

		video::SColor mfootColour;		// Color at the source
		video::SColor mtailColour;		// Color at the end.
		
		core::vector3df lightDimensions; // lightDimensions.Y Distance of shooting -- Length of beams
										 // lightDimensions.X and lightDimensions.Z determine the size/dimension of the plane
	};

} // end namespace scene
} // end namespace irr

#endif
