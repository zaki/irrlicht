// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h
//
// created by Dean Wadsworth aka Varmint Dec 31 2007

#ifndef __I_VOLUME_LIGHT_SCENE_NODE_H_INCLUDED__
#define __I_VOLUME_LIGHT_SCENE_NODE_H_INCLUDED__

#include "ISceneNode.h"

namespace irr
{
namespace scene
{
	class IVolumeLightSceneNode : public ISceneNode
	{
	public:

		//! constructor
		IVolumeLightSceneNode(ISceneNode* parent, ISceneManager* mgr, s32 id,
			const core::vector3df& position,
			const core::vector3df& rotation,
			const core::vector3df& scale)
			: ISceneNode(parent, mgr, id, position, rotation, scale) {};

		//! Returns type of the scene node
		virtual ESCENE_NODE_TYPE getType() const { return ESNT_CUBE; }

		virtual void setSubDivideU (const u32 inU) =0;
		virtual void setSubDivideV (const u32 inV) =0;

		virtual u32 getSubDivideU () const =0;
		virtual u32 getSubDivideV () const =0;

		virtual void setFootColour(const video::SColor inColour) =0;
		virtual void setTailColour(const video::SColor inColour) =0;

		virtual video::SColor getFootColour () const =0;
		virtual video::SColor getTailColour () const =0;
	};

} // end namespace scene
} // end namespace irr

#endif

