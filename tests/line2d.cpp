// Copyright (C) 2017 Dario Oliveri
// No rights reserved: this software is in the public domain.

#include "testUtils.h"

#include <iostream>

using namespace irr;
using namespace irr::core;

#define EXPECT( condition, value, name) if( condition != value) \
	{ std::cout<< name << ": test failed"<< std::endl; return false;}

//! Tests the basic functionality of the software device.
bool line2DTest(void)
{
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
