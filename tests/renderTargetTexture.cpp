// Copyright (C) 2008-2009 Christian Stehno, Colin MacDonald
// No rights reserved: this software is in the public domain.

#include "testUtils.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

//! Tests rendering RTTs with draw2DImage
/** This test is very special in its setup, problematic situation was found by stefbuet. */
static bool testWith2DImage(E_DRIVER_TYPE driverType)
{
	IrrlichtDevice *device = createDevice (driverType, core::dimension2d < u32 > (128, 128));
	if (!device)
		return true; // No error if device does not exist

	device->setWindowCaption (L"Irrlicht - RTT Bug report");
	IVideoDriver *driver = device->getVideoDriver ();
	ISceneManager *smgr = device->getSceneManager ();

	ITexture *image = driver->getTexture ("../media/irrlichtlogo2.png");
	ITexture *RTT_texture = driver->addRenderTargetTexture (core::dimension2d < u32 > (128, 128));

	smgr->addCameraSceneNode (0, core::vector3df (100, 100, 100),
			      core::vector3df (0, 0, 0));

	/*to reproduce the bug :
	-draw the image : it's normal
	-apply an RTT texture to a model
	-remove the model
	-draw the image again : it's flipped
	*/

	SColor colors[4]={0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff};
	//draw the image :
	driver->beginScene (true, true, video::SColor (255, 200, 200, 200));
	driver->draw2DImage (image,
		       core::rect < s32 >
		       (64 - image->getSize ().Width / 2,
			64 - image->getSize ().Height / 2,
			64 + image->getSize ().Width / 2,
			64 + image->getSize ().Height / 2),
		       core::rect < s32 > (0, 0, image->getSize ().Width,
					   image->getSize ().Height), 0, colors,
		       true);
	driver->endScene ();

	//then create a model and apply to it the RTT Texture
	//rendering the model is important, if not rendered 1 time, bug won't appear.
	//after the render, we remove the node : important, if not done, bug won't appear too.
	IMesh *modelMesh = smgr->getMesh ("../media/earth.x");
	ISceneNode *modelNode = smgr->addMeshSceneNode(modelMesh);
	modelNode->setMaterialTexture (0, RTT_texture);
	driver->beginScene (true, true, video::SColor (255, 200, 200, 200));
	smgr->drawAll ();
	driver->endScene ();

	modelNode->remove ();

	//then we render the image normaly
	//it's now fliped...
	for (u32 i=0; i<10; ++i)
	{
		driver->beginScene (true, true, video::SColor (255, 200, 200, 200));

		//draw img
		driver->draw2DImage (image,
				   core::rect < s32 >
				   (64 - image->getSize ().Width / 2,
				    64 - image->getSize ().Height / 2,
				    64 + image->getSize ().Width / 2,
				    64 + image->getSize ().Height / 2),
				   core::rect < s32 > (0, 0, image->getSize ().Width,
						       image->getSize ().Height), 0,
				   colors, true);

		//call this is important :
		//if not called, the bug won't appear
		smgr->drawAll ();

		driver->endScene ();
	}

	bool result = takeScreenshotAndCompareAgainstReference(driver, "-rttWith2DImage.png", 99.91f);

	device->drop();

	return result;
}


bool renderTargetTexture(void)
{
	bool passed = true;

	passed &= testWith2DImage(EDT_OPENGL);
	passed &= testWith2DImage(EDT_SOFTWARE);
	passed &= testWith2DImage(EDT_BURNINGSVIDEO);
	passed &= testWith2DImage(EDT_DIRECT3D9);
	passed &= testWith2DImage(EDT_DIRECT3D8);

	return passed;
}

