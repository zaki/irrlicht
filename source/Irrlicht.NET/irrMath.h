// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

namespace Irrlicht
{
namespace Core
{
	//! Rounding error constant often used when comparing f32 values.
	const float ROUNDING_ERROR	= 0.0001f;

	//! Constant for PI.
	const float PI				= 3.14159f;

	//! Constant for converting bettween degrees and radiants.
	const double GRAD_PI		= 180.0 / 3.1415926535897932384626433832795;

	//! Constant for converting bettween degrees and radiants.
	const double GRAD_PI2		= 3.1415926535897932384626433832795 / 180.0;

	inline float mfabs(float f)
	{
		return System::Math::Abs(f);
	}

} // end namespace core
}// end namespace irr

