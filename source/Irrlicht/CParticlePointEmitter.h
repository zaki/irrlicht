// Copyright (C) 2002-2007 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_PARTICLE_POINT_EMITTER_H_INCLUDED__
#define __C_PARTICLE_POINT_EMITTER_H_INCLUDED__

#include "IParticleEmitter.h"
#include "irrArray.h"

namespace irr
{
namespace scene
{

//! A default point emitter
class CParticlePointEmitter : public IParticleEmitter
{
public:

	//! constructor
	CParticlePointEmitter(
		const core::vector3df& direction = core::vector3df(0.0f,0.03f,0.0f),
		u32 minParticlesPerSecond = 5,
		u32 maxParticlesPerSecond = 10,
		video::SColor minStartColor = video::SColor(255,0,0,0),
		video::SColor maxStartColor = video::SColor(255,255,255,255),
		u32 lifeTimeMin=2000,
		u32 lifeTimeMax=4000,
		s32 maxAngleDegrees=0);

	//! Prepares an array with new particles to emitt into the system
	//! and returns how much new particles there are.
	virtual s32 emitt(u32 now, u32 timeSinceLastCall, SParticle*& outArray);

	//! Writes attributes of the object.
	virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options);

	//! Reads attributes of the object.
	virtual s32 deserializeAttributes(s32 startIndex, io::IAttributes* in, io::SAttributeReadWriteOptions* options);

	//! Get emitter type
	virtual E_PARTICLE_EMITTER_TYPE getType() const { return EPET_POINT; }

private:

	SParticle Particle;
	core::vector3df Direction;
	s32 MinParticlesPerSecond, MaxParticlesPerSecond;
	video::SColor MinStartColor, MaxStartColor;
	s32 MinLifeTime, MaxLifeTime;
	s32 MaxAngleDegrees;

	u32 Time;
	u32 Emitted;
};

} // end namespace scene
} // end namespace irr


#endif

