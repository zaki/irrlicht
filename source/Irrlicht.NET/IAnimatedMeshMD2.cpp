// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "IAnimatedMeshMD2.h"

namespace Irrlicht
{
namespace Scene
{


IAnimatedMeshMD2::IAnimatedMeshMD2(irr::scene::IAnimatedMeshMD2* realMesh)
: IAnimatedMesh(realMesh)
{
}

void IAnimatedMeshMD2::GetFrameLoop(MD2AnimationType l, int& outBegin,
			int& outEnd, int& outFPS)
{
	getMD2()->getFrameLoop((irr::scene::EMD2_ANIMATION_TYPE)l, outBegin, outEnd, outFPS);
}

bool IAnimatedMeshMD2::GetFrameLoop(System::String* name, int& outBegin, 
									int& outEnd, int& outFps)
{
	char* str = (char*)(void*)
			System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(name);

	bool ret = getMD2()->getFrameLoop(str, outBegin, outEnd, outFps);

	System::Runtime::InteropServices::Marshal::FreeHGlobal(str);

	return ret;
}

int IAnimatedMeshMD2::get_AnimationCount()
{
	return getMD2()->getAnimationCount();
}

System::String* IAnimatedMeshMD2::GetAnimationName(int nr)
{
	return new System::String(getMD2()->getAnimationName(nr));
}

} // end namespace scene
} // end namespace irr


