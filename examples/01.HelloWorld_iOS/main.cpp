#include <irrlicht.h>
#include "exampleHelper.h"

using namespace irr;

using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

// It's important for iOS projects to use 'irrlicht_main' instead of standard 'main' function.

void irrlicht_main()
{
	IrrlichtDevice *device = createDevice(EDT_OGLES2, dimension2d<u32>(0, 0), 16, false, false, false, 0);

	if (!device)
		return;

	device->setWindowCaption(L"Hello World! - Irrlicht Engine Demo");

	IVideoDriver* driver = device->getVideoDriver();
	ISceneManager* smgr = device->getSceneManager();
	IGUIEnvironment* guienv = device->getGUIEnvironment();

	guienv->addStaticText(L"Hello World! This is the Irrlicht Software renderer!", rect<s32>(10,10,260,22), true);

	const io::path mediaPath = getExampleMediaPath();

	IAnimatedMesh* mesh = smgr->getMesh(mediaPath + "sydney.md2");
	
	if (!mesh)
	{
		device->drop();
		return;
	}
	
	IAnimatedMeshSceneNode* node = smgr->addAnimatedMeshSceneNode( mesh );

	if (node)
	{
		node->setMaterialFlag(EMF_LIGHTING, false);
		node->setMD2Animation(scene::EMAT_STAND);
		node->setMaterialTexture( 0, driver->getTexture(mediaPath + "sydney.bmp") );
	}

	smgr->addCameraSceneNode(0, vector3df(0,30,-40), vector3df(0,5,0));

	while (device->run())
	if (device->isWindowActive())
	{
		driver->beginScene(ECBF_COLOR | ECBF_DEPTH, SColor(255,255,255,255));

		smgr->drawAll();
		guienv->drawAll();

		driver->endScene();
	}

	device->drop();
}
