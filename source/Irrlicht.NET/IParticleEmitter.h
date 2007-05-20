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
	/// A Particle emitter emitts new particles into a particle system.
	///</summary>
	public __gc class IParticleEmitter
	{

	public:

		IParticleEmitter( irr::scene::IParticleEmitter* realEmitter );

		~IParticleEmitter();

		/// <summary>
		/// Returns the internal pointer to the native C++ irrlicht object, don't use this.
		/// </summary>
		inline irr::scene::IParticleEmitter* getParticleEmitter()
		{
			return (irr::scene::IParticleEmitter*)particleEmitter;
		}

	protected:

		irr::scene::IParticleEmitter* particleEmitter;

	};

}
}