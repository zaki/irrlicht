// Copyright (C) 2009 Gaz Davidson
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h


#ifndef __C_IRR_DEVICE_CONSOLE_H_INCLUDED__
#define __C_IRR_DEVICE_CONSOLE_H_INCLUDED__

#include "IrrCompileConfig.h"
#ifdef _IRR_USE_CONSOLE_DEVICE_

#include "SIrrCreationParameters.h"
#include "CIrrDeviceStub.h"
#include "IImagePresenter.h"


namespace irr
{
	class CIrrDeviceConsole : public CIrrDeviceStub, video::IImagePresenter
	{
	public:

		//! constructor
		CIrrDeviceConsole(const SIrrlichtCreationParameters& params);

		//! destructor
		virtual ~CIrrDeviceConsole();

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

		//! returns if window has focus
		virtual bool isWindowFocused() const;

		//! returns if window is minimized
		virtual bool isWindowMinimized() const;

		//! presents a surface in the client area
		virtual bool present(video::IImage* surface, void* windowId=0, core::rect<s32>* src=0);

		//! notifies the device that it should close itself
		virtual void closeDevice();

		//! Sets if the window should be resizeable in windowed mode.
		virtual void setResizeAble(bool resize=false);

	private:
		bool IsDeviceRunning;

	};

} // end namespace irr



#endif // _IRR_USE_CONSOLE_DEVICE_
#endif // __C_IRR_DEVICE_CONSOLE_H_INCLUDED__
