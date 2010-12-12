// Copyright (C) 2008-2009 Christian Stehno, Colin MacDonald
// No rights reserved: this software is in the public domain.

#include "testUtils.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;


//! Check that EMT_TRANSPARENT_ALPHA_CHANNEL_REF works as expected
bool testTransparentAlphaChannelRef(video::E_DRIVER_TYPE driverType)
{
	IrrlichtDevice* device = createDevice(driverType, core::dimension2d<u32>(160, 120), 32);
	if(!device)
		return true;

	video::IVideoDriver* driver = device->getVideoDriver();
	scene::ISceneManager* smgr = device->getSceneManager();

	driver->setTextureCreationFlag(video::ETCF_ALWAYS_32_BIT, true);

	ISceneNode * backCube = smgr->addCubeSceneNode();
	backCube->setPosition(vector3df(0, 0, 10));
	backCube->setScale(vector3df(5, 5, 1));
	backCube->setMaterialTexture(0, driver->getTexture("../media/wall.bmp"));
	backCube->setMaterialType(video::EMT_SOLID);
	backCube->setMaterialFlag(video::EMF_LIGHTING, false);

	ISceneNode * frontCube = smgr->addCubeSceneNode();
	frontCube->setMaterialTexture(0, driver->getTexture("../media/help.png"));
	frontCube->setMaterialType(video::EMT_TRANSPARENT_ALPHA_CHANNEL_REF);
	frontCube->setMaterialFlag(video::EMF_LIGHTING, false);

	(void)smgr->addCameraSceneNode(0, vector3df(0, 0, -15));

	driver->beginScene(true, true, video::SColor(255,113,113,133));
	smgr->drawAll();
	driver->endScene();

	bool result = takeScreenshotAndCompareAgainstReference(driver, "-transparentAlphaChannelRef.png", 99.18f);

	device->closeDevice();
	device->run();
	device->drop();

	return result;
}


//! Check that EMT_TRANSPARENT_ALPHA_CHANNEL works as expected
bool testTransparentAlphaChannel(video::E_DRIVER_TYPE driverType)
{
	IrrlichtDevice* device = createDevice(driverType, core::dimension2d<u32>(160, 120), 32);
	if(!device)
		return true;

	video::IVideoDriver* driver = device->getVideoDriver();
	scene::ISceneManager* smgr = device->getSceneManager();

	driver->setTextureCreationFlag(video::ETCF_ALWAYS_32_BIT, true);

	ISceneNode * backCube = smgr->addCubeSceneNode();
	backCube->setPosition(vector3df(0, 0, 10));
	backCube->setScale(vector3df(5, 5, 1));
	backCube->setMaterialTexture(0, driver->getTexture("../media/wall.bmp"));
	backCube->setMaterialType(video::EMT_SOLID);
	backCube->setMaterialFlag(video::EMF_LIGHTING, false);

	ISceneNode * frontCube = smgr->addCubeSceneNode();
	frontCube->setMaterialTexture(0, driver->getTexture("../media/help.png"));
	frontCube->setMaterialType(video::EMT_TRANSPARENT_ALPHA_CHANNEL);
	frontCube->setMaterialFlag(video::EMF_LIGHTING, false);

	(void)smgr->addCameraSceneNode(0, vector3df(0, 0, -15));

	driver->beginScene(true, true, video::SColor(255,113,113,133));
	smgr->drawAll();
	driver->endScene();

	bool result = takeScreenshotAndCompareAgainstReference(driver, "-transparentAlphaChannel.png");

	device->closeDevice();
	device->run();
	device->drop();

	return result;
}


