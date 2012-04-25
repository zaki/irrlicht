// Copyright (C) 2008-2011 Colin MacDonald
// No rights reserved: this software is in the public domain.

#include "testUtils.h"
#include <irrlicht.h>

using namespace irr;
using namespace core;

template<class T>
static bool isOnSameSide(const vector3d<T>& p1, const vector3d<T>& p2,
	const vector3d<T>& a, const vector3d<T>& b)
{
	vector3d<T> bminusa = b - a;
	vector3d<T> cp1 = bminusa.crossProduct(p1 - a);
	vector3d<T> cp2 = bminusa.crossProduct(p2 - a);
	return (cp1.dotProduct(cp2)+core::ROUNDING_ERROR_f64 >= 0.0f);
}

template<class T>
static bool testGetIntersectionWithLine(core::triangle3d<T>& triangle, const core::line3d<T>& ray)
{
	bool allExpected=true;
	const vector3d<T> linevect = ray.getVector().normalize();
	vector3d<T> intersection;
	for (u32 i=0; i<100; ++i)
	{
		if (!triangle.getIntersectionOfPlaneWithLine(ray.start, linevect, intersection))
		{
			allExpected=false;
			logTestString("triangle3d plane test %d failed\n", i);
		}
		if (!triangle.isPointInsideFast(intersection))
		{
			allExpected=false;
			logTestString("triangle3d fast point test %d failed\n", i);
		}
		if (!triangle.isPointInside(intersection))
		{
			allExpected=false;
			logTestString("triangle3d point test %d failed\n", i);
		        if (!isOnSameSide(intersection, triangle.pointA, triangle.pointB, triangle.pointC))
				logTestString("triangle3d side1 test %d failed\n", i);
			if (!isOnSameSide(intersection, triangle.pointB, triangle.pointA, triangle.pointC))
				logTestString("triangle3d side2 test %d failed\n", i);
			if (!isOnSameSide(intersection, triangle.pointC, triangle.pointA, triangle.pointB))
				logTestString("triangle3d side3 test %d failed\n", i);
		}

		if (!triangle.getIntersectionWithLine(ray.start, linevect, intersection))
		{
			allExpected=false;
			logTestString("triangle3d tri test %d failed\n", i);
		}

		triangle.pointB.Y += 1;
	}
	return allExpected;
}

// modifying the same triangle in diverse ways get some more test-cases automatically
template<class T>
static bool stageModifications(int stage, triangle3d<T>& triangle)
{
	switch ( stage )
	{
		case 0:
			return true;
		case 1:
			swap(triangle.pointB, triangle.pointC);
			return true;
		case 2:
			swap(triangle.pointA, triangle.pointC);
			return true;
		case 3:
			triangle.pointA.Z += 1000;
			triangle.pointB.Z += 1000;
			triangle.pointC.Z += 1000;
			return true;
		case 4:
			swap(triangle.pointA.Y, triangle.pointA.Z);
			swap(triangle.pointB.Y, triangle.pointB.Z);
			swap(triangle.pointC.Y, triangle.pointC.Z);
			return true;
	}
	return false;
}

template<class T>
static void stageModifications(int stage, vector3d<T>& point)
{
	switch ( stage )
	{
		case 3:
			point.Z += 1000;
			break;
		case 4:
			swap(point.Y, point.Z);
			break;
	}
}

