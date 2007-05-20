// Copyright (C) 2002-2007 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_PARTICLE_SYSTEM_SCENE_NODE_H_INCLUDED__
#define __I_PARTICLE_SYSTEM_SCENE_NODE_H_INCLUDED__

#include "ISceneNode.h"
#include "IParticleEmitter.h"
#include "IParticleAffector.h"
#include "dimension2d.h"

namespace irr
{
namespace scene
{

//! A particle system scene node for creating snow, fire, exlosions, smoke...
/** A scene node controlling a particle System. The behavior of the particles
can be controlling by setting the right particle emitters and effectors.
You can for example easily a campfire by doing this:

\code
	scene::IParticleSystemSceneNode* p = scenemgr->addParticleSystemSceneNode();
	p->setParticleSize(core::dimension2d<f32>(20.0f, 10.0f));
	scene::IParticleEmitter* em = p->createBoxEmitter(
		core::aabbox3d<f32>(-5,0,-5,5,1,5), 
		core::vector3df(0.0f,0.03f,0.0f),
		40,80, video::SColor(0,255,255,255),video::SColor(0,255,255,255), 1100,2000);
	p->setEmitter(em);
	em->drop();
	scene::IParticleAffector* paf = p->createFadeOutParticleAffector();
	p->addAffector(paf);
	paf->drop();
\endcode

*/
class IParticleSystemSceneNode : public ISceneNode
{
public:

	//! constructor
	IParticleSystemSceneNode(ISceneNode* parent, ISceneManager* mgr, s32 id, 
		const core::vector3df& position = core::vector3df(0,0,0),
		const core::vector3df& rotation = core::vector3df(0,0,0),
		const core::vector3df& scale = core::vector3df(1.0f, 1.0f, 1.0f))
			: ISceneNode(parent, mgr, id, position, rotation, scale) {}

	//! Sets the size of all particles.
	virtual void setParticleSize(
		const core::dimension2d<f32> &size = core::dimension2d<f32>(5.0f, 5.0f)) = 0;

	//! Sets if the particles should be global. If it is, the particles are affected by 
	//! the movement of the particle system scene node too, otherwise they completely 
	//! ignore it. Default is true.
	virtual void setParticlesAreGlobal(bool global) = 0;

	//! Sets the particle emitter, which creates the particles.
	//! A particle emitter can be created using one of the 
	//! methods. For example to create and use a simple PointEmitter,
	//! call IParticleEmitter* p = createPointEmitter(); setEmitter(p); p->drop();
	//! \param emitter: Sets the particle emitter. You can set this to 0
	//! for removing the current emitter and stopping the particle system
	//! emitting new particles.
	virtual void setEmitter(IParticleEmitter* emitter) = 0;

	//! Adds new particle effector to the particle system. A particle
	//! affector modifies the particles. For example, the FadeOut
	//! affector lets all particles fade out after some time. It is created
	//! and used in this way: IParticleAffector* p = createFadeOutParticleAffector();
	//! addAffector(p); p->drop();
	//! Please note that a affector is not necessary for the particle
	//! system to work.
	//! \param affector: New affector.
	virtual void addAffector(IParticleAffector* affector) = 0;

	//! Removes all particle affectors in the particle system.
	virtual void removeAllAffectors() = 0;

	//! Creates a point particle emitter.
	//! \param direction: Direction and speed of particle emission.
	//! \param minParticlesPerSecond: Minimal amount of particles emitted
	//! per second.
	//! \param maxParticlesPerSecond: Maximal amount of particles emitted
	//! per second.
	//! \param minStartColor: Minimal initial start color of a particle. 
	//! The real color of every particle is calculated as random interpolation
	//! between minStartColor and maxStartColor.
	//! \param maxStartColor: Maximal initial start color of a particle. 
	//! The real color of every particle is calculated as random interpolation
	//! between minStartColor and maxStartColor.
	//! \param lifeTimeMin: Minimal lifetime of a particle, in milliseconds.
	//! \param lifeTimeMax: Maximal lifetime of a particle, in milliseconds.
	//! \param maxAngleDegrees: Maximal angle in degrees, the emitting direction
	//! of the particle will differ from the orignial direction.
	//! \return Returns a pointer to the created particle emitter.
	//! To set this emitter as new emitter of this particle system,
	//! just call setEmitter(). Note that you'll have to drop() the
	//! returned pointer, after you don't need it any more, see
	//! IUnknown::drop() for more informations.
	virtual IParticleEmitter* createPointEmitter(
		const core::vector3df& direction = core::vector3df(0.0f,0.03f,0.0f), 
		u32 minParticlesPerSecond = 5,
		u32 maxParticlesPerSecond = 10,
		video::SColor minStartColor = video::SColor(255,0,0,0),
		video::SColor maxStartColor = video::SColor(255,255,255,255),
		u32 lifeTimeMin=2000, u32 lifeTimeMax=4000,
		s32 maxAngleDegrees=0) = 0;