//! Check that EMT_TRANSPARENT_VERTEX_ALPHA works as expected
bool testTransparentVertexAlpha(video::E_DRIVER_TYPE driverType)
{
	IrrlichtDevice* device = createDevice(driverType, core::dimension2d<u32>(160, 120), 32);
	if(!device)
		return true;

	video::IVideoDriver* driver = device->getVideoDriver();
	scene::ISceneManager* smgr = device->getSceneManager();

	driver->setTextureCreationFlag(video::ETCF_ALWAYS_32_BIT, true);

	ISceneNode * backCube = smgr->addCubeSceneNode();
	backCube->setPosition(vector3df(0, 0, 10));
	backCube->setScale(vector3df(5, 5, 1));
	backCube->setMaterialTexture(0, driver->getTexture("../media/wall.bmp"));
	// vertex color has alpha 255, hence solid
	backCube->setMaterialType(video::EMT_TRANSPARENT_VERTEX_ALPHA);
	backCube->setMaterialFlag(video::EMF_LIGHTING, false);

	IMeshSceneNode * frontCube = smgr->addCubeSceneNode(10,0,-1,core::vector3df(-10,0,0));
	frontCube->setMaterialTexture(0, driver->getTexture("../media/help.png"));
	frontCube->setMaterialType(video::EMT_TRANSPARENT_VERTEX_ALPHA);
	frontCube->setMaterialFlag(video::EMF_LIGHTING, false);
	driver->getMeshManipulator()->setVertexColorAlpha(frontCube->getMesh(), 128);
	frontCube = smgr->addCubeSceneNode(10,0,-1,core::vector3df(10,0,0));
	frontCube->setMaterialTexture(0, driver->getTexture("../media/help.png"));
	frontCube->setMaterialType(video::EMT_TRANSPARENT_VERTEX_ALPHA);
	frontCube->setMaterialFlag(video::EMF_LIGHTING, false);
	driver->getMeshManipulator()->setVertexColorAlpha(frontCube->getMesh(), 45);

	(void)smgr->addCameraSceneNode(0, vector3df(0, 0, -15));

	driver->beginScene(true, true, video::SColor(255,113,113,133));
	smgr->drawAll();
	driver->endScene();

	bool result = takeScreenshotAndCompareAgainstReference(driver, "-transparentVertexAlpha.png", 98.76f);

	device->closeDevice();
	device->run();
	device->drop();

	return result;
}


//! Check that EMT_TRANSPARENT_REFLECTION_2_LAYER works as expected
bool testTransparentReflection2Layer(video::E_DRIVER_TYPE driverType)
{
	IrrlichtDevice* device = createDevice(driverType, core::dimension2d<u32>(160, 120), 32);
	if(!device)
		return true;

	video::IVideoDriver* driver = device->getVideoDriver();
	scene::ISceneManager* smgr = device->getSceneManager();

	driver->setTextureCreationFlag(video::ETCF_ALWAYS_32_BIT, true);

	ISceneNode * backCube = smgr->addCubeSceneNode();
	backCube->setPosition(vector3df(0, 0, 10));
	backCube->setScale(vector3df(5, 5, 1));
	backCube->setMaterialTexture(0, driver->getTexture("../media/wall.bmp"));
	backCube->setMaterialTexture(1, driver->getTexture("../media/water.jpg"));
	// vertex color has alpha 255, hence solid
	backCube->setMaterialType(video::EMT_TRANSPARENT_REFLECTION_2_LAYER);
	backCube->setMaterialFlag(video::EMF_LIGHTING, false);

	IMeshSceneNode * frontCube = smgr->addCubeSceneNode(10,0,-1,core::vector3df(-10,0,0));
	frontCube->setMaterialTexture(0, driver->getTexture("../media/help.png"));
	frontCube->setMaterialTexture(1, driver->getTexture("../media/water.jpg"));
	frontCube->setMaterialType(video::EMT_TRANSPARENT_REFLECTION_2_LAYER);
	frontCube->setMaterialFlag(video::EMF_LIGHTING, false);
	driver->getMeshManipulator()->setVertexColorAlpha(frontCube->getMesh(), 128);
	frontCube = smgr->addCubeSceneNode(10,0,-1,core::vector3df(10,0,0));
	frontCube->setMaterialTexture(0, driver->getTexture("../media/help.png"));
	frontCube->setMaterialTexture(1, driver->getTexture("../media/water.jpg"));
	frontCube->setMaterialType(video::EMT_TRANSPARENT_REFLECTION_2_LAYER);
	frontCube->setMaterialFlag(video::EMF_LIGHTING, false);
	driver->getMeshManipulator()->setVertexColorAlpha(frontCube->getMesh(), 45);

	(void)smgr->addCameraSceneNode(0, vector3df(0, 0, -15));

	driver->beginScene(true, true, video::SColor(255,113,113,133));
	smgr->drawAll();
	driver->endScene();

	bool result = takeScreenshotAndCompareAgainstReference(driver, "-transparentReflection2Layer.png");

	device->closeDevice();
	device->run();
	device->drop();

	return result;
}


