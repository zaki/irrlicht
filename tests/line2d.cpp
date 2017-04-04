// Copyright (C) 2017 Dario Oliveri
// No rights reserved: this software is in the public domain.

#include "testUtils.h"

using namespace irr;
using namespace irr::core;

#include <stdint.h>
#include <iostream>

/* The state must be seeded so that it is not everywhere zero. */
uint64_t s[16];
int p;

uint64_t xorshift1024star(void) {
	const uint64_t s0 = s[p];
	uint64_t s1 = s[p = (p + 1) & 15];
	s1 ^= s1 << 31; // a
	s[p] = s1 ^ s0 ^ (s1 >> 11) ^ (s0 >> 30); // b, c
	return s[p] * UINT64_C(1181783497276652981);
}

void initRand() {
	for (int i = 0; i < 16; i++) {
		s[i] = i*i*i - i +13;
	}
}

float randomFloat() {
	return (float) xorshift1024star() / 2e62;
}

#define EXPECT( condition, value, name) if( condition != value) \
	{ std::cout<< name << ": test failed"<< std::endl; return false;}

//! Tests the basic functionality of the software device.
bool line2DTest(void)
{
	initRand();

	{
		line2d< f32> a(0, 0, 0, 1);
		line2d< f32> b(2, 0, 2, 1);
		line2d< f32> c(2, 0, 2, 1 + 0.00001f);

		EXPECT( a.nearlyParallel( b), true, "parallel Lines are parallel");
		EXPECT( a.nearlyParallel( c, (f32)32), true, "nearly parallel lines are parallel");
	}

	{
		line2d< f32> a( 0, 0, 0, 1);
		line2d< f32> b( 0, 2, 2, 1);
		EXPECT( a.nearlyParallel( b, 1), false, "orthogonal lines are NOT parallel");
	}
	
	{
		line2d< f32> a( 0, 0, 100, 100);
		line2d< f32> b( 100, 0, 0, 100);

		EXPECT( a.nearlyParallel( b, 1), false, "orthogonal lines are NOT parallel 2");

		vector2df t = a.fastLinesIntersection( b);
		vector2df u = vector2df( 50.0f, 50.0f);

		EXPECT( t.equals( u, roundingError<f32>() ), true, "fast intersection in known point");

		EXPECT( a .intersectAsSegments(b), true, "intersect as Segments");

		EXPECT( a.incidentSegments(b), true, "incidentSegments");

		vector2df out;
		EXPECT( a.lineIntersectSegment( b, out), true, "lineIntersectSegment");
		EXPECT( t.equals( out), true, "line intersect segment in known point");

		EXPECT( a.isPointBetweenStartAndEnd( out), true, "point isBetween StartEnd of first line");
		EXPECT( b.isPointBetweenStartAndEnd( out), true, "point isBetween StartEnd of second line");

		EXPECT( a.isPointOnLine( out), true, "is point on first line");
		EXPECT( b.isPointOnLine( out), true, "is point on second line");

		EXPECT( out.isBetweenPoints( a.start, a.end), true, "test point is on segment with first line");
		EXPECT( out.isBetweenPoints( b.start, b.end), true, "test point is on segment with first line");
	}

	{
		vector2df a( 0, 0);
		vector2df b( 10, 0);
		vector2df c( 0, 10);
		vector2df d( 0, 40);

		EXPECT( a.areClockwise( c, b), true, "test if points are clockwise");
		EXPECT( a.areClockwise( b, c), false, "test if points are NOT clockwise");
		EXPECT( a.areCounterClockwise( b, c), true, "test if points are counter clockwise");
		EXPECT( a.areCounterClockwise( c, b), false, "test if points are NOT counter clockwise");

		EXPECT( a.checkOrientation( c, b), 1, "test if orientation is clockwise");
		EXPECT( a.checkOrientation( b, c), 2, "test if orientation is anticlockwise");
		EXPECT( a.checkOrientation( c, d), 0, "test if orientation is colinear");
		EXPECT( a.checkOrientation( d, c), 0, "test if orientation is colinear 2");

		
	}

	return true;
}

#undef EXPECT
