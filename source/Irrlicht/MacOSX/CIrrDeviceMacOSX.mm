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

#if defined _IRR_COMPILE_WITH_JOYSTICK_EVENTS_

#include <IOKit/IOKitLib.h>
#include <IOKit/IOCFPlugIn.h>
#ifdef MACOS_10_0_4
#include <IOKit/hidsystem/IOHIDUsageTables.h>
#else
/* The header was moved here in Mac OS X 10.1 */
#include <Kernel/IOKit/hidsystem/IOHIDUsageTables.h>
#endif
#include <IOKit/hid/IOHIDLib.h>
#include <IOKit/hid/IOHIDKeys.h>

struct JoystickComponent
{
	IOHIDElementCookie cookie;				// unique value which identifies element, will NOT change
	long min;								// reported min value possible
	long max;								// reported max value possible

	long minRead;							//min read value
	long maxRead;							//max read value

	JoystickComponent() : min(0), minRead(0), max(0), maxRead(0)
	{
	}
};

struct JoystickInfo
{
	irr::core::array <JoystickComponent> axisComp;
	irr::core::array <JoystickComponent> buttonComp;
	irr::core::array <JoystickComponent> hatComp;
	
	int	hats;
	int	axes;
	int	buttons;
	
	int numActiveJoysticks;

	irr::SEvent persistentData;

	IOHIDDeviceInterface ** interface;
	bool removed;
	char joystickName[256];
	long usage;								// usage page from IOUSBHID Parser.h which defines general usage
	long usagePage;							// usage within above page from IOUSBHID Parser.h which defines specific usage

	JoystickInfo() : hats(0), axes(0), buttons(0), interface(0), removed(false), usage(0), usagePage(0), numActiveJoysticks(0)
	{
		interface = NULL;
		memset(joystickName, '\0', 256);
		axisComp.clear();
		buttonComp.clear();
		hatComp.clear();

		persistentData.EventType = irr::EET_JOYSTICK_INPUT_EVENT;
		persistentData.JoystickEvent.POV = 65535;
		persistentData.JoystickEvent.ButtonStates = 0;
	}
};
irr::core::array<JoystickInfo> ActiveJoysticks;

//helper functions for init joystick
static IOReturn closeJoystickDevice (JoystickInfo* joyInfo)
{
	IOReturn result = kIOReturnSuccess;
	if (joyInfo && joyInfo->interface) {
		/* close the interface */
		result = (*(joyInfo->interface))->close (joyInfo->interface);
		if (kIOReturnNotOpen == result)
		{
			/* do nothing as device was not opened, thus can't be closed */
		}
		else if (kIOReturnSuccess != result)
			irr::os::Printer::log("IOHIDDeviceInterface failed to close", irr::ELL_ERROR);
		/* release the interface */
		result = (*(joyInfo->interface))->Release (joyInfo->interface);
		if (kIOReturnSuccess != result)
			irr::os::Printer::log("IOHIDDeviceInterface failed to release", irr::ELL_ERROR);
		joyInfo->interface = NULL;
	}
	return result;
}

static void addComponentInfo (CFTypeRef refElement, JoystickComponent *pComponent, int numActiveJoysticks)
{
	long number;
	CFTypeRef refType;

	refType = CFDictionaryGetValue ((CFDictionaryRef)refElement, CFSTR(kIOHIDElementCookieKey));
	if (refType && CFNumberGetValue ((CFNumberRef)refType, kCFNumberLongType, &number))
		pComponent->cookie = (IOHIDElementCookie) number;
	refType = CFDictionaryGetValue ((CFDictionaryRef)refElement, CFSTR(kIOHIDElementMinKey));
	if (refType && CFNumberGetValue ((CFNumberRef)refType, kCFNumberLongType, &number))
		pComponent->minRead = pComponent->min = number;
	refType = CFDictionaryGetValue ((CFDictionaryRef)refElement, CFSTR(kIOHIDElementMaxKey));
	if (refType && CFNumberGetValue ((CFNumberRef)refType, kCFNumberLongType, &number))
		pComponent->maxRead = pComponent->max = number;
}

static void getJoystickComponentArrayHandler (const void * value, void * parameter);

