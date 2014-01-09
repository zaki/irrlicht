// Copyright (C) 2002-2007 Nikolaus Gebhardt
// Copyright (C) 2007-2011 Christian Stehno
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CIrrDeviceAndroid.h"

#ifdef _IRR_COMPILE_WITH_ANDROID_DEVICE_

#include "os.h"
#include "CFileSystem.h"
#include "CAndroidAssetReader.h"
#include "CAndroidAssetFileArchive.h"
#include "CEGLManager.h"
#include "ISceneManager.h"
#include "IGUIEnvironment.h"
#include "CEGLManager.h"

namespace irr
{
	namespace video
	{
		IVideoDriver* createOGLES1Driver(const SIrrlichtCreationParameters& params,
			io::IFileSystem* io, video::IContextManager* contextManager);

		IVideoDriver* createOGLES2Driver(const SIrrlichtCreationParameters& params,
			io::IFileSystem* io, video::IContextManager* contextManager);
	}
}

namespace irr
{

CIrrDeviceAndroid::CIrrDeviceAndroid(const SIrrlichtCreationParameters& param)
	: CIrrDeviceStub(param), Focused(false), Initialized(false), Paused(true)
{
#ifdef _DEBUG
	setDebugName("CIrrDeviceAndroid");
#endif

	// Get the interface to the native Android activity.
	Android = (android_app*)(param.PrivateData);

	io::CAndroidAssetReader::Activity = Android->activity;
	io::CAndroidAssetFileArchive::Activity = Android->activity;

	// Set the private data so we can use it in any static callbacks.
	Android->userData = this;

	// Set the default command handler. This is a callback function that the Android
	// OS invokes to send the native activity messages.
	Android->onAppCmd = handleAndroidCommand;

	// Create a sensor manager to receive touch screen events from the java activity.
	SensorManager = ASensorManager_getInstance();
	SensorEventQueue = ASensorManager_createEventQueue(SensorManager, Android->looper, LOOPER_ID_USER, 0, 0);
	Android->onInputEvent = handleInput;

	// Create EGL manager.
	ContextManager = new video::CEGLManager();

	os::Printer::log("Waiting for Android activity window to be created.", ELL_DEBUG);

	do
	{
		s32 Events = 0;
		android_poll_source* Source = 0;

		while ((ALooper_pollAll(((Focused && !Paused) || !Initialized) ? 0 : -1, 0, &Events, (void**)&Source)) >= 0)
		{
			if(Source)
				Source->process(Android, Source);
		}
	}
	while(!Initialized);
}


CIrrDeviceAndroid::~CIrrDeviceAndroid()
{
	if (GUIEnvironment)
	{
		GUIEnvironment->drop();
		GUIEnvironment = 0;
	}

	if (SceneManager)
	{
		SceneManager->drop();
		SceneManager = 0;
	}

	if (VideoDriver)
	{
		VideoDriver->drop();
		VideoDriver = 0;
	}
}

bool CIrrDeviceAndroid::run()
{
	if (!Initialized)
		return false;

	os::Timer::tick();

	s32 Events = 0;
	android_poll_source* Source = 0;

	while ((ALooper_pollAll(((Focused && !Paused) || !Initialized) ? 0 : -1, 0, &Events, (void**)&Source)) >= 0)
	{
		if(Source)
			Source->process(Android, Source);

		if(!Initialized)
			break;
	}

	return Initialized;
}

void CIrrDeviceAndroid::yield()
{
}

void CIrrDeviceAndroid::sleep(u32 timeMs, bool pauseTimer)
{
}

void CIrrDeviceAndroid::setWindowCaption(const wchar_t* text)
{
}

bool CIrrDeviceAndroid::present(video::IImage* surface, void* windowId, core::rect<s32>* srcClip)
{
	return true;
}

bool CIrrDeviceAndroid::isWindowActive() const
{
	return (Focused && !Paused);
}

bool CIrrDeviceAndroid::isWindowFocused() const
{
	return Focused;
}

bool CIrrDeviceAndroid::isWindowMinimized() const
{
	return !Focused;
}

void CIrrDeviceAndroid::closeDevice()
{
	ANativeActivity_finish(Android->activity);
}

void CIrrDeviceAndroid::setResizable(bool resize)
{
}

void CIrrDeviceAndroid::minimizeWindow()
{
}

void CIrrDeviceAndroid::maximizeWindow()
{
}

void CIrrDeviceAndroid::restoreWindow()
{
}

core::position2di CIrrDeviceAndroid::getWindowPosition()
{
	return core::position2di(0, 0);
}

E_DEVICE_TYPE CIrrDeviceAndroid::getType() const
{
	return EIDT_ANDROID;
}

void CIrrDeviceAndroid::handleAndroidCommand(android_app* app, int32_t cmd)
{
	CIrrDeviceAndroid* Device = (CIrrDeviceAndroid*)app->userData;

	switch (cmd)
	{
		case APP_CMD_SAVE_STATE:
			os::Printer::log("Android command APP_CMD_SAVE_STATE", ELL_DEBUG);
		break;
		case APP_CMD_INIT_WINDOW:
			os::Printer::log("Android command APP_CMD_INIT_WINDOW", ELL_DEBUG);
			Device->getExposedVideoData().OGLESAndroid.window = app->window;

			if (Device->CreationParams.WindowSize.Width == 0 || Device->CreationParams.WindowSize.Height == 0)
			{
				Device->CreationParams.WindowSize.Width = ANativeWindow_getWidth(app->window);
				Device->CreationParams.WindowSize.Height = ANativeWindow_getHeight(app->window);
			}

			Device->getContextManager()->initialize(Device->CreationParams, Device->ExposedVideoData);
			Device->getContextManager()->generateSurface();
			Device->getContextManager()->generateContext();
			Device->getContextManager()->activateContext(Device->getContextManager()->getContext());

			if (!Device->Initialized)
			{
				io::CAndroidAssetFileArchive* Assets = new io::CAndroidAssetFileArchive(false, false);
				Assets->addDirectory("media");
				Device->FileSystem->addFileArchive(Assets);

				Device->createDriver();

				if (Device->VideoDriver)
					Device->createGUIAndScene();
			}
			Device->Initialized = true;
		break;
		case APP_CMD_TERM_WINDOW:
			os::Printer::log("Android command APP_CMD_TERM_WINDOW", ELL_DEBUG);
			Device->getContextManager()->destroySurface();
		break;
		case APP_CMD_GAINED_FOCUS:
			os::Printer::log("Android command APP_CMD_GAINED_FOCUS", ELL_DEBUG);
			Device->Focused = true;
		break;
		case APP_CMD_LOST_FOCUS:
			os::Printer::log("Android command APP_CMD_LOST_FOCUS", ELL_DEBUG);
			Device->Focused = false;
		break;
		case APP_CMD_DESTROY:
			os::Printer::log("Android command APP_CMD_DESTROY", ELL_DEBUG);
			Device->Initialized = false;
			break;
		case APP_CMD_PAUSE:
			os::Printer::log("Android command APP_CMD_PAUSE", ELL_DEBUG);
			Device->Paused = true;
			break;
		case APP_CMD_STOP:
			os::Printer::log("Android command APP_CMD_STOP", ELL_DEBUG);
			break;
		case APP_CMD_RESUME:
			os::Printer::log("Android command APP_CMD_RESUME", ELL_DEBUG);
			Device->Paused = false;
			break;
		default:
			break;
	}
}

s32 CIrrDeviceAndroid::handleInput(android_app* app, AInputEvent* androidEvent)
{
	CIrrDeviceAndroid* Device = (CIrrDeviceAndroid*)app->userData;
	s32 Status = 0;

	if (AInputEvent_getType(androidEvent) == AINPUT_EVENT_TYPE_MOTION)
	{
		SEvent Event;
		s32 PointerCount = AMotionEvent_getPointerCount(androidEvent);
		s32 EventAction = AMotionEvent_getAction(androidEvent);

		bool MultiTouchEvent = true;
		bool Touched = false;

		switch (EventAction)
		{
		case AMOTION_EVENT_ACTION_DOWN:
			Event.MultiTouchInput.Event = EMTIE_PRESSED_DOWN;
			Touched = true;
			break;
		case AMOTION_EVENT_ACTION_MOVE:
			Event.MultiTouchInput.Event = EMTIE_MOVED;
			Touched = true;
			break;
		case AMOTION_EVENT_ACTION_UP:
			Event.MultiTouchInput.Event = EMTIE_LEFT_UP;
			break;
		default:
			MultiTouchEvent = false;
			break;
		}

		if (MultiTouchEvent)
		{
			Event.EventType = EET_MULTI_TOUCH_EVENT;
			Event.MultiTouchInput.clear();

			for (s32 i = 0; i < PointerCount; ++i)
			{
				if (i >= NUMBER_OF_MULTI_TOUCHES)
					break;

				Event.MultiTouchInput.PrevX[i] = 0; // TODO
				Event.MultiTouchInput.PrevY[i] = 0; // TODO
				Event.MultiTouchInput.X[i] = AMotionEvent_getX(androidEvent, i);
				Event.MultiTouchInput.Y[i] = AMotionEvent_getY(androidEvent, i);
				Event.MultiTouchInput.Touched[i] = Touched;
			}

			Device->postEventFromUser(Event);

			Status = 1;
		}
	}

	return Status;
}

void CIrrDeviceAndroid::createDriver()
{
	switch(CreationParams.DriverType)
	{
	case video::EDT_OGLES1:
#ifdef _IRR_COMPILE_WITH_OGLES1_
		VideoDriver = video::createOGLES1Driver(CreationParams, FileSystem, ContextManager);
#else
		os::Printer::log("No OpenGL ES 1.0 support compiled in.", ELL_ERROR);
#endif
		break;
	case video::EDT_OGLES2:
#ifdef _IRR_COMPILE_WITH_OGLES2_
		VideoDriver = video::createOGLES2Driver(CreationParams, FileSystem, ContextManager);
#else
		os::Printer::log("No OpenGL ES 2.0 support compiled in.", ELL_ERROR);
#endif
		break;
	case video::EDT_NULL:
		VideoDriver = video::createNullDriver(FileSystem, CreationParams.WindowSize);
		break;
	case video::EDT_SOFTWARE:
	case video::EDT_BURNINGSVIDEO:
	case video::EDT_OPENGL:
	case video::EDT_DIRECT3D8:
	case video::EDT_DIRECT3D9:
		os::Printer::log("This driver is not available in Android. Try OpenGL ES 1.0 or ES 2.0.", ELL_ERROR);
		break;
	default:
		os::Printer::log("Unable to create video driver of unknown type.", ELL_ERROR);
		break;
	}
}

video::SExposedVideoData& CIrrDeviceAndroid::getExposedVideoData()
{
	return ExposedVideoData;
}

} // end namespace irr

#endif

