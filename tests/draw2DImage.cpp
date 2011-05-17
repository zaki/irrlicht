#include "testUtils.h"

using namespace irr;

namespace
{

bool testWithRenderTarget(video::E_DRIVER_TYPE driverType)
{
	// create device

	IrrlichtDevice *device = createDevice(driverType, core::dimension2d<u32>(160,120));

	if (device == 0)
		return true; // could not create selected driver.

	video::IVideoDriver* driver = device->getVideoDriver();

	video::ITexture* RenderTarget=driver->addRenderTargetTexture(core::dimension2d<u32>(64,64), "BASEMAP");

	video::ITexture *tex=driver->getTexture("../media/water.jpg");

	driver->beginScene(true, true, video::SColor(255,255,0,255));//Backbuffer background is pink

	//draw the 256x256 water image on the rendertarget:
	driver->setRenderTarget(RenderTarget,true,true,video::SColor(255,0,0,255));//Rendertarget background is blue
	driver->draw2DImage(tex, core::position2d<s32>(0,0), core::recti(0,0,32,32));
	driver->setRenderTarget(0, false);

	//draw the rendertarget on screen:
	//this should normally draw a 64x64 image containing a 32x32 image in the top left corner
	driver->draw2DImage(RenderTarget, core::position2d<s32>(0,0));
	driver->endScene();

	bool result = takeScreenshotAndCompareAgainstReference(driver, "-draw2DImageRTT.png");

	device->closeDevice();
	device->run();
	device->drop();

	return result;
}

// draws a complex (interlaced, paletted, alpha) png image
bool testWithPNG(video::E_DRIVER_TYPE driverType)
{
	// create device

	IrrlichtDevice *device = createDevice(driverType, core::dimension2d<u32>(160,120));

	if (device == 0)
		return true; // could not create selected driver.

	video::IVideoDriver* driver = device->getVideoDriver();

	video::ITexture *tex=driver->getTexture("media/RedbrushAlpha-0.25.png");

	driver->beginScene(true, true, video::SColor(255,40,40,255));//Backbuffer background is blue
	driver->draw2DImage(tex, core::recti(0,0,160,120), core::recti(0,0,256,256), 0, 0, true);
	driver->endScene();

	bool result = takeScreenshotAndCompareAgainstReference(driver, "-draw2DImagePNG.png");

	device->closeDevice();
	device->run();
	device->drop();

	return result;
}

}

bool draw2DImage()
{
	bool result = testWithRenderTarget(video::EDT_DIRECT3D9);
	result &= testWithRenderTarget(video::EDT_DIRECT3D8);
	result &= testWithRenderTarget(video::EDT_OPENGL);
	result &= testWithRenderTarget(video::EDT_BURNINGSVIDEO);
	result &= testWithRenderTarget(video::EDT_SOFTWARE);

	result &= testWithPNG(video::EDT_DIRECT3D9);
	result &= testWithPNG(video::EDT_OPENGL);

	return result;
}
