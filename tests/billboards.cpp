// Copyright (C) 2008-2011 Colin MacDonald
// No rights reserved: this software is in the public domain.

#include "testUtils.h"

using namespace irr;

// Test billboards
bool billboards(void)
{
	// Use EDT_BURNINGSVIDEO since it is not dependent on (e.g.) OpenGL driver versions.
	IrrlichtDevice *device = createDevice(video::EDT_BURNINGSVIDEO, core::dimension2d<u32>(160, 120), 32);
	assert(device);
	if (!device)
		return false;

	video::IVideoDriver* driver = device->getVideoDriver();
	scene::ISceneManager * smgr = device->getSceneManager();

	scene::ICameraSceneNode* cam = smgr->addCameraSceneNode();
	scene::IBillboardSceneNode* bill = smgr->addBillboardSceneNode();
	bill->setPosition(core::vector3df(0,0,50));
	bill = smgr->addBillboardSceneNode();
	bill->setPosition(core::vector3df(-30,0,50));
	bill->getMaterial(0).Lighting=false;
	bill = smgr->addBillboardSceneNode();
	bill->setPosition(core::vector3df(30,0,50));
	bill->getMaterial(0).Lighting=false;
	bill->getMaterial(0).Wireframe=true;
	bill = smgr->addBillboardSceneNode();
	bill->setPosition(core::vector3df(30,0,50));
	bill->setSize(2,2,2);

	bill = smgr->addBillboardSceneNode();
	bill->setSize(10,20,2);
	bill->setPosition(core::vector3df(0,30,50));
	bill = smgr->addBillboardSceneNode();
	bill->setSize(10,2,20);
	bill->setPosition(core::vector3df(-30,30,50));
	bill->getMaterial(0).Lighting=false;
	bill = smgr->addBillboardSceneNode();
	bill->setSize(10,2,20);
	bill->setPosition(core::vector3df(30,30,50));
	bill->getMaterial(0).Lighting=false;
	bill->getMaterial(0).Wireframe=true;
	bill = smgr->addBillboardSceneNode();
	bill->setPosition(core::vector3df(30,30,50));
	bill->setSize(2,2,2);

	video::ITexture* tex = driver->getTexture("../media/fireball.bmp");
	bill = smgr->addBillboardSceneNode();
	bill->getMaterial(0).Lighting=false;
	bill->getMaterial(0).TextureLayer[0].AnisotropicFilter=true;
	bill->getMaterial(0).setTexture(0, tex);
	bill->setSize(10,20,2);
	bill->setPosition(core::vector3df(0,-30,50));
	bill = smgr->addBillboardSceneNode();
	bill->setSize(10,2,20);
	bill->setPosition(core::vector3df(-30,-30,50));
	bill->getMaterial(0).TextureLayer[0].AnisotropicFilter=true;
	bill->getMaterial(0).setTexture(0, tex);
	bill->getMaterial(0).Lighting=false;
	bill = smgr->addBillboardSceneNode();
	bill->setSize(10,2,20);
	bill->setPosition(core::vector3df(30,-30,50));
	bill->getMaterial(0).TextureLayer[0].AnisotropicFilter=true;
	bill->getMaterial(0).setTexture(0, tex);
	bill->getMaterial(0).Lighting=false;
	bill->getMaterial(0).Wireframe=true;
	bill = smgr->addBillboardSceneNode();
	bill->setPosition(core::vector3df(30,-30,50));
	bill->setSize(2,2,2);

	bill = smgr->addBillboardSceneNode();
	bill->getMaterial(0).Lighting=false;
	bill->getMaterial(0).setTexture(0, tex);
	bill->setSize(10,20,14);
	bill->setPosition(core::vector3df(0,-15,50));
	bill = smgr->addBillboardSceneNode();
	bill->setSize(10,14,20);
	bill->setPosition(core::vector3df(-30,-15,50));
	bill->getMaterial(0).setTexture(0, tex);
	bill->getMaterial(0).Lighting=false;
	bill = smgr->addBillboardSceneNode();
	bill->setSize(10,14,20);
	bill->setPosition(core::vector3df(30,-15,50));
	bill->getMaterial(0).setTexture(0, tex);
	bill->getMaterial(0).Lighting=false;
	bill->getMaterial(0).Wireframe=true;
	bill = smgr->addBillboardSceneNode();
	bill->setPosition(core::vector3df(30,-15,50));
	bill->setSize(2,2,2);

	bool result = false;

	device->run();
	driver->beginScene(true, true, video::SColor(255, 60, 60, 60));
	smgr->drawAll();
	driver->endScene();

	result = takeScreenshotAndCompareAgainstReference(driver, "-billboard.png");

	device->closeDevice();
	device->run();
	device->drop();

	return result;
}
