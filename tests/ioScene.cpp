// Copyright (C) 2008-2011 Colin MacDonald
// No rights reserved: this software is in the public domain.

#include "testUtils.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

// Tests save scene.
static bool saveScene(void)
{
	IrrlichtDevice *device = createDevice( EDT_NULL, dimension2d<u32>(160, 120), 32);
	assert(device);
	if (!device)
		return false;

	IVideoDriver* driver = device->getVideoDriver();
	ISceneManager * smgr = device->getSceneManager();

	ISkinnedMesh* mesh = (ISkinnedMesh*)smgr->getMesh("../media/ninja.b3d");
	if (!mesh)
		return false;

	IAnimatedMeshSceneNode* node1 = smgr->addAnimatedMeshSceneNode(mesh);
	if (node1)
	{
		node1->setPosition(vector3df(-3, -3, 10));
		node1->setMaterialFlag(EMF_LIGHTING, false);
		node1->setAnimationSpeed(0.f);
		node1->setCurrentFrame(10.f);
		node1->setDebugDataVisible(irr::scene::EDS_BBOX_BUFFERS);
	}

	ISkinnedMesh* mesh2 = (ISkinnedMesh*)smgr->getMesh(device->getFileSystem()->getAbsolutePath("../media/dwarf.x"));
	if (!mesh2)
		return false;

	IAnimatedMeshSceneNode* node2 = smgr->addAnimatedMeshSceneNode(mesh2);
	if (node2)
	{
		node2->setPosition(vector3df(33, -93, 120));
		node2->setMaterialFlag(EMF_LIGHTING, false);
		node2->setAnimationSpeed(10.f);
		node2->setCurrentFrame(2.f);
	}

	IAnimatedMeshSceneNode* node3 = smgr->addAnimatedMeshSceneNode(mesh2, node2);
	if (node3)
	{
		node3->setPosition(vector3df(-88, -300, 150));
		node3->setMaterialFlag(EMF_LIGHTING, false);
		node3->setAnimationSpeed(0.f);
		node3->setCurrentFrame(12.f);
	}

	smgr->addCameraSceneNode();

	smgr->saveScene("results/scene.irr");
	bool result = binaryCompareFiles("results/scene.irr", "media/scene.irr");

	smgr->saveScene("results/scene2.irr", 0, node3);
	result &= binaryCompareFiles("results/scene2.irr", "media/scene2.irr");

	device->closeDevice();
	device->run();
	device->drop();

	// TODO: The relative texture names are not yet fixed, so ignore this test
	return true;
}

static bool loadScene(void)
{
    IrrlichtDevice *device = createDevice(video::EDT_BURNINGSVIDEO,
										core::dimension2du(160,120), 32);
    if (!device)
        return false;

    IVideoDriver* driver = device->getVideoDriver();
	ISceneManager* smgr = device->getSceneManager();
	// load scene from example, with correct relative path
	device->getFileSystem()->changeWorkingDirectoryTo("results");
	smgr->loadScene("../../media/example.irr");
	smgr->addCameraSceneNode(0, core::vector3df(0,0,-50));
	device->getFileSystem()->changeWorkingDirectoryTo("..");

    bool result = false;
    device->run();
	if (driver->beginScene(true, true, video::SColor(0, 80, 80, 80)))
	{
		smgr->drawAll();
		driver->endScene();
		result = takeScreenshotAndCompareAgainstReference(driver, "-loadScene.png", 98.91f);
		if (!result)
			logTestString("Rendering the loaded scene failed.\n");
	}

	ISceneNode* node = smgr->getSceneNodeFromId(128);
	if (!node)
		result=false;
	else if (result) // only check if scene was correctly loaded
	{
		result &= (node->getChildren().size()==0);
		if (!result)
			logTestString("Node has an illegal child node.\n");
		device->getSceneManager()->loadScene("results/scene2.irr", 0, node);
		result &= (node->getChildren().size()!=0);
		if (!result)
			logTestString("Loading second scene as child failed.\n");
	}

	device->closeDevice();
	device->run();
    device->drop();

    return result;
}

bool ioScene(void)
{
	bool result = saveScene();
	result &= loadScene();
	return result;
}
