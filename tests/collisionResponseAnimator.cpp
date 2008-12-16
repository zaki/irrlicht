// Copyright (C) 2008 Colin MacDonald
// No rights reserved: this software is in the public domain.

#include "testUtils.h"
#include "irrlicht.h"
#include <assert.h>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;


/** Test that collision response animator will reset itself when removed from a
	scene node, so that the scene node can then be moved without the animator
	jumping it back again. */
bool collisionResponseAnimator(void)
{
	IrrlichtDevice * device = irr::createDevice(video::EDT_NULL);
	assert(device);
	if(!device)
		return false;

	ISceneManager * smgr = device->getSceneManager();

	// Create 2 nodes to the left of a "wall"
	ISceneNode * testNode1 = smgr->addEmptySceneNode();
	ISceneNode * testNode2 = smgr->addEmptySceneNode();
	testNode1->setPosition(vector3df(-50, 0,0));
	testNode2->setPosition(vector3df(-50, 0,0));

	// Create a "wall" node, and collision response animators for each test node.
	IMeshSceneNode * wallNode = smgr->addCubeSceneNode(10.f);

	ITriangleSelector * wallSelector = smgr->createTriangleSelectorFromBoundingBox(wallNode);
	ISceneNodeAnimatorCollisionResponse * collisionAnimator1 =
		smgr->createCollisionResponseAnimator(wallSelector,
												testNode1,
												vector3df(10,10,10),
												vector3df(0, 0, 0));
	testNode1->addAnimator(collisionAnimator1);
	collisionAnimator1->drop();
	collisionAnimator1 = 0;

	ISceneNodeAnimatorCollisionResponse * collisionAnimator2 =
		smgr->createCollisionResponseAnimator(wallSelector,
												testNode2,
												vector3df(10,10,10),
												vector3df(0, 0, 0));
	testNode2->addAnimator(collisionAnimator2);
	
	wallSelector->drop();
	// Don't drop() collisionAnimator2 since we're going to use it.

	// Get the system in a good state
	device->run();
	smgr->drawAll();

	// Try to move both nodes to the right of the wall.
	// This one should be stopped by its animator.
	testNode1->setPosition(vector3df(50, 0,0));

	// Whereas this one, by forcing the animator to update its target node, should be
	// able to pass through the wall. (In <=1.6 it was stopped by the wall even if
	// the animator was removed and later re-added);
	testNode2->setPosition(vector3df(50, 0,0));
	collisionAnimator2->setTargetNode(testNode2);
	collisionAnimator2->drop(); // We're done using this now.

	device->run();
	smgr->drawAll();

	bool result = true;

	if(testNode1->getAbsolutePosition().X > -15.f)
	{
		logTestString("collisionResponseAnimator test node 1 wasn't stopped from moving.\n");
		assert(false);
		result = false;
	}

	if(testNode2->getAbsolutePosition().X < 50.f)
	{
		logTestString("collisionResponseAnimator test node 2 was stopped from moving.\n");
		assert(false);
		result = false;
	}

	// Now try to move the second node back through the wall again. Now it should be
	// stopped by the wall.
	testNode2->setPosition(vector3df(-50, 0, 0));
	device->run();
	smgr->drawAll();

	if(testNode2->getAbsolutePosition().X < 15.f)
	{
		logTestString("collisionResponseAnimator test node 2 wasn't stopped from moving.\n");
		assert(false);
		result = false;
	}

	device->drop();
	return result;
}


