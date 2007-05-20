// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#using <mscorlib.dll>
using namespace System;

#include "edrivertypes.h"
#include "Dimension2D.h"
#include "Event.h"

#pragma unmanaged
#include "..\\..\\include\\irrlicht.h"

#pragma managed

namespace Irrlicht
{
	public __gc class ITimer;
namespace GUI
{
	public __gc class ICursorControl;
}
namespace Video
{
	public __gc class IVideoDriver;
}
namespace Scene
{
	public __gc class ISceneManager;
}
namespace IO
{
	public __gc class IFileSystem;
}
namespace GUI
{
	public __gc class IGUIEnvironment;
}

	/// <summary>
	/// The Irrlicht.NET device. This is the most important class of the Irrlicht Engine. You can access everything
	/// in the engine if you have a pointer to an instance of this class. 
	///
	/// This class has been ported directly from the native C++ Irrlicht Engine, so it may not 
	/// be 100% complete yet and the design may not be 100% .NET like.
	/// </summary>
	public __gc class IrrlichtDevice
	{
	public:

		/// <summary>
		/// Creates an Irrlicht device. This is the main way to start using Irrlicht.
		/// </summary>
		/// <param name="driverType">Type of the driver used to render everything. Choose
		/// between D3D8, D3D9, OpenGL, Irrlicht's Software Renderer and the Null Device.
		/// </param>
		IrrlichtDevice(Video::DriverType driverType);

		/// <summary>
		/// Creates an Irrlicht device. This is the main way to start using Irrlicht.
		/// </summary>
		/// <param name="driverType">Type of the driver used to render everything. Choose
		/// between D3D8, D3D9, OpenGL, Irrlicht's Software Renderer and the Null Device.
		/// </param>
		/// <param name="windowSize">Size of the window to be used, in pixels</param>
		/// <param name="bits">Bits per pixel to be used. Usually, this should be 16 or 32</param>
		/// <param name="fullScreen">If true, the engine starts in fullscreen mode,
		/// if false, it will run in windowed mode.</param>
		/// <param name="stencilbuffer">Specifies if the stencil buffer should be enabled. 
		/// Set this to true, if you want the engine be able to draw stencil buffer shadows. Note that not all
		/// devices are able to use the stencil buffer. If they don't no shadows will be drawn.</param>
		/// <param name="vsync">Specifies vertical syncronisation: If set to true, the driver will wait 
		/// for the vertical retrace period, otherwise not.</param>
		IrrlichtDevice(Video::DriverType driverType, Core::Dimension2D windowSize, 
			int bits, bool fullScreen, bool stencilBuffer, bool vsync);

		/// <summary>
		/// Creates an Irrlicht device. This is the main way to start using Irrlicht.
		/// </summary>
		/// <param name="driverType">Type of the driver used to render everything. Choose
		/// between D3D8, D3D9, OpenGL, Irrlicht's Software Renderer and the Null Device.
		/// </param>
		/// <param name="windowSize">Size of the window to be used, in pixels</param>
		/// <param name="bits">Bits per pixel to be used. Usually, this should be 16 or 32</param>
		/// <param name="fullScreen">If true, the engine starts in fullscreen mode,
		/// if false, it will run in windowed mode.</param>
		/// <param name="stencilbuffer">Specifies if the stencil buffer should be enabled. 
		/// Set this to true, if you want the engine be able to draw stencil buffer shadows. Note that not all
		/// devices are able to use the stencil buffer. If they don't no shadows will be drawn.</param>
		/// <param name="vsync">Specifies vertical syncronisation: If set to true, the driver will wait 
		/// for the vertical retrace period, otherwise not.</param>
		/// <param name="antiAlias">Specifies if the device should use fullscreen anti aliasing.
		/// Makes sharp/pixellated edges softer, but requires more performance. Also, 2D 
		/// elements might look blurier with this switched on. The resulting rendering quality 
		/// also depends on the hardware and driver you are using, your program might look
		/// different on different hardware with this. So if you are writing a 
		/// game/application with antiAlias switched on, it would be a good idea to make it
		/// possible to switch this option off again by the user.
		/// This is only supported in D3D9 and D3D8.</param>
		/// <param name="windowHandle">Window handle if you want Irrlicht do run embedded
		/// in a precreated window. If you created a System.Windows.Forms.Form, then you
		/// can get the handle with its .Handle Property. Set this to zero if 
		/// you don't want Irrlicht to run embedded in another window.</param>
		IrrlichtDevice(Video::DriverType driverType, Core::Dimension2D windowSize, 
			int bits, bool fullScreen, bool stencilBuffer, bool vsync, bool antiAlias,
			System::IntPtr windowHandle);

		//! Destructor
		~IrrlichtDevice();

		/// <summary>
		/// Runs the device. Returns false if device wants to be deleted.
		/// </summary>
		bool Run();

		/// <summary>
		/// Provides access to the video driver. This can be null, if the engine
		/// was not able to create the video driver you specified. For example if 
		/// you wanted D3D9, but it is not installed in the system.
		/// </summary>		
		__property Video::IVideoDriver* get_VideoDriver();

		/// <summary>
		/// Provides access to the scene manager.
		/// </summary>
		__property Scene::ISceneManager* get_SceneManager();

		/// <summary>
		/// Returns a pointer to the mouse cursor control interface.
		/// </summary>
		__property GUI::ICursorControl* get_CursorControl();

		/// <summary>
		/// Returns a pointer to the file system.
		/// </summary>
		__property IO::IFileSystem* get_FileSystem();

		/// <summary>
		/// Sets the caption of the window.
		/// <param name="text">New text of the window caption.</param>
		/// </summary>
		__property void set_WindowCaption(System::String* text);

		/// <summary>
		/// Returns a pointer to the gui environment.
		/// </summary>
		__property GUI::IGUIEnvironment* get_GUIEnvironment();

		/// <summary>		
		/// Returns a pointer to the ITimer object. The system time can be retrieved by it as
		/// well as the virtual time, which also can be manipulated.
		/// </summary>
		__property ITimer* get_Timer();

		/// <summary>
		/// \return Returns true if window is active. If the window is inactive,
		/// nothing need to be drawn. So if you don't want to draw anything when
		/// the window is inactive, create your drawing loop this way:
		/// \code
		/// while(device->run())
		///		if (device->isWindowActive())
		///		{
		///			// draw everything here
		///		}
		/// \endcode
		/// </summary>
		__property bool get_WindowActive();

		/// <summary>
		/// Notifies the device that it should close itself.
		/// IrrlichtDevice::run() will always return false after closeDevice() was called.
		/// </summary>
		void CloseDevice();

		/// <summary>
		/// Returns the version of the engine. The returned string
		/// will look like this: "1.2.3" or this: "1.2". 
		/// </summary>
		__property System::String* get_Version();

		/// <summary>
		/// Sets a new event receiver to receive events.
		/// </summary>
		__property void set_EventReceiver(IEventReceiver* receiver);

		/// <summary>
		/// Sets if the window should be resizeable in windowed mode. The default
		/// is false. This method only works in windowed mode.
		/// </summary>
		__property void set_ResizeAble(bool resize);

		
		/// \return Returns a pointer to the logger.
		//virtual ILogger* getLogger() = 0;

		/// Gets a list with all video modes available. If you are confused 
		/// now, because you think you have to create an Irrlicht Device with a video
		/// mode before being able to get the video mode list, let me tell you that
		/// there is no need to start up an Irrlicht Device with DT_DIRECTX8, DT_OPENGL or
		/// DT_SOFTWARE: For this (and for lots of other reasons) the null device,
		/// DT_NULL exists.
		/// \return Returns a pointer to a list with all video modes supported
		/// by the gfx adapter.
		//virtual video::IVideoModeList* getVideoModeList() = 0;

		/// Returns the operation system opertator object. It provides methods for
		/// getting operation system specific informations and doing operation system
		/// specific operations. Like for example exchanging data with the clipboard
		/// or reading the operation system version.
		//virtual IOSOperator* getOSOperator() = 0;

	private:

		/// <summary>
		/// Private method for receiving events from the native C++ Irrlicht engine and 
		/// to map them to the .NET event receiver
		/// </summary>
		__nogc class NativeEventReceiver : public irr::IEventReceiver
		{
		public:
			bool OnEvent(irr::SEvent e);
		};

		irr::IrrlichtDevice* Device;
		Irrlicht::Video::IVideoDriver* ManagedVideoDriver;
		Irrlicht::Scene::ISceneManager* ManagedSceneManager;
		Irrlicht::GUI::ICursorControl* ManagedCursorControl;
		Irrlicht::IEventReceiver* ManagedEventReceiver;
		Irrlicht::IO::IFileSystem* ManagedFileSystem;
		Irrlicht::GUI::IGUIEnvironment* ManagedGUIEnvironment;
		Irrlicht::ITimer* ManagedTimer;
		irr::video::E_DRIVER_TYPE CreatedDriverType;

		void createManagedStuff();

	public:

		/// <summary>
		/// Internal class for mapping C++ events to .NET events. This was made public because 
		/// of an obvious bug in managed C++. Simply don't use this.
		/// </summary>
		__gc class SEventMapping
		{
		public:
			Irrlicht::IEventReceiver* ManagedEventReceiver;
			NativeEventReceiver* NativeEventReceiver;
		};

	private:

		static System::Collections::ArrayList* EventMapList;

	public:

		/// <summary>
		/// Internal list for mapping C++ events to .NET events. This was made public because 
		/// of an obvious bug in managed C++. Simply don't use this.
		/// </summary>
		__property static System::Collections::ArrayList* get_EventMap()
		{
			return EventMapList;
		}
	};

}