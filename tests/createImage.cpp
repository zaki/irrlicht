#include "testUtils.h"

using namespace irr;

static bool testImageCreation()
{
	// create device

	IrrlichtDevice *device = createDevice(video::EDT_SOFTWARE, core::dimension2d<u32>(160,120));

	if (device == 0)
		return true; // could not create selected driver.

	video::IVideoDriver* driver = device->getVideoDriver();
	video::ITexture* tex=driver->getTexture("../media/water.jpg");
	video::IImage* img1=driver->createImage(tex, core::vector2di(0,0), core::dimension2du(32,32));
	video::ITexture* tex1=driver->addTexture("new1", img1);
	video::IImage* img2=driver->createImage(tex, core::vector2di(0,0), tex->getSize());
	video::ITexture* tex2=driver->addTexture("new2", img2);

	driver->beginScene(true, true, video::SColor(255,255,0,255));//Backbuffer background is pink

	driver->draw2DImage(tex, core::position2d<s32>(0,0), core::recti(0,0,32,32));
	driver->draw2DImage(tex1, core::position2d<s32>(32,0));
	driver->draw2DImage(tex2, core::position2d<s32>(64,0), core::recti(0,0,32,32));

	driver->endScene();

	bool result = takeScreenshotAndCompareAgainstReference(driver, "-createImage.png");

	device->closeDevice();
	device->run();
	device->drop();

	return result;
}

bool createImage()
{
	bool result = testImageCreation();
	return result;
}
