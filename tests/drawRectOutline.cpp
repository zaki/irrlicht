#include "testUtils.h"

using namespace irr;

bool testWithDriver(video::E_DRIVER_TYPE driverType)
{
	IrrlichtDevice *device =
		createDevice(driverType, core::dimension2du(160, 120));
	if (!device)
		return true;

	video::IVideoDriver* driver = device->getVideoDriver();

	driver->beginScene(true, true, video::SColor(255,100,101,140));

	core::recti r;
	r.UpperLeftCorner = core::position2di(1,1);
	r.LowerRightCorner = core::position2di(100,100);
	driver->draw2DRectangleOutline( r );

	r += core::position2di( 10 , 10 );
	driver->draw2DRectangleOutline( r , video::SColor(128, 255, 128, 128) );

	driver->getMaterial2D().Thickness=12.f;
	driver->enableMaterial2D();
	r += core::position2di( 10 , 10 );
	driver->draw2DRectangleOutline( r , video::SColor(128, 255, 128, 128) );

	driver->endScene();

	bool result = takeScreenshotAndCompareAgainstReference(driver, "-drawRectOutline.png", 99.9f );

	device->drop();

	return result ;
}

bool drawRectOutline(void)
{
	// TODO: Only OpenGL supports thick lines
	bool result = testWithDriver(video::EDT_BURNINGSVIDEO);
	result &= testWithDriver(video::EDT_DIRECT3D8);
	result &= testWithDriver(video::EDT_DIRECT3D9);
	result &= testWithDriver(video::EDT_OPENGL);
	result &= testWithDriver(video::EDT_SOFTWARE);
	return result;
}
