// Copyright (C) 2005-2008 Etienne Petitjean
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#include "IrrCompileConfig.h"

#ifdef _IRR_USE_OSX_DEVICE_

#import <Cocoa/Cocoa.h>
#import <OpenGL/gl.h>
#import <Carbon/Carbon.h>

#include "CIrrDeviceMacOSX.h"
#include "IEventReceiver.h"
#include "irrList.h"
#include "os.h"
#include "CTimer.h"
#include "irrString.h"
#include "Keycodes.h"
#include <stdio.h>
#include <sys/utsname.h>
#include "COSOperator.h"
#include "irrlicht.h"
#import <wchar.h>
#import <time.h>
#import "AppDelegate.h"

//------------------------------------------------------------------------------------------
Boolean GetDictionaryBoolean(CFDictionaryRef theDict, const void* key)
{
	// get a boolean from the dictionary
	Boolean value = false;
	CFBooleanRef boolRef;
	boolRef = (CFBooleanRef)CFDictionaryGetValue(theDict, key);
	if (boolRef != NULL)
		value = CFBooleanGetValue(boolRef);
	return value;
}
//------------------------------------------------------------------------------------------
long GetDictionaryLong(CFDictionaryRef theDict, const void* key)
{
	// get a long from the dictionary
	long value = 0;
	CFNumberRef numRef;
	numRef = (CFNumberRef)CFDictionaryGetValue(theDict, key);
	if (numRef != NULL)
		CFNumberGetValue(numRef, kCFNumberLongType, &value);
	return value;
}

namespace irr
{
	namespace video
	{
		IVideoDriver* createOpenGLDriver(const SIrrlichtCreationParameters& param, io::IFileSystem* io, CIrrDeviceMacOSX *device);
	}
} // end namespace irr

static bool firstLaunch = true;

