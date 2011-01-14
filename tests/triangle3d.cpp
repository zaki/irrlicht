// Copyright (C) 2008-2011 Colin MacDonald
// No rights reserved: this software is in the public domain.

#include "testUtils.h"
#include <irrlicht.h>
#include <assert.h>

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


// Test the functionality of triangle3d<T>
/** Validation is done with asserts() against expected results. */
bool testTriangle3d(void)
{
	bool allExpected = true;

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

	if(allExpected)
		logTestString("\nAll tests passed\n");
	else
		logTestString("\nFAIL!\n");

	return allExpected;
}

