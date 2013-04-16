// Copyright (C) 2002-2008 Nikolaus Gebhardt
// Copyright (C) 2008 Redshift Software, Inc.
// Copyright (C) 2012 Patryk Nadrowski
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_IRR_DEVICE_IPHONE_H_INCLUDED__
#define __C_IRR_DEVICE_IPHONE_H_INCLUDED__

#include "IrrCompileConfig.h"

#ifdef _IRR_COMPILE_WITH_IPHONE_DEVICE_

#include "CIrrDeviceStub.h"
#include "IrrlichtDevice.h"
#include "IImagePresenter.h"

namespace irr
{

	class CIrrDeviceIPhone : public CIrrDeviceStub, public video::IImagePresenter
	{
	public:
		//! constructor
		CIrrDeviceIPhone(const SIrrlichtCreationParameters& params);

		//! destructor
		virtual ~CIrrDeviceIPhone();
        
        //! Display initialization. It's return video exposed data.
        void displayInitialize(void** context, void** view);
        
        //! Display begin.
        void displayBegin();

        //! Display end.
        void displayEnd();

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
        
        //! Get the position of this window on screen
		virtual core::position2di getWindowPosition();

		//! De/activates the window. When not active no rendering takes place.
		virtual void setWindowActive(bool);
        
        //! Activate accelerometer.
        virtual bool activateAccelerometer(float updateInterval = 0.016666f);
        
        //! Deactivate accelerometer.
        virtual bool deactivateAccelerometer();
        
        //! Is accelerometer active.
        virtual bool isAccelerometerActive();
        
        //! Is accelerometer available.
        virtual bool isAccelerometerAvailable();
        
        //! Activate gyroscope.
        virtual bool activateGyroscope(float updateInterval = 0.016666f);
        
        //! Deactivate gyroscope.
        virtual bool deactivateGyroscope();
        
        //! Is gyroscope active.
        virtual bool isGyroscopeActive();
        
        //! Is gyroscope available.
        virtual bool isGyroscopeAvailable();
        
        //! Activate device motion.
        virtual bool activateDeviceMotion(float updateInterval = 0.016666f);
        
        //! Deactivate device motion.
        virtual bool deactivateDeviceMotion();
        
        //! Is device motion active.
        virtual bool isDeviceMotionActive();
        
        //! Is device motion available.
        virtual bool isDeviceMotionAvailable();

		//! Get the type of the device.
		/** This allows the user to check which windowing system is currently being
		used. */
		virtual E_DEVICE_TYPE getType() const
		{
			return EIDT_IPHONE;
		}

	private:
		
		//! ObjC device object.
        void * DeviceM;
	
		volatile bool Close;
		volatile bool Closed;
		volatile bool WindowActive;
		volatile bool WindowMinimized;

		//! Create the driver.
		void createDriver();

		//! Create the native display view.
		bool createDisplay();
	};

} // end namespace irr

#endif // _IRR_COMPILE_WITH_IPHONE_DEVICE_

#endif // __C_IRR_DEVICE_IPHONE_H_INCLUDED__

