// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

using namespace System;

#pragma unmanaged
#include "..\\..\\include\\irrlicht.h"
#pragma managed

#include "ISceneNode.h"
#include "Dimension2D.h"
#include "IParticleEmitter.h"
#include "IParticleAffector.h"

using namespace Irrlicht;
using namespace Irrlicht::Core;

namespace Irrlicht
{
namespace Scene
{
	public __gc class IParticleSystemSceneNode : public ISceneNode
	{

	public:

		/// <summary>
		/// Creates a scene node from a native C++ scene node. Don't use this, its better to create 
		/// scene nodes using the SceneManager with its addSceneNode() methods.
		/// </summary>
		IParticleSystemSceneNode(irr::scene::IParticleSystemSceneNode* realSceneNode);

		/// <summary>
		/// Sets the size of all particles.
		/// </summary>
		__property void set_ParticleSize( Dimension2Df size );

		/// <summary>
		/// Sets if the particles should be global. If it is, the particles are affected by 
		/// the movement of the particle system scene node too, otherwise they completely 
		/// ignore it. Default is true.
		/// </summary>
		__property void set_ParticlesAreGlobal(bool global);


		/// <summary>
		/// Sets the particle emitter, which creates the particles.
		/// </summary>
		void SetEmitter( IParticleEmitter* emitter );

		/// <summary>
		/// Adds new particle effector to the particle system. 
		/// </summary>
		void AddAffector( IParticleAffector* affector );

		/// <summary>
		/// Removes all particle affectors in the particle system.
		/// </summary>
		void RemoveAllAffectors();

		/// <summary>
		/// Creates a point particle emitter.
		/// </summary>
		IParticleEmitter* CreatePointEmitter( Vector3D direction, unsigned int minParticlesPerSecond,
			unsigned int maxParticlePerSecond, Video::Color minStartColor, Video::Color maxStartColor,
			unsigned int lifeTimeMin, unsigned int lifeTimeMax, int maxAngleDegrees );

		/// <summary>
		/// Creates a box particle emitter.
		/// </summary>
		IParticleEmitter* CreateBoxEmitter( Box3D box, Vector3D direction, 
			unsigned int minParticlesPerSecond, unsigned int maxParticlePerSecond,
			Video::Color minStartColor, Video::Color maxStartColor,
			unsigned int lifeTimeMin, unsigned int lifeTimeMax,
			int maxAngleDegrees );

		/// <summary>
		/// Creates a fade out particle affector. This affector modifies
		/// the color of every particle and and reaches the final color
		/// when the particle dies.
		/// </summary>
		IParticleAffector* CreateFadeOutParticleAffector( Video::Color targetColor,
			unsigned int timeNeededToFadeOut );

		/// <summary>
		/// Creates a gravity affector. This affector modifies the direction
		/// of the particle. It assumes that the particle is fired out of the
		/// emitter with huge force, but is loosing this after some time
		/// and is catched by the gravity then. This affector is ideal for
		/// creating things like fountains.
		/// </summary>
		IParticleAffector* CreateGravityAffector( Vector3D gravity, 
			unsigned int timeForceLost );

	protected:

		inline irr::scene::IParticleSystemSceneNode* getParticleSystemSceneNode()
		{
			return (irr::scene::IParticleSystemSceneNode*)SceneNode;
		}

	};

}
}