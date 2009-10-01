// Copyright Redshift Software, Inc. 2008
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#import "CIrrDeviceIPhone.h"

#ifdef _IRR_COMPILE_WITH_IPHONE_DEVICE_

#import <UIKit/UIKit.h>
#import <QuartzCore/CAEAGLLayer.h>
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#import <OpenGLES/EAGL.h>
#import <stdio.h>

@interface IrrIPhoneView : UIView
{
}
@end

@implementation IrrIPhoneView
+ (Class) layerClass
{
	return [CAEAGLLayer class];
}
@end

@interface IrrIPhoneDevice : NSObject <UIApplicationDelegate>
{
	@public
	
	EAGLContext * context;
	IrrIPhoneView * view;
	struct MIrrIPhoneDevice * dev;
}
- (id) initWithDevice: (struct MIrrIPhoneDevice*) device;
- (void) dealloc;
- (void) applicationWillResignActive: (UIApplication *) application;
- (void) applicationDidBecomeActive: (UIApplication *) application;
- (void) applicationWillTerminate: (UIApplication *) application;
- (void) displayCreateInWindow: (UIWindow**) window Width: (int) w Height: (int) h;
- (void) displayInitialize: (EAGLContext**) context_ View: (IrrIPhoneView**) view_;
- (void) displayBegin;
- (void) displayEnd;
@end

@implementation IrrIPhoneDevice
- (id) initWithDevice: (struct MIrrIPhoneDevice*) device
{
	if ( (self = [super init]) )
	{
		context = nil;
		view = nil;
		dev = device;
	}
	return self;
}
- (void) dealloc
{
	[super dealloc];
}
- (void) applicationWillResignActive: (UIApplication *) application
{
	// Pause rendering.
	(*(dev->onWindowActive))(dev,0);
}
- (void) applicationDidBecomeActive: (UIApplication *) application
{
	// Resume after non-active pause of rendering.
	(*(dev->onWindowActive))(dev,1);
}
- (void) applicationWillTerminate: (UIApplication *) application
{
	(*(dev->onTerminate))(dev);
}
- (void) displayCreateInWindow: (UIWindow**) window Width: (int) w Height: (int) h
{
	// Create our view.
	CGRect rect;
	rect.origin.x = 0;
	rect.origin.y = 0;
	rect.size.width = w;
	rect.size.height = h;
    view = [[IrrIPhoneView alloc] initWithFrame: rect];
	view.layer.opaque = YES;
	if (nil != *window)
	{
		[(*window) addSubview: view];
	}
	
	// Create the GL context now, so that the driver initializetion
	// can make OpenGL calls.
	context = [[EAGLContext alloc] initWithAPI: kEAGLRenderingAPIOpenGLES1];
	[EAGLContext setCurrentContext: context];
}
- (void) displayInitialize: (EAGLContext**) context_ View: (IrrIPhoneView**) view_
{
	*context_ = context;
	*view_ = view;
	[context renderbufferStorage: GL_RENDERBUFFER_OES fromDrawable: (CAEAGLLayer*)view.layer];
}
- (void) displayBegin
{
	if (context != [EAGLContext currentContext])
	{
		[EAGLContext setCurrentContext: context];
	}
}
- (void) displayEnd
{
	if (context == [EAGLContext currentContext])
	{
		[context presentRenderbuffer: GL_RENDERBUFFER_OES];
	}
}
@end

void irr_device_iphone_display_create(struct MIrrIPhoneDevice * dev,
	void** window, int w, int h)
{
	[((IrrIPhoneDevice*)dev->DeviceM) displayCreateInWindow: (UIWindow**)window Width: w Height: h];
}

void irr_device_iphone_display_init(struct MIrrIPhoneDevice * dev,
	void** context, void** view)
{
	[((IrrIPhoneDevice*)dev->DeviceM) displayInitialize: (EAGLContext**)context View: (IrrIPhoneView**)view];
}

void irr_device_iphone_display_begin(struct MIrrIPhoneDevice * dev)
{
	[((IrrIPhoneDevice*)dev->DeviceM) displayBegin];
}

void irr_device_iphone_display_end(struct MIrrIPhoneDevice * dev)
{
	[((IrrIPhoneDevice*)dev->DeviceM) displayEnd];
}

void irr_device_iphone_create(struct MIrrIPhoneDevice * dev)
{
	dev->DeviceM = [[IrrIPhoneDevice alloc] initWithDevice: dev];
	dev->displayCreate = &irr_device_iphone_display_create;
	dev->displayInit = &irr_device_iphone_display_init;
	dev->displayBegin = &irr_device_iphone_display_begin;
	dev->displayEnd = &irr_device_iphone_display_end;
}

#endif // _IRR_COMPILE_WITH_IPHONE_DEVICE_