namespace irr
{
//! constructor
CIrrDeviceMacOSX::CIrrDeviceMacOSX(const SIrrlichtCreationParameters& param)
	: CIrrDeviceStub(param), _window(NULL), _active(true), _oglcontext(NULL), _cglcontext(NULL)
{
	struct utsname name;
	NSString	*path;

	#ifdef _DEBUG
	setDebugName("CIrrDeviceMacOSX");
	#endif

	if (firstLaunch)
	{
		firstLaunch = false;

		[[NSAutoreleasePool alloc] init];
		[NSApplication sharedApplication];
		[NSApp setDelegate:[[[AppDelegate alloc] initWithDevice:this] autorelease]];
		[NSBundle loadNibNamed:@"MainMenu" owner:[NSApp delegate]];
		[NSApp finishLaunching];

		path = [[[NSBundle mainBundle] bundlePath] stringByDeletingLastPathComponent];
		chdir([path cString]);
	}

	uname(&name);
	Operator = new COSOperator(name.version);
	os::Printer::log(name.version,ELL_INFORMATION);

	initKeycodes();
	if (CreationParams.DriverType != video::EDT_NULL)
		createWindow();
	CursorControl = new CCursorControl(CreationParams.WindowSize, this);
	createDriver();
	createGUIAndScene();
}

CIrrDeviceMacOSX::~CIrrDeviceMacOSX()
{
	SetSystemUIMode(kUIModeNormal, 0);
	closeDevice();
}

void CIrrDeviceMacOSX::closeDevice()
{
	if (_window != NULL)
	{
		[(NSWindow *)_window setIsVisible:FALSE];

		if (_oglcontext != NULL)
		{
			[(NSOpenGLContext *)_oglcontext clearDrawable];
			[(NSOpenGLContext *)_oglcontext release];
			_oglcontext = NULL;
		}

		[(NSWindow *)_window setReleasedWhenClosed:TRUE];
		[(NSWindow *)_window release];
		_window = NULL;
	}
	else
	{
		if (_cglcontext != NULL)
		{
			CGLSetCurrentContext(NULL);
			CGLClearDrawable(_cglcontext);
			CGLDestroyContext(_cglcontext);
		}
	}

	_active = FALSE;
	_cglcontext = NULL;
}

bool CIrrDeviceMacOSX::createWindow()
{
	int				index;
	CGDisplayErr			error;
	bool				result;
	NSOpenGLPixelFormat		*format;
	CGDirectDisplayID		display;
	CGLPixelFormatObj		pixelFormat;
	CGRect				displayRect;
	CGLPixelFormatAttribute		fullattribs[32];
	NSOpenGLPixelFormatAttribute	windowattribs[32];
	CFDictionaryRef			displaymode,olddisplaymode;
	GLint				numPixelFormats,newSwapInterval;
	int alphaSize = CreationParams.WithAlphaChannel?4:0, depthSize = CreationParams.ZBufferBits;

	if (CreationParams.WithAlphaChannel && (CreationParams.Bits == 32))
		alphaSize = 8;

	result = false;
	display = CGMainDisplayID();
	_screenWidth = (int) CGDisplayPixelsWide(display);
	_screenHeight = (int) CGDisplayPixelsHigh(display);

	VideoModeList.setDesktop(CreationParams.Bits,core::dimension2d<s32>(_screenWidth, _screenHeight));

	if (!CreationParams.Fullscreen)
	{
		_window = [[NSWindow alloc] initWithContentRect:NSMakeRect(0,0,CreationParams.WindowSize.Width,CreationParams.WindowSize.Height) styleMask:NSTitledWindowMask+NSClosableWindowMask+NSResizableWindowMask backing:NSBackingStoreBuffered defer:FALSE];
		if (_window != NULL)
		{
			index = 0;
			windowattribs[index++] = NSOpenGLPFANoRecovery;
			windowattribs[index++] = NSOpenGLPFADoubleBuffer;
			windowattribs[index++] = NSOpenGLPFAAccelerated;
			windowattribs[index++] = NSOpenGLPFADepthSize;
			windowattribs[index++] = (NSOpenGLPixelFormatAttribute)depthSize;
			windowattribs[index++] = NSOpenGLPFAColorSize;
			windowattribs[index++] = (NSOpenGLPixelFormatAttribute)CreationParams.Bits;
			windowattribs[index++] = NSOpenGLPFAAlphaSize;
			windowattribs[index++] = (NSOpenGLPixelFormatAttribute)alphaSize;

			if (CreationParams.AntiAlias) {
				windowattribs[index++] = NSOpenGLPFASampleBuffers;
				windowattribs[index++] = (NSOpenGLPixelFormatAttribute)1;
				windowattribs[index++] = NSOpenGLPFASamples;
				windowattribs[index++] = (NSOpenGLPixelFormatAttribute)2;
			}

			if (CreationParams.Stencilbuffer)
			{
				windowattribs[index++] = NSOpenGLPFAStencilSize;
				windowattribs[index++] = (NSOpenGLPixelFormatAttribute)1;
			}

			windowattribs[index++] = (NSOpenGLPixelFormatAttribute)NULL;

			format = [[NSOpenGLPixelFormat alloc] initWithAttributes:windowattribs];
			if (format != NULL)
			{
				_oglcontext = [[NSOpenGLContext alloc] initWithFormat:format shareContext:NULL];
				[format release];
			}

			if (_oglcontext != NULL)
			{
				[(NSWindow *)_window center];
				[(NSWindow *)_window setDelegate:[NSApp delegate]];
				[(NSOpenGLContext *)_oglcontext setView:[(NSWindow *)_window contentView]];
				[(NSWindow *)_window setAcceptsMouseMovedEvents:TRUE];
				[(NSWindow *)_window setIsVisible:TRUE];
				[(NSWindow *)_window makeKeyAndOrderFront:nil];

				_cglcontext = (CGLContextObj) [(NSOpenGLContext *)_oglcontext CGLContextObj];
				_width = CreationParams.WindowSize.Width;
				_height = CreationParams.WindowSize.Height;
				result = true;
			}
		}
	}
	else
	{
		displaymode = CGDisplayBestModeForParameters(display,CreationParams.Bits,CreationParams.WindowSize.Width,CreationParams.WindowSize.Height,NULL);
		if (displaymode != NULL)
		{
			olddisplaymode = CGDisplayCurrentMode(display);
			error = CGCaptureAllDisplays();
			if (error == CGDisplayNoErr)
			{
				error = CGDisplaySwitchToMode(display,displaymode);
				if (error == CGDisplayNoErr)
				{
					pixelFormat = NULL;
					numPixelFormats = 0;

					index = 0;
					fullattribs[index++] = kCGLPFAFullScreen;
					fullattribs[index++] = kCGLPFADisplayMask;
					fullattribs[index++] = (CGLPixelFormatAttribute)CGDisplayIDToOpenGLDisplayMask(display);
					fullattribs[index++] = kCGLPFADoubleBuffer;
					fullattribs[index++] = kCGLPFAAccelerated;
					fullattribs[index++] = kCGLPFADepthSize;
					fullattribs[index++] = (CGLPixelFormatAttribute)depthSize;
					fullattribs[index++] = kCGLPFAColorSize;
					fullattribs[index++] = (CGLPixelFormatAttribute)CreationParams.Bits;
					fullattribs[index++] = kCGLPFAAlphaSize;
					fullattribs[index++] = (CGLPixelFormatAttribute)alphaSize;

					if (CreationParams.AntiAlias) {
						fullattribs[index++] = kCGLPFASampleBuffers;
						fullattribs[index++] = (CGLPixelFormatAttribute)1;
						fullattribs[index++] = kCGLPFASamples;
						fullattribs[index++] = (CGLPixelFormatAttribute)2;
					}

					if (CreationParams.Stencilbuffer)
					{
						fullattribs[index++] = kCGLPFAStencilSize;
						fullattribs[index++] = (CGLPixelFormatAttribute)1;
					}

					fullattribs[index++] = (CGLPixelFormatAttribute)NULL;
					CGLChoosePixelFormat(fullattribs,&pixelFormat,&numPixelFormats);

					if (pixelFormat != NULL)
					{
						CGLCreateContext(pixelFormat,NULL,&_cglcontext);
						CGLDestroyPixelFormat(pixelFormat);
					}

					if (_cglcontext != NULL)
					{
						CGLSetFullScreen(_cglcontext);
						displayRect = CGDisplayBounds(display);
						_width = (int)displayRect.size.width;
						_height = (int)displayRect.size.height;
						result = true;
					}
				}
			}
		}
	}

	if (result)
	{
		if (_window == NULL)
			SetSystemUIMode(kUIModeAllHidden, kUIOptionAutoShowMenuBar);
		CGLSetCurrentContext(_cglcontext);
		newSwapInterval = (CreationParams.Vsync) ? 1 : 0;
		CGLSetParameter(_cglcontext,kCGLCPSwapInterval,&newSwapInterval);
		glViewport(0,0,_width,_height);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}

	return (result);
}

void CIrrDeviceMacOSX::setResize(int width,int height)
{
	_width = width;
	_height = height;
	[(NSOpenGLContext *)_oglcontext update];
	getVideoDriver()->OnResize(core::dimension2d<s32>(width, height));
}

void CIrrDeviceMacOSX::createDriver()
{
	switch (CreationParams.DriverType)
	{
		case video::EDT_SOFTWARE:
		#ifdef _IRR_COMPILE_WITH_SOFTWARE_
			VideoDriver = video::createSoftwareDriver(CreationParams.WindowSize, CreationParams.Fullscreen, FileSystem, this);
		#else
			os::Printer::log("No Software driver support compiled in.", ELL_ERROR);
		#endif
			break;

		case video::EDT_BURNINGSVIDEO:
		#ifdef _IRR_COMPILE_WITH_BURNINGSVIDEO_
			VideoDriver = video::createSoftwareDriver2(CreationParams.WindowSize, CreationParams.Fullscreen, FileSystem, this);
		#else
			os::Printer::log("Burning's video driver was not compiled in.", ELL_ERROR);
		#endif
			break;

		case video::EDT_OPENGL:
		#ifdef _IRR_COMPILE_WITH_OPENGL_
			VideoDriver = video::createOpenGLDriver(CreationParams, FileSystem, this);
		#else
			os::Printer::log("No OpenGL support compiled in.", ELL_ERROR);
		#endif
			break;

		case video::EDT_DIRECT3D8:
		case video::EDT_DIRECT3D9:
			os::Printer::log("This driver is not available in OSX. Try OpenGL or Software renderer.", ELL_ERROR);
			break;

		case video::EDT_NULL:
			VideoDriver = video::createNullDriver(FileSystem, CreationParams.WindowSize);
			break;

		default:
			os::Printer::log("Unable to create video driver of unknown type.", ELL_ERROR);
			break;
	}
}

void CIrrDeviceMacOSX::flush()
{
	if (_cglcontext != NULL)
	{
		glFinish();
		CGLFlushDrawable(_cglcontext);
	}
}

bool CIrrDeviceMacOSX::run()
{
	NSEvent		*event;
	irr::SEvent	ievent;

	os::Timer::tick();
	storeMouseLocation();

	event = [NSApp nextEventMatchingMask:NSAnyEventMask untilDate:nil inMode:NSDefaultRunLoopMode dequeue:YES];
	if (event != nil)
	{
		bzero(&ievent,sizeof(ievent));

		switch([(NSEvent *)event type])
		{
			case NSKeyDown:
				postKeyEvent(event,ievent,true);
				break;

			case NSKeyUp:
				postKeyEvent(event,ievent,false);
				break;

			case NSLeftMouseDown:
				ievent.EventType = irr::EET_MOUSE_INPUT_EVENT;
				ievent.MouseInput.Event = irr::EMIE_LMOUSE_PRESSED_DOWN;
				postMouseEvent(event,ievent);
				break;

			case NSLeftMouseUp:
				ievent.EventType = irr::EET_MOUSE_INPUT_EVENT;
				ievent.MouseInput.Event = irr::EMIE_LMOUSE_LEFT_UP;
				postMouseEvent(event,ievent);
				break;

			case NSMouseMoved:
			case NSLeftMouseDragged:
			case NSRightMouseDragged:
				ievent.EventType = irr::EET_MOUSE_INPUT_EVENT;
				ievent.MouseInput.Event = irr::EMIE_MOUSE_MOVED;
				postMouseEvent(event,ievent);
				break;

			case NSRightMouseDown:
				ievent.EventType = irr::EET_MOUSE_INPUT_EVENT;
				ievent.MouseInput.Event = irr::EMIE_RMOUSE_PRESSED_DOWN;
				postMouseEvent(event,ievent);
				break;

			case NSRightMouseUp:
				ievent.EventType = irr::EET_MOUSE_INPUT_EVENT;
				ievent.MouseInput.Event = irr::EMIE_RMOUSE_LEFT_UP;
				postMouseEvent(event,ievent);
				break;

			case NSScrollWheel:
				ievent.EventType = irr::EET_MOUSE_INPUT_EVENT;
				ievent.MouseInput.Event = irr::EMIE_MOUSE_WHEEL;
				ievent.MouseInput.Wheel = [(NSEvent *)event deltaY];
				if (ievent.MouseInput.Wheel < 1.0f) ievent.MouseInput.Wheel *= 10.0f;
				else ievent.MouseInput.Wheel *= 5.0f;
				postMouseEvent(event,ievent);
				break;

			default:
				[NSApp sendEvent:event];
				break;
		}
	}

	return (![[NSApp delegate] isQuit] && _active);
}

//! Pause the current process for the minimum time allowed only to allow other processes to execute
void CIrrDeviceMacOSX::yield()
{
	// TODO: Does this work or maybe is there a better way?
	struct timespec ts = {0,0};
	nanosleep(&ts, NULL);
}

//! Pause execution and let other processes to run for a specified amount of time.
void CIrrDeviceMacOSX::sleep(u32 timeMs, bool pauseTimer=false)
{
	// TODO: Does this work or maybe is there a better way?

	bool wasStopped = Timer ? Timer->isStopped() : true;

	struct timespec ts;
	ts.tv_sec = (time_t) (timeMs / 1000);
	ts.tv_nsec = (long) (timeMs % 1000) * 1000000;

	if (pauseTimer && !wasStopped)
		Timer->stop();

	nanosleep(&ts, NULL);

	if (pauseTimer && !wasStopped)
		Timer->start();
}

void CIrrDeviceMacOSX::setWindowCaption(const wchar_t* text)
{
	size_t	size;
	char	title[1024];

	if (_window != NULL)
	{
		size = wcstombs(title,text,1024);
		if (size == 1024) title[1023] = 0;
		[(NSWindow *)_window setTitle:[NSString stringWithCString:title length:size]];
	}
}


bool CIrrDeviceMacOSX::isWindowActive() const
{
	return (_active);
}


bool CIrrDeviceMacOSX::isWindowFocused() const
{
	if (_window != NULL)
		return [(NSWindow *)_window isKeyWindow];
	return false;
}


bool CIrrDeviceMacOSX::isWindowMinimized() const
{
	if (_window != NULL)
		return [(NSWindow *)_window isMiniaturized];
	return false;
}


void CIrrDeviceMacOSX::postKeyEvent(void *event,irr::SEvent &ievent,bool pressed)
{
	NSString				*str;
	std::map<int,int>::const_iterator	iter;
	unsigned int				result,c,mkey,mchar;
	const unsigned char			*cStr;
	BOOL					skipCommand;

	str = [(NSEvent *)event characters];
	if (str != nil && [str length] > 0)
	{
		mkey = mchar = 0;
		skipCommand = false;
		c = [str characterAtIndex:0];

		iter = _keycodes.find(c);
		if (iter != _keycodes.end()) mkey = (*iter).second;
		else
		{
			// workaround for period character
			if (c == 0x2E)
			{
				mkey = irr::KEY_PERIOD;
				mchar = '.';
			} else {
				cStr = (unsigned char *)[str cStringUsingEncoding:NSWindowsCP1252StringEncoding];
				if (cStr != NULL && strlen((char*)cStr) > 0)
				{
					mchar = cStr[0];
					mkey = toupper(mchar);
					if ([(NSEvent *)event modifierFlags] & NSCommandKeyMask)
					{
						if (mkey == 'C' || mkey == 'V' || mkey == 'X')
						{
							mchar = 0;
							skipCommand = true;
						}
					}
				}
			}
		}

		ievent.EventType = irr::EET_KEY_INPUT_EVENT;
		ievent.KeyInput.Key = (irr::EKEY_CODE)mkey;
		ievent.KeyInput.PressedDown = pressed;
		ievent.KeyInput.Shift = ([(NSEvent *)event modifierFlags] & NSShiftKeyMask) != 0;
		ievent.KeyInput.Control = ([(NSEvent *)event modifierFlags] & NSControlKeyMask) != 0;
		ievent.KeyInput.Char = (irr::EKEY_CODE)mchar;

		if (skipCommand)
			ievent.KeyInput.Control = true;
		else if ([(NSEvent *)event modifierFlags] & NSCommandKeyMask)
			[NSApp sendEvent:(NSEvent *)event];

		postEventFromUser(ievent);
	}
}

void CIrrDeviceMacOSX::postMouseEvent(void *event,irr::SEvent &ievent)
{
	BOOL	post = true;

	if (_window != NULL)
	{
		ievent.MouseInput.X = (int)[(NSEvent *)event locationInWindow].x;
		ievent.MouseInput.Y = _height - (int)[(NSEvent *)event locationInWindow].y;
		if (ievent.MouseInput.Y < 0) post = false;
	}
	else
	{
		ievent.MouseInput.X = (int)[NSEvent mouseLocation].x;
		ievent.MouseInput.Y = _height - (int)[NSEvent mouseLocation].y;
	}

	if (post) postEventFromUser(ievent);
	[NSApp sendEvent:(NSEvent *)event];
}

void CIrrDeviceMacOSX::storeMouseLocation()
{
	NSPoint	p;
	int	x,y;

	p = [NSEvent mouseLocation];

	if (_window != NULL)
	{
		p = [(NSWindow *)_window convertScreenToBase:p];
		x = (int)p.x;
		y = _height - (int)p.y;
	}
	else
	{
		x = (int)p.x;
		y = (int)p.y;
		y -= (_screenHeight - _height);
	}

	((CCursorControl *)CursorControl)->updateInternalCursorPosition(x,y);
}


void CIrrDeviceMacOSX::setMouseLocation(int x,int y)
{
	NSPoint	p;
	CGPoint	c;

	if (_window != NULL)
	{
		p.x = (float) x;
		p.y = (float) (_height - y);
		p = [(NSWindow *)_window convertBaseToScreen:p];
		p.y = _screenHeight - p.y;
	}
	else
	{
		p.x = (float) x;
		p.y = (float) y + (_screenHeight - _height);
	}

	c.x = p.x;
	c.y = p.y;
	CGSetLocalEventsSuppressionInterval(0);
	CGWarpMouseCursorPosition(c);
}


void CIrrDeviceMacOSX::setCursorVisible(bool visible)
{
	CGDirectDisplayID	display;

	display = CGMainDisplayID();
	if (visible)
		CGDisplayShowCursor(display);
	else
		CGDisplayHideCursor(display);
}


void CIrrDeviceMacOSX::initKeycodes()
{
	_keycodes[NSUpArrowFunctionKey] = irr::KEY_UP;
	_keycodes[NSDownArrowFunctionKey] = irr::KEY_DOWN;
	_keycodes[NSLeftArrowFunctionKey] = irr::KEY_LEFT;
	_keycodes[NSRightArrowFunctionKey] = irr::KEY_RIGHT;
	_keycodes[NSF1FunctionKey] = irr::KEY_F1;
	_keycodes[NSF2FunctionKey] = irr::KEY_F2;
	_keycodes[NSF3FunctionKey] = irr::KEY_F3;
	_keycodes[NSF4FunctionKey] = irr::KEY_F4;
	_keycodes[NSF5FunctionKey] = irr::KEY_F5;
	_keycodes[NSF6FunctionKey] = irr::KEY_F6;
	_keycodes[NSF7FunctionKey] = irr::KEY_F7;
	_keycodes[NSF8FunctionKey] = irr::KEY_F8;
	_keycodes[NSF9FunctionKey] = irr::KEY_F9;
	_keycodes[NSF10FunctionKey] = irr::KEY_F10;
	_keycodes[NSF11FunctionKey] = irr::KEY_F11;
	_keycodes[NSF12FunctionKey] = irr::KEY_F12;
	_keycodes[NSF13FunctionKey] = irr::KEY_F13;
	_keycodes[NSF14FunctionKey] = irr::KEY_F14;
	_keycodes[NSF15FunctionKey] = irr::KEY_F15;
	_keycodes[NSF16FunctionKey] = irr::KEY_F16;
	_keycodes[NSHomeFunctionKey] = irr::KEY_HOME;
	_keycodes[NSEndFunctionKey] = irr::KEY_END;
	_keycodes[NSInsertFunctionKey] = irr::KEY_INSERT;
	_keycodes[NSDeleteFunctionKey] = irr::KEY_DELETE;
	_keycodes[NSHelpFunctionKey] = irr::KEY_HELP;
	_keycodes[NSSelectFunctionKey] = irr::KEY_SELECT;
	_keycodes[NSPrintFunctionKey] = irr::KEY_PRINT;
	_keycodes[NSExecuteFunctionKey] = irr::KEY_EXECUT;
	_keycodes[NSPrintScreenFunctionKey] = irr::KEY_SNAPSHOT;
	_keycodes[NSPauseFunctionKey] = irr::KEY_PAUSE;
	_keycodes[NSScrollLockFunctionKey] = irr::KEY_SCROLL;
	_keycodes[0x7F] = irr::KEY_BACK;
	_keycodes[0x09] = irr::KEY_TAB;
	_keycodes[0x0D] = irr::KEY_RETURN;
	_keycodes[0x03] = irr::KEY_RETURN;
	_keycodes[0x1B] = irr::KEY_ESCAPE;
}


//! Sets if the window should be resizeable in windowed mode.
void CIrrDeviceMacOSX::setResizeAble(bool resize)
{
	// todo: implement resize
}


bool CIrrDeviceMacOSX::present(video::IImage* surface, void* windowId, core::rect<s32>* src )
{
	// todo: implement
	return false;
}


video::IVideoModeList* CIrrDeviceMacOSX::getVideoModeList()
{
	if (!VideoModeList.getVideoModeCount())
	{
		CGDirectDisplayID display;
		display = CGMainDisplayID();

		CFArrayRef availableModes = CGDisplayAvailableModes(display);
		unsigned int numberOfAvailableModes = CFArrayGetCount(availableModes);
		for (u32 i= 0; i<numberOfAvailableModes; ++i)
		{
			// look at each mode in the available list
			CFDictionaryRef mode = (CFDictionaryRef)CFArrayGetValueAtIndex(availableModes, i);
			long bitsPerPixel = GetDictionaryLong(mode, kCGDisplayBitsPerPixel);
			Boolean safeForHardware = GetDictionaryBoolean(mode, kCGDisplayModeIsSafeForHardware);
			Boolean stretched = GetDictionaryBoolean(mode, kCGDisplayModeIsStretched);

			if (!safeForHardware)
				continue;

			long width = GetDictionaryLong(mode, kCGDisplayWidth);
			long height = GetDictionaryLong(mode, kCGDisplayHeight);
			// long refresh = GetDictionaryLong((mode), kCGDisplayRefreshRate);
			VideoModeList.addMode(core::dimension2d<s32>(width, height),
				bitsPerPixel);
		}
	}
	return &VideoModeList;
}

IRRLICHT_API IrrlichtDevice* IRRCALLCONV createDeviceEx(const SIrrlichtCreationParameters& param)
{
	CIrrDeviceMacOSX* dev = new CIrrDeviceMacOSX(param);

	if (dev && !dev->getVideoDriver() && param.DriverType != video::EDT_NULL)
	{
		dev->drop();
		dev = 0;
	}

	return dev;
}

}

#endif // _IRR_USE_OSX_DEVICE_

