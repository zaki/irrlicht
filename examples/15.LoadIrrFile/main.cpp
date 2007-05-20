/*
Since version 1.1, Irrlicht is able to save and load
the full scene graph into an .irr file, an xml based
format. There is an editor available to edit
those files, named irrEdit on http://www.ambiera.com/irredit,
which can also be used as world and particle editor.
This tutorial shows how to use .irr files.

Lets start: Create an Irrlicht device and setup the window.
*/

#include <irrlicht.h>
#include <iostream>
using namespace irr;

#pragma comment(lib, "Irrlicht.lib")

int main()
{
	// ask user for driver

	video::E_DRIVER_TYPE driverType;

	printf("Please select the driver you want for this example:\n"\
		" (a) Direct3D 9.0c\n (b) Direct3D 8.1\n (c) OpenGL 1.5\n"\
		" (d) Software Renderer\n (e) Burning's Software Renderer\n"\
		" (f) NullDevice\n (otherKey) exit\n\n");

	char i;
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

	// create device and exit if creation failed

	IrrlichtDevice* device =
		createDevice(driverType, core::dimension2d<s32>(640, 480));

	if (device == 0)
		return 1; // could not create selected driver.

	device->setWindowCaption(L"Load .irr file example");

	video::IVideoDriver* driver = device->getVideoDriver();
	scene::ISceneManager* smgr = device->getSceneManager();

	/* Now load our .irr file.
	.irr files can store the whole scene graph including animators, materials
	and particle systems. And there is also the possibility to store arbitrary
	user data for every scene node in that file. To keep this 
	example simple, we are simply loading the scene here. See the documentation
	at ISceneManager::loadScene and ISceneManager::saveScene for more information.
	So to load and display a complicated huge scene, we only need a single call
	to loadScene().
	*/

	// load the scene

	smgr->loadScene("../../media/example.irr");

	/* 
	That was it already. Now add a camera and draw the scene
	*/

	// add a user controlled camera

	smgr->addCameraSceneNodeFPS();

	// and draw everything.

	int lastFPS = -1;

	while(device->run())
	if (device->isWindowActive())
	{
		driver->beginScene(true, true, video::SColor(0,200,200,200));
		smgr->drawAll();
		driver->endScene();

		int fps = driver->getFPS();

		if (lastFPS != fps)
		{
		  core::stringw str = L"Load Irrlicht File example - Irrlicht Engine [";
		  str += driver->getName();
		  str += "] FPS:";
		  str += fps;

		  device->setWindowCaption(str.c_str());
		  lastFPS = fps;
		}

	}

	device->drop();
	
	return 0;
}

