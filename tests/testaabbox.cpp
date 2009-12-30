// Copyright (C) 2008-2009 Colin MacDonald
// No rights reserved: this software is in the public domain.

#include "testUtils.h"

using namespace irr;
using namespace core;

template <class T>
static bool doTests()
{
	aabbox3d<T> empty;
	aabbox3d<T> one(-1,-1,-1,1,1,1);
	if (empty != one)
	{
		logTestString("default aabbox3d<T> wrong, or comparison failed\n");
		return false;
	}
	if (empty.getCenter() != core::vector3d<T>(0,0,0))
	{
		logTestString("default aabbox3d<T> has wrong Center\n");
		return false;
	}
	if (empty.getExtent() != core::vector3d<T>(2,2,2))
	{
		logTestString("default aabbox3d<T> has wrong Extent\n");
		return false;
	}
	if (empty.isEmpty())
	{
		logTestString("default aabbox3d<T> is empty\n");
		return false;
	}
	aabbox3d<T> two(core::vector3d<T>(-1,-1,-1),core::vector3d<T>(2,2,2));
	if (empty == two)
	{
		logTestString("empty aabbox3d<T> too large, or comparison failed\n");
		return false;
	}
	if (two.getCenter() != core::vector3d<T>((T)0.5,(T)0.5,(T)0.5))
	{
		logTestString("extended aabbox3d<T> has wrong Center\n");
		return false;
	}
	if (two.getExtent() != core::vector3d<T>(3,3,3))
	{
		logTestString("extended aabbox3d<T> has wrong Extent\n");
		return false;
	}
	if (two.isEmpty())
	{
		logTestString("extended aabbox3d<T> is empty\n");
		return false;
	}
	one.reset(1,1,1);
	if (one==empty)
	{
		logTestString("reset failed, or comparison failed\n");
		return false;
	}
	if (one.getCenter() != core::vector3d<T>(1,1,1))
	{
		logTestString("singular aabbox3d<T> has wrong Center\n");
		return false;
	}
	if (one.getExtent() != core::vector3d<T>(0,0,0))
	{
		logTestString("singular aabbox3d<T> has Extent\n");
		return false;
	}
	if (!one.isEmpty())
	{
		logTestString("empty aabbox3d<T> is not empty\n");
		return false;
	}
	one.addInternalPoint(core::vector3d<T>(-1,-1,-1));
	if (one!=empty)
	{
		logTestString("addInternalPoint failed, creating default bbox\n");
		return false;
	}
	one.reset(1,1,1);
	one.reset(empty);
	if (one!=empty)
	{
		logTestString("reset with bbox failed, creating default bbox\n");
		return false;
	}
	one.addInternalPoint(core::vector3d<T>(2,2,2));
	if (one != two)
	{
		logTestString("addInternalPoint for aabbox3d<T> failed.\n");
		return false;
	}
	one.addInternalBox(empty);
	if (one != two)
	{
		logTestString("addInternalBox with smaller box failed.\n");
		return false;
	}
	one.addInternalBox(two);
	if (one != two)
	{
		logTestString("addInternalBox with same box failed.\n");
		return false;
	}
	one.addInternalPoint(-1,-2,-3);
	two.addInternalPoint(-1,-2,-3);
	empty.addInternalBox(one);
	if (empty != two)
	{
		logTestString("addInternalBox with larger box failed\n");
		return false;
	}
	if (!checkCollisions<T>())
		return false;
	return true;
}

template<class T>
static bool checkCollisions()
{
	aabbox3d<T> one(-1,-2,-3,2,2,2);

	if (!one.isPointInside(core::vector3d<T>(-1,-2,-3)))
	{
		logTestString("isPointInside failed with min vertex\n");
		return false;
	}
	if (!one.isPointInside(core::vector3d<T>(-1,2,-3)))
	{
		logTestString("isPointInside failed with other min vertex\n");
		return false;
	}
	if (!one.isPointInside(core::vector3d<T>(2,-2,2)))
	{
		logTestString("isPointInside failed with other max vertex\n");
		return false;
	}
	if (!one.isPointInside(core::vector3d<T>(2,2,2)))
	{
		logTestString("isPointInside failed with max vertex\n");
		return false;
	}
	if (!one.isPointInside(core::vector3d<T>(0,0,0)))
	{
		logTestString("isPointInside failed with origin\n");
		return false;
	}
	if (!one.isPointInside(core::vector3d<T>((T)1.2,-1,1)))
	{
		logTestString("isPointInside failed with random point inside\n");
		return false;
	}
	if (one.isPointInside(core::vector3d<T>(-2,-2,-3)))
	{
		logTestString("isPointInside failed near min vertex\n");
		return false;
	}
	if (one.isPointInside(core::vector3d<T>(2,3,2)))
	{
		logTestString("isPointInside failed near max vertex\n");
		return false;
	}
	if (one.isPointInside(core::vector3d<T>(3,0,0)))
	{
		logTestString("isPointInside failed near origin\n");
		return false;
	}
	if (one.isPointInside(core::vector3d<T>((T)10.2,-1,1)))
	{
		logTestString("isPointInside failed with random point outside\n");
		return false;
	}
	if (one.isPointTotalInside(core::vector3d<T>(-1,-2,-3)))
	{
		logTestString("isPointTotalInside failed with min vertex\n");
		return false;
	}
	if (one.isPointTotalInside(core::vector3d<T>(-1,2,-3)))
	{
		logTestString("isPointTotalInside failed with other min vertex\n");
		return false;
	}
	if (one.isPointTotalInside(core::vector3d<T>(2,-2,2)))
	{
		logTestString("isPointTotalInside failed with other max vertex\n");
		return false;
	}
	if (one.isPointTotalInside(core::vector3d<T>(2,2,2)))
	{
		logTestString("isPointTotalInside failed with max vertex\n");
		return false;
	}
	if (!one.isPointTotalInside(core::vector3d<T>(0,0,0)))
	{
		logTestString("isPointTotalInside failed with origin\n");
		return false;
	}
	if (!one.isPointTotalInside(core::vector3d<T>((T)1.2,-1,1)))
	{
		logTestString("isPointTotalInside failed with random point inside\n");
		return false;
	}
	if (one.isPointTotalInside(core::vector3d<T>((T)10.2,-1,1)))
	{
		logTestString("isPointTotalInside failed with random point outside\n");
		return false;
	}
	return true;
}

/** Test the functionality of aabbox3d<T>. */
bool testaabbox3d(void)
{
	bool f32Success = doTests<f32>();
	if(f32Success)
		logTestString("aabbox3d<f32> tests passed\n\n");
	else
		logTestString("\n*** aabbox3d<f32> tests failed ***\n\n");

	bool f64Success = doTests<f64>();
	if(f64Success)
		logTestString("aabbox3d<f64> tests passed\n\n");
	else
		logTestString("\n*** aabbox3d<f64> tests failed ***\n\n");

	bool s32Success = doTests<s32>();
	if(s32Success)
		logTestString("aabbox3d<s32> tests passed\n\n");
	else
		logTestString("\n*** aabbox3d<s32> tests failed ***\n\n");

	return f32Success && f64Success && s32Success;
}