static void addJoystickComponent (CFTypeRef refElement, JoystickInfo* joyInfo)
{
	long elementType, usagePage, usage;
	CFTypeRef refElementType = CFDictionaryGetValue ((CFDictionaryRef)refElement, CFSTR(kIOHIDElementTypeKey));
	CFTypeRef refUsagePage = CFDictionaryGetValue ((CFDictionaryRef)refElement, CFSTR(kIOHIDElementUsagePageKey));
	CFTypeRef refUsage = CFDictionaryGetValue ((CFDictionaryRef)refElement, CFSTR(kIOHIDElementUsageKey));


	if ((refElementType) && (CFNumberGetValue ((CFNumberRef)refElementType, kCFNumberLongType, &elementType)))
	{
		/* look at types of interest */
		if ((elementType == kIOHIDElementTypeInput_Misc) || (elementType == kIOHIDElementTypeInput_Button) ||
			(elementType == kIOHIDElementTypeInput_Axis))
		{
			if (refUsagePage && CFNumberGetValue ((CFNumberRef)refUsagePage, kCFNumberLongType, &usagePage) &&
				refUsage && CFNumberGetValue ((CFNumberRef)refUsage, kCFNumberLongType, &usage))
			{
				switch (usagePage) /* only interested in kHIDPage_GenericDesktop and kHIDPage_Button */
				{
					case kHIDPage_GenericDesktop:
						{
							switch (usage) /* look at usage to determine function */
							{
								case kHIDUsage_GD_X:
								case kHIDUsage_GD_Y:
								case kHIDUsage_GD_Z:
								case kHIDUsage_GD_Rx:
								case kHIDUsage_GD_Ry:
								case kHIDUsage_GD_Rz:
								case kHIDUsage_GD_Slider:
								case kHIDUsage_GD_Dial:
								case kHIDUsage_GD_Wheel:
								{
									joyInfo->axes++;
									JoystickComponent newComponent;
									addComponentInfo(refElement, &newComponent, joyInfo->numActiveJoysticks);
									joyInfo->axisComp.push_back(newComponent);
								}
								break;
								case kHIDUsage_GD_Hatswitch:
								{
									joyInfo->hats++;
									JoystickComponent newComponent;
									addComponentInfo(refElement, &newComponent, joyInfo->numActiveJoysticks);
									joyInfo->hatComp.push_back(newComponent);
								}
								break;
							}							
						}
						break;
					case kHIDPage_Button:
						{
							joyInfo->buttons++;
							JoystickComponent newComponent;
							addComponentInfo(refElement, &newComponent, joyInfo->numActiveJoysticks);
							joyInfo->buttonComp.push_back(newComponent);
						}
						break;
					default:
						break;
				}
			}
		}
		else if (kIOHIDElementTypeCollection == elementType) {
			//get elements
			CFTypeRef refElementTop = CFDictionaryGetValue ((CFMutableDictionaryRef) refElement, CFSTR(kIOHIDElementKey));
			if (refElementTop) {
				CFTypeID type = CFGetTypeID (refElementTop);
				if (type == CFArrayGetTypeID()) {
					CFRange range = {0, CFArrayGetCount ((CFArrayRef)refElementTop)};
					CFArrayApplyFunction ((CFArrayRef)refElementTop, range, getJoystickComponentArrayHandler, joyInfo);
				}
			}
		}
	}

}

static void getJoystickComponentArrayHandler (const void * value, void * parameter)
{
	if (CFGetTypeID (value) == CFDictionaryGetTypeID ())
		addJoystickComponent ((CFTypeRef) value, (JoystickInfo *) parameter);
}

static void joystickTopLevelElementHandler (const void * value, void * parameter)
{
	CFTypeRef refCF = 0;
	if (CFGetTypeID (value) != CFDictionaryGetTypeID ())
		return;
	refCF = CFDictionaryGetValue ((CFDictionaryRef)value, CFSTR(kIOHIDElementUsagePageKey));
	if (!CFNumberGetValue ((CFNumberRef)refCF, kCFNumberLongType, &((JoystickInfo *) parameter)->usagePage))
		irr::os::Printer::log("CFNumberGetValue error retrieving JoystickInfo->usagePage", irr::ELL_ERROR);
	refCF = CFDictionaryGetValue ((CFDictionaryRef)value, CFSTR(kIOHIDElementUsageKey));
	if (!CFNumberGetValue ((CFNumberRef)refCF, kCFNumberLongType, &((JoystickInfo *) parameter)->usage))
		irr::os::Printer::log("CFNumberGetValue error retrieving JoystickInfo->usage", irr::ELL_ERROR);
}

