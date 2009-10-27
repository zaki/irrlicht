// Copyright (C) 2002-2008 Nikolaus Gebhardt
// Copyright (C) 2008 Redshift Software, Inc.
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_IRR_DEVICE_IPHONE_H_INCLUDED__
#define __C_IRR_DEVICE_IPHONE_H_INCLUDED__

#include "IrrCompileConfig.h"

#ifdef _IRR_COMPILE_WITH_IPHONE_DEVICE_

//! Interface between ObjC device and C++ device.
struct MIrrIPhoneDevice
{
	//! ObjC device object.
	void * DeviceM;
	
	//! C++ device object.
	void * DeviceCPP;

	/* The *display* functions call into the Obj-C instance. */

	void (*displayCreate)(struct MIrrIPhoneDevice * dev,
			void** window, int w, int h);
	void (*displayInit)(struct MIrrIPhoneDevice * dev,
			void** context, void** view);
	void (*displayBegin)(struct MIrrIPhoneDevice * dev);
	void (*displayEnd)(struct MIrrIPhoneDevice * dev);

	/* The *on* functions are callbacks from Obj-C to C++. */

	void (*onTerminate)(struct MIrrIPhoneDevice * dev);
	void (*onWindowActive)(struct MIrrIPhoneDevice * dev, int active);
};

/* 	The single link time interface call between ObjC and C++.
	It creates the ObjC device object and initializes the interface functions.
*/
#ifdef __cplusplus
extern "C"
#else
extern
#endif
void irr_device_iphone_create(struct MIrrIPhoneDevice * dev);

#ifdef __cplusplus

#include "CIrrDeviceStub.h"
#include "IrrlichtDevice.h"
#include "IImagePresenter.h"

//~ #include <video/ogles1.h>
#include <CoreFoundation/CFRunLoop.h>

namespace irr
{

	class CIrrDeviceIPhone : public CIrrDeviceStub, public video::IImagePresenter
	{
	public:
		typedef void * NativeWindowType;

		//! constructor
		CIrrDeviceIPhone(const SIrrlichtCreationParameters& params);

		//! destructor
		virtual ~CIrrDeviceIPhone();

		//! runs the device. Returns false if device wants to be deleted
		virtual bool run();

		//! Cause the device to temporarily pause execution and let other processes to run
		// This should bring down processor usage without major performance loss for Irrlicht
		virtual void yield();

		//! Pause execution and let other processes to run for a specified amount of time.
		virtual void sleep(u32 timeMs, bool pauseTimer);

		//! sets the caption of the window
		virtual void setWindowCaption(const wchar_t* text);

		//! returns if window is active. if not, nothing need to be drawn
		virtual bool isWindowActive() const;

		//! Checks if the Irrlicht window has focus
		virtual bool isWindowFocused() const;

		//! Checks if the Irrlicht window is minimized
		virtual bool isWindowMinimized() const;

		//! presents a surface in the client area
		virtual bool present(video::IImage* surface, void * windowId = 0, core::rect<s32>* src=0 );

		//! notifies the device that it should close itself
		virtual void closeDevice();

		//! Sets if the window should be resizeable in windowed mode.
		virtual void setResizable(bool resize=false);
		
		//! Minimizes the window
		virtual void minimizeWindow();
		
		//! Maximizes the window if possible.
		virtual void maximizeWindow();

		//! Restore the window to normal size if possible.
		virtual void restoreWindow();

		//! De/activates the window. When not active no rendering takes place.
		virtual void setWindowActive(bool);

		//! Get the type of the device.
		/** This allows the user to check which windowing system is currently being
		used. */
		virtual E_DEVICE_TYPE getType() const
		{
			return EIDT_OSX;
		}

	private:
		
		MIrrIPhoneDevice IrrIPhoneDevice;
	
		volatile bool Close;
		volatile bool Closed;
		volatile bool WindowActive;
		volatile bool WindowMinimized;

		//! Create the driver.
		void createDriver();

		//! Create the native display view.
		bool createDisplay();
		
		//! Callback for when the device/app will terminate and hence
		//! need immediate cleanup.
		static void onTerminate(MIrrIPhoneDevice * dev);
		
		//! Callback to set the active state of the window. This calls
		//! setWindowActive(bool) as needed.
		static void onWindowActive(MIrrIPhoneDevice * dev, int active);
	};

} // end namespace irr

#endif // __cplusplus

#endif // _IRR_COMPILE_WITH_IPHONE_DEVICE_

#endif // __C_IRR_DEVICE_IPHONE_H_INCLUDED__

