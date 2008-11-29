// Test the functionality of vector2d<T>, particularly methods that
// involve calculations done using different precision than <T>.
// Note that all reference vector2d<T>s are creating using double precision
// values cast to (T), as we need to test <f64>.

#include "irrlicht.h"
#include <assert.h>

using namespace irr;
using namespace core;

template<class T>
static bool compareVectors(const core::vector2d<T> & compare,
						   const core::vector2d<T> & with)
{
	if(compare != with)
	{
		(void)printf("\nERROR: vector2d %.16f, %.16f != vector2d %.16f, %.16f\n", 
			(f64)compare.X, (f64)compare.Y, (f64)with.X, (f64)with.Y);
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

	vector2d<T> vec(5, 5);
	vector2d<T> otherVec(10, 20);
	if(!equals(vec.getDistanceFrom(otherVec), (T)15.8113883))
	{
		(void)printf("vector2d::getDistanceFrom() failed\n");
		assert(0);
		return false;
	}

	vector2d<T> center(0, 0);

	vec.rotateBy(45, center);
	COMPARE_VECTORS(vec, vector2d<T>(0, (T)7.0710678118654755));

	vec.normalize();
	COMPARE_VECTORS(vec, vector2d<T>(0, (T)1.0000000461060017));

	vec.set(10, 10);
	center.set(5, 5);
	vec.rotateBy(-5, center);
	// -5 means rotate clockwise slightly, so expect the X to increase 
	// slightly and the Y to decrease slightly.
	COMPARE_VECTORS(vec, vector2d<T>((T)10.416752204197017, (T)9.5451947767204359));

	vec.set(5, 5);
	vec.normalize();
	compareVectors(vec, vector2d<T>((T)0.70710681378841400, (T)0.70710681378841400));

	vec.set(5, 5);
	otherVec.set(10, 20);

	vector2d<T> interpolated;
	(void)interpolated.interpolate(vec, otherVec, 0.f);
	COMPARE_VECTORS(interpolated, otherVec); // 0.f means all the second vector

	(void)interpolated.interpolate(vec, otherVec, 0.25f);
	COMPARE_VECTORS(interpolated, vector2d<T>((T)8.75, (T)16.25));

	(void)interpolated.interpolate(vec, otherVec, 0.75f);
	COMPARE_VECTORS(interpolated, vector2d<T>((T)6.25, (T)8.75));

	(void)interpolated.interpolate(vec, otherVec, 1.f);
	COMPARE_VECTORS(interpolated, vec); // 1.f means all the first vector


	interpolated = vec.getInterpolated(otherVec, 0.f);
	COMPARE_VECTORS(interpolated, otherVec); // 0.f means all the second vector

	interpolated = vec.getInterpolated(otherVec, 0.25f);
	COMPARE_VECTORS(interpolated, vector2d<T>((T)8.75, (T)16.25));

	interpolated = vec.getInterpolated(otherVec, 0.75f);
	COMPARE_VECTORS(interpolated, vector2d<T>((T)6.25, (T)8.75));

	interpolated = vec.getInterpolated(otherVec, 1.f);
	COMPARE_VECTORS(interpolated, vec); // 1.f means all the first vector


	vector2d<T> thirdVec(20, 10);
	interpolated = vec.getInterpolated_quadratic(otherVec, thirdVec, 0.f);
	COMPARE_VECTORS(interpolated, vec); // 0.f means all the 1st vector

	interpolated = vec.getInterpolated_quadratic(otherVec, thirdVec, 0.25f);
	COMPARE_VECTORS(interpolated, vector2d<T>((T)7.8125, (T)10.9375));

	interpolated = vec.getInterpolated_quadratic(otherVec, thirdVec, 0.5f);
	COMPARE_VECTORS(interpolated, vector2d<T>((T)11.25, (T)13.75));

	interpolated = vec.getInterpolated_quadratic(otherVec, thirdVec, 0.75f);
	COMPARE_VECTORS(interpolated, vector2d<T>((T)15.3125, (T)13.4375));

	interpolated = vec.getInterpolated_quadratic(otherVec, thirdVec, 1.f);
	COMPARE_VECTORS(interpolated, thirdVec); // 1.f means all the 3rd vector

	return true;
}


bool testVector2d(void)
{
	bool f32Success = doTests<f32>();
	if(f32Success)
		(void)printf("vector2df tests passed\n\n");
	else
		(void)printf("\n*** vector2df tests failed ***\n\n");

	bool f64Success = doTests<f64>();
	if(f64Success)
		(void)printf("vector2d<f64> tests passed\n\n");
	else
		(void)printf("\n*** vector2d<f64> tests failed ***\n\n");

	bool s32Success = doTests<s32>();
	if(s32Success)
		(void)printf("vector2di tests passed\n\n");
	else
		(void)printf("\n*** vector2di tests failed ***\n\n");

	return f32Success && f64Success && s32Success;
}
