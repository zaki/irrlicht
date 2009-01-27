// Copyright (C) 2008 Colin MacDonald
// No rights reserved: this software is in the public domain.

#include "testUtils.h"
#include "irrlicht.h"
#include <assert.h>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

// Test some matrix ops.
bool matrixOps(void)
{
	matrix4 rotationMatrix;
	if (!rotationMatrix.isOrthogonal())
	{
		logTestString("irr::core::matrix4::isOrthogonal() failed with Identity.\n");
		return false;
	}

	rotationMatrix.setRotationDegrees(vector3df(90, 0, 0));
	if (!rotationMatrix.isOrthogonal())
	{
		logTestString("irr::core::matrix4::isOrthogonal() failed with rotation.\n");
		return false;
	}

	matrix4 translationMatrix;
	translationMatrix.setTranslation(vector3df(0, 3, 0));
	if (translationMatrix.isOrthogonal())
	{
		logTestString("irr::core::matrix4::isOrthogonal() failed with translation.\n");
		return false;
	}

	matrix4 scaleMatrix;
	scaleMatrix.setScale(vector3df(1, 2, 3));
	if (!scaleMatrix.isOrthogonal())
	{
		logTestString("irr::core::matrix4::isOrthogonal() failed with scale.\n");
		return false;
	}

	return true;
}
