// Copyright (C) 2008-2011 Colin MacDonald
// No rights reserved: this software is in the public domain.

#include "testUtils.h"

using namespace irr;

namespace
{
inline bool compareQ(const core::vector3df& v, const core::vector3df& turn=core::vector3df(0,0,1))
{
	core::quaternion q(v*core::DEGTORAD);
	core::vector3df v2;

	const core::vector3df v3=v.rotationToDirection(turn);
	if (!v3.equals(q*turn, 0.002f))
	{
		logTestString("Inequality before quat.toEuler(): %f,%f,%f\n", v.X,v.Y,v.Z);
		return false;
	}
	
	q.toEuler(v2);
	v2*=core::RADTODEG;
	v2=v2.rotationToDirection(turn);

	// this yields pretty far values sometimes, so don't be too picky
	if (!v3.equals(v2, 0.0035f))
	{
		logTestString("Inequality: %f,%f,%f != %f,%f,%f\n", v.X,v.Y,v.Z, v2.X,v2.Y,v2.Z);
		return false;
	}
	return true;
}

core::vector3df vals[] = {
	core::vector3df(0.f, 0.f, 0.f),
	core::vector3df(0.f, 0.f, 24.04f),
	core::vector3df(0.f, 0.f, 71.f),
	core::vector3df(0.f, 0.f, 71.19f),
	core::vector3df(0.f, 0.f, 80.f),
	core::vector3df(0.f, 0.f, 103.99f),
	core::vector3df(0.f, 0.f, 261.73f),
	core::vector3df(0.f, 0.f, 276.f),
	core::vector3df(0.f, 0.f, 286.29f),
	core::vector3df(0.f, 0.f, 295.f),
	core::vector3df(0.f, 0.f, 318.3f),
	core::vector3df(360.f, 75.55f, 155.89f),
	core::vector3df(0.f, 90.f, 159.51f),
	core::vector3df(0.f, 90.f, 249.48f),
	core::vector3df(0.f, 90.f, 269.91f),
	core::vector3df(0.f, 90.f, 270.f),
	core::vector3df(0.f, 284.45f, 155.89f),
	core::vector3df(0.01f, 0.42f, 90.38f),
	core::vector3df(0.04f, 359.99f, 9.5f),
	core::vector3df(0.34f, 89.58f, 360.f),
	core::vector3df(0.58f, 4.36f, 334.36f),
	core::vector3df(3.23f, 359.65f, 10.17f),
	core::vector3df(3.23f, 359.65f, 10.21f),
	core::vector3df(4.85f, 359.3f, 94.33f),
	core::vector3df(8.90f, 6.63f, 9.27f),
	core::vector3df(11.64f, 311.52f, 345.35f),
	core::vector3df(12.1f, 4.72f, 11.24f),
	core::vector3df(14.63f, 48.72f, 31.79f),
	core::vector3df(76.68f, 1.11f, 18.65f),
	core::vector3df(90.f, 0.f, 0.f),
	core::vector3df(90.01f, 270.49f, 360.f),
	core::vector3df(90.95f, 0.f, 0.f),
	core::vector3df(173.58f, 348.13f, 132.25f),
	core::vector3df(115.52f, 89.04f, 205.51f),
	core::vector3df(179.3f, 359.18f, 0.58f),
	core::vector3df(180.09f, 270.06f, 0.f),
	core::vector3df(180.41f, 359.94f, 179.69f),
	core::vector3df(180.92f, 10.79f, 144.53f),
	core::vector3df(181.95f, 270.03f, 0.f),
	core::vector3df(269.05f, 0.f, 0.f),
	core::vector3df(269.99f, 270.49f, 360.f),
	core::vector3df(283.32f, 358.89f, 18.65f),
	core::vector3df(347.9f, 355.28f, 11.24f),
	core::vector3df(351.1f, 353.37f, 9.27f),
	core::vector3df(355.82f, 345.96f, 273.26f),
	core::vector3df(358.24f, 358.07f, 342.82f),
	core::vector3df(359.78f, 357.69f, 7.52f),
	core::vector3df(359.96f, 0.01f, 9.5f),
	core::vector3df(-57.197479f,-90.f,0.f),
	core::vector3df(-57.187481f,-90.f,0.f)
};

bool testEulerConversion()
{
	bool result = true;
	for (u32 i=0; i<sizeof(vals)/sizeof(vals[0]); ++i)
	{
		// make sure the rotations work with different turn vectors
		result &= compareQ(vals[i]) && compareQ(vals[i], core::vector3df(1,2,3)) &&
			compareQ(vals[i], core::vector3df(0,1,0));
	}
	return result;
}

bool testRotationFromTo()
{
	bool result = true;
	core::quaternion q1;
	core::matrix4 mat;
	core::quaternion q4(mat);

	q4.rotationFromTo(core::vector3df(1.f,0.f,0.f), core::vector3df(1.f,0.f,0.f));
	if (q4 != q1)
	{
		logTestString("Quaternion rotationFromTo method did not yield identity.\n");
		result = false;
	}

	q1.set(0.f,0.f,core::PI);
	core::quaternion q2(0.f,core::PI,0.f);
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

bool testInterpolation()
{
	bool result=true;
	core::quaternion q(1.f,2.f,3.f,4.f);
	core::quaternion q2;
	q2.lerp(q,q,0);
	if (q != q2)
	{
		logTestString("Quaternion lerp with same quaternion did not yield same quaternion back (with t==0).\n");
		result = false;
	}
	q2.lerp(q,q,0.5f);
	if (q != q2)
	{
		logTestString("Quaternion lerp with same quaternion did not yield same quaternion back (with t==0.5).\n");
		result = false;
	}
	q2.lerp(q,q,1);
	if (q != q2)
	{
		logTestString("Quaternion lerp with same quaternion did not yield same quaternion back (with t==1).\n");
		result = false;
	}
	q2.lerp(q,q,0.2345f);
	if (q != q2)
	{
		logTestString("Quaternion lerp with same quaternion did not yield same quaternion back (with t==0.2345).\n");
		result = false;
	}
	q2.slerp(q,q,0);
	if (q != q2)
	{
		logTestString("Quaternion slerp with same quaternion did not yield same quaternion back (with t==0).\n");
		result = false;
	}
	q2.slerp(q,q,0.5f);
	if (q != q2)
	{
		logTestString("Quaternion slerp with same quaternion did not yield same quaternion back (with t==0.5).\n");
		result = false;
	}
	q2.slerp(q,q,1);
	if (q != q2)
	{
		logTestString("Quaternion slerp with same quaternion did not yield same quaternion back (with t==1).\n");
		result = false;
	}
	q2.slerp(q,q,0.2345f);
	if (q != q2)
	{
		logTestString("Quaternion slerp with same quaternion did not yield same quaternion back (with t==0.2345).\n");
		result = false;
	}
	core::quaternion q3(core::vector3df(45,135,85)*core::DEGTORAD);
	q.set(core::vector3df(35,125,75)*core::DEGTORAD);
	q2.slerp(q,q3,0);
	if (q != q2)
	{
		logTestString("Quaternion slerp with different quaternions did not yield first quaternion back (with t==0).\n");
		result = false;
	}
	q2.slerp(q,q3,1);
	if (q3 != q2)
	{
		logTestString("Quaternion slerp with different quaternions did not yield second quaternion back (with t==1).\n");
		result = false;
	}
	q2.slerp(q,q3,0.5);
	if (!q2.equals(core::quaternion(-0.437f,0.742f,0.017f,0.506f),0.001f))
	{
		logTestString("Quaternion slerp with different quaternions did not yield correct result (with t==0.5).\n");
		result = false;
	}
	q2.slerp(q,q3,0.2345f);
	if (!q2.equals(core::quaternion(-0.4202f,0.7499f,0.03814f,0.5093f),0.0007f))
	{
		logTestString("Quaternion slerp with different quaternions did not yield correct result (with t==0.2345).\n");
		result = false;
	}
	return result;
}
}

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

	q2.set(4.f,3.f,2.f,1.f);
	if ((q2.W != 1.f)||(q2.X != 4.f)||(q2.Y != 3.f)||(q2.Z != 2.f))
	{
		logTestString("Quaternion set method not working(1).\n");
		result = false;
	}
	q2.set(0.f,0.f,0.f,1.f);
	if ((q2.W != 1.f)||(q2.X != 0.f)||(q2.Y != 0.f)||(q2.Z != 0.f))
	{
		logTestString("Quaternion set method not working(2).\n");
		result = false;
	}
	if (q1 != q2)
	{
		logTestString("Quaternion equals method not working.\n");
		result = false;
	}

	result &= testRotationFromTo();
	result &= testInterpolation();
	result &= testEulerConversion();

	return result;
}