//! Check that EMT_TRANSPARENT_ADD_COLOR works as expected
bool testTransparentAddColor(video::E_DRIVER_TYPE driverType)
{
	IrrlichtDevice* device = createDevice(driverType, core::dimension2d<u32>(160, 120), 32);
	if(!device)
		return true;

	video::IVideoDriver* driver = device->getVideoDriver();
	scene::ISceneManager* smgr = device->getSceneManager();

	driver->setTextureCreationFlag(video::ETCF_ALWAYS_32_BIT, true);

	ISceneNode * backCube = smgr->addCubeSceneNode();
	backCube->setPosition(vector3df(0, 0, 10));
	backCube->setScale(vector3df(5, 5, 1));
	backCube->setMaterialTexture(0, driver->getTexture("../media/wall.bmp"));
	backCube->setMaterialType(video::EMT_SOLID);
	backCube->setMaterialFlag(video::EMF_LIGHTING, false);

	IMeshSceneNode * frontCube = smgr->addCubeSceneNode();
	frontCube->setMaterialTexture(0, driver->getTexture("../media/help.png"));
	frontCube->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
	frontCube->setMaterialFlag(video::EMF_LIGHTING, false);

	(void)smgr->addCameraSceneNode(0, vector3df(0, 0, -15));

	driver->beginScene(true, true, video::SColor(255,113,113,133));
	smgr->drawAll();
	driver->endScene();

	bool result = takeScreenshotAndCompareAgainstReference(driver, "-transparentAddColor.png");

	device->closeDevice();
	device->run();
	device->drop();

	return result;
}


bool testTransparentVertexAlphaMore(E_DRIVER_TYPE driverType)
{
	IrrlichtDevice *device = createDevice(driverType, dimension2d<u32>(160, 120));

	IVideoDriver* driver = device->getVideoDriver();
	ISceneManager* smgr = device->getSceneManager();

	IAnimatedMesh* mesh = smgr->getMesh("../media/sydney.md2");
	IAnimatedMeshSceneNode* node = smgr->addAnimatedMeshSceneNode( mesh );
	IMeshSceneNode* cube = smgr->addCubeSceneNode(10.0f,0,-1,vector3df(-5,3,-15));

	if (node)
	{
		node->setMaterialFlag(EMF_LIGHTING, false);
		node->setFrameLoop(0, 310);
		node->setMaterialTexture( 0, driver->getTexture("../media/sydney.bmp") );
	}
	if (cube)
	{
		cube->getMaterial(0).MaterialType = EMT_TRANSPARENT_VERTEX_ALPHA;
		cube->setMaterialTexture(0, driver->getTexture("../media/wall.bmp"));
		cube->setMaterialFlag(EMF_LIGHTING, false);
		smgr->getMeshManipulator()->setVertexColorAlpha(cube->getMesh(),128);
	}
	// second cube without texture
	cube = smgr->addCubeSceneNode(10.0f,0,-1,vector3df(5,3,-15));
	if (cube)
	{
		cube->getMaterial(0).MaterialType = EMT_TRANSPARENT_VERTEX_ALPHA;
		cube->setMaterialFlag(EMF_LIGHTING, false);
		smgr->getMeshManipulator()->setVertexColorAlpha(cube->getMesh(),128);
	}

	smgr->addCameraSceneNode(0, vector3df(0,30,-40), vector3df(0,5,0));

	driver->beginScene(true, true, SColor(0,200,200,200));
	smgr->drawAll();
	driver->endScene();

	bool result = takeScreenshotAndCompareAgainstReference(driver, "-transparentVertexAlphaChannelMore.png", 99.18f);

	device->closeDevice();
	device->run();
	device->drop();

	return result;
}


bool transparentMaterials(void)
{
	bool result = testTransparentAlphaChannel(EDT_DIRECT3D9);
	result &= testTransparentAlphaChannel(EDT_OPENGL);
	result &= testTransparentAlphaChannel(EDT_BURNINGSVIDEO);

	result &= testTransparentAlphaChannelRef(EDT_DIRECT3D9);
	result &= testTransparentAlphaChannelRef(EDT_OPENGL);
	// FIXME Rogerborg 8-January-2009. Burning's video currently produces unexpected results,
	// blending using the full alpha value instead of using a boolean mask. This test is being
	// added now anyway to help verify the fix when it's done; it should just require an
	// update of the reference image.
	result &= testTransparentAlphaChannelRef(EDT_BURNINGSVIDEO);

	result &= testTransparentVertexAlpha(EDT_DIRECT3D9);
	result &= testTransparentVertexAlpha(EDT_OPENGL);
	// This type seems to be broken as well for Burning's video.
	result &= testTransparentVertexAlpha(EDT_BURNINGSVIDEO);

	result &= testTransparentAddColor(EDT_DIRECT3D9);
	result &= testTransparentAddColor(EDT_OPENGL);
	result &= testTransparentAddColor(EDT_BURNINGSVIDEO);

	result &= testTransparentReflection2Layer(EDT_OPENGL);
	result &= testTransparentReflection2Layer(EDT_DIRECT3D9);
	result &= testTransparentReflection2Layer(EDT_BURNINGSVIDEO);

	result &= testTransparentVertexAlphaMore(EDT_DIRECT3D9);
	result &= testTransparentVertexAlphaMore(EDT_OPENGL);
	// This type seems to be broken as well for Burning's video.
	result &= testTransparentVertexAlphaMore(EDT_BURNINGSVIDEO);

	return result;
}
