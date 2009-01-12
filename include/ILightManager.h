// Written by Colin MacDonald - all rights assigned to Nikolaus Gebhardt
// Copyright (C) 2008-2009 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_LIGHT_MANAGER_H_INCLUDED__
#define __I_LIGHT_MANAGER_H_INCLUDED__

#include "IReferenceCounted.h"
#include "irrArray.h"

namespace irr
{
namespace scene
{
	class ILightSceneNode;

	//! Provides callbacks to when the the scene graph is rendered.
	/** ILightManager is an optional manager that the user application can 
		register in order to receive callbacks as the scene is rendered.
		The OnPreRender() callback provides the user application with a list of the
		lights in the scene.  This list can be stored and used to turn lights
		on and off as each scene node is rendered in order to provide per-node
		or 'local' lights.
		It is assumed that the ILightManager implementation will store any
		data that it wishes to retain, i.e. the ISceneManager to which it is assigned, 
		the lightList, the current render pass, and the current scene node.
	*/
	class ILightManager : public IReferenceCounted
	{
	public:
		//! Called after the scene's light list has been built, but before rendering has begun.
		//! As actual device/hardware lights are not created until the ESNRP_LIGHT render pass,
		//! this provides an opportunity for the light manager to trim or re-order the light
		//! list, before any device/hardware lights have actually been created.
		//! \param[in] smgr: the Scene Manager
		//! \param[inout] lightLight: the Scene Manager's light list, which the light manager may
		//!					modify. This reference will remain valid until OnPostRender().
		virtual void OnPreRender(core::array<ILightSceneNode*> & lightList) = 0;

		//! Called after the last scene node is rendered.
		//! After this call returns, the lightList passed to OnPreRender() becomes invalid.
		virtual void OnPostRender(void) = 0;

		//! Called before a render pass begins
		//! \param[in] renderPass: the render pass that's about to begin
		virtual void OnRenderPassPreRender(E_SCENE_NODE_RENDER_PASS renderPass) = 0;

		//! Called after the render pass specified in OnRenderPassPreRender() ends
		//! \param[in] renderPass: the render pass that has finished
		virtual void OnRenderPassPostRender(E_SCENE_NODE_RENDER_PASS renderPass) = 0;

		//! Called before the given scene node is rendered
		//! \param[in] node: the scene node that's about to be rendered
		virtual void OnNodePreRender(ISceneNode* node) = 0;

		//! Called after the the node specified in OnNodePreRender() has been rendered
		//! \param[in] node: the scene node that has just been rendered
		virtual void OnNodePostRender(ISceneNode* node) = 0;
	};
} // end namespace scene
} // end namespace irr

#endif
