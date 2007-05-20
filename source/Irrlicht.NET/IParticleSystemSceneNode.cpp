// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "IParticleSystemSceneNode.h"
#include "NativeConverter.h"

namespace Irrlicht
{
namespace Scene
{
	IParticleSystemSceneNode::IParticleSystemSceneNode(irr::scene::IParticleSystemSceneNode* realSceneNode)
		: ISceneNode(realSceneNode)
	{
	}

	void IParticleSystemSceneNode::set_ParticleSize( Dimension2Df size )
	{
		getParticleSystemSceneNode()->setParticleSize( irr::NativeConverter::getNativeDim( size ) );
	}

	void IParticleSystemSceneNode::set_ParticlesAreGlobal( bool global )
	{
		getParticleSystemSceneNode()->setParticlesAreGlobal( global );
	}

	void IParticleSystemSceneNode::SetEmitter( IParticleEmitter* emitter )
	{
		getParticleSystemSceneNode()->setEmitter( emitter->getParticleEmitter() );
	}

	void IParticleSystemSceneNode::AddAffector( IParticleAffector* affector )
	{
		getParticleSystemSceneNode()->addAffector( affector->getParticleAffector() );
	}

	void IParticleSystemSceneNode::RemoveAllAffectors()
	{
		getParticleSystemSceneNode()->removeAllAffectors();
	}

	IParticleEmitter*  IParticleSystemSceneNode::CreatePointEmitter( Vector3D direction, unsigned int minParticlesPerSecond,
			unsigned int maxParticlePerSecond, Video::Color minStartColor, Video::Color maxStartColor,
			unsigned int lifeTimeMin, unsigned int lifeTimeMax, int maxAngleDegrees )
	{
		irr::scene::IParticleEmitter* emitter = 
			getParticleSystemSceneNode()->createPointEmitter( irr::NativeConverter::getNativeVector( direction ), minParticlesPerSecond,
			maxParticlePerSecond, irr::NativeConverter::getNativeColor(minStartColor), 
			irr::NativeConverter::getNativeColor(maxStartColor),
			lifeTimeMin, lifeTimeMax, maxAngleDegrees );

		if (emitter)
			return new IParticleEmitter( emitter );

		return 0;
	}

	IParticleEmitter* IParticleSystemSceneNode::CreateBoxEmitter( Box3D box, Vector3D direction, 
			unsigned int minParticlesPerSecond, unsigned int maxParticlePerSecond,
			Video::Color minStartColor, Video::Color maxStartColor,
			unsigned int lifeTimeMin, unsigned int lifeTimeMax,
			int maxAngleDegrees )
	{
		irr::scene::IParticleEmitter* emitter = 
			getParticleSystemSceneNode()->createBoxEmitter( irr::NativeConverter::getNativeBox( box ), 
				irr::NativeConverter::getNativeVector(direction), 
				minParticlesPerSecond, maxParticlePerSecond,
				irr::NativeConverter::getNativeColor(minStartColor), 
				irr::NativeConverter::getNativeColor(maxStartColor),
				lifeTimeMin, lifeTimeMax, maxAngleDegrees );

		if( emitter )
			return new IParticleEmitter(emitter);

		return 0;
	}

	IParticleAffector* IParticleSystemSceneNode::CreateFadeOutParticleAffector( Video::Color targetColor,
			unsigned int timeNeededToFadeOut )
	{
		irr::scene::IParticleAffector* affector =
			getParticleSystemSceneNode()->createFadeOutParticleAffector( irr::NativeConverter::getNativeColor( targetColor ),
			timeNeededToFadeOut);

		if( affector )
			return new IParticleAffector(affector);

		return 0;
	}

	IParticleAffector* IParticleSystemSceneNode::CreateGravityAffector( Vector3D gravity, 
			unsigned int timeForceLost )
	{
		irr::scene::IParticleAffector* affector =
			getParticleSystemSceneNode()->createGravityAffector( irr::NativeConverter::getNativeVector( gravity ),
			timeForceLost);

		if (affector)
			return new IParticleAffector(affector);

		return 0;
	}
}
}