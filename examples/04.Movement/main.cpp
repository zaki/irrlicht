/** Example 004 Movement

This Tutorial shows how to move and animate SceneNodes. The
basic concept of SceneNodeAnimators is shown as well as manual
movement of nodes using the keyboard.

As always, I include the header files, use the irr namespace,
and tell the linker to link with the .lib file.
*/
#include <irrlicht.h>
#include <iostream>

using namespace irr;

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif

/*
To receive events like mouse and keyboard input, or GUI events like "the OK
button has been clicked", we need an object which is derived from the
irr::IEventReceiver object. There is only one method to override:
irr::IEventReceiver::OnEvent(). This method will be called by the engine once
when an event happens. What we really want to know is whether a key is being
held down, and so we will remember the current state of each key.
*/
class MyEventReceiver : public IEventReceiver
{
public:
	// This is the one method that we have to implement
	virtual bool OnEvent(const SEvent& event)
	{
		// Remember whether each key is down or up
		if (event.EventType == irr::EET_KEY_INPUT_EVENT)
			KeyIsDown[event.KeyInput.Key] = event.KeyInput.PressedDown;

		return false;
	}

	// This is used to check whether a key is being held down
	virtual bool IsKeyDown(EKEY_CODE keyCode) const
	{
		return KeyIsDown[keyCode];
	}

	MyEventReceiver()
	{
		for (u32 i=0; i<KEY_KEY_CODES_COUNT; ++i)
			KeyIsDown[i] = false;
	}

private:
	// We use this array to store the current state of each key
	bool KeyIsDown[KEY_KEY_CODES_COUNT];
};


/*
The event receiver for keeping the pressed keys is ready, the actual responses
will be made inside the render loop, right before drawing the scene. So lets
just create an irr::IrrlichtDevice and the scene node we want to move. We also
create some other additional scene nodes, to show that there are also some
different possibilities to move and animate scene nodes.
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

	IrrlichtDevice* device = createDevice(driverType,
			core::dimension2d<s32>(640, 480), 16, false, false, false, &receiver);

	if (device == 0)
		return 1; // could not create selected driver.

	video::IVideoDriver* driver = device->getVideoDriver();
	scene::ISceneManager* smgr = device->getSceneManager();

	/*
	Create the node which will be moved with the 'W' and 'S' key. We create a
	sphere node, which is a built-in geometry primitive. We place the node
	at (0,0,30) and assign a texture to it to let it look a little bit more
	interesting. Because we have no dynamic lights in this scene we disable
	lighting for each model (otherwise the models would be black).
	*/
	scene::ISceneNode * node = smgr->addSphereSceneNode();
	if (node)
	{
		node->setPosition(core::vector3df(0,0,30));
		node->setMaterialTexture(0, driver->getTexture("../../media/wall.bmp"));
		node->setMaterialFlag(video::EMF_LIGHTING, false);
	}

	/*
	Now we create another node, movable using a scene node animator. Scene
	node animators modify scene nodes and can be attached to any scene node
	like mesh scene nodes, billboards, lights and even camera scene nodes.
	Scene node animators are not only able to modify the position of a
	scene node, they can also animate the textures of an object for
	example. We create a cube scene node and attach a 'fly circle' scene
	node animator to it, letting this node fly around our sphere scene node.
	*/
	scene::ISceneNode* n = smgr->addCubeSceneNode();

	if (n)
	{
		n->setMaterialTexture(0, driver->getTexture("../../media/t351sml.jpg"));
		n->setMaterialFlag(video::EMF_LIGHTING, false);
		scene::ISceneNodeAnimator* anim =
			smgr->createFlyCircleAnimator(core::vector3df(0,0,30), 20.0f);
		if (anim)
		{
			n->addAnimator(anim);
			anim->drop();
		}
	}

	/*
	The last scene node we add to show possibilities of scene node animators is
	a md2 model, which uses a 'fly straight' animator to run between to points.
	*/
	scene::IAnimatedMeshSceneNode* anms =
		smgr->addAnimatedMeshSceneNode(smgr->getMesh("../../media/sydney.md2"));

	if (anms)
	{
		scene::ISceneNodeAnimator* anim =
			smgr->createFlyStraightAnimator(core::vector3df(100,0,60),
			core::vector3df(-100,0,60), 2500, true);
		if (anim)
		{
			anms->addAnimator(anim);
			anim->drop();
		}

		/*
		To make the model look right we disable lighting, set the
		frames between which the animation should loop, rotate the
		model around 180 degrees, and adjust the animation speed and
		the texture. To set the right animation (frames and speed), we
		would also be able to just call
		"anms->setMD2Animation(scene::EMAT_RUN)" for the 'run'
		animation instead of "setFrameLoop" and "setAnimationSpeed",
		but this only works with MD2 animations, and so you know how to
		start other animations. But a good advice is to not use
		hardcoded frame-numbers...
		*/
		anms->setMaterialFlag(video::EMF_LIGHTING, false);

		anms->setFrameLoop(160, 183);
		anms->setAnimationSpeed(40);
		anms->setMD2Animation(scene::EMAT_RUN);

		anms->setRotation(core::vector3df(0,180.0f,0));
		anms->setMaterialTexture(0, driver->getTexture("../../media/sydney.bmp"));

	}


	/*
	To be able to look at and move around in this scene, we create a first
	person shooter style camera and make the mouse cursor invisible.
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
		/* Check if key W or key S is being held down, and move the
		sphere node up or down respectively.
		*/
		if(receiver.IsKeyDown(irr::KEY_KEY_W))
		{
			core::vector3df v = node->getPosition();
			v.Y += 0.02f;
			node->setPosition(v);
		}
		else if(receiver.IsKeyDown(irr::KEY_KEY_S))
		{
			core::vector3df v = node->getPosition();
			v.Y -= 0.02f;
			node->setPosition(v);
		}

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

/*
That's it. Compile and play around with the program.
**/
