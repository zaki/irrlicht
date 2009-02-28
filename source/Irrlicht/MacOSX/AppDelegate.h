// Copyright (C) 2005-2009 Etienne Petitjean
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#include "IrrCompileConfig.h"

#ifdef _IRR_USE_OSX_DEVICE_

#import <Cocoa/Cocoa.h>
#import "CIrrDeviceMacOSX.h"

@interface AppDelegate : NSObject
{
	BOOL			_quit;
	irr::CIrrDeviceMacOSX	*_device;
}

- (id)initWithDevice:(irr::CIrrDeviceMacOSX *)device;
- (BOOL)isQuit;

@end

#endif // _IRR_USE_OSX_DEVICE_
