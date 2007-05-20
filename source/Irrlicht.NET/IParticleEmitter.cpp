// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "IParticleEmitter.h"
#include "NativeConverter.h"

namespace Irrlicht
{
namespace Scene
{
	IParticleEmitter::IParticleEmitter( irr::scene::IParticleEmitter* realEmitter )
		: particleEmitter( realEmitter )
	{
		particleEmitter->grab();
	}

	IParticleEmitter::~IParticleEmitter()
	{
		particleEmitter->drop();
	}
}
}