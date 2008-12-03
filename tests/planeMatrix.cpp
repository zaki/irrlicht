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

// There's all sorts of minor and inevitable FP accuracy errors here, so use a sloppy comparison.
static bool sloppyComparePlanes(const plane3df plane1, const plane3df plane2)
{
	return(equals(plane1.D, plane2.D, 0.001f) &&
		equals(plane1.Normal.X, plane2.Normal.X, 0.001f) &&
		equals(plane1.Normal.Y, plane2.Normal.Y, 0.001f) &&
		equals(plane1.Normal.Z, plane2.Normal.Z, 0.001f));
}

static bool transformPlane(const vector3df & point, const vector3df & normal,
						   const matrix4 & matrix, const plane3df & expected)
{
	plane3df plane(point, vector3df(normal).normalize());

	logTestString("\n     Pre: (%.3ff,%.3ff,%.3ff), %.3ff\n", 
		plane.Normal.X, plane.Normal.Y, plane.Normal.Z, plane.D);

	matrix.transformPlane(plane);

	logTestString("    Post: (%.3ff,%.3ff,%.3ff), %.3ff\n", 
		plane.Normal.X, plane.Normal.Y, plane.Normal.Z, plane.D);

	logTestString("Expected: (%.3ff,%.3ff,%.3ff), %.3ff\n", 
		expected.Normal.X, expected.Normal.Y, expected.Normal.Z, expected.D);

	assert(sloppyComparePlanes(plane, expected));
	if(!sloppyComparePlanes(plane, expected))
	{
		logTestString("Unexpected result\n");
		return false;
	}

	return true;
}

