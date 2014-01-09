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


enum GUI_IDS
{
	GUI_INFO_FPS,
	GUI_IRR_LOGO
};


/*
	Android is using multitouch events
	We move around our Irrlicht logo to show how to use them.
*/
class MyEventReceiver : public IEventReceiver
{
public:
	MyEventReceiver(IrrlichtDevice *device ) : Device(device), SpriteToMove(0) 
	{
	}

	virtual bool OnEvent(const SEvent& event)
	{
		if (event.EventType == EET_MULTI_TOUCH_EVENT)
		{
			switch ( event.MultiTouchInput.Event)
			{
				case EMTIE_PRESSED_DOWN:
				{
			
					// We only work with the first for now, but could be up to NUMBER_OF_MULTI_TOUCHES touches.
					position2d<s32> touchPoint(event.MultiTouchInput.X[0], event.MultiTouchInput.Y[0]);
					IGUIElement * logo = Device->getGUIEnvironment()->getRootGUIElement()->getElementFromId ( GUI_IRR_LOGO );
					if ( logo && logo->isPointInside (touchPoint) )
					{
						SpriteToMove = logo;
						SpriteStartRect =  SpriteToMove->getRelativePosition();
						TouchStartPos = touchPoint;
					}
					break;
				}
				case EMTIE_MOVED:
					if ( SpriteToMove )
					{
						position2d<s32> touchPoint(event.MultiTouchInput.X[0], event.MultiTouchInput.Y[0]);
						MoveSprite(touchPoint);
					}
					break;
				case EMTIE_LEFT_UP:
					if ( SpriteToMove )
					{
						position2d<s32> touchPoint(event.MultiTouchInput.X[0], event.MultiTouchInput.Y[0]);
						MoveSprite(touchPoint);
						SpriteToMove = 0;						
					}
					break;
				default:
					break;
			}
		}

		return false;
	}
	
	void MoveSprite(const irr::core::position2d<irr::s32> &touchPos)
	{
		irr::core::position2d<irr::s32> move(touchPos-TouchStartPos);
		SpriteToMove->setRelativePosition(SpriteStartRect.UpperLeftCorner + move);
	}
	
private:
	IrrlichtDevice * Device;
	irr::gui::IGUIElement * SpriteToMove;
	irr::core::rect<s32> SpriteStartRect;
	irr::core::position2d<irr::s32> TouchStartPos;
};

/*!
	You have currently the choice between 2 drivers:
	EDT_OGLES1 Is basically a opengl fixed function pipeline.
	EDT_OGLES2 Is a shader pipeline. Irrlicht comes with shaders to simulate 
               typical fixed function materials. For this to work the 
               corresponding shaders from the Irrlicht media/Shaders folder are
               copied to the application assets folder (done in the Makefile).
*/
IrrlichtDevice *startup(android_app* app)
{
	// create device
	SIrrlichtCreationParameters param;
//	param.DriverType = EDT_OGLES1;				// android:glEsVersion in AndroidManifest.xml should be "0x00010000"
	param.DriverType = EDT_OGLES2;				// android:glEsVersion in AndroidManifest.xml should be "0x00020000"
	param.WindowSize = dimension2d<u32>(0,0);	// using 0,0 it will automatically set it to the maximal size
	param.PrivateData = app;
	param.Bits = 24;
	param.ZBufferBits = 16;
	param.AntiAlias  = 0;

	return createDeviceEx(param);
}

/*! mainloop
*/
int run ( IrrlichtDevice *device )
{
	IGUIElement *stat = device->getGUIEnvironment()->getRootGUIElement()->getElementFromId ( GUI_INFO_FPS );
	
	while(device->run())
	{
		if (device->isWindowActive())
		{
			device->getVideoDriver()->beginScene(true, true, SColor(0,100,100,100));
			device->getSceneManager()->drawAll();
			device->getGUIEnvironment()->drawAll();
			device->getVideoDriver()->endScene ();

			if ( stat )
			{
				stringw str = L"FPS: ";
				str += (s32)device->getVideoDriver()->getFPS();

				stat->setText ( str.c_str() );
			}
		}
		device->yield(); // probably nicer to the battery
	}

	return 1;
}

int example_helloworld(android_app* app)
{
	// create device
	IrrlichtDevice *device = startup(app);
	if (device == 0)
       	return 1;

	MyEventReceiver receiver(device);
	device->setEventReceiver(&receiver);
	
	IVideoDriver* driver = device->getVideoDriver();
	ISceneManager* smgr = device->getSceneManager();
	IGUIEnvironment* guienv = device->getGUIEnvironment();
	
	// access to the Android native window
	ANativeWindow* nativeWindow = static_cast<ANativeWindow*>(driver->getExposedVideoData().OGLESAndroid.window);
	int32_t windowWidth = ANativeWindow_getWidth(app->window);
	int32_t windowHeight = ANativeWindow_getHeight(app->window);

   	stringc mediaPath = "media/";
	
	
	IGUIStaticText *text = guienv->addStaticText(L"FPS: 25",
		rect<s32>(10,15,200,30), false, false, 0, GUI_INFO_FPS );

	// add irrlicht logo
	IGUIImage * logo = guienv->addImage(driver->getTexture(mediaPath + "irrlichtlogo3.png"),
					core::position2d<s32>(10,40), true, 0, GUI_IRR_LOGO);
	s32 minLogoWidth = windowWidth/3;
	if ( logo && logo->getRelativePosition().getWidth() < minLogoWidth )
	{
		// scale to make it better visible on high-res devices		
		logo->setScaleImage(true);
		core::rect<s32> logoPos(logo->getRelativePosition());
		f32 scale = (f32)minLogoWidth/(f32)logoPos.getWidth();
		logoPos.LowerRightCorner.X = logoPos.UpperLeftCorner.X + minLogoWidth;
		logoPos.LowerRightCorner.Y = logoPos.UpperLeftCorner.Y + (s32)((f32)logoPos.getHeight()*scale);
		logo->setRelativePosition(logoPos);
	}
	

	// Add a 3d model
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
	
	device->setEventReceiver(0);
	device->closeDevice();
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
