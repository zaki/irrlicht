// Copyright (C) 2002-2008 Nikolaus Gebhardt
// Copyright (C) 2008 Redshift Software, Inc.
// Copyright (C) 2012-2015 Patryk Nadrowski
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_IRR_DEVICE_IOS_H_INCLUDED__
#define __C_IRR_DEVICE_IOS_H_INCLUDED__

#include "IrrCompileConfig.h"

#ifdef _IRR_COMPILE_WITH_IOS_DEVICE_

#include "CIrrDeviceStub.h"
#include "IrrlichtDevice.h"
#include "IImagePresenter.h"

namespace irr
{

	class CIrrDeviceiOS : public CIrrDeviceStub, public video::IImagePresenter
	{
	public:
		CIrrDeviceiOS(const SIrrlichtCreationParameters& params);
		virtual ~CIrrDeviceiOS();

		virtual bool run() _IRR_OVERRIDE_;
		virtual void yield() _IRR_OVERRIDE_;
		virtual void sleep(u32 timeMs, bool pauseTimer) _IRR_OVERRIDE_;

		virtual void setWindowCaption(const wchar_t* text) _IRR_OVERRIDE_;

		virtual bool isWindowActive() const _IRR_OVERRIDE_;
		virtual bool isWindowFocused() const _IRR_OVERRIDE_;
		virtual bool isWindowMinimized() const _IRR_OVERRIDE_;

		virtual bool present(video::IImage* surface, void * windowId = 0, core::rect<s32>* src = 0) _IRR_OVERRIDE_;

		virtual void closeDevice() _IRR_OVERRIDE_;

		virtual void setResizable(bool resize = false) _IRR_OVERRIDE_;

		virtual void minimizeWindow() _IRR_OVERRIDE_;
		virtual void maximizeWindow() _IRR_OVERRIDE_;
		virtual void restoreWindow() _IRR_OVERRIDE_;

		virtual core::position2di getWindowPosition() _IRR_OVERRIDE_;

        virtual bool activateAccelerometer(float updateInterval = 0.016666f) _IRR_OVERRIDE_;
        virtual bool deactivateAccelerometer() _IRR_OVERRIDE_;
        virtual bool isAccelerometerActive() _IRR_OVERRIDE_;
        virtual bool isAccelerometerAvailable() _IRR_OVERRIDE_;
        virtual bool activateGyroscope(float updateInterval = 0.016666f) _IRR_OVERRIDE_;
        virtual bool deactivateGyroscope() _IRR_OVERRIDE_;
        virtual bool isGyroscopeActive() _IRR_OVERRIDE_;
        virtual bool isGyroscopeAvailable() _IRR_OVERRIDE_;
        virtual bool activateDeviceMotion(float updateInterval = 0.016666f) _IRR_OVERRIDE_;
        virtual bool deactivateDeviceMotion() _IRR_OVERRIDE_;
        virtual bool isDeviceMotionActive() _IRR_OVERRIDE_;
        virtual bool isDeviceMotionAvailable() _IRR_OVERRIDE_;

        virtual E_DEVICE_TYPE getType() const _IRR_OVERRIDE_;

	private:
        void createWindow();
        void createViewAndDriver();
        
        void* DataStorage;
		
		bool Close;
	};

}

#ifdef _IRR_COMPILE_WITH_IOS_BUILTIN_MAIN_
extern void irrlicht_main();
#endif

#endif
#endif
