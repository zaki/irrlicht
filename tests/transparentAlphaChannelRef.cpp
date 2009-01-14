// Copyright (C) 2008-2009 Christian Stehno, Colin MacDonald
// No rights reserved: this software is in the public domain.

#include "irrlicht.h"
#include "testUtils.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;


//! Check that EMT_TRANSPARENT_ALPHA_CHANNEL_REF works as expected
bool testWithDriver(video::E_DRIVER_TYPE driverType)
{
	IrrlichtDevice* device = createDevice(driverType, core::dimension2d<s32>(160, 120), 32);
	if(!device)
		return true;

	video::IVideoDriver* driver = device->getVideoDriver();
	scene::ISceneManager* smgr = device->getSceneManager();

	driver->setTextureCreationFlag(video::ETCF_ALWAYS_32_BIT, true);

	ISceneNode * backCube = smgr->addCubeSceneNode();
	backCube->setPosition(vector3df(0, 0, 10));
	backCube->setScale(vector3df(5, 5, 1));
	backCube->setMaterialTexture(0, driver->getTexture("../media/wall.bmp"));
	backCube->setMaterialType(video::EMT_SOLID );  //solid.....?
	backCube->setMaterialFlag(video::EMF_LIGHTING, false);

	ISceneNode * frontCube = smgr->addCubeSceneNode();
	frontCube->setMaterialTexture(0, driver->getTexture("../media/help.png"));
	frontCube->setMaterialType(video::EMT_TRANSPARENT_ALPHA_CHANNEL_REF );  //solid.....?
	frontCube->setMaterialFlag(video::EMF_LIGHTING, false);

	(void)smgr->addCameraSceneNode(0, vector3df(0, 0, -15));

	driver->beginScene(true, true, video::SColor(255,113,113,133));
	smgr->drawAll();
	driver->endScene();

	bool result = takeScreenshotAndCompareAgainstReference(driver, "-transparentAlphaChannelRef.png", 99.88);

	device->drop();

	return result;
}


bool transparentAlphaChannelRef(void)
{
	bool result = testWithDriver(EDT_DIRECT3D9);

	// FIXME Rogerborg 8-January-2009. Burning's video currently produces unexpected results,
	// blending using the full alpha value instead of using a boolean mask. This test is being
	// added now anyway to help verify the fix when it's done; it should just require an 
	// update of the reference image.
	result &= testWithDriver(EDT_BURNINGSVIDEO);
	return result;
}
