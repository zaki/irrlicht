// Copyright (C) 2008 Colin MacDonald
// No rights reserved: this software is in the public domain.

#include "testUtils.h"
#include "irrlicht.h"
#include <assert.h>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;

static bool testGetCollisionResultPosition(IrrlichtDevice * device,
										   ISceneManager * smgr,
										   ISceneCollisionManager * collMgr)
{
	IMeshSceneNode * cubeNode = smgr->addCubeSceneNode(10.f);
	ITriangleSelector * cubeSelector = smgr->createTriangleSelectorFromBoundingBox(cubeNode);

	triangle3df triOut;
	vector3df hitPosition;
	bool falling;

	vector3df resultPosition =
		collMgr->getCollisionResultPosition(cubeSelector,
										vector3df(0, 50, 0),
										vector3df(10, 20, 10),
										vector3df(0, -100, 0),
										triOut,
										hitPosition,
										falling);

	bool result = true;
	if(!equals(resultPosition.Y, 25.f, 0.01f))
	{
		logTestString("Unexpected collision response position\n");
		assert(false);
		result = false;
	}

	if(!equals(hitPosition.Y, 5.f, 0.01f))
	{
		logTestString("Unexpected collision position\n");
		assert(false);
		result = false;
	}

	resultPosition =
		collMgr->getCollisionResultPosition(cubeSelector,
										vector3df(-20, 0, 0),
										vector3df(10, 20, 10),
										vector3df(100, 0, 0),
										triOut,
										hitPosition,
										falling);

	if(!equals(resultPosition.X, -15.f, 0.01f))
	{
		logTestString("Unexpected collision response position\n");
		assert(false);
		result = false;
	}

	if(!equals(hitPosition.X, -5.f, 0.01f))
	{
		logTestString("Unexpected collision position\n");
		assert(false);
		result = false;
	}

	cubeSelector->drop();
	smgr->clear();
	return result;
}


static bool testGetSceneNodeFromScreenCoordinatesBB(IrrlichtDevice * device,
													ISceneManager * smgr,
													ISceneCollisionManager * collMgr)
{
	IMeshSceneNode * cubeNode1 = smgr->addCubeSceneNode(10.f, 0, -1, vector3df(0, 0, 20));
	IMeshSceneNode * cubeNode2 = smgr->addCubeSceneNode(10.f, 0, -1, vector3df(0, 0, 30));
	IMeshSceneNode * cubeNode3 = smgr->addCubeSceneNode(10.f, 0, -1, vector3df(0, 0, 40));

	ICameraSceneNode * camera = smgr->addCameraSceneNode();
	device->run();
	smgr->drawAll(); // Get the camera in a good state

	ISceneNode * hitNode = collMgr->getSceneNodeFromScreenCoordinatesBB(position2d<s32>(80, 60));

	// Expect the first node to be hit.
	bool result = true;
	if(hitNode != cubeNode1)
	{
		logTestString("Unexpected node hit. Expected cubeNode1.\n");
		result = false;
	}

	// Now make cubeNode1 invisible and check that cubeNode2 is hit.
	cubeNode1->setVisible(false);
	hitNode = collMgr->getSceneNodeFromScreenCoordinatesBB(position2d<s32>(80, 60));
	if(hitNode != cubeNode2)
	{
		logTestString("Unexpected node hit. Expected cubeNode2.\n");
		result = false;
	}

	// Make cubeNode1 the parent of cubeNode2.
	cubeNode2->setParent(cubeNode1);

	// Check visibility.
	bool visible = cubeNode2->isVisible();
	if(!visible)
	{
		logTestString("cubeNode2 should think that it (in isolation) is visible.\n");
		result = false;
	}

	visible = cubeNode2->isTrulyVisible();
	if(visible)
	{
		logTestString("cubeNode2 should know that it (recursively) is invisible.\n");
		result = false;
	}

	// cubeNode2 should now be an invalid target as well, and so the final cube node should be hit.
	hitNode = collMgr->getSceneNodeFromScreenCoordinatesBB(position2d<s32>(80, 60));
	if(hitNode != cubeNode3)
	{
		logTestString("Unexpected node hit. Expected cubeNode3.\n");
		result = false;
	}


	// Make cubeNode3 invisible and check that the camera node is hit (since it has a valid bounding box).
	cubeNode3->setVisible(false);
	hitNode = collMgr->getSceneNodeFromScreenCoordinatesBB(position2d<s32>(80, 60));
	if(hitNode != camera)
	{
		logTestString("Unexpected node hit. Expected the camera node.\n");
		result = false;
	}

	// Now verify bitmasking
	camera->setID(0xAAAAAAAA); // == 101010101010101010101010101010
	hitNode = collMgr->getSceneNodeFromScreenCoordinatesBB(position2d<s32>(80, 60), 0x02);
	if(hitNode != camera)
	{
		logTestString("Unexpected node hit. Expected the camera node.\n");
		result = false;
	}

	// Test the 01010101010101010101010101010101 bitmask (0x55555555)
	hitNode = collMgr->getSceneNodeFromScreenCoordinatesBB(position2d<s32>(80, 60), 0x55555555);
	if(hitNode != 0)
	{
		logTestString("A node was hit when none was expected.\n");
		result = false;
	}

	smgr->clear();

	if(!result)
		assert(false);
	return result;
}


static bool getScaledPickedNodeBB(IrrlichtDevice * device,
									ISceneManager * smgr,
									ISceneCollisionManager * collMgr)
{
    ISceneNode* farTarget = smgr->addCubeSceneNode(1.f);
    farTarget->setScale(vector3df(100.f, 100.f, 10.f));
    farTarget->setPosition(vector3df(0.f, 0.f, 500.f));
    farTarget->updateAbsolutePosition();

    ISceneNode* nearTarget = smgr->addCubeSceneNode(10.f);
    nearTarget->setPosition(vector3df(0.f, 0.f, 100.f));
    nearTarget->updateAbsolutePosition();

	line3df ray(0.f, 0.f, 0.f, 0.f, 0.f, 500.f);

	const ISceneNode * const hit = collMgr->getSceneNodeFromRayBB(ray);

	bool result = (hit == nearTarget);

	if(hit == 0)
		logTestString("getSceneNodeFromRayBB() didn't hit anything.\n");
	else if(hit == farTarget)
		logTestString("getSceneNodeFromRayBB() hit the far (scaled) target.\n");

	device->drop();

	if(!result)
		assert(false);

	return result;
}



/** Test functionality of the sceneCollisionManager */
bool sceneCollisionManager(void)
{
	IrrlichtDevice * device = irr::createDevice(video::EDT_NULL, dimension2d<s32>(160, 120));
	assert(device);
	if(!device)
		return false;

	ISceneManager * smgr = device->getSceneManager();
	ISceneCollisionManager * collMgr = smgr->getSceneCollisionManager();

	bool result = testGetCollisionResultPosition(device, smgr, collMgr);

	result &= testGetSceneNodeFromScreenCoordinatesBB(device, smgr, collMgr);

	result &= getScaledPickedNodeBB(device, smgr, collMgr);

	device->drop();
	return result;
}


