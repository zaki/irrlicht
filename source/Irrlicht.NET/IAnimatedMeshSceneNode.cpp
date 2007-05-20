// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "IAnimatedMeshSceneNode.h"


namespace Irrlicht
{
namespace Scene
{

	IAnimatedMeshSceneNode::IAnimatedMeshSceneNode(irr::scene::IAnimatedMeshSceneNode* realSceneNode)
		: ISceneNode(realSceneNode)
	{
	}

	void IAnimatedMeshSceneNode::set_CurrentFrame(int frame)
	{
		getAnimatedMeshSceneNode()->setCurrentFrame(frame);
	}

	int IAnimatedMeshSceneNode::get_CurrentFrame()
	{
		return getAnimatedMeshSceneNode()->getFrameNr();
	}

	bool IAnimatedMeshSceneNode::SetFrameLoop(int begin, int end)
	{
		return getAnimatedMeshSceneNode()->setFrameLoop(begin, end);
	}

	void IAnimatedMeshSceneNode::set_AnimationSpeed(float framesPerSecond)
	{
		getAnimatedMeshSceneNode()->setAnimationSpeed(framesPerSecond);
	}

	ISceneNode* IAnimatedMeshSceneNode::AddShadowVolumeSceneNode(int id, 
		bool zfailmethod, float infinity)
	{
		irr::scene::ISceneNode* shadow = 
			getAnimatedMeshSceneNode()->addShadowVolumeSceneNode(id, zfailmethod, infinity);

		if ( shadow )
			return new ISceneNode(shadow);

		return 0;
	}

	ISceneNode* IAnimatedMeshSceneNode::AddShadowVolumeSceneNode()
	{
		return AddShadowVolumeSceneNode(-1, true, 10000);
	}

	ISceneNode* IAnimatedMeshSceneNode::GetMS3DJointNode(System::String* jointName)
	{
		char* str = (char*)(void*)
			System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(jointName);

		irr::scene::ISceneNode* node = 
			getAnimatedMeshSceneNode()->getMS3DJointNode(str);

		System::Runtime::InteropServices::Marshal::FreeHGlobal(str);

		if (!node)
			return 0;

        return new ISceneNode(node);
	}

	bool IAnimatedMeshSceneNode::SetMD2Animation(MD2AnimationType anim)
	{
		return getAnimatedMeshSceneNode()->setMD2Animation((irr::scene::EMD2_ANIMATION_TYPE)anim);
	}

	bool IAnimatedMeshSceneNode::SetMD2Animation(System::String* animationName)
	{
		char* str = (char*)(void*)
			System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(animationName);
		bool ret = getAnimatedMeshSceneNode()->setMD2Animation(str);

		System::Runtime::InteropServices::Marshal::FreeHGlobal(str);

		return ret;
	}

}
}
