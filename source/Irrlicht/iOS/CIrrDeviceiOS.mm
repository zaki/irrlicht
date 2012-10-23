// Copyright Redshift Software, Inc. 2008
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#import "CIrrDeviceiOS.h"
//#include "IEventReceiver.h"

#ifdef _IRR_COMPILE_WITH_IPHONE_DEVICE_

#import <UIKit/UIKit.h>
#import <QuartzCore/CAEAGLLayer.h>
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#import <OpenGLES/EAGL.h>
#import <stdio.h>

@interface IrrIPhoneView : UIView
{
    irr::CIrrDeviceIPhone* dev;
}
//@property (nonatomic) id * dev;
- (void) setDevice:(struct irr::MIrrIPhoneDevice*)device;
@end

@implementation IrrIPhoneView
+ (Class) layerClass
{
	return [CAEAGLLayer class];
}

- (BOOL) isMultipleTouchEnabled
{
	return YES;
}

- (void) setDevice:(struct irr::MIrrIPhoneDevice*)device
{
	dev = (irr::CIrrDeviceIPhone*)(device->DeviceCPP); 
}

- (void) touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
	//NSLog(@">> IrrIPhoneView.touchBegan");

	UITouch *touch = [touches anyObject];
	CGPoint touchPoint = [touch locationInView:self];
    
    float scale = 1.0f;
    
    if ([self respondsToSelector:@selector(setContentScaleFactor:)])
        scale = [[UIScreen mainScreen] scale];

	// event as mouse.
	irr::SEvent ev;
	ev.EventType = irr::EET_MOUSE_INPUT_EVENT;
	ev.MouseInput.Event = irr::EMIE_LMOUSE_PRESSED_DOWN;
	ev.MouseInput.X = touchPoint.x*scale;
	ev.MouseInput.Y = touchPoint.y*scale;
    
	//ev.MouseInput.Shift = 0;
	//ev.MouseInput.Control = 0;
	ev.MouseInput.ButtonStates = 0;	//MK_LBUTTON;
	//ev.MouseInput.Wheel = 0.f;
    dev->postEventFromUser(ev);


	// event as multi touch
	CGPoint nowTouchPoint, prevTouchPoint;
	ev.EventType = irr::EET_MULTI_TOUCH_EVENT;
	ev.MultiTouchInput.Event = irr::EMTIE_PRESSED_DOWN;
	ev.MultiTouchInput.clear();
	int idx = 0;
	for (touch in touches)
	{
		if (idx >= irr::NUMBER_OF_MULTI_TOUCHES) break;
		prevTouchPoint = [touch previousLocationInView:self];
		nowTouchPoint = [touch locationInView:self];
		ev.MultiTouchInput.Touched[idx] = 1;
		ev.MultiTouchInput.PrevX[idx] = prevTouchPoint.x*scale;
		ev.MultiTouchInput.PrevY[idx] = prevTouchPoint.y*scale;
		ev.MultiTouchInput.X[idx] = nowTouchPoint.x*scale;
		ev.MultiTouchInput.Y[idx] = nowTouchPoint.y*scale;
		idx ++;
	}
	dev->postEventFromUser(ev);
}

- (void) touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
	//NSLog(@">> IrrIPhoneView.touchesMoved");

	UITouch *touch = [touches anyObject];
	CGPoint touchPoint = [touch locationInView:self];
    
    float scale = 1.0f;
    
    if ([self respondsToSelector:@selector(setContentScaleFactor:)])
        scale = [[UIScreen mainScreen] scale];

	irr::SEvent ev;
	ev.EventType = irr::EET_MOUSE_INPUT_EVENT;
	ev.MouseInput.Event = irr::EMIE_MOUSE_MOVED;
	ev.MouseInput.X = touchPoint.x*scale;
	ev.MouseInput.Y = touchPoint.y*scale;
	//ev.MouseInput.Shift = 0;
	//ev.MouseInput.Control = 0;
	ev.MouseInput.ButtonStates = 0;	//MK_LBUTTON;
	//ev.MouseInput.Wheel = 0.f;
	dev->postEventFromUser(ev);


	CGPoint nowTouchPoint, prevTouchPoint;
	ev.EventType = irr::EET_MULTI_TOUCH_EVENT;
	ev.MultiTouchInput.Event = irr::EMTIE_MOVED;
	ev.MultiTouchInput.clear();
	int idx = 0;
	for (touch in touches)
	{
		if (idx >= irr::NUMBER_OF_MULTI_TOUCHES) break;
		prevTouchPoint = [touch previousLocationInView:self];
		nowTouchPoint = [touch locationInView:self];
		ev.MultiTouchInput.Touched[idx] = 1;
		ev.MultiTouchInput.PrevX[idx] = prevTouchPoint.x*scale;
		ev.MultiTouchInput.PrevY[idx] = prevTouchPoint.y*scale;
		ev.MultiTouchInput.X[idx] = nowTouchPoint.x*scale;
		ev.MultiTouchInput.Y[idx] = nowTouchPoint.y*scale;
		idx ++;
	}
	dev->postEventFromUser(ev);
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
	//NSLog(@">> IrrIPhoneView.touchesEnded");

	UITouch *touch = [touches anyObject];
	CGPoint touchPoint = [touch locationInView:self];
    
    float scale = 1.0f;
    
    if ([self respondsToSelector:@selector(setContentScaleFactor:)])
        scale = [[UIScreen mainScreen] scale];

	irr::SEvent ev;
	ev.EventType = irr::EET_MOUSE_INPUT_EVENT;
	ev.MouseInput.Event = irr::EMIE_LMOUSE_LEFT_UP;
	ev.MouseInput.X = touchPoint.x*scale;
	ev.MouseInput.Y = touchPoint.y*scale;
	//ev.MouseInput.Shift = 0;
	//ev.MouseInput.Control = 0;
	ev.MouseInput.ButtonStates = 0;	//MK_LBUTTON;
	//ev.MouseInput.Wheel = 0.f;
	dev->postEventFromUser(ev);


	CGPoint nowTouchPoint, prevTouchPoint;
	ev.EventType = irr::EET_MULTI_TOUCH_EVENT;
	ev.MultiTouchInput.Event = irr::EMTIE_LEFT_UP;
	ev.MultiTouchInput.clear();
	int idx = 0;
	for (touch in touches)
	{
		if (idx >= irr::NUMBER_OF_MULTI_TOUCHES) break;
		prevTouchPoint = [touch previousLocationInView:self];
		nowTouchPoint = [touch locationInView:self];
		ev.MultiTouchInput.Touched[idx] = 1;
		ev.MultiTouchInput.PrevX[idx] = prevTouchPoint.x*scale;
		ev.MultiTouchInput.PrevY[idx] = prevTouchPoint.y*scale;
		ev.MultiTouchInput.X[idx] = nowTouchPoint.x*scale;
		ev.MultiTouchInput.Y[idx] = nowTouchPoint.y*scale;
		idx ++;
	}
	dev->postEventFromUser(ev);
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
	NSLog(@">> IrrIPhoneView.touchesCancelled");

	UITouch *touch = [touches anyObject];
	CGPoint touchPoint = [touch locationInView:self];
    
    float scale = 1.0f;
    
    if ([self respondsToSelector:@selector(setContentScaleFactor:)])
        scale = [[UIScreen mainScreen] scale];

	irr::SEvent ev;
	ev.EventType = irr::EET_MOUSE_INPUT_EVENT;
	ev.MouseInput.Event = irr::EMIE_LMOUSE_LEFT_UP;
	ev.MouseInput.X = touchPoint.x*scale;
	ev.MouseInput.Y = touchPoint.y*scale;
	//ev.MouseInput.Shift = 0;
	//ev.MouseInput.Control = 0;
	ev.MouseInput.ButtonStates = 0;	//MK_LBUTTON;
	//ev.MouseInput.Wheel = 0.f;

	dev->postEventFromUser(ev);
}
@end


