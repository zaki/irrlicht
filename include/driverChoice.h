// Copyright (C) 2009-2012 Christian Stehno
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __E_DRIVER_CHOICE_H_INCLUDED__
#define __E_DRIVER_CHOICE_H_INCLUDED__

#include <iostream>
#include <cstdio>
#include "EDriverTypes.h"
#include "IrrlichtDevice.h"

namespace irr
{

//! ask user for driver
static irr::video::E_DRIVER_TYPE driverChoiceConsole(bool allDrivers=true)
{
	printf("Please select the driver you want:\n");
	irr::u32 i=0;
	for (i=irr::video::EDT_COUNT; i>0; --i)
	{
		irr::video::E_DRIVER_TYPE driverType = irr::video::E_DRIVER_TYPE(i-1);
		if ( driverType == irr::video::DEPRECATED_EDT_DIRECT3D8_NO_LONGER_EXISTS )
			continue;
		if (allDrivers || irr::IrrlichtDevice::isDriverSupported(driverType) )
			printf(" (%c) %s\n", 'a'+irr::video::EDT_COUNT-i, irr::video::DRIVER_TYPE_NAMES[i-1]);
	}

	char c;
	std::cin >> c;
	c = irr::video::EDT_COUNT+'a'-c;

	for (i=irr::video::EDT_COUNT; i>0; --i)
	{
		if (!(allDrivers || (irr::IrrlichtDevice::isDriverSupported(irr::video::E_DRIVER_TYPE(i-1)))))
			--c;
		if ((char)i==c)
			return irr::video::E_DRIVER_TYPE(i-1);
	}
	return irr::video::EDT_COUNT;
}

} // end namespace irr

#endif
