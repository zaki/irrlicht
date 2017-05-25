/** Example 027 Helloworld_Android
	This example shows a simple application for Android.
*/

#include <irrlicht.h>

#ifdef _IRR_ANDROID_PLATFORM_

#include <android_native_app_glue.h>
#include "android_tools.h"
#include "android/window.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;


enum GUI_IDS
{
	GUI_INFO_FPS,
	GUI_IRR_LOGO,
};


/*
	Android is using multitouch events.
	We allow users to move around the Irrlicht logo as example of how to use those.
*/
class MyEventReceiver : public IEventReceiver
{
public:
	MyEventReceiver(android_app* app )
	: Device(0), AndroidApp(app), SpriteToMove(0), TouchID(-1)
	{
	}

	void Init(IrrlichtDevice *device)
	{
		Device = device;
	}

	virtual bool OnEvent(const SEvent& event)
	{
		if (event.EventType == EET_TOUCH_INPUT_EVENT)
		{
			/*
				For now we fake mouse-events. Touch-events will be handled inside Irrlicht in the future, but until
				that is implemented you can use this workaround to get a GUI which works at least for simple elements like
				buttons. That workaround does ignore multi-touch events - if you need several buttons pressed at the same
				time you have to handle that yourself.
			*/
			SEvent fakeMouseEvent;
			fakeMouseEvent.EventType = EET_MOUSE_INPUT_EVENT;
			fakeMouseEvent.MouseInput.X = event.TouchInput.X;
			fakeMouseEvent.MouseInput.Y = event.TouchInput.Y;
			fakeMouseEvent.MouseInput.Shift = false;
			fakeMouseEvent.MouseInput.Control = false;
			fakeMouseEvent.MouseInput.ButtonStates = 0;
			fakeMouseEvent.MouseInput.Event = EMIE_COUNT;

			switch (event.TouchInput.Event)
			{
				case ETIE_PRESSED_DOWN:
				{
					// We only work with the first for now.force opengl error
					if ( TouchID == -1 )
					{
						fakeMouseEvent.MouseInput.Event = EMIE_LMOUSE_PRESSED_DOWN;

						if (Device)
						{
							position2d<s32> touchPoint(event.TouchInput.X, event.TouchInput.Y);
							IGUIElement * logo = Device->getGUIEnvironment()->getRootGUIElement()->getElementFromId ( GUI_IRR_LOGO );
							if ( logo && logo->isPointInside (touchPoint) )
							{
								TouchID = event.TouchInput.ID;
								SpriteToMove = logo;
								SpriteStartRect =  SpriteToMove->getRelativePosition();
								TouchStartPos = touchPoint;
							}
						}
					}
					break;
				}
				case ETIE_MOVED:
					if ( TouchID == event.TouchInput.ID )
					{
						fakeMouseEvent.MouseInput.Event = EMIE_MOUSE_MOVED;
						fakeMouseEvent.MouseInput.ButtonStates = EMBSM_LEFT;

						if ( SpriteToMove && TouchID == event.TouchInput.ID )
						{

							position2d<s32> touchPoint(event.TouchInput.X, event.TouchInput.Y);
							MoveSprite(touchPoint);
						}
					}
					break;
				case ETIE_LEFT_UP:
					if ( TouchID == event.TouchInput.ID )
					{
						fakeMouseEvent.MouseInput.Event = EMIE_LMOUSE_LEFT_UP;

						if ( SpriteToMove )
						{
							TouchID = -1;
							position2d<s32> touchPoint(event.TouchInput.X, event.TouchInput.Y);
							MoveSprite(touchPoint);
							SpriteToMove = 0;
						}
					}
					break;
				default:
					break;
			}

			if ( fakeMouseEvent.MouseInput.Event != EMIE_COUNT && Device )
			{
				Device->postEventFromUser(fakeMouseEvent);
			}
		}
		else if ( event.EventType == EET_GUI_EVENT )
		{
			/*
				Show and hide the soft input keyboard when an edit-box get's the focus.
			*/
			switch(event.GUIEvent.EventType)
			{
				case EGET_EDITBOX_ENTER:
					if ( event.GUIEvent.Caller->getType() == EGUIET_EDIT_BOX )
					{
						if( Device->getGUIEnvironment() )
							Device->getGUIEnvironment()->setFocus(NULL);
						android::setSoftInputVisibility(AndroidApp, false);
					}
				break;
                case EGET_ELEMENT_FOCUS_LOST:
					if ( event.GUIEvent.Caller->getType() == EGUIET_EDIT_BOX )
					{
						/* 	Unfortunatly this only works on some android devices.
							On other devices Android passes through touch-input events when the virtual keyboard is clicked while blocking those events in areas where the keyboard isn't.
							Very likely an Android bug as it only happens in certain cases (like Android Lollipop with landscape mode on MotoG, but also some reports from other devices).
							Or maybe Irrlicht still does something wrong.
							Can't figure it out so far - so be warned - with landscape mode you might be better off writing your own keyboard.
						*/
						android::setSoftInputVisibility(AndroidApp, false);
					}
                break;
                case EGET_ELEMENT_FOCUSED:
					if ( event.GUIEvent.Caller->getType() == EGUIET_EDIT_BOX )
					{
						android::setSoftInputVisibility(AndroidApp, true);
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
	android_app* AndroidApp;
	gui::IGUIElement * SpriteToMove;
	core::rect<s32> SpriteStartRect;
	core::position2d<irr::s32> TouchStartPos;
	s32 TouchID;
};

/* Mainloop.
*/
void mainloop( IrrlichtDevice *device, IGUIStaticText * infoText )
{
	u32 loop = 0;	// loop is reset when the app is destroyed unlike runCounter
	static u32 runCounter = 0;	// static's seem to survive even an app-destroy message (not sure if that's guaranteed).
	while(device->run())
	{
		/*
			The window seems to be always active in this setup.
			That's because when it's not active Android will stop the code from running.
		*/
		if (device->isWindowActive())
		{
			/*
				Show FPS and some counters to show which parts of an app run
				in different app-lifecycle states.
			*/
			if ( infoText )
			{
				stringw str = L"FPS:";
				str += (s32)device->getVideoDriver()->getFPS();
				str += L" r:";
				str += runCounter;
				str += L" l:";
				str += loop;
				infoText->setText ( str.c_str() );
			}

			device->getVideoDriver()->beginScene(true, true, SColor(0,100,100,100));
			device->getSceneManager()->drawAll();
			device->getGUIEnvironment()->drawAll();
			device->getVideoDriver()->endScene ();
		}
		device->yield(); // probably nicer to the battery
		++runCounter;
		++loop;
	}
}

/* Main application code. */
void android_main(android_app* app)
{
	// Make sure glue isn't stripped.
	app_dummy();

	/*
		The receiver can already receive system events while createDeviceEx is called.
		So we create it first.
	*/
	MyEventReceiver receiver(app);

	/*
		Create the device.
		You have currently the choice between 2 drivers:
		EDT_OGLES1 is basically a opengl fixed function pipeline.
		EDT_OGLES2 is a shader pipeline. Irrlicht comes with shaders to simulate
				   typical fixed function materials. For this to work the
				   corresponding shaders from the Irrlicht media/Shaders folder are
				   copied to the application assets folder (done in the Makefile).
	*/
	SIrrlichtCreationParameters param;
//	param.DriverType = EDT_OGLES1;				// android:glEsVersion in AndroidManifest.xml should be "0x00010000" (requesting 0x00020000 will also guarantee that ES1 works)
	param.DriverType = EDT_OGLES2;				// android:glEsVersion in AndroidManifest.xml should be "0x00020000"
	param.WindowSize = dimension2d<u32>(0,0);	// using 0,0 it will automatically set it to the maximal size
	param.PrivateData = app;
	param.Bits = 24;
	param.ZBufferBits = 16;
	param.AntiAlias  = 0;
	param. EventReceiver = &receiver;

	/* Logging is written to a file. So your application should disable all logging when you distribute your
       application or it can fill up that file over time.
	*/
#ifndef _DEBUG
	param.LoggingLevel = ELL_NONE;
#endif

	IrrlichtDevice *device = createDeviceEx(param);
	if (device == 0)
       	return;

	receiver.Init(device);

//	ANativeActivity_setWindowFlags(app->activity, AWINDOW_FLAG_FULLSCREEN, 0);

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

	core::dimension2d<s32> dim(driver->getScreenSize());
	sprintf(strDisplay, "getScreenSize:(%d/%d)", dim.Width, dim.Height);
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
		rect<s32>(5,5,635,35), false, false, 0, GUI_INFO_FPS );
	guienv->addEditBox( L"", rect<s32>(5,40,475,80));

	// add irrlicht logo
	IGUIImage * logo = guienv->addImage(driver->getTexture(mediaPath + "irrlichtlogo3.png"),
					core::position2d<s32>(5,85), true, 0, GUI_IRR_LOGO);
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

	/*
		Add a 3d model. Note that you might need to add light when using other models.
		A copy of the model and it's textures must be inside the assets folder to be installed to Android.
		In this example we do copy it to the assets folder in the Makefile jni/Android.mk
	*/
	IAnimatedMesh* mesh = smgr->getMesh(mediaPath + "dwarf.x");
	if (!mesh)
	{
		device->closeDevice();
		device->drop();
       	return;
	}
	smgr->addAnimatedMeshSceneNode( mesh );


	/*
	To look at the mesh, we place a camera.
	*/
	smgr->addCameraSceneNode(0, vector3df(15,40,-90), vector3df(0,30,0));

	/*
		Mainloop. Applications usually never quit themself in Android. The OS is responsible for that.
	*/
	mainloop(device, text);

	/* Cleanup */
	device->setEventReceiver(0);
	device->closeDevice();
	device->drop();
}

#endif	// defined(_IRR_ANDROID_PLATFORM_)

/*
**/
