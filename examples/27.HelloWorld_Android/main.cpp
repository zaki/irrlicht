/** Example 27 Helloworld_Android
	This example shows Hello World for Android.
	You need a GUI, because otherwise you can't quit the application.
	You need a Filesystem, which is relative based to your executable.
*/

#include <irrlicht.h>

#ifdef _IRR_ANDROID_PLATFORM_

#include <android_native_app_glue.h>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

#pragma comment(lib, "Irrlicht.lib")


enum GUI_IDS
{
	GUI_INFO_FPS = 100
};

IrrlichtDevice *startup(android_app* app)
{
	// create device
	IrrlichtDevice *device = 0;
    
	SIrrlichtCreationParameters param;
	param.DriverType = EDT_OGLES2;
	param.WindowSize = dimension2d<u32>(480,854);
	param.PrivateData = app;
	param.Bits = 24;
    	param.ZBufferBits = 16;
	param.AntiAlias  = 0;

	device = createDeviceEx(param);
	if (!device)
		return 0;

	IVideoDriver* driver = device->getVideoDriver();
	ISceneManager* smgr = device->getSceneManager();
	IGUIEnvironment* guienv = device->getGUIEnvironment();
    
   	stringc mediaPath = "media/";

	IGUIStaticText *text = guienv->addStaticText(L"FPS: 25",
		rect<s32>(140,15,200,30), false, false, 0, GUI_INFO_FPS );

	// add irrlicht logo
	guienv->addImage(driver->getTexture(mediaPath + "irrlichtlogo3.png"),
					core::position2d<s32>(0,-2));
	return device;
}

/*! mainloop
*/
int run ( IrrlichtDevice *device )
{
	while(device->run())
	{
		if (device->isWindowActive())
		{
			device->getVideoDriver()->beginScene(true, true, SColor(0,100,100,100));
			device->getSceneManager()->drawAll();
			device->getGUIEnvironment()->drawAll();
			device->getVideoDriver()->endScene ();

			IGUIElement *stat = device->getGUIEnvironment()->
				getRootGUIElement()->getElementFromId ( GUI_INFO_FPS );
			if ( stat )
			{
				stringw str = L"FPS: ";
				str += (s32)device->getVideoDriver()->getFPS();

				stat->setText ( str.c_str() );
			}
		}
	}

	device->drop();

	return 1;
}

int example_helloworld(android_app* app)
{
	// create device
	IrrlichtDevice *device = startup(app);

	if (device == 0)
       	return 1;

	IVideoDriver* driver = device->getVideoDriver();
	ISceneManager* smgr = device->getSceneManager();
	IGUIEnvironment* guienv = device->getGUIEnvironment();

   	stringc mediaPath = "media/";

	IAnimatedMesh* mesh = smgr->getMesh(mediaPath + "sydney.md2");

	if (!mesh)
	{
		device->drop();
       	return 1;
	}

	/*
	To let the mesh look a little bit nicer, we change its material. We
	disable lighting because we do not have a dynamic light in here, and
	the mesh would be totally black otherwise. Then we set the frame loop,
	such that the predefined STAND animation is used. And last, we apply a
	texture to the mesh. Without it the mesh would be drawn using only a
	color.
	*/
	IAnimatedMeshSceneNode* node = smgr->addAnimatedMeshSceneNode( mesh );	
	if (node)
	{
		node->setMaterialFlag(EMF_LIGHTING, false);
		node->setMD2Animation(scene::EMAT_STAND);
		node->setMaterialTexture( 0, driver->getTexture(mediaPath + "sydney.bmp") );
	}

	/*
	To look at the mesh, we place a camera into 3d space at the position
	(0, 30, -40). The camera looks from there to (0,5,0), which is
	approximately the place where our md2 model is.
	*/
	smgr->addCameraSceneNode(0, vector3df(0,30,-40), vector3df(0,5,0));

	run(device);
	device->drop();

	return 0;
}

void android_main(android_app* app)
{
	// Make sure glue isn't stripped.
	app_dummy();
	
	example_helloworld(app);
}

#endif	// defined(_IRR_ANDROID_PLATFORM_)
