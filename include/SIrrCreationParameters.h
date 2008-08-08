// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_IRRLICHT_CREATION_PARAMETERS_H_INCLUDED__
#define __I_IRRLICHT_CREATION_PARAMETERS_H_INCLUDED__

#include "EDriverTypes.h"

namespace irr
{
	class IEventReceiver;

	//! Structure for holding Irrlicht Device creation parameters.
	/** This structure is used in the createDeviceEx() function. */
	struct SIrrlichtCreationParameters
	{
		//! Constructs a SIrrlichtCreationParameters structure with default values.
		SIrrlichtCreationParameters() :
			DriverType(video::EDT_BURNINGSVIDEO),
			WindowSize(core::dimension2d<s32>(800, 600)),
			Bits(16),
			ZBufferBits(16),
			Fullscreen(false),
			Stencilbuffer(false),
			Vsync(false),
			AntiAlias(false),
			WithAlphaChannel(false),
			IgnoreInput(false),
			HighPrecisionFPU(false),
			EventReceiver(0),
			WindowId(0),
			SDK_version_do_not_use(IRRLICHT_SDK_VERSION)
		{
		}

		SIrrlichtCreationParameters(const SIrrlichtCreationParameters& other) :
			SDK_version_do_not_use(IRRLICHT_SDK_VERSION)
		{*this = other;}

		SIrrlichtCreationParameters& operator=(const SIrrlichtCreationParameters& other)
		{
			DriverType = other.DriverType;
			WindowSize = other.WindowSize;
			Bits = other.Bits;
			ZBufferBits = other.ZBufferBits;
			Fullscreen = other.Fullscreen;
			Stencilbuffer = other.Stencilbuffer;
			Vsync = other.Vsync;
			AntiAlias = other.AntiAlias;
			WithAlphaChannel = other.WithAlphaChannel;
			IgnoreInput = other.IgnoreInput;
			HighPrecisionFPU = other.HighPrecisionFPU;
			EventReceiver = other.EventReceiver;
			WindowId = other.WindowId;
			return *this;
		}

		//! Type of the device.
		/** This can currently be video::EDT_NULL, video::EDT_SOFTWARE,
		video::EDT_BURNINGSVIDEO, video::EDT_DIRECT3D8,
		video::EDT_DIRECT3D9, and video::EDT_OPENGL.
		Default: Software. */
		video::E_DRIVER_TYPE DriverType;

		//! Size of the window or the video mode in fullscreen mode. Default: 800x600
		core::dimension2d<s32> WindowSize;

		//! Minimum Bits per pixel of the color buffer in fullscreen mode. Ignored if windowed mode. Default: 16.
		u8 Bits;

		//! Minimum Bits per pixel of the depth buffer. Default: 16.
		u8 ZBufferBits;

		//! Should be set to true if the device should run in fullscreen.
		/** Otherwise the device runs in windowed mode. Default: false. */
		bool Fullscreen;

		//! Specifies if the stencil buffer should be enabled.
		/** Set this to true, if you want the engine be able to draw
		stencil buffer shadows. Note that not all devices are able to
		use the stencil buffer. If they don't no shadows will be drawn.
		Default: false. */
		bool Stencilbuffer;

		//! Specifies vertical syncronisation.
		/** If set to true, the driver will wait for the vertical
		retrace period, otherwise not.
		Default: false */
		bool Vsync;

		//! Specifies if the device should use fullscreen anti aliasing
		/** Makes sharp/pixelated edges softer, but requires more
		performance. Also, 2D elements might look blurred with this
		switched on. The resulting rendering quality also depends on
		the hardware and driver you are using, your program might look
		different on different hardware with this. So if you are
		writing a game/application with AntiAlias switched on, it would
		be a good idea to make it possible to switch this option off
		again by the user.
		This is curently not supported in OpenGL under Windows.
		Default value: false */
		bool AntiAlias;

		//! Whether the main framebuffer uses an alpha channel.
		/** In some situations it might be desireable to get a color
		buffer with an alpha channel, e.g. when rendering into a
		transparent window or overlay. If this flag is set the device
		tries to create a framebuffer with alpha channel.
		Default value: false */
		bool WithAlphaChannel;

		//! Specifies if the device should ignore input events
		/** This is only relevant when using external I/O handlers.
		External windows need to take care of this themselves.
		Currently only supported under X11.
		Default value: false */
		bool IgnoreInput;

		//! Specifies if the device should use high precision FPU setting
		/** This is only relevant for DirectX Devices, which switch to
		low FPU precision by default for performance reasons. However,
		this may lead to problems with the other computations of the
		application. In this case setting this flag to true should help
		- on the expense of performance loss, though.
		Default value: false */
		bool HighPrecisionFPU;

		//! A user created event receiver.
		IEventReceiver* EventReceiver;

		//! Window Id.
		/** If this is set to a value other than 0, the Irrlicht Engine
		will be created in an already existing window. For windows, set
		this to the HWND of the window you want. The windowSize and
		FullScreen options will be ignored when using the WindowId
		parameter. Default this is set to 0.
		To make Irrlicht run inside the custom window, you still will
		have to draw Irrlicht on your own. You can use this loop, as
		usual:
		\code
		while (device->run())
		{
			driver->beginScene(true, true, 0);
			smgr->drawAll();
			driver->endScene();
		}
		\endcode
		Instead of this, you can also simply use your own message loop
		using GetMessage, DispatchMessage and whatever. Calling
		IrrlichtDevice::run() will cause Irrlicht to dispatch messages
		internally too.  You need not call Device->run() if you want to
		do your own message dispatching loop, but Irrlicht will not be
		able to fetch user input then and you have to do it on your own
		using the window messages, DirectInput, or whatever. Also,
		you'll have to increment the Irrlicht timer.
		An alternative, own message dispatching loop without
		device->run() would look like this:
		\code
		MSG msg;
		while (true)
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);

				if (msg.message == WM_QUIT)
					break;
			}

			// increase virtual timer time
			device->getTimer()->tick();

			// draw engine picture
			driver->beginScene(true, true, 0);
			smgr->drawAll();
			driver->endScene();
		}
		\endcode
		However, there is no need to draw the picture this often. Just
		do it how you like. */
		void* WindowId;

		//! Don't use or change this parameter.
		/** Always set it to IRRLICHT_SDK_VERSION, which is done by default.
		This is needed for sdk version checks. */
		const c8* const SDK_version_do_not_use;
	};


} // end namespace irr

#endif

