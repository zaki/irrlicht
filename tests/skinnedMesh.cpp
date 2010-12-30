// Copyright (C) 2008-2009 Colin MacDonald
// No rights reserved: this software is in the public domain.

#include "testUtils.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

// Tests skinned meshes.
bool skinnedMesh(void)
{
	// Use EDT_BURNINGSVIDEO since it is not dependent on (e.g.) OpenGL driver versions.
	IrrlichtDevice *device = createDevice( EDT_BURNINGSVIDEO, dimension2d<u32>(160, 120), 32);
	if (!device)
		return false;

	IVideoDriver* driver = device->getVideoDriver();
	ISceneManager * smgr = device->getSceneManager();

	logTestString("Testing setMesh()\n");

	ISkinnedMesh* mesh = (ISkinnedMesh*)smgr->getMesh("../media/ninja.b3d");
	if (!mesh)
		return false;

	IAnimatedMeshSceneNode* node = smgr->addAnimatedMeshSceneNode(mesh);
	if (!node)
		return false;

	// test if certain joint is found
	bool result = (node->getJointNode("Joint1") != 0);

	mesh = (ISkinnedMesh*)smgr->getMesh("../media/dwarf.x");
	if (!mesh)
		return false;
	node->setMesh(mesh);

	// make sure old joint is non-existant anymore
	logTestString("Ignore error message in log, this is intended.\n");
	result &= (node->getJointNode("Joint1")==0);

	// and check that a new joint can be found
	// we use a late one, in order to see also inconsistencies in the joint cache
	result &= (node->getJointNode("cam") != 0);

	device->closeDevice();
	device->run();
	device->drop();

	return result;
}