// Test the ability to transform a plane with a matrix.
bool planeMatrix(void)
{
	matrix4 rotationMatrix;
	rotationMatrix.setRotationDegrees(vector3df(90, 0, 0));

	matrix4 translationMatrix;
	translationMatrix.setTranslation(vector3df(0, 3, 0));

	matrix4 scaleMatrix;
	scaleMatrix.setScale(vector3df(1, 2, 3));

	bool success = true;

	matrix4 matrix = rotationMatrix;
	logTestString("\nRotation matrix\n%02.02f %02.02f %02.02f %02.02f"
		"\n%02.02f %02.02f %02.02f %02.02f"
		"\n%02.02f %02.02f %02.02f %02.02f"
		"\n%02.02f %02.02f %02.02f %02.02f\n",
		matrix[0], matrix[1], matrix[2], matrix[3], 
		matrix[4], matrix[5], matrix[6], matrix[7], 
		matrix[8], matrix[9], matrix[10], matrix[11], 
		matrix[12], matrix[13], matrix[14], matrix[15]);
	success &= transformPlane(vector3df(0, 0, 0), vector3df(-1, -1, 0), matrix, plane3df(vector3df(-.707f, 0.f, -.707f), 0.f));
	success &= transformPlane(vector3df(0, 0, 0), vector3df(1, 1, 0), matrix, plane3df(vector3df(.707f, 0.f, .707f), 0.f));
	success &= transformPlane(vector3df(0, 0, 0), vector3df(-1, 1, 0), matrix, plane3df(vector3df(-.707f, 0.f, .707f), 0.f));
	success &= transformPlane(vector3df(0, 0, 0), vector3df(1, -1, 0), matrix, plane3df(vector3df(.707f, 0.f, -.707f), 0.f));
	success &= transformPlane(vector3df(0, 1, 0), vector3df(-1, -1, 0), matrix, plane3df(vector3df(-.707f, 0.f, -.707f), .707f));
	success &= transformPlane(vector3df(0, 1, 0), vector3df(1, 1, 0), matrix, plane3df(vector3df(.707f, 0.f, .707f), -.707f));
	success &= transformPlane(vector3df(0, 1, 0), vector3df(-1, 1, 0), matrix, plane3df(vector3df(-.707f, 0.f, .707f), -.707f));
	success &= transformPlane(vector3df(0, 1, 0), vector3df(1, -1, 0), matrix, plane3df(vector3df(.707f, 0.f, -.707f), .707f));


	matrix = translationMatrix;
	logTestString("\nTranslation matrix\n%02.02f %02.02f %02.02f %02.02f"
		"\n%02.02f %02.02f %02.02f %02.02f"
		"\n%02.02f %02.02f %02.02f %02.02f"
		"\n%02.02f %02.02f %02.02f %02.02f\n",
		matrix[0], matrix[1], matrix[2], matrix[3], 
		matrix[4], matrix[5], matrix[6], matrix[7], 
		matrix[8], matrix[9], matrix[10], matrix[11], 
		matrix[12], matrix[13], matrix[14], matrix[15]);
	success &= transformPlane(vector3df(0, 0, 0), vector3df(-1, -1, 0), matrix, plane3df(vector3df(-0.707f,-0.707f,0.000f), 2.121f));
	success &= transformPlane(vector3df(0, 0, 0), vector3df(1, 1, 0), matrix, plane3df(vector3df(0.707f,0.707f,0.000f), -2.121f));
	success &= transformPlane(vector3df(0, 0, 0), vector3df(-1, 1, 0), matrix, plane3df(vector3df(-0.707f,0.707f,0.000f), -2.121f));
	success &= transformPlane(vector3df(0, 0, 0), vector3df(1, -1, 0), matrix, plane3df(vector3df(0.707f,-0.707f,0.000f), 2.121f));
	success &= transformPlane(vector3df(0, 1, 0), vector3df(-1, -1, 0), matrix, plane3df(vector3df(-0.707f,-0.707f,0.000f), 2.828f));
	success &= transformPlane(vector3df(0, 1, 0), vector3df(1, 1, 0), matrix, plane3df(vector3df(0.707f,0.707f,0.000f), -2.828f));
	success &= transformPlane(vector3df(0, 1, 0), vector3df(-1, 1, 0), matrix, plane3df(vector3df(-0.707f,0.707f,0.000f), -2.828f));
	success &= transformPlane(vector3df(0, 1, 0), vector3df(1, -1, 0), matrix, plane3df(vector3df(0.707f,-0.707f,0.000f), 2.828f));


	matrix = scaleMatrix;
	logTestString("\nScale matrix\n%02.02f %02.02f %02.02f %02.02f"
		"\n%02.02f %02.02f %02.02f %02.02f"
		"\n%02.02f %02.02f %02.02f %02.02f"
		"\n%02.02f %02.02f %02.02f %02.02f\n",
		matrix[0], matrix[1], matrix[2], matrix[3], 
		matrix[4], matrix[5], matrix[6], matrix[7], 
		matrix[8], matrix[9], matrix[10], matrix[11], 
		matrix[12], matrix[13], matrix[14], matrix[15]);
	success &= transformPlane(vector3df(0, 0, 0), vector3df(-1, -1, 0), matrix, plane3df(vector3df(-0.894f,-0.447f,0.000f), -0.000f));
	success &= transformPlane(vector3df(0, 0, 0), vector3df(1, 1, 0), matrix, plane3df(vector3df(0.894f,0.447f,0.000f), -0.000f));
	success &= transformPlane(vector3df(0, 0, 0), vector3df(-1, 1, 0), matrix, plane3df(vector3df(-0.894f,0.447f,0.000f), -0.000f));
	success &= transformPlane(vector3df(0, 0, 0), vector3df(1, -1, 0), matrix, plane3df(vector3df(0.894f,-0.447f,0.000f), -0.000f));
	success &= transformPlane(vector3df(0, 1, 0), vector3df(-1, -1, 0), matrix, plane3df(vector3df(-0.894f,-0.447f,0.000f), 0.894f));
	success &= transformPlane(vector3df(0, 1, 0), vector3df(1, 1, 0), matrix, plane3df(vector3df(0.894f,0.447f,0.000f), -0.894f));
	success &= transformPlane(vector3df(0, 1, 0), vector3df(-1, 1, 0), matrix, plane3df(vector3df(-0.894f,0.447f,0.000f), -0.894f));
	success &= transformPlane(vector3df(0, 1, 0), vector3df(1, -1, 0), matrix, plane3df(vector3df(0.894f,-0.447f,0.000f), 0.894f));
	
	matrix = rotationMatrix * translationMatrix;
	logTestString("\nRotation * translation matrix\n%02.02f %02.02f %02.02f %02.02f"
		"\n%02.02f %02.02f %02.02f %02.02f"
		"\n%02.02f %02.02f %02.02f %02.02f"
		"\n%02.02f %02.02f %02.02f %02.02f\n",
		matrix[0], matrix[1], matrix[2], matrix[3], 
		matrix[4], matrix[5], matrix[6], matrix[7], 
		matrix[8], matrix[9], matrix[10], matrix[11], 
		matrix[12], matrix[13], matrix[14], matrix[15]);
	success &= transformPlane(vector3df(0, 0, 0), vector3df(-1, -1, 0), matrix, plane3df(vector3df(-0.707f,0.000f,-0.707f), 2.121f));
	success &= transformPlane(vector3df(0, 0, 0), vector3df(1, 1, 0), matrix, plane3df(vector3df(0.707f,-0.000f,0.707f), -2.121f));
	success &= transformPlane(vector3df(0, 0, 0), vector3df(-1, 1, 0), matrix, plane3df(vector3df(-0.707f,-0.000f,0.707f), -2.121f));
	success &= transformPlane(vector3df(0, 0, 0), vector3df(1, -1, 0), matrix, plane3df(vector3df(0.707f,0.000f,-0.707f), 2.121f));
	success &= transformPlane(vector3df(0, 1, 0), vector3df(-1, -1, 0), matrix, plane3df(vector3df(-0.707f,0.000f,-0.707f), 2.828f));
	success &= transformPlane(vector3df(0, 1, 0), vector3df(1, 1, 0), matrix, plane3df(vector3df(0.707f,-0.000f,0.707f), -2.828f));
	success &= transformPlane(vector3df(0, 1, 0), vector3df(-1, 1, 0), matrix, plane3df(vector3df(-0.707f,-0.000f,0.707f), -2.828f));
	success &= transformPlane(vector3df(0, 1, 0), vector3df(1, -1, 0), matrix, plane3df(vector3df(0.707f,0.000f,-0.707f), 2.828f));

	matrix = rotationMatrix * scaleMatrix;
	logTestString("\nRotation * scale matrix\n%02.02f %02.02f %02.02f %02.02f"
		"\n%02.02f %02.02f %02.02f %02.02f"
		"\n%02.02f %02.02f %02.02f %02.02f"
		"\n%02.02f %02.02f %02.02f %02.02f\n",
		matrix[0], matrix[1], matrix[2], matrix[3], 
		matrix[4], matrix[5], matrix[6], matrix[7], 
		matrix[8], matrix[9], matrix[10], matrix[11], 
		matrix[12], matrix[13], matrix[14], matrix[15]);
	success &= transformPlane(vector3df(0, 0, 0), vector3df(-1, -1, 0), matrix, plane3df(vector3df(-0.447f,0.000f,-0.894f), -0.000f));
	success &= transformPlane(vector3df(0, 0, 0), vector3df(1, 1, 0), matrix, plane3df(vector3df(0.447f,-0.000f,0.894f), -0.000f));
	success &= transformPlane(vector3df(0, 0, 0), vector3df(-1, 1, 0), matrix, plane3df(vector3df(-0.447f,-0.000f,0.894f), -0.000f));
	success &= transformPlane(vector3df(0, 0, 0), vector3df(1, -1, 0), matrix, plane3df(vector3df(0.447f,0.000f,-0.894f), -0.000f));
	success &= transformPlane(vector3df(0, 1, 0), vector3df(-1, -1, 0), matrix, plane3df(vector3df(-0.447f,0.000f,-0.894f), 1.118f));
	success &= transformPlane(vector3df(0, 1, 0), vector3df(1, 1, 0), matrix, plane3df(vector3df(0.447f,-0.000f,0.894f), -1.118f));
	success &= transformPlane(vector3df(0, 1, 0), vector3df(-1, 1, 0), matrix, plane3df(vector3df(-0.447f,-0.000f,0.894f), -1.118f));
	success &= transformPlane(vector3df(0, 1, 0), vector3df(1, -1, 0), matrix, plane3df(vector3df(0.447f,0.000f,-0.894f), 1.118f));

	matrix = translationMatrix * scaleMatrix;
	logTestString("\nTranslation * scale matrix\n%02.02f %02.02f %02.02f %02.02f"
		"\n%02.02f %02.02f %02.02f %02.02f"
		"\n%02.02f %02.02f %02.02f %02.02f"
		"\n%02.02f %02.02f %02.02f %02.02f\n",
		matrix[0], matrix[1], matrix[2], matrix[3], 
		matrix[4], matrix[5], matrix[6], matrix[7], 
		matrix[8], matrix[9], matrix[10], matrix[11], 
		matrix[12], matrix[13], matrix[14], matrix[15]);
	success &= transformPlane(vector3df(0, 0, 0), vector3df(-1, -1, 0), matrix, plane3df(vector3df(-0.894f,-0.447f,0.000f), 1.342f));
	success &= transformPlane(vector3df(0, 0, 0), vector3df(1, 1, 0), matrix, plane3df(vector3df(0.894f,0.447f,0.000f), -1.342f));
	success &= transformPlane(vector3df(0, 0, 0), vector3df(-1, 1, 0), matrix, plane3df(vector3df(-0.894f,0.447f,0.000f), -1.342f));
	success &= transformPlane(vector3df(0, 0, 0), vector3df(1, -1, 0), matrix, plane3df(vector3df(0.894f,-0.447f,0.000f), 1.342f));
	success &= transformPlane(vector3df(0, 1, 0), vector3df(-1, -1, 0), matrix, plane3df(vector3df(-0.894f,-0.447f,0.000f), 2.236f));
	success &= transformPlane(vector3df(0, 1, 0), vector3df(1, 1, 0), matrix, plane3df(vector3df(0.894f,0.447f,0.000f), -2.236f));
	success &= transformPlane(vector3df(0, 1, 0), vector3df(-1, 1, 0), matrix, plane3df(vector3df(-0.894f,0.447f,0.000f), -2.236f));
	success &= transformPlane(vector3df(0, 1, 0), vector3df(1, -1, 0), matrix, plane3df(vector3df(0.894f,-0.447f,0.000f), 2.236f));

	matrix = rotationMatrix * translationMatrix * scaleMatrix;
	logTestString("\nRotation * translation * scale matrix\n%02.02f %02.02f %02.02f %02.02f"
		"\n%02.02f %02.02f %02.02f %02.02f"
		"\n%02.02f %02.02f %02.02f %02.02f"
		"\n%02.02f %02.02f %02.02f %02.02f\n",
		matrix[0], matrix[1], matrix[2], matrix[3], 
		matrix[4], matrix[5], matrix[6], matrix[7], 
		matrix[8], matrix[9], matrix[10], matrix[11], 
		matrix[12], matrix[13], matrix[14], matrix[15]);
	success &= transformPlane(vector3df(0, 0, 0), vector3df(-1, -1, 0), matrix, plane3df(vector3df(-0.447f,0.000f,-0.894f), 2.683f));
	success &= transformPlane(vector3df(0, 0, 0), vector3df(1, 1, 0), matrix, plane3df(vector3df(0.447f,-0.000f,0.894f), -2.683f));
	success &= transformPlane(vector3df(0, 0, 0), vector3df(-1, 1, 0), matrix, plane3df(vector3df(-0.447f,-0.000f,0.894f), -2.683f));
	success &= transformPlane(vector3df(0, 0, 0), vector3df(1, -1, 0), matrix, plane3df(vector3df(0.447f,0.000f,-0.894f), 2.683f));
	success &= transformPlane(vector3df(0, 1, 0), vector3df(-1, -1, 0), matrix, plane3df(vector3df(-0.447f,0.000f,-0.894f), 3.801f));
	success &= transformPlane(vector3df(0, 1, 0), vector3df(1, 1, 0), matrix, plane3df(vector3df(0.447f,-0.000f,0.894f), -3.801f));
	success &= transformPlane(vector3df(0, 1, 0), vector3df(-1, 1, 0), matrix, plane3df(vector3df(-0.447f,-0.000f,0.894f), -3.801f));
	success &= transformPlane(vector3df(0, 1, 0), vector3df(1, -1, 0), matrix, plane3df(vector3df(0.447f,0.000f,-0.894f), 3.801f));

	return success;
}

