#include "irrlicht.h"
#include "testUtils.h"
using namespace irr;

bool drawRectOutline(void)
{
	IrrlichtDevice *device =
		createDevice(video::EDT_BURNINGSVIDEO, core::dimension2di(160, 120));
	video::IVideoDriver* driver = device->getVideoDriver();

	driver->beginScene(true, true, video::SColor(255,100,101,140));

	core::recti r;
	r.UpperLeftCorner = core::position2di(1,1);
	r.LowerRightCorner = core::position2di(100,100);
	driver->draw2DRectangleOutline( r );

	r += core::position2di( 10 , 10 );
	driver->draw2DRectangleOutline( r , video::SColor(128, 255, 128, 128) );

	driver->endScene();

	bool result = takeScreenshotAndCompareAgainstReference(driver, "-drawRectOutline.png" );

	device->drop();

	return result ;
}