template<class T>
static bool isPointInside(triangle3d<T> triangleOrig, bool testIsInside, bool testIsInsideFast)
{
	bool allExpected=true;

	array< vector3d<T> > pointsInside;
	pointsInside.push_back( vector3d<T>(0,0,0) );
	pointsInside.push_back( (triangleOrig.pointA + triangleOrig.pointB + triangleOrig.pointC) / 3 );
	pointsInside.push_back( (triangleOrig.pointA + triangleOrig.pointB)/2 + vector3d<T>(0,1,0) );
	pointsInside.push_back( (triangleOrig.pointA + triangleOrig.pointC)/2 + vector3d<T>(1,0,0) );
	pointsInside.push_back( (triangleOrig.pointB + triangleOrig.pointC)/2 - vector3d<T>(1,0,0) );

	for (u32 stage=0; ; ++stage)
	{
		triangle3d<T> triangle = triangleOrig;
		if ( !stageModifications(stage, triangle) )
			break;

		for ( u32 i=0; i < pointsInside.size(); ++i )
		{
			vector3d<T> point = pointsInside[i];
			stageModifications(stage, point);

			if ( testIsInside )
			{
				allExpected &= triangle.isPointInside( point );
				if ( !allExpected )
				{
					logTestString("triangle3d::isPointInside pointsInside test failed in stage %d point %d\n", stage, i);
					return false;
				}
			}

			if ( testIsInsideFast )
			{
				allExpected &= triangle.isPointInsideFast( point );
				if ( !allExpected )
				{
					logTestString("triangle3d::isPointInsideFast pointsInside test failed in stage %d point %d\n", stage, i);
					return false;
				}
			}
		}
	}

	array< vector3d<T> > pointsOutside;
	pointsOutside.push_back( triangleOrig.pointA - vector3d<T>(1,0,0) );
	pointsOutside.push_back( triangleOrig.pointA - vector3d<T>(0,1,0) );
	pointsOutside.push_back( triangleOrig.pointB + vector3d<T>(1,0,0) );
	pointsOutside.push_back( triangleOrig.pointB - vector3d<T>(0,1,0) );
	pointsOutside.push_back( triangleOrig.pointC - vector3d<T>(1,0,0) );
	pointsOutside.push_back( triangleOrig.pointC + vector3d<T>(1,0,0) );
	pointsOutside.push_back( triangleOrig.pointC + vector3d<T>(0,1,0) );
	pointsOutside.push_back( (triangleOrig.pointA + triangleOrig.pointB)/2 - vector3d<T>(0,1,0) );
	pointsOutside.push_back( (triangleOrig.pointA + triangleOrig.pointC)/2 - vector3d<T>(1,0,0) );
	pointsOutside.push_back( (triangleOrig.pointB + triangleOrig.pointC)/2 + vector3d<T>(1,0,0) );

	for (u32 stage=0; ; ++stage)
	{
		triangle3d<T> triangle = triangleOrig;
		if ( !stageModifications(stage, triangle) )
			break;

		for ( u32 i=0; i < pointsOutside.size(); ++i )
		{
			vector3d<T> point = pointsOutside[i];
			stageModifications(stage, point);

			if ( testIsInside )
			{
				allExpected &= !triangle.isPointInside( point );
				if ( !allExpected )
				{
					logTestString("triangle3d::isPointInside pointsOutside test failed in stage %d point %d\n", stage, i);
					return false;
				}
			}

			if ( testIsInsideFast )
			{
				allExpected &= !triangle.isPointInsideFast( point );
				if ( !allExpected )
				{
					logTestString("triangle3d::isPointInsideFast pointsOutside test failed in stage %d point %d\n", stage, i);
					return false;
				}
			}
		}
	}

	array< vector3d<T> > pointsBorder;
	pointsBorder.push_back( triangleOrig.pointA );
	pointsBorder.push_back( triangleOrig.pointB );
	pointsBorder.push_back( triangleOrig.pointC );
	pointsBorder.push_back( (triangleOrig.pointA + triangleOrig.pointB)/2 );
	pointsBorder.push_back( (triangleOrig.pointA + triangleOrig.pointC)/2 );
	pointsBorder.push_back( (triangleOrig.pointB + triangleOrig.pointC)/2 );

	for (u32 stage=0; ; ++stage)
	{
		triangle3d<T> triangle = triangleOrig;
		if ( !stageModifications(stage, triangle) )
			break;

		for ( u32 i=0; i < pointsBorder.size(); ++i )
		{
			vector3d<T> point = pointsBorder[i];
			stageModifications(stage, point);

			if ( testIsInside )
			{
				allExpected &= triangle.isPointInside( point );
				if ( !allExpected )
				{
					logTestString("triangle3d::isPointInside pointsBorder test failed in stage %d point %d\n", stage, i);
					return false;
				}
			}

			if ( testIsInsideFast )
			{
				allExpected &= triangle.isPointInsideFast( point );
				if ( !allExpected )
				{
					logTestString("triangle3d::isPointInsideFast pointsBorder test failed in stage %d point %d\n", stage, i);
					return false;
				}
			}
		}
	}

	return allExpected;
}

