// Copyright (C) 2009 Gaz Davidson
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CIrrDeviceConsole.h"

#ifdef _IRR_USE_CONSOLE_DEVICE_

#include "os.h"

#ifdef _IRR_WINDOWS_API_
#define WIN32_LEAN_AND_MEAN
#if !defined(_IRR_XBOX_PLATFORM_)
	#include <windows.h>
#endif
#else
#include <time.h>
#endif

namespace irr
{

//! constructor
CIrrDeviceConsole::CIrrDeviceConsole(const SIrrlichtCreationParameters& params)
  : CIrrDeviceStub(params), IsDeviceRunning(true)
{
	switch (params.DriverType)
	{
	case video::EDT_SOFTWARE:
		#ifdef _IRR_COMPILE_WITH_SOFTWARE_
		VideoDriver = video::createSoftwareDriver(CreationParams.WindowSize, CreationParams.Fullscreen, FileSystem, this);
		#else
		os::Printer::log("Software driver was not compiled in.", ELL_ERROR);
		#endif
		break;

	case video::EDT_BURNINGSVIDEO:
		#ifdef _IRR_COMPILE_WITH_BURNINGSVIDEO_
		VideoDriver = video::createSoftwareDriver2(CreationParams.WindowSize, CreationParams.Fullscreen, FileSystem, this);
		#else
		os::Printer::log("Burning's Video driver was not compiled in.", ELL_ERROR);
		#endif
		break;

	case video::EDT_DIRECT3D8:
	case video::EDT_DIRECT3D9:
	case video::EDT_OPENGL:
		os::Printer::log("The console device cannot use hardware drivers", ELL_ERROR);
		break;
	default:
		break;
	}

	if (VideoDriver)
		createGUIAndScene();

}

//! destructor
CIrrDeviceConsole::~CIrrDeviceConsole()
{
	// GUI and scene are dropped in the stub
}

//! runs the device. Returns false if device wants to be deleted
bool CIrrDeviceConsole::run()
{
	// increment timer
	os::Timer::tick();

	// todo: process keyboard input with cin/getch and catch kill signals
	
	return IsDeviceRunning;
}

//! Cause the device to temporarily pause execution and let other processes to run
// This should bring down processor usage without major performance loss for Irrlicht
void CIrrDeviceConsole::yield()
{
#ifdef _IRR_WINDOWS_API_
	Sleep(1);
#else
	struct timespec ts = {0,0};
	nanosleep(&ts, NULL);
#endif

}

//! Pause execution and let other processes to run for a specified amount of time.
void CIrrDeviceConsole::sleep(u32 timeMs, bool pauseTimer)
{

	const bool wasStopped = Timer ? Timer->isStopped() : true;

#ifdef _IRR_WINDOWS_API_
	Sleep(timeMs);
#else
	struct timespec ts;
	ts.tv_sec = (time_t) (timeMs / 1000);
	ts.tv_nsec = (long) (timeMs % 1000) * 1000000;

	if (pauseTimer && !wasStopped)
		Timer->stop();

	nanosleep(&ts, NULL);
#endif

	if (pauseTimer && !wasStopped)
		Timer->start();

}

//! sets the caption of the window
void CIrrDeviceConsole::setWindowCaption(const wchar_t* text)
{
	// do nothing - there is no caption
}

//! returns if window is active. if not, nothing need to be drawn
bool CIrrDeviceConsole::isWindowActive() const
{
	// there is no window, but we always assume it is active
	return true;
}

//! returns if window has focus
bool CIrrDeviceConsole::isWindowFocused() const
{
	// no way to tell, so we always assume it is
	return true;
}

//! returns if window is minimized
bool CIrrDeviceConsole::isWindowMinimized() const
{
	return false;
}

//! presents a surface in the client area
//! returns false on failure
bool CIrrDeviceConsole::present(video::IImage* surface, void* windowId, core::rect<s32>* src)
{
	// always fails!
	return false;
}

//! notifies the device that it should close itself
void CIrrDeviceConsole::closeDevice()
{
	// 
}

//! Sets if the window should be resizeable in windowed mode.
void CIrrDeviceConsole::setResizeAble(bool resize)
{
	// do nothing
}


extern "C" IRRLICHT_API IrrlichtDevice* IRRCALLCONV createDeviceEx(
	const SIrrlichtCreationParameters& parameters)
{
	CIrrDeviceConsole* dev = new CIrrDeviceConsole(parameters);

	if (dev && !dev->getVideoDriver() && parameters.DriverType != video::EDT_NULL)
	{
		dev->closeDevice(); // close device
		dev->run(); // consume quit message
		dev->drop();
		dev = 0;
	}

	return dev;
}


} // end namespace irr

#endif // _IRR_USE_CONSOLE_DEVICE_
