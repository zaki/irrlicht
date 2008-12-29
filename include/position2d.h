// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __IRR_POSITION_H_INCLUDED__
#define __IRR_POSITION_H_INCLUDED__

// position2d is deprecated.  It is currently defined as a vector2d, but
// vector2d should be used directly.
#include "vector2d.h"

namespace irr
{
namespace core
{
// Use explicit typedefs where possible
typedef vector2d<f32> position2df;

typedef vector2d<s32> position2di;
}; // namespace core
}; // namespace irr

// And a #define to catch the rest
#define position2d vector2d

#endif // __IRR_POSITION_H_INCLUDED__
