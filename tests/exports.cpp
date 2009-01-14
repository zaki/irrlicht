// Copyright (C) 2008-2009 Colin MacDonald
// No rights reserved: this software is in the public domain.

#include "testUtils.h"
#include "irrlicht.h"

//! Tests that symbols exported from Irrlicht can be used by the user app.
bool exports(void)
{
	irr::core::matrix4 identity = irr::core::IdentityMatrix;
	(void)identity; // Satisfy the compiler that it's used.

	// If it built, we're done.
	return true;
}
