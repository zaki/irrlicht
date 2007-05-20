// Copyright (C) 2002-2007 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_MESH_SCENE_NODE_H_INCLUDED__
#define __I_MESH_SCENE_NODE_H_INCLUDED__

#include "ISceneNode.h"

namespace irr
{
namespace scene
{

class IMesh;


//! A scene node displaying a static mesh
class IMeshSceneNode : public ISceneNode
{
public:

	//! constructor
	IMeshSceneNode(ISceneNode* parent, ISceneManager* mgr, s32 id, 
		const core::vector3df& position, const core::vector3df& rotation,
			const core::vector3df& scale)
		: ISceneNode(parent, mgr, id, position, rotation, scale) {}

	//! Sets a new mesh to display
	virtual void setMesh(IMesh* mesh) = 0;

	//! Returns the current mesh
	virtual IMesh* getMesh(void) = 0;

	//! Sets if the scene node should not copy the materials of the mesh but use them in a read only style.
	/* In this way it is possible to change the materials a mesh causing all mesh scene nodes 
	referencing this mesh to change too. */
	virtual void setReadOnlyMaterials(bool readonly) = 0;

	//! Returns if the scene node should not copy the materials of the mesh but use them in a read only style
	virtual bool isReadOnlyMaterials() = 0;
};

} // end namespace scene
} // end namespace irr


#endif

