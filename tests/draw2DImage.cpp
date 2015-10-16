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

	if (!driver->queryFeature(video::EVDF_RENDER_TO_TARGET))
	{
		device->closeDevice();
		device->run();
		device->drop();
		return true;
	}

	stabilizeScreenBackground(driver);

	logTestString("Testing driver %ls\n", driver->getName());

	video::ITexture* renderTargetTex = driver->addRenderTargetTexture(core::dimension2d<u32>(64, 64), "BASEMAP");
	video::ITexture* renderTargetDepth = driver->addRenderTargetTexture(core::dimension2d<u32>(64, 64), "rtd", video::ECF_D16);

	video::IRenderTarget* renderTarget = driver->addRenderTarget();
	renderTarget->setTexture(renderTargetTex, renderTargetDepth);

	video::ITexture* tex=driver->getTexture("../media/water.jpg");

	driver->beginScene(video::ECBF_COLOR | video::ECBF_DEPTH, video::SColor(255,255,0,255));//Backbuffer background is pink

	//draw the 256x256 water image on the rendertarget:


	driver->setRenderTarget(renderTarget,video::ECBF_COLOR|video::ECBF_DEPTH,video::SColor(255,0,0,255));//Rendertarget background is blue
	driver->draw2DImage(tex, core::position2d<s32>(0,0), core::recti(0,0,32,32));
	driver->setRenderTarget((video::IRenderTarget*)0, 0);

	//draw the rendertarget on screen:
	//this should normally draw a 64x64 image containing a 32x32 image in the top left corner
	driver->draw2DImage(renderTargetTex, core::position2d<s32>(0,0));
	driver->endScene();

	bool result = takeScreenshotAndCompareAgainstReference(driver, "-draw2DImageRTT.png");

	device->closeDevice();
	device->run();
	device->drop();

	return result;
}

// Test various special destination rectangles
bool testRectangles(video::E_DRIVER_TYPE driverType)
{
	// create device
	IrrlichtDevice *device = createDevice(driverType, core::dimension2d<u32>(160,120));

	if (device == 0)
		return true; // could not create selected driver.

	video::IVideoDriver* driver = device->getVideoDriver();

	stabilizeScreenBackground(driver);

	logTestString("Testing driver %ls\n", driver->getName());

	video::ITexture *tex=driver->getTexture("../media/fireball.bmp");

	driver->beginScene(video::ECBF_COLOR | video::ECBF_DEPTH, video::SColor(255,255,0,255));//Backbuffer background is pink

	// draw normal, will be overdrwan in error case
	driver->draw2DImage(tex, core::recti(68,32,132,96), core::recti(0,0,64,64));
	//draw the image larger
	driver->draw2DImage(tex, core::recti(0,0,64,64), core::recti(0,0,32,32));
	//draw the image flipped horizontally
	driver->draw2DImage(tex, core::recti(132,0,68,64), core::recti(0,0,64,64));
	//draw the image smaller
	driver->draw2DImage(tex, core::recti(0,64,32,96), core::recti(0,0,64,64));
	//draw the image much smaller
	driver->draw2DImage(tex, core::recti(36,64,44,72), core::recti(0,0,64,64));
	//draw the image flipped horizontally
	driver->draw2DImage(tex, core::recti(68,64,132,0), core::recti(0,0,64,64));
	driver->endScene();

	bool result = takeScreenshotAndCompareAgainstReference(driver, "-draw2DImageRect.png");

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

	stabilizeScreenBackground(driver);

	logTestString("Testing driver %ls\n", driver->getName());

	video::ITexture *tex=driver->getTexture("media/RedbrushAlpha-0.25.png");

	driver->beginScene(video::ECBF_COLOR | video::ECBF_DEPTH, video::SColor(255,40,40,255));//Backbuffer background is blue
	driver->draw2DImage(tex, core::recti(0,0,160,120), core::recti(0,0,256,256), 0, 0, true);
	driver->endScene();

	bool result = takeScreenshotAndCompareAgainstReference(driver, "-draw2DImagePNG.png", 98.f);

	device->closeDevice();
	device->run();
	device->drop();

	return result;
}

// draws an image and checks if the written example equals the original image
bool testExactPlacement(video::E_DRIVER_TYPE driverType)
{
	// create device

	IrrlichtDevice *device = createDevice(driverType, core::dimension2d<u32>(160,120), 32);

	if (device == 0)
		return true; // could not create selected driver.

	video::IVideoDriver* driver = device->getVideoDriver();

	if (driver->getColorFormat() != video::ECF_A8R8G8B8 || !driver->queryFeature(video::EVDF_RENDER_TO_TARGET))
	{
		device->closeDevice();
		device->run();
		device->drop();
		return true;
	}

	stabilizeScreenBackground(driver);

	logTestString("Testing driver %ls\n", driver->getName());

	video::ITexture* renderTargetTex = driver->addRenderTargetTexture(core::dimension2d<u32>(32, 32), "rt1");
	video::ITexture* renderTargetDepth = driver->addRenderTargetTexture(core::dimension2d<u32>(32, 32), "rtd", video::ECF_D16);

	video::IRenderTarget* renderTarget = driver->addRenderTarget();
	renderTarget->setTexture(renderTargetTex, renderTargetDepth);

	video::ITexture* tex=driver->getTexture("../media/fireball.bmp");

	driver->beginScene(video::ECBF_COLOR | video::ECBF_DEPTH, video::SColor(255,40,40,255));//Backbuffer background is blue
	driver->setRenderTarget(renderTarget, 0, video::ECBF_COLOR | video::ECBF_DEPTH);
	driver->draw2DImage(tex, core::recti(0,0,32,32), core::recti(0,0,64,64));
	driver->setRenderTarget((video::IRenderTarget*)0, 0, 0);
	driver->endScene();

	video::IImage* img = driver->createImage(renderTargetTex, core::vector2di(), renderTargetTex->getSize());
	driver->writeImageToFile(img, "results/fireball.png");
	img->drop();
	bool result = fuzzyCompareImages(driver, "media/fireball.png", "results/fireball.png")>98.25f;

	device->closeDevice();
	device->run();
	device->drop();

	return result;
}

}

bool draw2DImage()
{
	bool result = true;
	TestWithAllDrivers(testWithRenderTarget);
	TestWithAllHWDrivers(testWithPNG);
	// TODO D3D driver moves image 1 pixel top-left in case of down scaling
	TestWithAllDrivers(testExactPlacement);
	TestWithAllDrivers(testRectangles);
	return result;
}
