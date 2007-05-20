/*
This Tutorial shows how to move and animate SceneNodes. The
basic concept of SceneNodeAnimators is shown as well as manual
movement of nodes using the keyboard.

As always, I include the header files, use the irr namespace,
and tell the linker to link with the .lib file.
*/
#include <irrlicht.h>
#include <iostream>

using namespace irr;

#pragma comment(lib, "Irrlicht.lib")

/*
In this tutorial, one of our goals is to move a scene node using some
keys on the keyboard. We store a pointer to the scene node we want to
move with the keys here.
The other pointer is a pointer to the Irrlicht Device, which we need
int the EventReceiver to manipulate the scene node and to get the 
active camera.
*/

scene::ISceneNode* node = 0;
IrrlichtDevice* device = 0;


/*
To get events like mouse and keyboard input, or GUI events like 
"the OK button has been clicked", we need an object wich is derived from the 
IEventReceiver object. There is only one method to override: OnEvent. 
This method will be called by the engine when an event happened. 
We will use this input to move the scene node with the keys W and S.
*/
class MyEventReceiver : public IEventReceiver
{
public:
	virtual bool OnEvent(SEvent event)
	{
		/*
		If the key 'W' or 'S' was left up, we get the position of the scene node,
		and modify the Y coordinate a little bit. So if you press 'W', the node
		moves up, and if you press 'S' it moves down.
		*/

		if (node != 0 && event.EventType == irr::EET_KEY_INPUT_EVENT&&
			!event.KeyInput.PressedDown)
		{
			switch(event.KeyInput.Key)
			{
			case KEY_KEY_W:
			case KEY_KEY_S:
				{
					core::vector3df v = node->getPosition();
					v.Y += event.KeyInput.Key == KEY_KEY_W ? 2.0f : -2.0f;
					node->setPosition(v);
				}
				return true;
			}
		}

		return false;
	}
};


/*
The event receiver for moving a scene node is ready. So lets just create
an Irrlicht Device and the scene node we want to move. We also create some
other additional scene nodes, to show that there are also some different 
possibilities to move and animate scene nodes.
*/
int main()
{
	// let user select driver type

	video::E_DRIVER_TYPE driverType = video::EDT_DIRECT3D9;

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
		default: return 0;
	}	

	// create device
	MyEventReceiver receiver;

	IrrlichtDevice* device = createDevice( driverType, core::dimension2d<s32>(640, 480),
		16, false, false, false, &receiver);

	if (device == 0)
		return 1; // could not create selected driver.


	video::IVideoDriver* driver = device->getVideoDriver();
	scene::ISceneManager* smgr = device->getSceneManager();


	/*
	Create the node for moving it with the 'W' and 'S' key. We create a
	sphere node, which is a built in geometry primitive. We place the node
	at (0,0,30) and assign a texture to it to let it look a little bit more
	interesting. Because we have no dynamic lights in this scene we disable
	lighting for each model (otherwise the models would be black).
	*/
	node = smgr->addSphereSceneNode();
	node->setPosition(core::vector3df(0,0,30));
	node->setMaterialTexture(0, driver->getTexture("../../media/wall.bmp"));
	node->setMaterialFlag(video::EMF_LIGHTING, false);


	/* 
	Now we create another node, moving using a scene node animator. Scene node
	animators modify scene nodes and can be attached to any scene node like
	mesh scene nodes, billboards, lights and even camera scene nodes. Scene node
	animators are not only able to modify the position of a scene node, they can
	also animate the textures of an object for example.
	We create a cube scene node and attach a 'fly circle' scene node to it, letting
	this node fly around our sphere scene node.
	*/
	scene::ISceneNode* n = smgr->addCubeSceneNode();

	if (n)
	{
		n->setMaterialTexture(0, driver->getTexture("../../media/t351sml.jpg"));
		n->setMaterialFlag(video::EMF_LIGHTING, false);
		scene::ISceneNodeAnimator* anim =
			smgr->createFlyCircleAnimator(core::vector3df(0,0,30), 20.0f);
		n->addAnimator(anim);
		anim->drop();
	}

	/*
	The last scene node we add to show possibilities of scene node animators is 
	a md2 model, which uses a 'fly straight' animator to run between to points.
	*/
	scene::IAnimatedMeshSceneNode* anms = smgr->addAnimatedMeshSceneNode(smgr->getMesh("../../media/sydney.md2"));

	if (anms)
	{
		scene::ISceneNodeAnimator* anim =
			smgr->createFlyStraightAnimator(core::vector3df(100,0,60), 
			core::vector3df(-100,0,60), 2500, true);
		anms->addAnimator(anim);
		anim->drop();

		/*
		To make to model look right we set the frames between which the animation
		should loop, rotate the model around 180 degrees, and adjust the animation speed
		and the texture.
		To set the right animation (frames and speed), we would also be able to just
		call "anms->setMD2Animation(scene::EMAT_RUN)" for the 'run' animation 
		instead of "setFrameLoop" and "setAnimationSpeed",
		but this only works with MD2 animations, and so you know how to start other animations.
		but it a good advice to use not hardcoded frame-numbers...
		*/
		anms->setMaterialFlag(video::EMF_LIGHTING, false);

		anms->setFrameLoop(160, 183);
		anms->setAnimationSpeed(40);
		anms->setMD2Animation(scene::EMAT_RUN);

		anms->setRotation(core::vector3df(0,180.0f,0));
		anms->setMaterialTexture(0, driver->getTexture("../../media/sydney.bmp"));

	}


	/*
	To be able to look at and move around in this scene, 
	we create a first person shooter style camera and make the 
	mouse cursor invisible.
	*/
	scene::ICameraSceneNode * cam = smgr->addCameraSceneNodeFPS(0, 100.0f, 100.0f);
	device->getCursorControl()->setVisible(false);

	/*
	Add a colorful irrlicht logo
	*/
	device->getGUIEnvironment()->addImage(
		driver->getTexture("../../media/irrlichtlogoalpha2.tga"),
		core::position2d<s32>(10,10));

	/*
	We have done everything, so lets draw it. We also write the current
	frames per second and the name of the driver to the caption of the
	window.
	*/
	int lastFPS = -1;

	while(device->run())
	{
		driver->beginScene(true, true, video::SColor(255,113,113,133));

		smgr->drawAll(); // draw the 3d scene
		device->getGUIEnvironment()->drawAll(); // draw the gui environment (the logo)

		driver->endScene();

		int fps = driver->getFPS();

		if (lastFPS != fps)
		{
			core::stringw tmp(L"Movement Example - Irrlicht Engine [");
			tmp += driver->getName();
			tmp += L"] fps: "; 
			tmp += fps;

			device->setWindowCaption(tmp.c_str());
			lastFPS = fps;
		}
	}

	/*
	In the end, delete the Irrlicht device.
	*/
	device->drop();
	
	return 0;
}

