// Copyright (C) 2008-2009 Colin MacDonald
// No rights reserved: this software is in the public domain.

#include "testUtils.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

// Tests mesh loading features and the mesh cache.
/** This won't test render results. Currently, not all mesh loaders are tested. */
bool meshLoaders(void)
{
	IrrlichtDevice *device = createDevice( EDT_NULL, dimension2d<u32>(160, 120), 32);
	assert(device);
	if (!device)
		return false;

	ISceneManager * smgr = device->getSceneManager();

	IAnimatedMesh* mesh = smgr->getMesh("../media/ninja.b3d");
	assert(mesh);

	bool result = (mesh != 0);

	if (mesh)
	{
		if (mesh != smgr->getMesh("../media/ninja.b3d"))
		{
			logTestString("Loading from same file results in different meshes!");
				result=false;
		}
	}

	device->drop();

	return result;
}
