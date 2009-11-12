// Copyright (C) 2008-2009 Christian Stehno, Colin MacDonald
// No rights reserved: this software is in the public domain.

#include "testUtils.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

//! Tests lightmaps under all drivers that support them
static bool runTestWithDriver(E_DRIVER_TYPE driverType)
{
	IrrlichtDevice *device = createDevice( driverType, dimension2d<u32>(160, 120), 32);
	if (!device)
		return true; // Treat a failure to create a driver as benign; this saves a lot of #ifdefs

	IVideoDriver* driver = device->getVideoDriver();
	ISceneManager * smgr = device->getSceneManager();

	bool result = true;
	bool added = device->getFileSystem()->addZipFileArchive("../media/map-20kdm2.pk3");
	assert(added);

	if(added)
	{
		ISceneNode * node = smgr->addOctTreeSceneNode(smgr->getMesh("20kdm2.bsp")->getMesh(0), 0, -1, 1024);
		assert(node);

		if (node)
		{
			node->setMaterialFlag(EMF_LIGHTING, false);
			node->setPosition(core::vector3df(-1300,-820,-1249));
			node->setScale(core::vector3df(1, 5, 1));

			(void)smgr->addCameraSceneNode(0, core::vector3df(0,0,0), core::vector3df(40,100,30));

			driver->beginScene(true, true, video::SColor(255,255,255,0));
			smgr->drawAll();
			driver->endScene();

			result = takeScreenshotAndCompareAgainstReference(driver, "-lightmaps.png", 96);
		}
	}

	device->drop();

	return result;
}


bool lightMaps(void)
{
	bool passed = true;

	passed &= runTestWithDriver(EDT_OPENGL);
	passed &= runTestWithDriver(EDT_BURNINGSVIDEO);
	passed &= runTestWithDriver(EDT_DIRECT3D9);
	passed &= runTestWithDriver(EDT_DIRECT3D8);

	return passed;
}

