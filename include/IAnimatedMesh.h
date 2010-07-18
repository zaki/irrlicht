// Copyright (C) 2002-2009 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_ANIMATED_MESH_H_INCLUDED__
#define __I_ANIMATED_MESH_H_INCLUDED__

#include "aabbox3d.h"
#include "IMesh.h"

namespace irr
{
namespace scene
{
	//! Possible types of (animated) meshes.
	enum E_ANIMATED_MESH_TYPE
	{
		//! Unknown animated mesh type.
		EAMT_UNKNOWN = 0,

		//! Quake 2 MD2 model file
		EAMT_MD2,

		//! Quake 3 MD3 model file
		EAMT_MD3,

		//! Maya .obj static model
		EAMT_OBJ,

		//! Quake 3 .bsp static Map
		EAMT_BSP,

		//! 3D Studio .3ds file
		EAMT_3DS,

		//! My3D Mesh, the file format by Zhuck Dimitry
		EAMT_MY3D,

		//! Pulsar LMTools .lmts file. This Irrlicht loader was written by Jonas Petersen
		EAMT_LMTS,

		//! Cartography Shop .csm file. This loader was created by Saurav Mohapatra.
		EAMT_CSM,

		//! .oct file for Paul Nette's FSRad or from Murphy McCauley's Blender .oct exporter.
		/** The oct file format contains 3D geometry and lightmaps and
		can be loaded directly by Irrlicht */
		EAMT_OCT,

		//! Halflife MDL model file
		EAMT_MDL_HALFLIFE,

		//! generic skinned mesh
		EAMT_SKINNED
	};


	//! Possible types of Animation Type
	enum E_ANIMATION_TYPE
	{
		//! No Animation
		EAMT_STILL,
		//! From Start to End, then Stop ( Limited Line )
		EAMT_WAYPOINT,
		//! Linear Cycling Animation	 ( Sawtooth )
		EAMT_LOOPING,
		//! Linear bobbing				 ( Triangle )
		EAMT_PINGPONG
	};

	//! Names for Animation Type
	const c8* const MeshAnimationTypeNames[] =
	{
		"still",
		"waypoint",
		"looping",
		"pingpong",
		0
	};


	//! Data for holding named Animation Info
	struct KeyFrameInterpolation
	{
		core::stringc Name;		// Name of the current Animation/Bone
		E_ANIMATION_TYPE AnimationType;	// Type of Animation ( looping, usw..)

		f32 CurrentFrame;		// Current Frame
		s32 NextFrame;			// Frame which will be used next. For blending

		s32 StartFrame;			// Absolute Frame where the current animation start
		s32 Frames;				// Relative Frames how much Frames this animation have
		s32 LoopingFrames;		// How much of Frames sould be looped
		s32 EndFrame;			// Absolute Frame where the current animation ends End = start + frames - 1

		f32 FramesPerSecond;	// Speed in Frames/Seconds the animation is played
		f32 RelativeSpeed;		// Factor Original fps is modified

		u32 BeginTime;			// Animation started at this thime
		u32 EndTime;			// Animation end at this time
		u32 LastTime;			// Last Keyframe was done at this time

		KeyFrameInterpolation ( const c8 * name = "", s32 start = 0, s32 frames = 0, s32 loopingframes = 0,
								f32 fps = 0.f, f32 relativefps = 1.f  )
			: Name ( name ), AnimationType ( loopingframes ? EAMT_LOOPING : EAMT_WAYPOINT),
			CurrentFrame ( (f32) start ), NextFrame ( start ), StartFrame ( start ),
			Frames ( frames ), LoopingFrames ( loopingframes ), EndFrame ( start + frames - 1 ),
			FramesPerSecond ( fps ), RelativeSpeed ( relativefps ),
			BeginTime ( 0 ), EndTime ( 0 ), LastTime ( 0 )
		{
		}

		// linear search
		bool operator == ( const KeyFrameInterpolation & other ) const
		{
			return Name.equals_ignore_case ( other.Name );
		}

	};


	//! a List holding named Animations
	typedef core::array < KeyFrameInterpolation > IAnimationList;

	//! a List holding named Skins
	typedef core::array < core::stringc > ISkinList;


	// Current Model per Body
	struct SubModel
	{
		core::stringc name;
		u32 startBuffer;
		u32 endBuffer;
		u32 state;
	};

	struct BodyPart
	{
		core::stringc name;
		u32 defaultModel;
		core::array < SubModel > model;
	};
	//! a List holding named Models and SubModels
	typedef core::array < BodyPart > IBodyList;


	//! Interface for an animated mesh.
	/** There are already simple implementations of this interface available so
	you don't have to implement this interface on your own if you need to:
	You might want to use irr::scene::SAnimatedMesh, irr::scene::SMesh,
	irr::scene::SMeshBuffer etc. */
	class IAnimatedMesh : public IMesh
	{
	public:

		//! Gets the frame count of the animated mesh.
		/** \return Returns the amount of frames. If the amount is 1,
		it is a static, non animated mesh. */
		virtual u32 getFrameCount() const = 0;

		//! Returns the IMesh interface for a frame.
		/** \param frame: Frame number as zero based index. The maximum
		frame number is getFrameCount() - 1;
		\param detailLevel: Level of detail. 0 is the lowest, 255 the
		highest level of detail. Most meshes will ignore the detail level.
		\param startFrameLoop: Because some animated meshes (.MD2) are
		blended between 2 static frames, and maybe animated in a loop,
		the startFrameLoop and the endFrameLoop have to be defined, to
		prevent the animation to be blended between frames which are
		outside of this loop.
		If startFrameLoop and endFrameLoop are both -1, they are ignored.
		\param endFrameLoop: see startFrameLoop.
		\return Returns the animated mesh based on a detail level. */
		virtual IMesh* getMesh(s32 frame, s32 detailLevel=255, s32 startFrameLoop=-1, s32 endFrameLoop=-1) = 0;

		//! Returns the type of the animated mesh.
		/** In most cases it is not neccessary to use this method.
		This is useful for making a safe downcast. For example,
		if getMeshType() returns EAMT_MD2 it's safe to cast the
		IAnimatedMesh to IAnimatedMeshMD2.
		\returns Type of the mesh. */
		virtual E_ANIMATED_MESH_TYPE getMeshType() const
		{
			return EAMT_UNKNOWN;
		}
	};

} // end namespace scene
} // end namespace irr

#endif