static void getJoystickDeviceInfo (io_object_t hidDevice, CFMutableDictionaryRef hidProperties, JoystickInfo *joyInfo)
{
	CFMutableDictionaryRef usbProperties = 0;
	io_registry_entry_t parent1, parent2;
	
	/* Mac OS X currently is not mirroring all USB properties to HID page so need to look at USB device page also
	 * get dictionary for usb properties: step up two levels and get CF dictionary for USB properties
	 */
	if ((KERN_SUCCESS == IORegistryEntryGetParentEntry (hidDevice, kIOServicePlane, &parent1)) &&
		(KERN_SUCCESS == IORegistryEntryGetParentEntry (parent1, kIOServicePlane, &parent2)) &&
		(KERN_SUCCESS == IORegistryEntryCreateCFProperties (parent2, &usbProperties, kCFAllocatorDefault, kNilOptions)))
	{
		if (usbProperties)
		{
			CFTypeRef refCF = 0;
			/* get device info
			 * try hid dictionary first, if fail then go to usb dictionary
			 */
			
			
			/* get joystickName name */
			refCF = CFDictionaryGetValue (hidProperties, CFSTR(kIOHIDProductKey));
			if (!refCF)
				refCF = CFDictionaryGetValue (usbProperties, CFSTR("USB Product Name"));
			if (refCF)
			{
				if (!CFStringGetCString ((CFStringRef)refCF, joyInfo->joystickName, 256, CFStringGetSystemEncoding ()))
					irr::os::Printer::log("CFStringGetCString error getting joyInfo->joystickName", irr::ELL_ERROR);
			}
			
			/* get usage page and usage */
			refCF = CFDictionaryGetValue (hidProperties, CFSTR(kIOHIDPrimaryUsagePageKey));
			if (refCF)
			{
				if (!CFNumberGetValue ((CFNumberRef)refCF, kCFNumberLongType, &joyInfo->usagePage))
					irr::os::Printer::log("CFNumberGetValue error getting joyInfo->usagePage", irr::ELL_ERROR);
				refCF = CFDictionaryGetValue (hidProperties, CFSTR(kIOHIDPrimaryUsageKey));
				if (refCF)
					if (!CFNumberGetValue ((CFNumberRef)refCF, kCFNumberLongType, &joyInfo->usage))
						irr::os::Printer::log("CFNumberGetValue error getting joyInfo->usage", irr::ELL_ERROR);
			}

			if (NULL == refCF) /* get top level element HID usage page or usage */
			{
				/* use top level element instead */
				CFTypeRef refCFTopElement = 0;
				refCFTopElement = CFDictionaryGetValue (hidProperties, CFSTR(kIOHIDElementKey));
				{
					/* refCFTopElement points to an array of element dictionaries */
					CFRange range = {0, CFArrayGetCount ((CFArrayRef)refCFTopElement)};
					CFArrayApplyFunction ((CFArrayRef)refCFTopElement, range, joystickTopLevelElementHandler, joyInfo);
				}
			}

			CFRelease (usbProperties);
		}
		else
			irr::os::Printer::log("IORegistryEntryCreateCFProperties failed to create usbProperties", irr::ELL_ERROR);

		if (kIOReturnSuccess != IOObjectRelease (parent2))
			irr::os::Printer::log("IOObjectRelease failed to release parent2", irr::ELL_ERROR);
		if (kIOReturnSuccess != IOObjectRelease (parent1))
			irr::os::Printer::log("IOObjectRelease failed to release parent1", irr::ELL_ERROR);
	}
}

#endif // _IRR_COMPILE_WITH_JOYSTICK_EVENTS_

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
#if defined(_IRR_COMPILE_WITH_JOYSTICK_EVENTS_)
	for(u32 joystick = 0; joystick < ActiveJoysticks.size(); ++joystick)
	{
		if(ActiveJoysticks[joystick].interface)
			closeJoystickDevice(&ActiveJoysticks[joystick]);
	}
#endif
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

	pollJoysticks();

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

#if defined (_IRR_COMPILE_WITH_JOYSTICK_EVENTS_)
static void joystickRemovalCallback(void * target,
                               IOReturn result,
                               void * refcon,
                               void * sender)
{
	JoystickInfo *joy = (JoystickInfo *) refcon;
	joy->removed = 1;
}
#endif // _IRR_COMPILE_WITH_JOYSTICK_EVENTS_