@interface IrrIPhoneDevice : NSObject <UIApplicationDelegate>
{
	@public
	
	EAGLContext * context;
	IrrIPhoneView * view;
	struct irr::MIrrIPhoneDevice * dev;
}
- (id) initWithDevice: (struct irr::MIrrIPhoneDevice*) device;
- (void) dealloc;
- (void) applicationWillResignActive: (UIApplication *) application;
- (void) applicationDidBecomeActive: (UIApplication *) application;
- (void) applicationWillTerminate: (UIApplication *) application;
- (void) displayCreateInWindow: (UIWindow**) window Width: (int) w Height: (int) h OGLESType: (bool) type;
- (void) displayInitialize: (EAGLContext**) context_ View: (IrrIPhoneView**) view_;
- (void) displayBegin;
- (void) displayEnd;
- (void) postEvent: (irr::SEvent *) event;
@end

@implementation IrrIPhoneDevice
- (id) initWithDevice: (struct irr::MIrrIPhoneDevice*) device
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
- (void) displayCreateInWindow: (UIWindow**) window Width: (int) w Height: (int) h OGLESType: (bool) type
{    
	// Create our view.
	CGRect rect = [[UIScreen mainScreen] applicationFrame];
    view = [[IrrIPhoneView alloc] initWithFrame: rect];
    
    if ([view respondsToSelector:@selector(setContentScaleFactor:)])
    {
        view.ContentScaleFactor = [[UIScreen mainScreen] scale];
    }

	view.layer.opaque = YES;
	if (nil != *window)
	{
		[(*window) addSubview: view];
	}
    
    [view setDevice:dev];
	
	// Create the GL context now, so that the driver initializetion
	// can make OpenGL calls.
    if(type)
        context = [[EAGLContext alloc] initWithAPI: kEAGLRenderingAPIOpenGLES2];
    else
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
- (void) postEvent: (irr::SEvent *)  event
{
//NSLog(@">> IrrIPhoneDevice.postEvent");
	// Post event
	(*(dev->postEvent))(dev, event);
}
@end

void irr_device_iphone_display_create(struct irr::MIrrIPhoneDevice * dev,
	void** window, int w, int h, bool type)
{
	[((IrrIPhoneDevice*)dev->DeviceM) displayCreateInWindow: (UIWindow**)window Width: w Height: h OGLESType: type];
}

void irr_device_iphone_display_init(struct irr::MIrrIPhoneDevice * dev,
	void** context, void** view)
{
	[((IrrIPhoneDevice*)dev->DeviceM) displayInitialize: (EAGLContext**)context View: (IrrIPhoneView**)view];
}

void irr_device_iphone_display_begin(struct irr::MIrrIPhoneDevice * dev)
{
	[((IrrIPhoneDevice*)dev->DeviceM) displayBegin];
}

void irr_device_iphone_display_end(struct irr::MIrrIPhoneDevice * dev)
{
	[((IrrIPhoneDevice*)dev->DeviceM) displayEnd];
}

void irr_device_iphone_create(struct irr::MIrrIPhoneDevice * dev)
{
	dev->DeviceM = [[IrrIPhoneDevice alloc] initWithDevice: dev];
	dev->displayCreate = &irr_device_iphone_display_create;
	dev->displayInit = &irr_device_iphone_display_init;
	dev->displayBegin = &irr_device_iphone_display_begin;
	dev->displayEnd = &irr_device_iphone_display_end;
}

#endif // _IRR_COMPILE_WITH_IPHONE_DEVICE_
