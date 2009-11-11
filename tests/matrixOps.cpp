// Copyright (C) 2008 Colin MacDonald
// No rights reserved: this software is in the public domain.

#include "testUtils.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

namespace
{

// Basic tests
bool matrices(void)
{
	bool result = true;
	matrix4 m;
	// Check default init
	result &= (m==core::IdentityMatrix);
	result &= (core::IdentityMatrix==m);
	assert(result);
	// Since the last test can be made with isDefinitelyIdentityMatrix we set it to false here
	m.setDefinitelyIdentityMatrix(false);
	result &= (m==core::IdentityMatrix);
	result &= (core::IdentityMatrix==m);
	assert(result);
	// also equals should see this
	result &= m.equals(core::IdentityMatrix);
	result &= core::IdentityMatrix.equals(m);
	assert(result);
	// Check inequality
	m[12]=5.f;
	result &= (m!=core::IdentityMatrix);
	result &= (core::IdentityMatrix!=m);
	result &= !m.equals(core::IdentityMatrix);
	result &= !core::IdentityMatrix.equals(m);
	assert(result);

	// Test multiplication
	result &= (m==(core::IdentityMatrix*m));
	result &= m.equals(core::IdentityMatrix*m);
	result &= (m==(m*core::IdentityMatrix));
	result &= m.equals(m*core::IdentityMatrix);
	assert(result);

	return result;
}

// Test rotations
bool rotations(void)
{
	bool result = true;
	matrix4 rot1,rot2,rot3,rot4,rot5;
	core::vector3df vec1(1,2,3),vec12(1,2,3);
	core::vector3df vec2(-5,0,0),vec22(-5,0,0);
	core::vector3df vec3(20,0,-20), vec32(20,0,-20);
	// Make sure the matrix multiplication and rotation application give same results
	rot1.setRotationDegrees(core::vector3df(90,0,0));
	rot2.setRotationDegrees(core::vector3df(0,90,0));
	rot3.setRotationDegrees(core::vector3df(0,0,90));
	rot4.setRotationDegrees(core::vector3df(90,90,90));
	rot5 = rot3*rot2*rot1;
	result &= (rot4.equals(rot5, ROUNDING_ERROR_f32));
	assert(result);
	rot4.transformVect(vec1);rot5.transformVect(vec12);
	rot4.transformVect(vec2);rot5.transformVect(vec22);
	rot4.transformVect(vec3);rot5.transformVect(vec32);
	result &= (vec1.equals(vec12));
	result &= (vec2.equals(vec22));
	result &= (vec3.equals(vec32));
	assert(result);

	vec1.set(1,2,3);vec12.set(1,2,3);
	vec2.set(-5,0,0);vec22.set(-5,0,0);
	vec3.set(20,0,-20);vec32.set(20,0,-20);
	rot1.setRotationDegrees(core::vector3df(45,0,0));
	rot2.setRotationDegrees(core::vector3df(0,45,0));
	rot3.setRotationDegrees(core::vector3df(0,0,45));
	rot4.setRotationDegrees(core::vector3df(45,45,45));
	rot5 = rot3*rot2*rot1;
	result &= (rot4.equals(rot5, ROUNDING_ERROR_f32));
	assert(result);
	rot4.transformVect(vec1);rot5.transformVect(vec12);
	rot4.transformVect(vec2);rot5.transformVect(vec22);
	rot4.transformVect(vec3);rot5.transformVect(vec32);
	result &= (vec1.equals(vec12));
	result &= (vec2.equals(vec22));
	result &= (vec3.equals(vec32));
	assert(result);

	vec1.set(1,2,3);vec12.set(1,2,3);
	vec2.set(-5,0,0);vec22.set(-5,0,0);
	vec3.set(20,0,-20);vec32.set(20,0,-20);
	rot1.setRotationDegrees(core::vector3df(-60,0,0));
	rot2.setRotationDegrees(core::vector3df(0,-60,0));
	rot3.setRotationDegrees(core::vector3df(0,0,-60));
	rot4.setRotationDegrees(core::vector3df(-60,-60,-60));
	rot5 = rot3*rot2*rot1;
	result &= (rot4.equals(rot5, ROUNDING_ERROR_f32));
	assert(result);
	rot4.transformVect(vec1);rot5.transformVect(vec12);
	rot4.transformVect(vec2);rot5.transformVect(vec22);
	rot4.transformVect(vec3);rot5.transformVect(vec32);
	result &= (vec1.equals(vec12));
	result &= (vec2.equals(vec22));
	result &= (vec3.equals(vec32));
	assert(result);

	vec1.set(1,2,3);vec12.set(1,2,3);
	vec2.set(-5,0,0);vec22.set(-5,0,0);
	vec3.set(20,0,-20);vec32.set(20,0,-20);
	rot1.setRotationDegrees(core::vector3df(113,0,0));
	rot2.setRotationDegrees(core::vector3df(0,-27,0));
	rot3.setRotationDegrees(core::vector3df(0,0,193));
	rot4.setRotationDegrees(core::vector3df(113,-27,193));
	rot5 = rot3*rot2*rot1;
	result &= (rot4.equals(rot5, ROUNDING_ERROR_f32));
	assert(result);
	rot4.transformVect(vec1);rot5.transformVect(vec12);
	rot4.transformVect(vec2);rot5.transformVect(vec22);
	rot4.transformVect(vec3);rot5.transformVect(vec32);
	result &= (vec1.equals(vec12));
	result &= (vec2.equals(vec22));
	result &= (vec3.equals(vec32));
	assert(result);

	return result;
}

// Test isOrthogonal
bool isOrthogonal(void)
{
	matrix4 rotationMatrix;
	if (!rotationMatrix.isOrthogonal())
	{
		logTestString("irr::core::matrix4::isOrthogonal() failed with Identity.\n");
		return false;
	}

	rotationMatrix.setRotationDegrees(vector3df(90, 0, 0));
	if (!rotationMatrix.isOrthogonal())
	{
		logTestString("irr::core::matrix4::isOrthogonal() failed with rotation.\n");
		return false;
	}

	matrix4 translationMatrix;
	translationMatrix.setTranslation(vector3df(0, 3, 0));
	if (translationMatrix.isOrthogonal())
	{
		logTestString("irr::core::matrix4::isOrthogonal() failed with translation.\n");
		return false;
	}

	matrix4 scaleMatrix;
	scaleMatrix.setScale(vector3df(1, 2, 3));
	if (!scaleMatrix.isOrthogonal())
	{
		logTestString("irr::core::matrix4::isOrthogonal() failed with scale.\n");
		return false;
	}

	return true;
}

}

bool matrixOps(void)
{
	bool result = true;
	result &= matrices();
	result &= rotations();
	result &= isOrthogonal();
	return result;
}

