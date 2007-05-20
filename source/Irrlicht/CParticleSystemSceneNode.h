// Copyright (C) 2002-2007 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_PARTICLE_SYSTEM_SCENE_NODE_H_INCLUDED__
#define __C_PARTICLE_SYSTEM_SCENE_NODE_H_INCLUDED__

#include "IParticleSystemSceneNode.h"
#include "irrArray.h"
#include "irrList.h"
#include "SMeshBuffer.h"

namespace irr
{
namespace scene
{

//! A particle system scene node.
/** A scene node controlling a particle system. The behavior of the particles
can be controlled by setting the right particle emitters and affectors.
*/
class CParticleSystemSceneNode : public IParticleSystemSceneNode
{
public:

	//! constructor
	CParticleSystemSceneNode(bool createDefaultEmitter,
		ISceneNode* parent, ISceneManager* mgr, s32 id, 
		const core::vector3df& position,
		const core::vector3df& rotation,
		const core::vector3df& scale);

	//! destructor
	virtual ~CParticleSystemSceneNode();

	//! Sets the particle emitter, which creates the particles.
	virtual void setEmitter(IParticleEmitter* emitter);

	//! Adds new particle affector to the particle system.
	virtual void addAffector(IParticleAffector* affector);

	//! Removes all particle affectors in the particle system.
	virtual void removeAllAffectors();

	//! Returns the material based on the zero based index i.
	virtual video::SMaterial& getMaterial(u32 i);

	//! Returns amount of materials used by this scene node.
	virtual u32 getMaterialCount();

	//! pre render event
	virtual void OnRegisterSceneNode();

	//! render
	virtual void render();

	//! returns the axis aligned bounding box of this node
	virtual const core::aabbox3d<f32>& getBoundingBox() const;

	//! Creates a point particle emitter.
	virtual IParticleEmitter* createPointEmitter(
		const core::vector3df& direction = core::vector3df(0.0f,0.03f,0.0f), 
		u32 minParticlesPerSecond = 5,
		u32 maxParticlePerSecond = 10,
		video::SColor minStartColor = video::SColor(255,0,0,0),
		video::SColor maxStartColor = video::SColor(255,255,255,255),
		u32 lifeTimeMin=2000, u32 lifeTimeMax=4000,
		s32 maxAngleDegrees=0);

	//! Creates a box particle emitter.
	virtual IParticleEmitter* createBoxEmitter(
		const core::aabbox3df& box = core::aabbox3d<f32>(-10,0,-10,5,30,10),
		const core::vector3df& direction = core::vector3df(0.0f,0.03f,0.0f), 
		u32 minParticlesPerSecond = 5,
		u32 maxParticlePerSecond = 10,
		video::SColor minStartColor = video::SColor(255,0,0,0),
		video::SColor maxStartColor = video::SColor(255,255,255,255),
		u32 lifeTimeMin=2000, u32 lifeTimeMax=4000,
		s32 maxAngleDegrees=0);

	//! Creates a fade out particle affector.
	virtual IParticleAffector* createFadeOutParticleAffector(
		video::SColor targetColor = video::SColor(0,0,0,0),
		u32 timeNeededToFadeOut = 1000);

	//! Creates a gravity affector.
	virtual IParticleAffector* createGravityAffector(
		const core::vector3df& gravity = core::vector3df(0.0f,-0.03f,0.0f),
		u32 timeForceLost = 1000);

	//! Sets the size of all particles.
	virtual void setParticleSize(
		const core::dimension2d<f32> &size = core::dimension2d<f32>(5.0f, 5.0f));

	//! Sets if the particles should be global. If they are, the particles are affected by
	//! the movement of the particle system scene node too, otherwise they completely
	//! ignore it. Default is true.
	virtual void setParticlesAreGlobal(bool global);

	//! Writes attributes of the scene node.
	virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options=0);

	//! Reads attributes of the scene node.
	virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options=0);

	//! Returns type of the scene node
	virtual ESCENE_NODE_TYPE getType() const { return ESNT_PARTICLE_SYSTEM; }

private:

	void doParticleSystem(u32 time);
	void reallocateBuffers();

	core::list<IParticleAffector*> AffectorList;
	IParticleEmitter* Emitter;
	core::array<SParticle> Particles;
	core::dimension2d<f32> ParticleSize;
	u32 LastEmitTime;
	s32 MaxParticles;

	SMeshBuffer Buffer;

	enum E_PARTICLES_PRIMITIVE
	{
		EPP_POINT=0,
		EPP_BILLBOARD,
		EPP_POINTSPRITE
	};
	E_PARTICLES_PRIMITIVE ParticlePrimitive;

	bool ParticlesAreGlobal;
};

} // end namespace scene
} // end namespace irr


#endif

