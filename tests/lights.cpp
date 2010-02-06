// Copyright (C) 2008-2009 Christian Stehno, Colin MacDonald
// No rights reserved: this software is in the public domain.

#include "testUtils.h"

using namespace irr;

static bool testLightTypes(video::E_DRIVER_TYPE driverType)
{
	IrrlichtDevice *device = createDevice (driverType, core::dimension2d<u32>(128,128));
	if (!device)
		return true; // No error if device does not exist
//	device->getSceneManager()->setAmbientLight(video::SColorf(0.3f,0.3f,0.3f));
	scene::ICameraSceneNode* cam = device->getSceneManager()->addCameraSceneNodeFPS();
	cam->setPosition(core::vector3df(0,200,0));
	cam->setTarget(core::vector3df());
	device->getSceneManager()->addAnimatedMeshSceneNode(device->getSceneManager()->addHillPlaneMesh("plane", core::dimension2df(4,4), core::dimension2du(128,128)));
	scene::ILightSceneNode* light1 = device->getSceneManager()->addLightSceneNode(0, core::vector3df(-100,30,-100));
	light1->setLightType(video::ELT_POINT);
	light1->setRadius(100.f);
	light1->getLightData().DiffuseColor.set(0,1,1);
//	device->getSceneManager()->addCubeSceneNode(10, light1)->setMaterialFlag(video::EMF_LIGHTING, false);
	scene::ILightSceneNode* light2 = device->getSceneManager()->addLightSceneNode(0, core::vector3df(100,30,100));
	light2->setRotation(core::vector3df(90,0,0));
	light2->setLightType(video::ELT_SPOT);
	light2->setRadius(100.f);
	light2->getLightData().DiffuseColor.set(1,0,0);
	light2->getLightData().InnerCone=10.f;
	light2->getLightData().OuterCone=30.f;
//	device->getSceneManager()->addCubeSceneNode(10, light2)->setMaterialFlag(video::EMF_LIGHTING, false);
	scene::ILightSceneNode* light3 = device->getSceneManager()->addLightSceneNode();
	light3->setRotation(core::vector3df(15,0,0));
	light3->setLightType(video::ELT_DIRECTIONAL);
	light1->getLightData().DiffuseColor.set(0,1,0);

	device->getVideoDriver()->beginScene (true, true, 0);
	device->getSceneManager()->drawAll();
	device->getVideoDriver()->endScene();

	const bool result = takeScreenshotAndCompareAgainstReference(device->getVideoDriver(), "-lightType.png", 99.91f);

	device->drop();

	return result;
}

bool lights(void)
{
	bool passed = true;

	passed &= testLightTypes(video::EDT_OPENGL);
	// no lights in sw renderer
//	passed &= testLightTypes(video::EDT_SOFTWARE);
	passed &= testLightTypes(video::EDT_BURNINGSVIDEO);
	passed &= testLightTypes(video::EDT_DIRECT3D9);
	passed &= testLightTypes(video::EDT_DIRECT3D8);

	return passed;
}