bool CIrrDeviceMacOSX::activateJoysticks(core::array<SJoystickInfo> & joystickInfo)
{
#if defined (_IRR_COMPILE_WITH_JOYSTICK_EVENTS_)
	ActiveJoysticks.clear();
	joystickInfo.clear();

	io_object_t hidObject = 0;
	io_iterator_t hidIterator = 0;
	IOReturn result = kIOReturnSuccess;
	mach_port_t masterPort = 0;
	CFMutableDictionaryRef hidDictionaryRef = NULL;

	result = IOMasterPort (bootstrap_port, &masterPort);
	if (kIOReturnSuccess != result) {
		os::Printer::log("initialiseJoysticks IOMasterPort failed", ELL_ERROR);
		return false;
	}

	hidDictionaryRef = IOServiceMatching (kIOHIDDeviceKey);
	if (!hidDictionaryRef) {
		os::Printer::log("initialiseJoysticks IOServiceMatching failed", ELL_ERROR);
		return false;
	}
	result = IOServiceGetMatchingServices (masterPort, hidDictionaryRef, &hidIterator);

	if (kIOReturnSuccess != result) {
		os::Printer::log("initialiseJoysticks IOServiceGetMatchingServices failed", ELL_ERROR);
		return false;
	}

	//no joysticks just return
	if (!hidIterator)
		return false;

	while ((hidObject = IOIteratorNext (hidIterator)))
	{
		JoystickInfo info;

		// get dictionary for HID properties
		CFMutableDictionaryRef hidProperties = 0;

		kern_return_t kern_result = IORegistryEntryCreateCFProperties (hidObject, &hidProperties, kCFAllocatorDefault, kNilOptions);
		if ((kern_result == KERN_SUCCESS) && hidProperties) {
			HRESULT plugInResult = S_OK;
			SInt32 score = 0;
			IOCFPlugInInterface ** ppPlugInInterface = NULL;
			result = IOCreatePlugInInterfaceForService (hidObject, kIOHIDDeviceUserClientTypeID,
													kIOCFPlugInInterfaceID, &ppPlugInInterface, &score);
			if (kIOReturnSuccess == result) {
				plugInResult = (*ppPlugInInterface)->QueryInterface (ppPlugInInterface,
									CFUUIDGetUUIDBytes (kIOHIDDeviceInterfaceID), (void **) &(info.interface));
				if (plugInResult != S_OK)
					os::Printer::log("initialiseJoysticks query HID class device interface failed", ELL_ERROR);
				(*ppPlugInInterface)->Release (ppPlugInInterface);
			}
			else
				continue;

			if (info.interface != NULL) {
				result = (*(info.interface))->open (info.interface, 0);
				if (result == kIOReturnSuccess) {
					(*(info.interface))->setRemovalCallback (info.interface, joystickRemovalCallback, &info, &info);
					getJoystickDeviceInfo(hidObject, hidProperties, &info);

					//get elements
					CFTypeRef refElementTop = CFDictionaryGetValue (hidProperties, CFSTR(kIOHIDElementKey));
					if (refElementTop) {
						CFTypeID type = CFGetTypeID (refElementTop);
						if (type == CFArrayGetTypeID()) {
							CFRange range = {0, CFArrayGetCount ((CFArrayRef)refElementTop)};
							info.numActiveJoysticks = ActiveJoysticks.size();
							CFArrayApplyFunction ((CFArrayRef)refElementTop, range, getJoystickComponentArrayHandler, &info);
						}
					}
				} else {
					CFRelease (hidProperties);
					os::Printer::log("initialiseJoysticks Open interface failed", ELL_ERROR);
					continue;				
				}

				CFRelease (hidProperties);

				result = IOObjectRelease (hidObject);
				
				if ( (info.usagePage != kHIDPage_GenericDesktop) ||
					 ((info.usage != kHIDUsage_GD_Joystick &&
					  info.usage != kHIDUsage_GD_GamePad &&
					  info.usage != kHIDUsage_GD_MultiAxisController)) ) {
					closeJoystickDevice (&info);
					continue;
				}

				for (u32 i = 0; i < 6; i++)
					info.persistentData.JoystickEvent.Axis[i] = 0;

				ActiveJoysticks.push_back(info);
				
				SJoystickInfo returnInfo;
				returnInfo.Axes = info.axes;
				//returnInfo.Hats = info.hats;
				returnInfo.Buttons = info.buttons;
				returnInfo.Name    = info.joystickName;
				returnInfo.PovHat  = SJoystickInfo::POV_HAT_UNKNOWN;

				//if (info.hatComp.size())
				//	returnInfo.PovHat = SJoystickInfo::POV_HAT_PRESENT;
				//else
				//	returnInfo.PovHat = SJoystickInfo::POV_HAT_ABSENT;

				joystickInfo.push_back(returnInfo);
			}

		} else
			continue;
	}
	result = IOObjectRelease (hidIterator);

	return true;
#endif // _IRR_COMPILE_WITH_JOYSTICK_EVENTS_

	return false;
}

