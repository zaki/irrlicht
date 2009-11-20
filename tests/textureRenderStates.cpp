// Copyright (C) 2008-2009 Christian Stehno, Colin MacDonald
// No rights reserved: this software is in the public domain.

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
static bool renderAndLoad(E_DRIVER_TYPE driverType)
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


// This test would cause a crash if it does not work
// in 1.5.1 and 1.6 an illegal access in the OpenGL driver caused this
static bool renderAndRemove(E_DRIVER_TYPE driverType)
{
	IrrlichtDevice *device = createDevice( driverType, dimension2d<u32>(160, 120), 32);
	if (!device)
		return true; // Treat a failure to create a driver as benign; this saves a lot of #ifdefs

	IVideoDriver* driver = device->getVideoDriver();
	ISceneManager * smgr = device->getSceneManager();

	driver->beginScene (true, true, irr::video::SColor (255, 0, 255, 0));
	smgr->drawAll ();
	driver->endScene ();

	smgr->addCameraSceneNode();
	ITexture* texture = driver->getTexture ("media/tools.png");
	ISceneNode * img = smgr->addCubeSceneNode();
	img->setMaterialTexture(0, texture);

	driver->beginScene (true, true, irr::video::SColor (255, 0, 255, 0));
	smgr->drawAll();
	driver->endScene();

	smgr->clear();	// Remove anything that used the texture
	driver->removeTexture(texture);

	driver->beginScene(true, true, SColor(255,100,101,140));
	smgr->drawAll();
	driver->endScene();

	smgr->addCameraSceneNode();
	texture = driver->getTexture ("media/tools.png");
	img = smgr->addCubeSceneNode();
	img->setMaterialTexture(0, texture);

	driver->beginScene (true, true, irr::video::SColor (255, 0, 255, 0));
	smgr->drawAll();
	driver->endScene();

	device->drop();

	return true;
}


bool textureRenderStates(void)
{
	bool passed = true;

	passed &= renderAndLoad(EDT_OPENGL);
	passed &= renderAndLoad(EDT_SOFTWARE);
	passed &= renderAndLoad(EDT_BURNINGSVIDEO);
	passed &= renderAndLoad(EDT_DIRECT3D9);
	passed &= renderAndLoad(EDT_DIRECT3D8);

	passed &= renderAndRemove(EDT_OPENGL);
	passed &= renderAndRemove(EDT_SOFTWARE);
	passed &= renderAndRemove(EDT_BURNINGSVIDEO);
	passed &= renderAndRemove(EDT_DIRECT3D9);
	passed &= renderAndRemove(EDT_DIRECT3D8);

	return passed;
}

