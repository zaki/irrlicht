// This test validates the last frame of a non-looped MD2 animation

#include "irrlicht.h"
#include "testUtils.h"
#include <assert.h>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

bool md2Animation(void)
{
	IrrlichtDevice *device = createDevice( EDT_OPENGL, dimension2d<s32>(640, 480));
	assert(device);
	if (!device)
		return false;
	
	IVideoDriver* driver = device->getVideoDriver();
	ISceneManager * smgr = device->getSceneManager();

	IAnimatedMesh* mesh = smgr->getMesh("../media/sydney.md2");
	IAnimatedMeshSceneNode* node;
	assert(mesh);

	if(mesh)
	{
		node = smgr->addAnimatedMeshSceneNode(mesh);
		assert(node);

		if(node)
		{
			node->setPosition(vector3df(20, 0, 50));
			node->setMaterialFlag(EMF_LIGHTING, false);
			node->setMaterialTexture(0, driver->getTexture("../media/sydney.bmp"));
			node->setLoopMode(false);

			(void)smgr->addCameraSceneNode();

			// We could just jump to the last frame, but where's the fun in that? 
			// Let's watch the animation, doing the initial run() first so that we
			// don't miss any of the fun.
			device->run();

			node->setMD2Animation(EMAT_DEATH_FALLBACK);
			const s32 endFrame = node->getEndFrame();

			while((s32)node->getFrameNr() < endFrame)
			{
				device->run();
				driver->beginScene(true, true, SColor(255, 255, 255, 0));
				smgr->drawAll();
				driver->endScene();
			}
		}
	}

	bool result = takeScreenshotAndCompareAgainstReference(driver, "-md2Animation.jpg");
	device->drop();

	return result;
}