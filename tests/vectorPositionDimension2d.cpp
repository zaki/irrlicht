// Copyright (C) 2008 Colin MacDonald
// No rights reserved: this software is in the public domain.

/** This test verifies that position2d and vector2d are interchangeable,
	and that they can convert from dimension2d */

#include "testUtils.h"
#include "irrlicht.h"
#include <assert.h>

using namespace irr;
using namespace core;


template <class DIMENSION, class VECTOR, class POSITION, class T>
static bool doTest(void)
{
	bool result = true;

	DIMENSION dimension((T)99.9, (T)99.9);
	VECTOR vector(dimension);
	POSITION position(vector);
	DIMENSION dimension2(vector);

	result &= (vector == position);
	result &= (vector == dimension); // The conversion should be explicit.
	result &= (dimension2 == position);
	result &= (position == POSITION((T)99.9, (T)99.9));
	
	dimension = (T)2 * position;
	result &= (dimension == VECTOR(2 * (T)99.9, 2 * (T)99.9));

	dimension /= (T)2;
	result &= (dimension == POSITION((T)99.9, (T)99.9));

	dimension += vector;
	result &= (dimension == VECTOR(2 * (T)99.9, 2 * (T)99.9));

	dimension -= position;
	result &= (dimension == POSITION((T)99.9, (T)99.9));
	
	position = dimension;
	result &= (position == VECTOR((T)99.9, (T)99.9));

	vector += position;
	result &= (vector == POSITION(2 * (T)99.9, 2 * (T)99.9));

	vector -= position;
	result &= (vector == dimension);

	position *= (T)3.5;
	result &= (position == VECTOR((T)3.5 * (T)99.9, (T)3.5 * (T)99.9));

	vector += dimension;
	result &= (vector == VECTOR(2 * (T)99.9, 2 * (T)99.9));

	return result;
}

bool vectorPositionDimension2d(void)
{
	bool result = true;

	result &= doTest<dimension2di, vector2di, position2di, s32>();
	result &= doTest<dimension2df, vector2df, position2df, f32>();
	result &= doTest<dimension2d<f64>, vector2d<f64>, position2d<f64>, f64>();
	
	if(!result)
		assert(false);

	return result;
}

