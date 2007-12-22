#include <irrlicht.h>
#include <iostream>

// include the gui creator element factory
#include "CGUIEditFactory.h"

using namespace irr;
using namespace gui;

#pragma comment(lib, "Irrlicht.lib")

int main()
{
	// ask user for driver

	video::E_DRIVER_TYPE driverType;

	printf("Please select the driver you want for this example:\n"\
		" (a) Direct3D 9.0c\n (b) Direct3D 8.1\n (c) OpenGL 1.5\n"\
		" (d) Software Renderer\n (e) Burning's Software Renderer\n"\
		" (f) NullDevice\n (otherKey) exit\n\n");

	char i='a';
	std::cin >> i;

	switch(i)
	{
		case 'a': driverType = video::EDT_DIRECT3D9;break;
		case 'b': driverType = video::EDT_DIRECT3D8;break;
		case 'c': driverType = video::EDT_OPENGL;   break;
		case 'd': driverType = video::EDT_SOFTWARE; break;
		case 'e': driverType = video::EDT_BURNINGSVIDEO;break;
		case 'f': driverType = video::EDT_NULL;     break;
		default: return 1;
	}	


	IrrlichtDevice *device =createDevice(driverType, core::dimension2d<s32>(800, 600));
	video::IVideoDriver* driver = device->getVideoDriver();
	scene::ISceneManager* smgr = device->getSceneManager();
	gui::IGUIEnvironment *env = device->getGUIEnvironment();

	/*
		first we create the factory which can make new GUI elements
		and register it with the gui environment.
	*/

	IGUIElementFactory* factory = new CGUIEditFactory(env);
	env->registerGUIElementFactory(factory);
	// remember to drop since we created with a create call
	factory->drop();

	IGUISkin *skin = env->createSkin(EGST_WINDOWS_METALLIC);
	env->setSkin(skin);

	IGUIFont *font = env->getFont("../../media/lucida.xml");
	if (font)
		skin->setFont(font);
	skin->drop();

	// change transparency of skin
	for (s32 i=0; i<gui::EGDC_COUNT ; ++i)
	{
		video::SColor col = env->getSkin()->getColor((gui::EGUI_DEFAULT_COLOR)i);
		col.setAlpha(250);
		env->getSkin()->setColor((gui::EGUI_DEFAULT_COLOR)i, col);
	}

	/*
		now we add the GUI Editor Workspace
	*/

	env->loadGUI("guiTest.xml");

	env->addGUIElement("GUIEditor");
	
	while(device->run()) 
	{
		device->sleep(10);

		if (device->isWindowActive())
		{
			driver->beginScene(true, true, video::SColor(0,200,200,200));
			smgr->drawAll();
			env->drawAll();
			driver->endScene();
		}
	}

	device->drop();
	
	return 0;
}


