// Copyright (C) 2002-2007 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_IRRLICHT_DEVICE_H_INCLUDED__
#define __I_IRRLICHT_DEVICE_H_INCLUDED__

#include "IUnknown.h"
#include "dimension2d.h"
#include "IVideoDriver.h"
#include "EDriverTypes.h"
#include "IEventReceiver.h"
#include "ICursorControl.h"
#include "IVideoModeList.h"
#include "ITimer.h"
#include "IOSOperator.h"

namespace irr
{
	class ILogger;
	class IEventReceiver;

	namespace io {
		class IFileSystem;
	} // end namespace io

	namespace gui {
		class IGUIEnvironment;
	} // end namespace gui

	namespace scene {
		class ISceneManager;
	} // end namespace scene

	//! The Irrlicht device. You can create it with createDevice() or createDeviceEx(). 
	/** This is the most important class of the Irrlicht Engine. You can access everything
	in the engine if you have a pointer to an instance of this class. 
	*/
	class IrrlichtDevice : public virtual IUnknown
	{
	public:

		//! destructor
		virtual ~IrrlichtDevice() {};

        //! Runs the device. 
		/** Also increments the virtual timer by calling ITimer::tick();. You can prevent this
		by calling ITimer::stop(); before and ITimer::start() after calling IrrlichtDevice::run().
		Returns false if device wants to be deleted. Use it in this way:
		\code
while(device->run())
{
  // draw everything here
}
		\endcode 
		If you want the device to do nothing if the window is inactive (recommended),
		use this slightly enhanced code instead:
		\code
while(device->run())
  if (device->isWindowActive())
  {
    // draw everything here
  }
		\endcode 
		Note if you are running Irrlicht inside an external, custom created window:
		Calling	Device->run() will cause Irrlicht to dispatch windows messages internally.
		If you are running Irrlicht in your own, custom window, you 
		you can also simply use your own message loop
		using GetMessage, DispatchMessage and whatever and simply don't use this method.
		But note that Irrlicht will not be able to fetch user input then. See 
		irr::SIrrlichtCreationParameters::WindowId for more informations and example code.
		*/
		virtual bool run() = 0;

		//! Cause the device to temporarily pause execution and let other processes to run
		// This should bring down processor usage without major performance loss for Irrlicht
		virtual void yield() = 0;

		//! Pause execution and let other processes to run for a specified amount of time.
		/** It may not wait the full given time, as sleep may be interrupted
		\param timeMs: Time to sleep for in milisecs. 
		\param pauseTimer: If true, pauses the device timer while sleeping
		*/
		virtual void sleep(u32 timeMs, bool pauseTimer=false) = 0;

		//! Provides access to the video driver for drawing 3d and 2d geometry.
		/** \return Returns a pointer the video driver. */
		virtual video::IVideoDriver* getVideoDriver() = 0;

		//! Provides access to the virtual file system.
		/** \return Returns a pointer to the file system. */
		virtual io::IFileSystem* getFileSystem() = 0;

		//! Provides access to the 2d user interface environment.
		/** \return Returns a pointer to the gui environment. */
		virtual gui::IGUIEnvironment* getGUIEnvironment() = 0;

		//! \return Returns a pointer to the scene manager.
		virtual scene::ISceneManager* getSceneManager() = 0;

		//! Provides access to the cursor control.
		/** \return Returns a pointer to the mouse cursor control interface. */
		virtual gui::ICursorControl* getCursorControl() = 0;

		//! Provides access to the logger. 
		/** \return Returns a pointer to the logger. */
		virtual ILogger* getLogger() = 0;

		//! Gets a list with all video modes available. 
		/** If you are confused 
		now, because you think you have to create an Irrlicht Device with a video
		mode before being able to get the video mode list, let me tell you that
		there is no need to start up an Irrlicht Device with EDT_DIRECT3D8, EDT_OPENGL or
		EDT_SOFTWARE: For this (and for lots of other reasons) the null device,
		EDT_NULL exists.
		\return Returns a pointer to a list with all video modes supported
		by the gfx adapter. */
		virtual video::IVideoModeList* getVideoModeList() = 0;

		//! Returns the operation system opertator object.
		/** It provides methods for
		getting operation system specific informations and doing operation system
		specific operations. Like for example exchanging data with the clipboard
		or reading the operation system version. */
		virtual IOSOperator* getOSOperator() = 0;

		//! Returns pointer to the timer.
		/** \return Returns a pointer to the ITimer object. The system time can be retrieved by it as
		well as the virtual time, which also can be manipulated. */
		virtual ITimer* getTimer() = 0;

		//! Sets the caption of the window.
		/** \param text: New text of the window caption. */
		virtual void setWindowCaption(const wchar_t* text) = 0;

		//! Returns if the window is active.
		/** \return Returns true if window is active. If the window is inactive,
		nothing need to be drawn. So if you don't want to draw anything when
		the window is inactive, create your drawing loop this way:
		\code
while(device->run())
  if (device->isWindowActive())
  {
    // draw everything here
  }
		\endcode */
		virtual bool isWindowActive() = 0;

		//! Notifies the device that it should close itself.
		/** IrrlichtDevice::run() will always return false after closeDevice() was called. */
		virtual void closeDevice() = 0;

		//! Returns the version of the engine.
		/** The returned string
		will look like this: "1.2.3" or this: "1.2". */
		virtual const c8* getVersion() = 0;

		//! Sets a new event receiver to receive events.
		virtual void setEventReceiver(IEventReceiver* receiver) = 0;

		//! Returns poinhter to the current event receiver. Returns 0 if there is none.
		virtual IEventReceiver* getEventReceiver() = 0;

		//! Sends a user created event to the engine.
		/** Is is usually not necessary to use this. However, if you are using an own
		input library for example for doing joystick input, you can use this to post key or mouse input 
		events to the engine. Internally, this method only delegates the events further to the 
		scene manager and the GUI environment. */
		virtual void postEventFromUser(SEvent event) = 0;

		//! Sets the input receiving scene manager. 
		/** If set to null, the main scene manager (returned by GetSceneManager()) will receive the input */
		virtual void setInputReceivingSceneManager(scene::ISceneManager* sceneManager) = 0;

		//! Sets if the window should be resizeable in windowed mode.
		/** The default is false. This method only works in windowed mode. */
		virtual void setResizeAble(bool resize=false) = 0;
	};

} // end namespace irr

#endif

