// Copyright (C) 2008-2009 Colin MacDonald
// No rights reserved: this software is in the public domain.

#include "testUtils.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

// Tests B3D animations.
bool b3dAnimation(void)
{
	// Use EDT_BURNINGSVIDEO since it is not dependent on (e.g.) OpenGL driver versions.
	IrrlichtDevice *device = createDevice( EDT_BURNINGSVIDEO, dimension2d<u32>(160, 120), 32);
	assert(device);
	if (!device)
		return false;

	IVideoDriver* driver = device->getVideoDriver();
	ISceneManager * smgr = device->getSceneManager();

	ISkinnedMesh* mesh = (ISkinnedMesh*)smgr->getMesh("../media/ninja.b3d");
	assert(mesh);

	bool result = false;
	if (!mesh)
		return false;

	IAnimatedMeshSceneNode* node1 = smgr->addAnimatedMeshSceneNode(mesh);
	assert(node1);

	/** Verify that two skinned animated mesh scene nodes can use different frames of the skinned mesh */
	if(node1)
	{
		node1->setPosition(vector3df(-3, -3, 10));
		node1->setMaterialFlag(EMF_LIGHTING, false);
		node1->setAnimationSpeed(0.f);
		node1->setCurrentFrame(10.f);
		node1->setDebugDataVisible(irr::scene::EDS_BBOX_BUFFERS);
	}

	IAnimatedMeshSceneNode* node2 = smgr->addAnimatedMeshSceneNode(mesh);
	assert(node2);
	if(node2)
	{
		node2->setPosition(vector3df(3, -3, 10));
		node2->setMaterialFlag(EMF_LIGHTING, false);
		node2->setAnimationSpeed(0.f);
		node2->setCurrentFrame(62.f);
		node2->setDebugDataVisible(irr::scene::EDS_BBOX_BUFFERS);
	}

	(void)smgr->addCameraSceneNode();

	// Just jump to the last frame since that's all we're interested in.
	device->run();
	driver->beginScene(true, true, SColor(255, 60, 60, 60));
	smgr->drawAll();
	driver->endScene();

	result = takeScreenshotAndCompareAgainstReference(driver, "-b3dAnimation.png");
	if (node2)
		node2->remove();

	// TODO: Does not yet work, seems like bones are not properly placed
#if 0
	/** Now test if bones are correctly positioned. */
	node1->setDebugDataVisible(scene::EDS_SKELETON);
	node1->setPosition(vector3df(1, -5, 8));
	node1->setRotation(core::vector3df(0,180,0));
	for (u32 i=0; i<node1->getJointCount(); ++i)
	{
		smgr->addCubeSceneNode(5,0,-1,node1->getJointNode(i)->getAbsolutePosition());
//		smgr->addCubeSceneNode(1.f,node1->getJointNode(i));
	}

	// Simple render call
	device->run();
	driver->beginScene(true, true, SColor(255, 60, 60, 60));
	smgr->drawAll();
	driver->endScene();

	result &= takeScreenshotAndCompareAgainstReference(driver, "-b3dJointPosition.png");
#endif

	device->drop();

	return result;
}
