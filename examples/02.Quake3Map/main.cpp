/*
This Tutorial shows how to load a Quake 3 map into the
engine, create a SceneNode for optimizing the speed of
rendering and how to create a user controlled camera.

Lets start like the HelloWorld example: We include
the irrlicht header files and an additional file to be able
to ask the user for a driver type using the console.
*/
#include <irrlicht.h>
#include <iostream>

/*
As already written in the HelloWorld example, in the Irrlicht
Engine, everything can be found in the namespace 'irr'. 
To get rid of the irr:: in front of the name of every class,
we tell the compiler that we use that namespace from now on, 
and we will not have to write that 'irr::'.
There are 5 other sub namespaces 'core', 'scene', 'video',
'io' and 'gui'. Unlike in the HelloWorld example,
we do not a 'using namespace' for these 5 other namespaces
because in this way you will see what can be found in which
namespace. But if you like, you can also include the namespaces
like in the previous example. Code just like you want to.
*/
using namespace irr;

/*
Again, to be able to use the Irrlicht.DLL file, we need to link with the 
Irrlicht.lib. We could set this option in the project settings, but
to make it easy, we use a pragma comment lib:
*/
#pragma comment(lib, "Irrlicht.lib")

/*
Ok, lets start. Again, we use the main() method as start, not the
WinMain(), because its shorter to write.
*/
int main()
{
	/*
	Like in the HelloWorld example, we create an IrrlichtDevice with
	createDevice(). The difference now is that we ask the user to select 
	which hardware accelerated driver to use. The Software device would be
	too slow to draw a huge Quake 3 map, but just for the fun of it, we make
	this decision possible too.
	*/

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

	IrrlichtDevice *device =
		createDevice(driverType, core::dimension2d<s32>(640, 480));

	if (device == 0)
		return 1; // could not create selected driver.

	/*
	Get a pointer to the video driver and the SceneManager so that
	we do not always have to write device->getVideoDriver() and
	device->getSceneManager().
	*/
	video::IVideoDriver* driver = device->getVideoDriver();
	scene::ISceneManager* smgr = device->getSceneManager();

	/*
	To display the Quake 3 map, we first need to load it. Quake 3 maps
	are packed into .pk3 files wich are nothing other than .zip files.
	So we add the .pk3 file to our FileSystem. After it was added,
	we are able to read from the files in that archive as they would
	directly be stored on disk.
	*/
	device->getFileSystem()->addZipFileArchive("../../media/map-20kdm2.pk3");

	/* 
	Now we can load the mesh by calling getMesh(). We get a pointer returned
	to a IAnimatedMesh. As you know, Quake 3 maps are not really animated,
	they are only a huge chunk of static geometry with some materials
	attached. Hence the IAnimated mesh consists of only one frame,
	so we get the "first frame" of the "animation", which is our quake level
	and create an OctTree scene node with it, using addOctTreeSceneNode().
	The OctTree optimizes the scene a little bit, trying to draw only geometry
	which is currently visible. An alternative to the OctTree would be a 
	AnimatedMeshSceneNode, which would draw always the complete geometry of 
	the mesh, without optimization. Try it out: Write addAnimatedMeshSceneNode
	instead of addOctTreeSceneNode and compare the primitives drawed by the
	video driver. (There is a getPrimitiveCountDrawed() method in the 
	IVideoDriver class). Note that this optimization with the Octree is only
	useful when drawing huge meshes consiting of lots of geometry.
	*/
	scene::IAnimatedMesh* mesh = smgr->getMesh("20kdm2.bsp");
	scene::ISceneNode* node = 0;
	
	if (mesh)
		node = smgr->addOctTreeSceneNode(mesh->getMesh(0), 0, -1, 128);

	/*
	Because the level was modelled not around the origin (0,0,0), we translate
	the whole level a little bit.
	*/
	if (node)
		node->setPosition(core::vector3df(-1300,-144,-1249));

	/*
	Now we only need a Camera to look at the Quake 3 map.
	And we want to create a user controlled camera. There are some
	different cameras available in the Irrlicht engine. For example the 
	Maya Camera which can be controlled compareable to the camera in Maya:
	Rotate with left mouse button pressed, Zoom with both buttons pressed,
	translate with right mouse button pressed. This could be created with
	addCameraSceneNodeMaya(). But for this example, we want to create a 
	camera which behaves like the ones in first person shooter games (FPS).
	*/
	smgr->addCameraSceneNodeFPS();

	/*
	The mouse cursor needs not to be visible, so we make it invisible.
	*/

	device->getCursorControl()->setVisible(false);

	/*
	We have done everything, so lets draw it. We also write the current
	frames per second and the drawn primitives to the caption of the
	window. The 'if (device->isWindowActive())' line is optional, but 
	prevents the engine render to set the position of the mouse cursor 
	after task switching when other program are active.
	*/
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
			core::stringw str = L"Irrlicht Engine - Quake 3 Map example [";
			str += driver->getName();
			str += "] FPS:";
			str += fps;

			device->setWindowCaption(str.c_str());
			lastFPS = fps;
		}
	}

	/*
	In the end, delete the Irrlicht device.
	*/
	device->drop();
	return 0;
}

