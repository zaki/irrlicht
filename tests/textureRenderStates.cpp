// Copyright (C) 2008-2009 Christian Stehno, Colin MacDonald
// No rights reserved: this software is in the public domain.

#include "irrlicht.h"
#include "testUtils.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

//! Tests interleaved loading and rendering of textures
/** The test loads a texture, renders it using draw2dimage, loads another
	texture and renders the first one again. Due to the texture cache this
	can lead to rendering of the second texture in second place. */
static bool runTestWithDriver(E_DRIVER_TYPE driverType)
{
	IrrlichtDevice *device = createDevice( driverType, dimension2d<u32>(160, 120), 32);
	if (!device)
		return true; // Treat a failure to create a driver as benign; this saves a lot of #ifdefs

	IVideoDriver* driver = device->getVideoDriver();
	ISceneManager * smgr = device->getSceneManager();

	ITexture* tex1 = driver->getTexture("../media/wall.bmp");

	(void)smgr->addCameraSceneNode();

	driver->beginScene(true, true, SColor(255,100,101,140));
	driver->draw2DImage(tex1, position2di(0,0));
	driver->endScene();

	driver->getTexture("../media/tools.png");

	driver->beginScene(true, true, SColor(255,100,101,140));
	driver->draw2DImage(tex1, position2di(0,0));
	driver->endScene();

	bool result = takeScreenshotAndCompareAgainstReference(driver, "-textureRenderStates.png", 100);

	device->drop();

	return result;
}


bool textureRenderStates(void)
{
	bool passed = true;

	passed &= runTestWithDriver(EDT_SOFTWARE);
	passed &= runTestWithDriver(EDT_BURNINGSVIDEO);
	passed &= runTestWithDriver(EDT_DIRECT3D9);
	passed &= runTestWithDriver(EDT_DIRECT3D8);
	passed &= runTestWithDriver(EDT_OPENGL);

	return passed;
}

