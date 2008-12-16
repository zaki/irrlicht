// Copyright (C) 2008 Colin MacDonald
// No rights reserved: this software is in the public domain.

#include "testUtils.h"
#include "irrlicht.h"
#include <assert.h>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;


/** Test functionality of the sceneCollisionManager */
bool sceneCollisionManager(void)
{
	IrrlichtDevice * device = irr::createDevice(video::EDT_NULL);
	assert(device);
	if(!device)
		return false;

	ISceneManager * smgr = device->getSceneManager();
	ISceneCollisionManager * collMgr = smgr->getSceneCollisionManager();

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
	device->drop();
	return result;
}


