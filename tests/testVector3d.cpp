// Test the functionality of vector3d<T>, particularly methods that
// involve calculations done using different precision than <T>.
// Note that all reference vector3d<T>s are creating using double precision
// values cast to (T), as we need to test <f64>.

#include "irrlicht.h"
#include <assert.h>

using namespace irr;
using namespace core;

template<class T>
static bool compareVectors(const core::vector3d<T> & compare,
						   const core::vector3d<T> & with)
{
	if(compare != with)
	{
		(void)printf("\nERROR: vector3d %.16f, %.16f, %.16f != vector3d %.16f, %.16f, %.16f\n", 
			(f64)compare.X, (f64)compare.Y, (f64)compare.Z,
			(f64)with.X, (f64)with.Y, (f64)with.Z);
		assert(compare == with);
		return false;
	}

	return true;
}

template <class T>
static bool doTests()
{
	#define COMPARE_VECTORS(compare, with)\
		if(!compareVectors(compare, with)) return false;

	vector3d<T> vec(5, 5, 0);
	vector3d<T> otherVec(10, 20, 0);
	if(!equals(vec.getDistanceFrom(otherVec), (T)15.8113883))
	{
		(void)printf("vector3d::getDistanceFrom() failed\n");
		assert(0);
		return false;
	}

	vector3d<T> center(0, 0, 0);

	vec.rotateXYBy(45, center);
	COMPARE_VECTORS(vec, vector3d<T>(0, (T)7.0710678118654755, 0));

	vec.normalize();
	COMPARE_VECTORS(vec, vector3d<T>(0, (T)1.0000000461060017, 0));

	vec.set(10, 10, 10);
	center.set(5, 5, 10);
	vec.rotateXYBy(-5, center);
	// -5 means rotate clockwise slightly, so expect the X to increase 
	// slightly and the Y to decrease slightly.
	COMPARE_VECTORS(vec, vector3d<T>((T)10.416752204197017, (T)9.5451947767204359, 10));

	vec.set(10, 10, 10);
	center.set(5, 10, 5);
	vec.rotateXZBy(-5, center);
	COMPARE_VECTORS(vec, vector3d<T>((T)10.416752204197017, 10, (T)9.5451947767204359));

	vec.set(10, 10, 10);
	center.set(10, 5, 5);
	vec.rotateYZBy(-5, center);
	COMPARE_VECTORS(vec, vector3d<T>(10, (T)10.416752204197017, (T)9.5451947767204359));

	vec.set(5, 5, 0);
	vec.normalize();
	compareVectors(vec, vector3d<T>((T)0.70710681378841400, (T)0.70710681378841400, 0));

	vec.set(5, 5, 0);
	otherVec.set(10, 20, 40);

	vector3d<T> interpolated;
	(void)interpolated.interpolate(vec, otherVec, 0.f);
	COMPARE_VECTORS(interpolated, otherVec); // 0.f means all the second vector

	(void)interpolated.interpolate(vec, otherVec, 0.25f);
	COMPARE_VECTORS(interpolated, vector3d<T>((T)8.75, (T)16.25, 30));

	(void)interpolated.interpolate(vec, otherVec, 0.75f);
	COMPARE_VECTORS(interpolated, vector3d<T>((T)6.25, (T)8.75, 10));

	(void)interpolated.interpolate(vec, otherVec, 1.f);
	COMPARE_VECTORS(interpolated, vec); // 1.f means all the first vector


	interpolated = vec.getInterpolated(otherVec, 0.f);
	COMPARE_VECTORS(interpolated, otherVec); // 0.f means all the second vector

	interpolated = vec.getInterpolated(otherVec, 0.25f);
	COMPARE_VECTORS(interpolated, vector3d<T>((T)8.75, (T)16.25, 30));

	interpolated = vec.getInterpolated(otherVec, 0.75f);
	COMPARE_VECTORS(interpolated, vector3d<T>((T)6.25, (T)8.75, 10));

	interpolated = vec.getInterpolated(otherVec, 1.f);
	COMPARE_VECTORS(interpolated, vec); // 1.f means all the first vector


	vector3d<T> thirdVec(20, 10, -30);
	interpolated = vec.getInterpolated_quadratic(otherVec, thirdVec, 0.f);
	COMPARE_VECTORS(interpolated, vec); // 0.f means all the 1st vector

	interpolated = vec.getInterpolated_quadratic(otherVec, thirdVec, 0.25f);
	COMPARE_VECTORS(interpolated, vector3d<T>((T)7.8125, (T)10.9375, (T)13.125));

	interpolated = vec.getInterpolated_quadratic(otherVec, thirdVec, 0.5f);
	COMPARE_VECTORS(interpolated, vector3d<T>((T)11.25, (T)13.75, (T)12.5));

	interpolated = vec.getInterpolated_quadratic(otherVec, thirdVec, 0.75f);
	COMPARE_VECTORS(interpolated, vector3d<T>((T)15.3125, (T)13.4375, (T)-1.875));

	interpolated = vec.getInterpolated_quadratic(otherVec, thirdVec, 1.f);
	COMPARE_VECTORS(interpolated, thirdVec); // 1.f means all the 3rd vector

	return true;
}


bool testVector3d(void)
{
	bool f32Success = doTests<f32>();
	if(f32Success)
		(void)printf("vector3df tests passed\n\n");
	else
		(void)printf("\n*** vector3df tests failed ***\n\n");

	bool f64Success = doTests<f64>();
	if(f64Success)
		(void)printf("vector3d<f64> tests passed\n\n");
	else
		(void)printf("\n*** vector3d<f64> tests failed ***\n\n");

	bool s32Success = doTests<s32>();
	if(s32Success)
		(void)printf("vector3di tests passed\n\n");
	else
		(void)printf("\n*** vector3di tests failed ***\n\n");

	return f32Success && f64Success && s32Success;
}