void CIrrDeviceMacOSX::pollJoysticks()
{
#if defined (_IRR_COMPILE_WITH_JOYSTICK_EVENTS_)
	if(0 == ActiveJoysticks.size())
		return;

	u32 joystick;
	for(joystick = 0; joystick < ActiveJoysticks.size(); ++joystick)
	{
		if (ActiveJoysticks[joystick].removed)
			continue;
		
		bool found = false;
		ActiveJoysticks[joystick].persistentData.JoystickEvent.Joystick = joystick;

		if (ActiveJoysticks[joystick].interface)
		{
			for (u32 n = 0; n < ActiveJoysticks[joystick].axisComp.size(); n++) {
				IOReturn result = kIOReturnSuccess;
				IOHIDEventStruct hidEvent;
				hidEvent.value = 0;
				result = (*(ActiveJoysticks[joystick].interface))->getElementValue(ActiveJoysticks[joystick].interface, ActiveJoysticks[joystick].axisComp[n].cookie, &hidEvent);
				if (kIOReturnSuccess == result) {
					f32 min = -32768.0f;
					f32 max = 32768.0f;
					f32 deviceScale = max - min;
					f32 readScale = (f32)ActiveJoysticks[joystick].axisComp[n].maxRead - (f32)ActiveJoysticks[joystick].axisComp[n].minRead;

					if (hidEvent.value < ActiveJoysticks[joystick].axisComp[n].minRead)
						ActiveJoysticks[joystick].axisComp[n].minRead = hidEvent.value;
					if (hidEvent.value > ActiveJoysticks[joystick].axisComp[n].maxRead)
						ActiveJoysticks[joystick].axisComp[n].maxRead = hidEvent.value;
					
					if (readScale != 0.0f)
						hidEvent.value = (int)(((f32)((f32)hidEvent.value - (f32)ActiveJoysticks[joystick].axisComp[n].minRead) * deviceScale / readScale) + min);

					if (ActiveJoysticks[joystick].persistentData.JoystickEvent.Axis[n] != (s16)hidEvent.value)
						found = true;
					ActiveJoysticks[joystick].persistentData.JoystickEvent.Axis[n] = (s16)hidEvent.value;
				}
			}//axis check

			for (u32 n = 0; n < ActiveJoysticks[joystick].buttonComp.size(); n++) {
				IOReturn result = kIOReturnSuccess;
				IOHIDEventStruct hidEvent;
				hidEvent.value = 0;
				result = (*(ActiveJoysticks[joystick].interface))->getElementValue(ActiveJoysticks[joystick].interface, ActiveJoysticks[joystick].buttonComp[n].cookie, &hidEvent);
				if (kIOReturnSuccess == result) {
					u32 ButtonStates = 0;

					if (hidEvent.value && !((ActiveJoysticks[joystick].persistentData.JoystickEvent.ButtonStates & (1 << n)) ? true : false) )
							found = true;
					else if (!hidEvent.value && ((ActiveJoysticks[joystick].persistentData.JoystickEvent.ButtonStates & (1 << n)) ? true : false))
							found = true;

					if (hidEvent.value)
							ActiveJoysticks[joystick].persistentData.JoystickEvent.ButtonStates |= (1 << n);
					else
							ActiveJoysticks[joystick].persistentData.JoystickEvent.ButtonStates &= ~(1 << n);
				}
			}//button check
			//still ToDo..will be done soon :)
/*
			for (u32 n = 0; n < ActiveJoysticks[joystick].hatComp.size(); n++) {
				IOReturn result = kIOReturnSuccess;
				IOHIDEventStruct hidEvent;
				hidEvent.value = 0;
				result = (*(ActiveJoysticks[joystick].interface))->getElementValue(ActiveJoysticks[joystick].interface, ActiveJoysticks[joystick].hatComp[n].cookie, &hidEvent);
				if (kIOReturnSuccess == result) {
					if (ActiveJoysticks[joystick].persistentData.JoystickEvent.POV != hidEvent.value)
						found = true;
					ActiveJoysticks[joystick].persistentData.JoystickEvent.POV = hidEvent.value;
				}
			}//hat check
*/
		}

		if (found)
			postEventFromUser(ActiveJoysticks[joystick].persistentData);
	}
#endif // _IRR_COMPILE_WITH_JOYSTICK_EVENTS_
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

