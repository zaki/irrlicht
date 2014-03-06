/** Example 027 Helloworld_Android
	This example shows a simple application for Android.
*/

#include <irrlicht.h>

#ifdef _IRR_ANDROID_PLATFORM_

#include <android_native_app_glue.h>
#include "android_tools.h"

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
	Android is using multitouch events.
	We allow users to move around the Irrlicht logo as example of how to use those.
*/
class MyEventReceiver : public IEventReceiver
{
public:
	MyEventReceiver(IrrlichtDevice *device ) : Device(device), SpriteToMove(0), TouchID(-1)
	{
	}

	virtual bool OnEvent(const SEvent& event)
	{
		if (event.EventType == EET_TOUCH_INPUT_EVENT)
		{
			switch (event.TouchInput.Event)
			{
				case ETIE_PRESSED_DOWN:
				{
					// We only work with the first for now.
					if ( TouchID == -1 )
					{
						TouchID = event.TouchInput.ID;
						position2d<s32> touchPoint(event.TouchInput.X, event.TouchInput.Y);
						IGUIElement * logo = Device->getGUIEnvironment()->getRootGUIElement()->getElementFromId ( GUI_IRR_LOGO );
						if ( logo && logo->isPointInside (touchPoint) )
						{
							SpriteToMove = logo;
							SpriteStartRect =  SpriteToMove->getRelativePosition();
							TouchStartPos = touchPoint;
						}
					}
					break;
				}
				case ETIE_MOVED:
					if ( SpriteToMove && TouchID == event.TouchInput.ID )
					{
						position2d<s32> touchPoint(event.TouchInput.X, event.TouchInput.Y);
						MoveSprite(touchPoint);
					}
					break;
				case ETIE_LEFT_UP:
					if ( SpriteToMove && TouchID == event.TouchInput.ID )
					{
						TouchID = -1;
						position2d<s32> touchPoint(event.TouchInput.X, event.TouchInput.Y);
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
	irr::s32 TouchID;
};

/*
	You have currently the choice between 2 drivers:
	EDT_OGLES1 is basically a opengl fixed function pipeline.
	EDT_OGLES2 is a shader pipeline. Irrlicht comes with shaders to simulate 
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

	/* Logging is written to a file. So your application should disable all logging when you distribute your
       application or it can fill up that file over time.
	*/
#ifndef _DEBUG
	param.LoggingLevel = ELL_NONE;	
#endif	
	
	return createDeviceEx(param);
}

/* Mainloop.
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

/* Main application code. */
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
	ILogger* logger = device->getLogger();
	IFileSystem * fs = device->getFileSystem();
	
	/* Access to the Android native window. You often need this when accessing NDK functions like we are doing here.
	   Note that windowWidth/windowHeight have already subtracted things like the taskbar which your device might have,
	   so you get the real size of your render-window.
	*/
	ANativeWindow* nativeWindow = static_cast<ANativeWindow*>(driver->getExposedVideoData().OGLESAndroid.Window);
	int32_t windowWidth = ANativeWindow_getWidth(app->window);
	int32_t windowHeight = ANativeWindow_getHeight(app->window);
	
	/* Get display metrics. We are accessing the Java functions of the JVM directly in this case as there is no NDK function for that yet.
	   Checkout android_tools.cpp if you want to know how that is done. */
	irr::android::SDisplayMetrics displayMetrics;
	memset(&displayMetrics, 0, sizeof displayMetrics);
	irr::android::getDisplayMetrics(app, displayMetrics);

	/* For troubleshooting you can use the Irrlicht logger.
       The Irrlicht logging messages are send to the Android logging system using the tag "Irrlicht". 
	   They stay in a file there, so you can check them even after running your app.
       You can watch them with the command: "adb logcat Irrlicht:V DEBUG:V *:S"
       This means Irrlicht _V_erbose, debug messages verbose (p.E callstack on crashes) and all other messages _S_ilent.
	   Clean the logging file with: "adb logcat -c".
       See http://developer.android.com/tools/debugging/debugging-log.html for more advanced log options.
	*/
	char strDisplay[1000];
	sprintf(strDisplay, "Window size:(%d/%d)\nDisplay size:(%d/%d)", windowWidth, windowHeight, displayMetrics.widthPixels, displayMetrics.heightPixels);
	logger->log(strDisplay);
	
	/* Your media must be somewhere inside the assets folder. The assets folder is the root for the file system.
	   This example copies the media in the Android.mk makefile. */
   	stringc mediaPath = "media/";
	
	// The Android assets file-system does not know which sub-directories it has (blame google).
	// So we have to add all sub-directories in assets manually. Otherwise we could still open the files, 
	// but existFile checks will fail (which are for example needed by getFont).
	for ( u32 i=0; i < fs->getFileArchiveCount(); ++i )
	{
		IFileArchive* archive = fs->getFileArchive(i);
		if ( archive->getType() == EFAT_ANDROID_ASSET )
		{
			archive->addDirectoryToFileList(mediaPath);
			break;
		}
	}

	/* Set the font-size depending on your device.
	   dpi=dots per inch. 1 inch = 2.54 cm. */
	IGUISkin* skin = guienv->getSkin();
	IGUIFont* font = 0;
	if ( displayMetrics.xdpi < 100 )	// just guessing some value where fontsize might start to get too small
		font = guienv->getFont(mediaPath + "fonthaettenschweiler.bmp");
	else
		font = guienv->getFont(mediaPath + "bigfont.png");
	if (font)
		skin->setFont(font);
	
	// A field to show some text. Comment out stat->setText in run() if you want to see the dpi instead of the fps.
	IGUIStaticText *text = guienv->addStaticText(stringw(displayMetrics.xdpi).c_str(),
		rect<s32>(15,15,300,60), false, false, 0, GUI_INFO_FPS );

	// add irrlicht logo
	IGUIImage * logo = guienv->addImage(driver->getTexture(mediaPath + "irrlichtlogo3.png"),
					core::position2d<s32>(10,40), true, 0, GUI_IRR_LOGO);
	s32 minLogoWidth = windowWidth/3;
	if ( logo && logo->getRelativePosition().getWidth() < minLogoWidth )
	{
		/* Scale to make it better visible on high-res devices (we could also work with dpi here).
		*/
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

	/*
		Mainloop. Application never quit themself in Android. The OS is responsible for that.
	*/
	run(device);
	
	/* Cleanup */
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

/*
**/
