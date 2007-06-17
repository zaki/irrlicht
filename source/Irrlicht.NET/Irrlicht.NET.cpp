// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

// This is the main DLL file.

#pragma unmanaged
#include "..\\..\\include\\irrlicht.h"

#ifdef _DEBUG
#pragma comment (lib, "..\\debug\\irrlicht.lib")
#else
#pragma comment (lib, "..\\..\\lib\\Win32-visualstudio\\irrlicht.lib")
#endif
#pragma managed

#include <windows.h>

#include "Irrlicht.NET.h"
#include "IVideoDriver.h"
#include "ISceneManager.h"
#include "ICursorControl.h"
#include "IFileSystem.h"
#include "IGUIEnvironment.h"
#include "ITimer.h"
#include "NativeConverter.h"
#include <vcclr.h> // for PtrToStringChars
#include "IGUIListBox.h"

namespace Irrlicht
{

IrrlichtDevice::IrrlichtDevice(Video::DriverType driverType, Core::Dimension2D windowSize, 
	int bits, bool fullScreen, bool stencilBuffer, bool vsync)
: Device(0), ManagedVideoDriver(0), ManagedSceneManager(0), ManagedCursorControl(0),
 ManagedEventReceiver(0)
{
	CreatedDriverType = (irr::video::E_DRIVER_TYPE)driverType;
	Device = irr::createDevice(CreatedDriverType,
		irr::NativeConverter::getNativeDim(windowSize), bits, fullScreen, stencilBuffer, vsync);
   createManagedStuff();
}

IrrlichtDevice::IrrlichtDevice(Video::DriverType driverType)
: Device(0), ManagedVideoDriver(0), ManagedSceneManager(0), ManagedCursorControl(0)
{
	CreatedDriverType = (irr::video::E_DRIVER_TYPE)driverType;
	Device = irr::createDevice(CreatedDriverType);
	createManagedStuff();
}

IrrlichtDevice::IrrlichtDevice(Video::DriverType driverType, Core::Dimension2D windowSize, 
			int bits, bool fullScreen, bool stencilBuffer, bool vsync, bool antiAlias,
			System::IntPtr windowHandle)
: Device(0), ManagedVideoDriver(0), ManagedSceneManager(0), ManagedCursorControl(0)
{
	irr::SIrrlichtCreationParameters p;
	CreatedDriverType = (irr::video::E_DRIVER_TYPE)driverType;

	p.AntiAlias = antiAlias;
	p.Bits = bits;
	p.DriverType = CreatedDriverType;
	p.Fullscreen = fullScreen;
	p.Stencilbuffer = stencilBuffer;
	p.Vsync = vsync;
	p.WindowId = (void*)windowHandle;
	p.WindowSize = irr::NativeConverter::getNativeDim(windowSize);

	Device = irr::createDeviceEx(p);
	createManagedStuff();
}


void IrrlichtDevice::createManagedStuff()
{
	if (!Device)
		throw new System::Exception(new System::String(
		"Irrlicht device could not be created with the parameters you specified."));

	Device->getLogger()->log("Irrlicht.NET running.");

	ManagedVideoDriver = new Video::IVideoDriver(Device->getVideoDriver());
	ManagedSceneManager = new Scene::ISceneManager(Device->getSceneManager());
	ManagedCursorControl = new GUI::ICursorControl(Device->getCursorControl());
	ManagedFileSystem = new IO::IFileSystem(Device->getFileSystem());
	ManagedGUIEnvironment = new GUI::IGUIEnvironment(Device->getGUIEnvironment());
	ManagedTimer = new ITimer(Device->getTimer());
}



//! Destructor
IrrlichtDevice::~IrrlichtDevice()
{
	try
	{
		System::GC::SuppressFinalize(this);

		set_EventReceiver(0);

		if (Device)
		{
			Device->drop();
			Device = 0;
		}
	}
	catch(Exception*)
	{
		// catch this exception if we created the device for OpenGL mode,
		// .net crashed here for some unknown reason (I don't want to know why, really..)
		if ( CreatedDriverType != irr::video::EDT_OPENGL )
			throw;
		else
			System::Diagnostics::Debug::WriteLine("Catched OpenGL shutdown exception");
	}
}

bool IrrlichtDevice::Run()
{
	return Device->run();
}

bool IrrlichtDevice::get_WindowActive()
{
	return Device->isWindowActive();
}

void IrrlichtDevice::CloseDevice()
{
	Device->closeDevice();
}

System::String* IrrlichtDevice::get_Version()
{
	return new System::String(Device->getVersion());
}

void IrrlichtDevice::set_ResizeAble(bool resize)
{
	return Device->setResizeAble(resize);
}

void IrrlichtDevice::set_WindowCaption(System::String* text)
{
	const wchar_t __pin* pinchars = PtrToStringChars(text); 
	Device->setWindowCaption(pinchars);
}

Video::IVideoDriver* IrrlichtDevice::get_VideoDriver()
{
	return ManagedVideoDriver;
}

Scene::ISceneManager* IrrlichtDevice::get_SceneManager()
{
	return ManagedSceneManager;
}

GUI::ICursorControl* IrrlichtDevice::get_CursorControl()
{
	return ManagedCursorControl;
}

IO::IFileSystem* IrrlichtDevice::get_FileSystem()
{
	return ManagedFileSystem;
}

GUI::IGUIEnvironment* IrrlichtDevice::get_GUIEnvironment()
{
	return ManagedGUIEnvironment;
}

ITimer* IrrlichtDevice::get_Timer()
{
	return ManagedTimer;
}

void IrrlichtDevice::set_EventReceiver(IEventReceiver* receiver)
{
	if (EventMapList == 0)
		EventMapList = new System::Collections::ArrayList();
	else
	if (ManagedEventReceiver != 0)
	{
		// search and delete current EventReceiver from list

		for (int i=0; i<EventMapList->get_Count(); ++i)
		{
			SEventMapping* mapping = __try_cast<SEventMapping*>(EventMapList->get_Item(i));
			if (mapping->ManagedEventReceiver == ManagedEventReceiver)
			{
				delete mapping->NativeEventReceiver;
				EventMapList->RemoveAt(i);
				break;
			}
		}
		
	}

	if (receiver != 0)
	{
		SEventMapping* map = new SEventMapping();
		map->ManagedEventReceiver = receiver;
		map->NativeEventReceiver = new NativeEventReceiver();
		ManagedEventReceiver = receiver;

		EventMapList->Add(map);
		Device->setEventReceiver(map->NativeEventReceiver);
	}
	else
	{
		Device->setEventReceiver(0);
		ManagedEventReceiver = 0;
	}
}

bool IrrlichtDevice::NativeEventReceiver::OnEvent(irr::SEvent e)
{
	// find .NET receiver
	System::Collections::ArrayList* l = IrrlichtDevice::get_EventMap();
	if (!l)
		return false;

	// this is not as slow as it looks, because the list usually has
	// only one single entry.
	for (int i=0; i<l->get_Count(); ++i)
	{
		SEventMapping* mapping = __try_cast<SEventMapping*>(l->get_Item(i));
		if (mapping->NativeEventReceiver == this)
		{
			// found .NET receiver, now marshal event
			return mapping->ManagedEventReceiver->OnEvent(Irrlicht::Event(e));
		}
	}

	return false;
}



GUI::IGUIElement* Event::get_GUIEventCaller()
{
	if ( NativeEvent.GUIEvent.Caller == 0 || NativeEvent.EventType != irr::EET_GUI_EVENT )
		return 0;

	irr::gui::EGUI_ELEMENT_TYPE type = NativeEvent.GUIEvent.Caller->getType();

	if ( type == irr::gui::EGUIET_LIST_BOX )
		return new GUI::IGUIListBox( (irr::gui::IGUIListBox*)NativeEvent.GUIEvent.Caller );

	return new GUI::IGUIElement(NativeEvent.GUIEvent.Caller);
}


} // end namespace 