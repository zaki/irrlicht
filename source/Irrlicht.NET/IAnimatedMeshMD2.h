// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once 

#include "IAnimatedMesh.h"

namespace Irrlicht
{
namespace Scene
{

	/// Types of standard md2 animations
	public __value enum MD2AnimationType
	{
		STAND = 0,
		RUN,
		ATTACK,
		PAIN_A,
		PAIN_B,
		PAIN_C,
		JUMP,
		FLIP,
		SALUTE,
		FALLBACK,
		WAVE,
		POINT,
		CROUCH_STAND,
		CROUCH_WALK,
		CROUCH_ATTACK,
		CROUCH_PAIN,
		CROUCH_DEATH,
		DEATH_FALLBACK,
		DEATH_FALLFORWARD,
		DEATH_FALLBACKSLOW,
		BOOM,

		/// Not an animation, but amount of animation types.
		COUNT
	};

	/// Interface for using some special functions of MD2 meshes
	public __gc class IAnimatedMeshMD2 : public IAnimatedMesh
	{
	public:

		/// <summary>
		/// You should create an IAnimatedMeshMD2
		/// through the Irrlicht::IrrlichtDevice::SceneManager::getMesh method. Simply don't use
		/// this constructor.
		///</summary>
		///<param name="realMesh">The real, unmanaged C++ mesh</param>
		IAnimatedMeshMD2(irr::scene::IAnimatedMeshMD2* realMesh);

		/// <summary>
		/// Returns frame loop data for a default MD2 animation type.
		/// </summary>
		/// <returns> Returns the beginframe, endframe and frames per 
		/// second for a default MD2 animation type.</returns>
		void GetFrameLoop(MD2AnimationType l, int& outBegin,
			int& outEnd, int& outFPS);

		/// <summary>
		/// Returns frame loop data for a special MD2 animation type, identified
		/// by a string.
		/// </summary>
		/// <returns> Returns the beginframe, endframe and frames per 
		/// second for a special MD2 animation type.</returns>
		bool GetFrameLoop(System::String* name, 
			int& outBegin, int& outEnd, int& outFps);

		/// <summary>
		/// Returns amount of md2 animations in this file.
		/// </summary>
		__property int get_AnimationCount();

		/// <summary>
		/// Returns name of md2 animation. 
		/// </summary>
		/// <param name="nr"> Zero based index of animation. </param>
		System::String* GetAnimationName(int nr);

	protected:

		inline irr::scene::IAnimatedMeshMD2* getMD2()
		{
			return (irr::scene::IAnimatedMeshMD2*)AnimatedMesh;
		}
        
	};

} // end namespace scene
} // end namespace irr