	//! Creates a box particle emitter.
	//! \param box: The box for the emitter.
	//! \param direction: Direction and speed of particle emission.
	//! \param minParticlesPerSecond: Minimal amount of particles emitted
	//! per second.
	//! \param maxParticlesPerSecond: Maximal amount of particles emitted
	//! per second.
	//! \param minStartColor: Minimal initial start color of a particle. 
	//! The real color of every particle is calculated as random interpolation
	//! between minStartColor and maxStartColor.
	//! \param maxStartColor: Maximal initial start color of a particle. 
	//! The real color of every particle is calculated as random interpolation
	//! between minStartColor and maxStartColor.
	//! \param lifeTimeMin: Minimal lifetime of a particle, in milliseconds.
	//! \param lifeTimeMax: Maximal lifetime of a particle, in milliseconds.
	//! \param maxAngleDegrees: Maximal angle in degrees, the emitting direction
	//! of the particle will differ from the orignial direction.
	//! \return Returns a pointer to the created particle emitter.
	//! To set this emitter as new emitter of this particle system,
	//! just call setEmitter(). Note that you'll have to drop() the
	//! returned pointer, after you don't need it any more, see
	//! IUnknown::drop() for more informations.
	virtual IParticleEmitter* createBoxEmitter(
		const core::aabbox3df& box = core::aabbox3df(-10,28,-10,10,30,10),
		const core::vector3df& direction = core::vector3df(0.0f,0.03f,0.0f), 
		u32 minParticlesPerSecond = 5,
		u32 maxParticlesPerSecond = 10,
		video::SColor minStartColor = video::SColor(255,0,0,0),
		video::SColor maxStartColor = video::SColor(255,255,255,255),
		u32 lifeTimeMin=2000, u32 lifeTimeMax=4000,
		s32 maxAngleDegrees=0) = 0;

	//! Creates a fade out particle affector. This affector modifies
	//! the color of every particle and and reaches the final color
	//! when the particle dies.
	//! This affector looks really good, if the EMT_TRANSPARENT_VERTEX_ALPHA
	//! material is used and the targetColor is video::SColor(0,0,0,0):
	//! Particles are fading out into void with this setting.
	//! \param targetColor: Color whereto the color of the particle is changed.
	//! \param timeNeededToFadeOut: How much time in milli seconds 
	//! should the affector need to change the color to the targetColor. 
	//! \return Returns a pointer to the created particle affector.
	//! To add this affector as new affector of this particle system,
	//! just call addAffector(). Note that you'll have to drop() the
	//! returned pointer, after you don't need it any more, see
	//! IUnknown::drop() for more informations.
	virtual IParticleAffector* createFadeOutParticleAffector(
		video::SColor targetColor = video::SColor(0,0,0,0),
		u32 timeNeededToFadeOut = 1000) = 0;

	//! Creates a gravity affector. This affector modifies the direction
	//! of the particle. It assumes that the particle is fired out of the
	//! emitter with huge force, but is loosing this after some time
	//! and is catched by the gravity then. This affector is ideal for
	//! creating things like fountains.
	//! \param gravity: Direction and force of gravity.
	//! \param timeForceLost: Time in milli seconds when the force 
	//! of the emitter is totally lost and the particle does not move any more.
	//! This is the time where gravity fully affects the particle.
	//! \return Returns a pointer to the created particle affector.
	//! To add this affector as new affector of this particle system,
	//! just call addAffector(). Note that you'll have to drop() the
	//! returned pointer, after you don't need it any more, see
	//! IUnknown::drop() for more informations.
	virtual IParticleAffector* createGravityAffector(
		const core::vector3df& gravity = core::vector3df(0.0f,-0.03f,0.0f),
		u32 timeForceLost = 1000) = 0;
};

} // end namespace scene
} // end namespace irr


#endif

