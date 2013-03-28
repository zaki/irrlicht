// Copyright (C) 2008-2012 Colin MacDonald
// No rights reserved: this software is in the public domain.

#include "testUtils.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

//! Tests IVideoDriver::drawPixel().
/** Expect to see two diagonal lines overlaying a wall texture cube.
	One line should run from green at the top left to red at the bottom right.
	The other should run from cyan 100% transparent at the bottom left to
	cyan 100% opaque at the top right. */
static bool lineRender(E_DRIVER_TYPE driverType)
{
	IrrlichtDevice *device = createDevice( driverType, dimension2d<u32>(160, 120), 32);
	if (!device)
		return true; // Treat a failure to create a driver as benign; this saves a lot of #ifdefs

	IVideoDriver* driver = device->getVideoDriver();
	ISceneManager * smgr = device->getSceneManager();

	logTestString("Testing driver %ls\n", driver->getName());

	// Draw a cube background so that we can check that the pixels' alpha is working.
	ISceneNode * cube = smgr->addCubeSceneNode(50.f, 0, -1, vector3df(0, 0, 60));
	cube->setMaterialTexture(0, driver->getTexture("../media/wall.bmp"));
	cube->setMaterialFlag(video::EMF_LIGHTING, false);
	(void)smgr->addCameraSceneNode();

	driver->beginScene(true, true, SColor(255,100,101,140));
	smgr->drawAll();

	// Test for benign handling of offscreen pixel values as well as onscreen ones.
	for(s32 x = -10; x < 170; ++x)
	{
		s32 y = 120 * x / 160;
		driver->drawPixel((u32)x, (u32)y, SColor(255, 255 * x / 640, 255 * (640 - x) / 640, 0));
		y = 120 - y;
		driver->drawPixel((u32)x, (u32)y, SColor(255 * x / 640, 0, 255, 255));
	}

	driver->endScene();

	bool result = takeScreenshotAndCompareAgainstReference(driver, "-drawPixel.png", 98.81f);

	device->closeDevice();
	device->run();
	device->drop();

	return result;
}

// this test draws alternating white and black borders with
// increasing thickness. Intended use of this test is to ensure
// the corect pixel alignment within the render window.
static bool pixelAccuracy(E_DRIVER_TYPE driverType)
{
	IrrlichtDevice *device = createDevice( driverType, dimension2d<u32>(160, 120), 32);
	if (!device)
		return true; // Treat a failure to create a driver as benign; this saves a lot of #ifdefs

	IVideoDriver* driver = device->getVideoDriver();

	logTestString("Testing driver %ls\n", driver->getName());

	device->getSceneManager()->addCameraSceneNode();

	driver->beginScene(true, true, SColor(255,100,101,140));
	u32 start=0;
	for (u32 count=1; count<10; ++count)
	{
		for (u32 j=0; j<count; ++j)
		{
			for (u32 x=0; x<100-start; ++x)
			{
				driver->drawPixel(start+x, start, (count%2==1)?0xffffffff:0xff000000);
			}
			++start;
		}
	}
	start=0;
	for (u32 count=1; count<10; ++count)
	{
		for (u32 j=0; j<count; ++j)
		{
			for (u32 x=0; x<100-start; ++x)
			{
				driver->drawPixel(start, start+x, (count%2==1)?0xffffffff:0xff000000);
			}
			++start;
		}
	}
	for (u32 x=0; x<100; ++x)
	{
		driver->drawPixel(x, x, 0xffff0000);
	}
	driver->endScene();

	bool result = takeScreenshotAndCompareAgainstReference(driver, "-pixelAccuracy.png");

	device->closeDevice();
	device->run();
	device->drop();

	return result;
}

bool drawPixel(void)
{
	bool result = true;

	TestWithAllDrivers(lineRender);
	TestWithAllDrivers(pixelAccuracy);

	return result;
}
