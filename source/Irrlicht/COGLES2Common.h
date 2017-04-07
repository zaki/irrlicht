// Copyright (C) 2015 Patryk Nadrowski
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_OGLES2_COMMON_H_INCLUDED__
#define __C_OGLES2_COMMON_H_INCLUDED__

#include "IrrCompileConfig.h"

#ifdef _IRR_COMPILE_WITH_OGLES2_

#if defined(_IRR_COMPILE_WITH_IOS_DEVICE_)
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#elif defined(_IRR_COMPILE_WITH_ANDROID_DEVICE_)
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <EGL/eglplatform.h>
#else
#if defined(_IRR_OPENGL_USE_EXTPOINTER_)
	#define GL_GLEXT_PROTOTYPES 1
	#define GLX_GLXEXT_PROTOTYPES 1
#endif
#include <GLES2/gl2.h>
#include <EGL/eglplatform.h>
typedef char GLchar;
#if defined(_IRR_OGLES2_USE_EXTPOINTER_)
#include "gles2-ext.h"
#endif
#endif

#ifndef GL_BGRA
#define GL_BGRA 0x80E1;
#endif

// FBO definitions.

#define GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER 1
#define GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER 2
#define GL_FRAMEBUFFER_INCOMPLETE_FORMATS 3

// Irrlicht's OpenGL version.

#define IRR_OPENGL_VERSION 20

namespace irr
{
namespace video
{

	// Forward declarations.

	class COpenGLCoreFeature;

	template <class TOpenGLDriver>
	class COpenGLCoreTexture;

	template <class TOpenGLDriver, class TOpenGLTexture>
	class COpenGLCoreRenderTarget;

	template <class TOpenGLDriver, class TOpenGLTexture>
	class COpenGLCoreCacheHandler;

	class COGLES2Driver;
	typedef COpenGLCoreTexture<COGLES2Driver> COGLES2Texture;
	typedef COpenGLCoreRenderTarget<COGLES2Driver, COGLES2Texture> COGLES2RenderTarget;
	typedef COpenGLCoreCacheHandler<COGLES2Driver, COGLES2Texture> COGLES2CacheHandler;

}
}

#endif
#endif
