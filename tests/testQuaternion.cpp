// Copyright (C) 2008-2011 Colin MacDonald
// No rights reserved: this software is in the public domain.

#include "testUtils.h"

using namespace irr;

bool testQuaternion(void)
{
	bool result = true;

	core::quaternion q1;
	if ((q1.W != 1.f)||(q1.X != 0.f)||(q1.Y != 0.f)||(q1.Z != 0.f))
	{
		logTestString("Default constructor did not create proper quaternion.\n");
		result = false;
	}

	core::quaternion q2(1.f,2.f,3.f,4.f);
	if ((q2.W != 4.f)||(q2.X != 1.f)||(q2.Y != 2.f)||(q2.Z != 3.f))
	{
		logTestString("Element constructor did not create proper quaternion.\n");
		result = false;
	}

	q2.set(0.f,0.f,0.f,1.f);
	if ((q1.W != 1.f)||(q1.X != 0.f)||(q1.Y != 0.f)||(q1.Z != 0.f))
	{
		logTestString("Quaternion set method not working.\n");
		result = false;
	}
	if (q1 != q2)
	{
		logTestString("Quaternion equals method not working.\n");
		result = false;
	}

	core::quaternion q3(1.f,2.f,3.f);
	core::matrix4 mat;
	core::quaternion q4(mat);


	q4.rotationFromTo(core::vector3df(1.f,0.f,0.f), core::vector3df(1.f,0.f,0.f));
	if (q4 != q1)
	{
		logTestString("Quaternion rotationFromTo method did not yield identity.\n");
		result = false;
	}

	q1.set(0.f,0.f,core::PI);
	q2.set(0.f,core::PI,0.f);
	q4.rotationFromTo(core::vector3df(1.f,0.f,0.f), core::vector3df(-1.f,0.f,0.f));
	if ((q4 != q1)&&(q4 != q2))
	{
		logTestString("Quaternion rotationFromTo method did not yield x flip.\n");
		result = false;
	}
	q4.rotationFromTo(core::vector3df(10.f,20.f,30.f), core::vector3df(-10.f,-20.f,-30.f));
	if ((q4 != q1)&&(q4 != q2))
	{
		logTestString("Quaternion rotationFromTo method did not yield x flip for non-axis.\n");
		result = false;
	}

	q1.set(0.f,0.f,core::PI/2);
	q4.rotationFromTo(core::vector3df(1.f,0.f,0.f), core::vector3df(0.f,1.f,0.f));
	if (!q4.equals(q1))
	{
		logTestString("Quaternion rotationFromTo method did not yield 90 degree rotation.\n");
		result = false;
	}

	return result;
}
