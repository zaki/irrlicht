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
		logTestString("empty aabbox3d<T> wrong, or comparison failed\n");
		return false;
	}
	aabbox3d<T> two(core::vector3d<T>(-1,-1,-1),core::vector3d<T>(2,2,2));
	if (empty == two)
	{
		logTestString("empty aabbox3d<T> too large, or comparison failed\n");
		return false;
	}
	one.reset(1,1,1);
	if (one==empty)
	{
		logTestString("reset failed, or comparison failed\n");
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
