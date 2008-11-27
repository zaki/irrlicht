// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __E_HARDWARE_BUFFER_FLAGS_INCLUDED__
#define __E_HARDWARE_BUFFER_FLAGS_INCLUDED__

namespace irr
{
namespace scene
{

	enum E_HARDWARE_MAPPING
	{
		//! Don't load in hardware
		EHM_NEVER=0,

		//! Rarely changed
		EHM_STATIC,

		//! Sometimes changed
		EHM_DYNAMIC,

		//! Always changed
		EHM_STREAM
	};

	enum E_BUFFER_TYPE
	{
		EBT_NONE=0,
		EBT_VERTEX,
		EBT_INDEX,
		EBT_VERTEX_AND_INDEX
	};

} // end namespace scene
} // end namespace irr

#endif

