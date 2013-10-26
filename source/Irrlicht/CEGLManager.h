// Copyright (C) 2013 Patryk Nadrowski
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#ifndef __C_EGL_MANAGER_H_INCLUDED__
#define __C_EGL_MANAGER_H_INCLUDED__

#include "IrrCompileConfig.h"

#if defined(_IRR_COMPILE_WITH_ANDROID_DEVICE_)
#include <EGL/egl.h>
#else
#include <GLES/egl.h>
#endif

#include "SIrrCreationParameters.h"
#include "SExposedVideoData.h"
#include "IContextManager.h"

namespace irr
{
namespace video
{
    // EGL manager.
    class CEGLManager : public IContextManager
    {
    public:
        //! Constructor.
        CEGLManager(const SIrrlichtCreationParameters& params, SExposedVideoData* data);

        //! Destructor.
        virtual ~CEGLManager();

        // Initialize EGL.
        bool initialize();

        // Terminate EGL.
        void terminate();

        // Create EGL surface.
        bool createSurface();

        // Destroy EGL surface.
        void destroySurface();

        // Create EGL context.
        bool createContext();

        // Destroy EGL context.
        void destroyContext();

        // Swap buffers.
        void swapBuffers();

    private:
        bool testEGLError();

		NativeWindowType EglWindow;
		EGLDisplay EglDisplay;
		EGLSurface EglSurface;
		EGLContext EglContext;

        EGLConfig EglConfig;

#ifdef _IRR_COMPILE_WITH_WINDOWS_DEVICE_
		HDC HDc;
#endif

        SIrrlichtCreationParameters Params;
        SExposedVideoData* Data;

        EGLint MajorVersion;
        EGLint MinorVersion;
    };
}
}

#endif

