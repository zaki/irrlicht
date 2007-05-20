// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

using namespace System;

#pragma unmanaged
#include "..\\..\\include\\irrlicht.h"
#pragma managed

namespace Irrlicht
{
namespace Scene
{

	/// <summary>
	/// A particle affector modifies particles.
	///</summary>
	public __gc class IParticleAffector
	{
	public:

		IParticleAffector( irr::scene::IParticleAffector* realAffector );

		~IParticleAffector();

		/// <summary>
		/// Returns the internal pointer to the native C++ irrlicht object, don't use this.
		/// </summary>
		inline irr::scene::IParticleAffector* getParticleAffector()
		{
			return (irr::scene::IParticleAffector*)particleAffector;
		}

	protected:

		irr::scene::IParticleAffector* particleAffector;
	};

}

}