// Checking behaviour when FPU is set to single precision mode.
// This is somewhat important as Direct3D does by default set the FPU into that mode.
static bool isPointInsideWithSinglePrecision()
{
#if defined(_MSC_VER) && (defined(_WIN32) || defined(WIN32))
	int original = _control87( 0, 0 );
	_control87(_PC_24, MCW_PC);	// single precision (double precision would be _PC_53)

	// Testcase just some example which popped up wwhic shows the difference between single precision and double precision
	irr::core::triangle3d<irr::f64> t;
	irr::core::vector3d<irr::f64> point;
	t.pointA.X = 3.7237894e+002f;
	t.pointA.Y = -1.0025123e+003f;
	t.pointA.Z = 0;
	t.pointB.X = 2.6698560e+002f;
	t.pointB.Y = -9.8957166e+002f;
	t.pointB.Z = 0;
	t.pointC.X = 2.6981503e+002f;
	t.pointC.Y = -9.3992731e+002f;
	t.pointC.Z = 0;

	point.X = 2.6981500e+002f;
	point.Y = -9.3992743e+002f;
	point.Z = 0;

	bool ok = !t.isPointInside( point );

	_control87(original, 0xfffff);	// restore

	return ok;
#else
	// TODO: Be free to try changing the fpu for other systems.
	// I think for MinGW it's still easy, but for Linux this probably also needs changed linker flags.
	return true;
#endif
}

// Test the functionality of triangle3d<T>
bool testTriangle3d(void)
{
	bool allExpected = true;

	/* TODO: disabled for now. I (aka CuteAlien) have by now an example which allows debugging
	   that problem easier and also found some workaround (which needs an interface change
	   and a behaviour change and won't get into 1.7 therefore).
	logTestString("Test getIntersectionWithLine with f32\n");
	{
		triangle3df triangle(
				vector3df(11300.000000f, 129.411758f, 200.000000f),
				vector3df(11200.000000f, 94.117645f, 300.000000f),
				vector3df(11300.000000f, 129.411758f, 300.000000f));
		line3df ray;
		ray.start = vector3df(11250.000000f, 329.000000f, 250.000000f);
		ray.end = vector3df(11250.000000, -1000.000000, 250.000000);
		allExpected &= testGetIntersectionWithLine(triangle, ray);
	}
	logTestString("Test getIntersectionWithLine with f64\n");
	{
		triangle3d<f64> triangle(
				vector3d<f64>(11300.000000f, 129.411758f, 200.000000f),
				vector3d<f64>(11200.000000f, 94.117645f, 300.000000f),
				vector3d<f64>(11300.000000f, 129.411758f, 300.000000f));
		line3d<f64> ray;
		ray.start = vector3d<f64>(11250.000000f, 329.000000f, 250.000000f);
		ray.end = vector3d<f64>(11250.000000, -1000.000000, 250.000000);
		allExpected &= testGetIntersectionWithLine(triangle, ray);
	}
	*/

	/* For now we have no solution yet to fix isPointInside for large integers without
	getting worse floating-point precision at the same time.
	So instead isPointInsideFast got fixed and should be used for int's.
	bool testEigen = triangle3di(vector3di(250, 0, 0), vector3di(0, 0, 500), vector3di(500, 0, 500)).isPointInside(vector3di(300,0,300));
	if ( !testEigen )	// test from Eigen from here: http://irrlicht.sourceforge.net/forum/viewtopic.php?f=7&t=44372&p=254331#p254331
		logTestString("Test isPointInside fails with integers\n");
	allExpected &= testEigen;
	*/

	logTestString("Test isPointInside with f32\n");
	{
		triangle3d<f32> t(vector3d<f32>(-1000,-1000,0), vector3d<f32>(1000,-1000,0), vector3d<f32>(0,1000,0));
		allExpected &= isPointInside(t, true, true);
	}

	logTestString("Test isPointInside with f64\n");
	{
		triangle3d<f64> t(vector3d<f64>(-1000,-1000,0), vector3d<f64>(1000,-1000,0), vector3d<f64>(0,1000,0));
		allExpected &= isPointInside(t, true, true);
	}

	logTestString("Test isPointInside with s32\n");
	{
		triangle3d<s32> t(vector3d<s32>(-1000,-1000,0), vector3d<s32>(1000,-1000,0), vector3d<s32>(0,1000,0));
		allExpected &= isPointInside(t, false, true);
	}

	logTestString("Test isPointInsideWithSinglePrecision\n");
	{
		allExpected &= isPointInsideWithSinglePrecision();
	}

	if(allExpected)
		logTestString("\nAll tests passed\n");
	else
		logTestString("\nFAIL!\n");

	return allExpected;
}

