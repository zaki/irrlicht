// Copyright (C) 2008-2011 Colin MacDonald
// No rights reserved: this software is in the public domain.

#include "testUtils.h"

using namespace irr;

// Tests screenshots.
/** At the moment, this just verifies that the last frame of the animation produces the expected bitmap. */
bool testShots(video::E_DRIVER_TYPE type)
{
	IrrlichtDevice *device = createDevice(type, core::dimension2d<u32>(160, 120), 32);
	if (!device)
		return true;

	video::IVideoDriver* driver = device->getVideoDriver();
	scene::ISceneManager * smgr = device->getSceneManager();

	scene::IAnimatedMesh* mesh = smgr->getMesh("../media/sydney.md2");
	scene::IAnimatedMeshSceneNode* node;

	if (!mesh)
		return false;
	node = smgr->addAnimatedMeshSceneNode(mesh);
	if (!node)
		return false;
	node->setPosition(core::vector3df(20, 0, 30));
	node->setMaterialFlag(video::EMF_LIGHTING, false);
	node->setMaterialTexture(0, driver->getTexture("../media/sydney.bmp"));
	node->setLoopMode(false);

	(void)smgr->addCameraSceneNode();

	// Just jump to the last frame since that's all we're interested in.
	node->setMD2Animation(scene::EMAT_DEATH_FALLBACK);
	node->setCurrentFrame((f32)(node->getEndFrame()));
	node->setAnimationSpeed(0);

	device->run();
	driver->beginScene(true, true, video::SColor(255, 255, 255, 0));
	smgr->drawAll();
	driver->endScene();

	for (u32 i=0; i<video::ECF_UNKNOWN; ++i)
	{
		video::IImage* img = driver->createScreenShot((video::ECOLOR_FORMAT)i);
		logTestString("Color Format %d %ssupported\n", i, (img && img->getColorFormat() == i)?"":"un");
		if (img)
			img->drop();
	}
	device->closeDevice();
	device->run();
	device->drop();

	return true;
}

bool screenshot()
{
	testShots(video::EDT_OPENGL);
	testShots(video::EDT_DIRECT3D9);
	return true;
}