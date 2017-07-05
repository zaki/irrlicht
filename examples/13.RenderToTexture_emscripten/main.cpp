/** Example 013 Render To Texture on emscripten
*/

#include <irrlicht.h>
#include <emscripten.h>
#include "exampleHelper.h"

using namespace irr;

IrrlichtDevice *device = 0;
video::IVideoDriver* driver = 0;
scene::ISceneManager* smgr = 0;
gui::IGUIEnvironment* guienv = 0;
scene::ICameraSceneNode* fpsCamera = 0;
scene::ICameraSceneNode* fixedCam = 0;
video::IRenderTarget* renderTarget = 0;
scene::ISceneNode* cube = 0;

void one_iter()
{
    if(!device->run())
	{
		// Could clean up here in theory, but not sure if it makes a difference

		/*
		This tells emscripten to not run any further code.
		*/
        emscripten_cancel_main_loop();
        return;
    }

	driver->beginScene(video::ECBF_COLOR | video::ECBF_DEPTH, video::SColor(0));

	if (renderTarget)
	{
		// draw scene into render target

		// set render target
		driver->setRenderTargetEx(renderTarget, video::ECBF_COLOR | video::ECBF_DEPTH, video::SColor(0,0,0,255));

		// make cube invisible and set fixed camera as active camera
		cube->setVisible(false);
		smgr->setActiveCamera(fixedCam);

		// draw whole scene into render buffer
		smgr->drawAll();

		// set back old render target
		// The buffer might have been distorted, so clear it
		driver->setRenderTargetEx(0, 0, video::SColor(0));

		// make the cube visible and set the user controlled camera as active one
		cube->setVisible(true);
		smgr->setActiveCamera(fpsCamera);
	}

	// draw scene normally
	smgr->drawAll();
	guienv->drawAll();

	driver->endScene();
}

int main()
{
	// create device and exit if creation failed

	device = createDevice(video::EDT_WEBGL1, core::dimension2d<u32>(640, 480), 16, false, false);

	if (device == 0)
		return 1; // could not create selected driver.

	driver = device->getVideoDriver();
	smgr = device->getSceneManager();
	guienv = device->getGUIEnvironment();

	const io::path mediaPath = getExampleMediaPath();


	// load and display animated fairy mesh
	scene::IAnimatedMeshSceneNode* fairy = smgr->addAnimatedMeshSceneNode(
		smgr->getMesh(mediaPath + "faerie.md2"));

	if (!fairy)
		return 1;

	fairy->setMaterialTexture(0, driver->getTexture(mediaPath + "faerie2.bmp")); // set diffuse texture
	fairy->setMaterialFlag(video::EMF_LIGHTING, true); // enable dynamic lighting
	fairy->getMaterial(0).Shininess = 20.0f; // set size of specular highlights
	fairy->setPosition(core::vector3df(-10,0,-100));
	fairy->setMD2Animation ( scene::EMAT_STAND );

	// add white light
	smgr->addLightSceneNode(0, core::vector3df(-15,5,-105),	video::SColorf(1.0f, 1.0f, 1.0f));

	// set ambient light
	smgr->setAmbientLight(video::SColor(0,60,60,60));

	/*
	The next is just some standard stuff: Add a test cube and let it rotate
	to make the scene more interesting. The user defined camera and cursor
	setup is made later on, right before the render loop.
	*/

	// create test cube
	cube = smgr->addCubeSceneNode(60);

	// let the cube rotate and set some light settings
	scene::ISceneNodeAnimator* anim = smgr->createRotationAnimator(
		core::vector3df(0.3f, 0.3f,0));

	cube->setPosition(core::vector3df(-100,0,-100));
	cube->setMaterialFlag(video::EMF_LIGHTING, false); // disable dynamic lighting
	cube->addAnimator(anim);
	anim->drop();

	// set window caption
	device->setWindowCaption(L"Irrlicht Engine - Render to Texture and Specular Highlights example");

	/*
	To test out the render to texture feature, we need a render target
	texture. These are not like standard textures, but need to be created
	first. To create one, we call IVideoDriver::addRenderTargetTexture()
	and specify the size of the texture. Please don't use sizes bigger than
	the frame buffer for this, because the render target shares the zbuffer
	with the frame buffer.
	Because we want to render the scene not from the user camera into the
	texture, we add another fixed camera to the scene. But before we do all
	this, we check if the current running driver is able to render to
	textures. If it is not, we simply display a warning text.
	*/

	// create render target
	if (driver->queryFeature(video::EVDF_RENDER_TO_TARGET))
	{
		video::ITexture* renderTargetTex = driver->addRenderTargetTexture(core::dimension2d<u32>(256, 256), "RTT1", video::ECF_A8R8G8B8);
		video::ITexture* renderTargetDepth = driver->addRenderTargetTexture(core::dimension2d<u32>(256, 256), "DepthStencil", video::ECF_D16);

		renderTarget = driver->addRenderTarget();
		renderTarget->setTexture(renderTargetTex, renderTargetDepth);

		cube->setMaterialTexture(0, renderTargetTex); // set material of cube to render target

		// add fixed camera
		fixedCam = smgr->addCameraSceneNode(0, core::vector3df(10,10,-80),
			core::vector3df(-10,10,-100));
	}
	else
	{
		return 1;
	}

	// add fps camera
	fpsCamera = smgr->addCameraSceneNodeFPS();
	fpsCamera->setPosition(core::vector3df(-50,50,-150));

/*
	Setting fps to 0 or a negative value will use the browser’s
	requestAnimationFrame mechanism to call the main loop function.
	Emscripten documentation recommends to do that, but you can also set
	another fps value and the browser will try to call the main-loop
	fps times per second.
	The simulate_infinite_loop tells emscripten that this is an application
	which will simulate an infinite loop. There is also a flag in the
	Makefile about that: -s NO_EXIT_RUNTIME=1
	*/
	int fps = 0;
	int simulate_infinite_loop = 1;
	emscripten_set_main_loop(one_iter, fps, simulate_infinite_loop);

	return 0;
}
