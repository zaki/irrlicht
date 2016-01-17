///////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2015 PakalEngine
// File: ResourceManager.h
// Original Author: Salvador Noel Romo Garcia.
// last modification: 27 Agosto 2015
///////////////////////////////////////////////////////////////////////////////////////////////////////
#include "CIrrDeviceAndroidPakal.h"

#ifdef _IRR_COMPILE_WITH_ANDROID_DEVICE_
#include <android/sensor.h>
#include <android/native_window.h>
#include <ctime>

#include <Android/CAndroidAssetFileArchive.h>
#include <CEGLManager.h>
#include <os.h>

namespace irr
{
	namespace video
	{
		IVideoDriver* createOGLES2Driver(const SIrrlichtCreationParameters& params,
			io::IFileSystem* io, video::IContextManager* contextManager);
	}
}

using namespace irr;

namespace irr
{
	void CIrrDeviceAndroidPakal::yield()
	{
		struct timespec ts = { 0,1 };
		nanosleep(&ts, NULL);
	}

	void CIrrDeviceAndroidPakal::sleep(irr::u32 timeMs, bool pauseTimer)
	{
		const bool wasStopped = Timer ? Timer->isStopped() : true;

		struct timespec ts;
		ts.tv_sec = (time_t)(timeMs / 1000);
		ts.tv_nsec = (long)(timeMs % 1000) * 1000000;

		if (pauseTimer && !wasStopped)
			Timer->stop();

		nanosleep(&ts, NULL);

		if (pauseTimer && !wasStopped)
			Timer->start();
	}

	void CIrrDeviceAndroidPakal::setWindowCaption(const wchar_t* text)
	{
	}

	bool CIrrDeviceAndroidPakal::isWindowActive() const
	{
		return m_is_window_active;
	}

	bool CIrrDeviceAndroidPakal::isWindowFocused() const
	{
		return m_is_window_focused;
	}

	bool CIrrDeviceAndroidPakal::isWindowMinimized() const
	{
		return m_is_window_minimized;
	}

	void CIrrDeviceAndroidPakal::setResizable(bool resize)
	{
	}

	void CIrrDeviceAndroidPakal::minimizeWindow()
	{
	}

	void CIrrDeviceAndroidPakal::maximizeWindow()
	{
	}

	void CIrrDeviceAndroidPakal::restoreWindow()
	{
	}

	bool CIrrDeviceAndroidPakal::postEventFromUser(const SEvent& event)
	{
		if( event.EventType == EET_USER_EVENT && event.UserEvent.UserData1 == 0)
		{			
			ExposedVideoData.OGLESAndroid.Window = CreationParams.WindowId = (void*)event.UserEvent.UserData2;
			setup_window();
			return true;
		}
		return CIrrDeviceStub::postEventFromUser(event);
	}

	irr::core::position2di CIrrDeviceAndroidPakal::getWindowPosition()
	{
		return irr::core::position2di(0, 0);
	}

	irr::E_DEVICE_TYPE CIrrDeviceAndroidPakal::getType() const
	{
		return irr::EIDT_ANDROID;
	}

	void CIrrDeviceAndroidPakal::setup_window()
	{
		ExposedVideoData.OGLESAndroid.Window = CreationParams.WindowId;

		if (this->CreationParams.WindowSize.Width == 0 || this->CreationParams.WindowSize.Height == 0)
		{
			this->CreationParams.WindowSize.Width = ANativeWindow_getWidth((ANativeWindow*)ExposedVideoData.OGLESAndroid.Window);
			this->CreationParams.WindowSize.Height = ANativeWindow_getHeight((ANativeWindow*)ExposedVideoData.OGLESAndroid.Window);
		}

		getContextManager()->initialize(CreationParams, ExposedVideoData);
		getContextManager()->generateSurface();
		getContextManager()->generateContext();
		getContextManager()->activateContext(getContextManager()->getContext());

		if (!m_is_initialized)
		{
			auto activity = (ANativeActivity *)CreationParams.PrivateData;
			io::CAndroidAssetFileArchive* assets = new io::CAndroidAssetFileArchive(activity->assetManager, false, false);
			assets->addDirectoryToFileList("");
			this->FileSystem->addFileArchive(assets);
			assets->drop();

			create_driver();

			if (VideoDriver)
				createGUIAndScene();
		}
		m_is_initialized = true;
	}

	CIrrDeviceAndroidPakal::CIrrDeviceAndroidPakal(const irr::SIrrlichtCreationParameters& param)
		: CIrrDeviceStub(param), m_is_window_focused(false), m_is_window_active(false), m_is_window_minimized(false), m_is_initialized(false)
	{
		// Create EGL manager.		
		ContextManager = new video::CEGLManager();
		setup_window();
	}

	CIrrDeviceAndroidPakal::~CIrrDeviceAndroidPakal()
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

	bool CIrrDeviceAndroidPakal::run()
	{
		return true;
	}

	void CIrrDeviceAndroidPakal::closeDevice()
	{
	}

	void CIrrDeviceAndroidPakal::create_driver()
	{
		switch (CreationParams.DriverType)
		{		
		case video::EDT_OGLES2:
#ifdef _IRR_COMPILE_WITH_OGLES2_
			VideoDriver = irr::video::createOGLES2Driver(CreationParams, FileSystem, ContextManager);
#else
			os::Printer::log("No OpenGL ES 2.0 support compiled in.", ELL_ERROR);
#endif
			break;
		case video::EDT_NULL:
			VideoDriver = video::createNullDriver(FileSystem, CreationParams.WindowSize);
			break;
		case video::EDT_OGLES1:
		case video::EDT_SOFTWARE:
		case video::EDT_BURNINGSVIDEO:
		case video::EDT_OPENGL:
		case video::EDT_DIRECT3D8:
		case video::EDT_DIRECT3D9:
			os::Printer::log("This driver is not available in Android. Try OpenGL ES 2.0.", ELL_ERROR);
			break;
		default:
			os::Printer::log("Unable to create video driver of unknown type.", ELL_ERROR);
			break;
		}
	}
}
#endif
