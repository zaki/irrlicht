// Copyright (C) 2008-2009 Colin MacDonald
// No rights reserved: this software is in the public domain.

#include "testUtils.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

// Tests MD2 animations.
/** At the moment, this just verifies that the last frame of the animation produces the expected bitmap. */
bool md2Animation(void)
{
	// Use EDT_BURNINGSVIDEO since it is not dependent on (e.g.) OpenGL driver versions.
	IrrlichtDevice *device = createDevice( EDT_BURNINGSVIDEO, dimension2d<u32>(160, 120), 32);
	assert(device);
	if (!device)
		return false;

	IVideoDriver* driver = device->getVideoDriver();
	ISceneManager * smgr = device->getSceneManager();

	IAnimatedMesh* mesh = smgr->getMesh("../media/sydney.md2");
	IAnimatedMeshSceneNode* node;
	assert(mesh);

	if(mesh)
	{
		node = smgr->addAnimatedMeshSceneNode(mesh);
		assert(node);

		if(node)
		{
			node->setPosition(vector3df(20, 0, 30));
			node->setMaterialFlag(EMF_LIGHTING, false);
			node->setMaterialTexture(0, driver->getTexture("../media/sydney.bmp"));
			node->setLoopMode(false);

			(void)smgr->addCameraSceneNode();

			// Just jump to the last frame since that's all we're interested in.
			node->setMD2Animation(EMAT_DEATH_FALLBACK);
			node->setCurrentFrame((f32)(node->getEndFrame()));
			device->run();
			driver->beginScene(true, true, SColor(255, 255, 255, 0));
			smgr->drawAll();
			driver->endScene();
		}
	}

	bool result = takeScreenshotAndCompareAgainstReference(driver, "-md2Animation.png");
	device->drop();

	return result;
}

