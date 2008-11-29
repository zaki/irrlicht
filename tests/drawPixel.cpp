// Tests IVideoDriver::drawPixel().
// Expect to see two diagonal lines overlaying a wall texture cube.
// One line should run from red at the top left to green at the bottom right.
// The other should run from cyan 100% transparent at the bottom left to
// cyan 100% opaque at the top right.

#include "irrlicht.h"
#include "testUtils.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

static bool runTestWithDriver(E_DRIVER_TYPE driverType)
{
	IrrlichtDevice *device = createDevice( driverType, dimension2d<s32>(640, 480));
	if (!device)
		return true; // Treat a failure to create a driver as benign; this saves a lot of #ifdefs
	
	IVideoDriver* driver = device->getVideoDriver();
	ISceneManager * smgr = device->getSceneManager();
	
	// Draw a cube to check that the pixels' alpha is working.
	ISceneNode * cube = smgr->addCubeSceneNode(50.f, 0, -1, vector3df(0, 0, 60));
	cube->setMaterialTexture(0, driver->getTexture("../media/wall.bmp"));
	cube->setMaterialFlag(video::EMF_LIGHTING, false);
	(void)smgr->addCameraSceneNode();

	driver->beginScene(true, true, SColor(255,100,101,140));
	smgr->drawAll();

	// Test for offscreen values as well as onscreen.
	for(s32 x = -10; x < 650; ++x)
	{
		s32 y = 480 * x / 640;
		driver->drawPixel((u32)x, (u32)y, SColor(255, 255 * x / 640, 255 * (640 - x) / 640, 0));
		y = 480 - y;
		driver->drawPixel((u32)x, (u32)y, SColor(255 * x / 640, 0, 255, 255));
	}

	driver->endScene();

	bool result = takeScreenshotAndCompareAgainstReference(driver, "-drawPixel.jpg");

	device->drop();

	return result;
}


bool drawPixel(void)
{
	bool passed = true;

	passed |= runTestWithDriver(EDT_NULL);
	passed |= runTestWithDriver(EDT_SOFTWARE);
	passed |= runTestWithDriver(EDT_BURNINGSVIDEO);
	passed |= runTestWithDriver(EDT_OPENGL);
	passed |= runTestWithDriver(EDT_DIRECT3D8);
	passed |= runTestWithDriver(EDT_DIRECT3D9);
	
	return passed;
} 

