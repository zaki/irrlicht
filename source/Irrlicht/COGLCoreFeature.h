// Copyright (C) 2015 Patryk Nadrowski
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_OGLCORE_COMMON_H_INCLUDED__
#define __C_OGLCORE_COMMON_H_INCLUDED__

#include "IrrCompileConfig.h"

#if defined(_IRR_COMPILE_WITH_OPENGL_) || defined(_IRR_COMPILE_WITH_OGLES1_) || defined(_IRR_COMPILE_WITH_OGLES2_)

#include "irrTypes.h"

namespace irr
{
namespace video
{

class COGLCoreFeature
{
public:
	COGLCoreFeature() : BlendOperation(false), ColorAttachment(0), MultipleRenderTarget(0), TextureUnit(1)
	{
	}

	virtual ~COGLCoreFeature()
	{
	}

	bool BlendOperation;
	
	u8 ColorAttachment;
	u8 MultipleRenderTarget;
	u8 TextureUnit;
};

}
}

#endif
#endif
