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
		/* This method initialize EGLand create EGL display, anyway surface and context
		aren't create. */
        bool initialize();

        // Terminate EGL.
		/* Terminate EGL context. This method break both existed surface and context. */
        void terminate();

        // Create EGL surface.
		/* This method create EGL surface. On some platforms eg. Android, we must
		recreate surface on each resume, because WindowID may change, so existed
		surface may not be valid. If EGL context already exist, this method
		automatically activates it. */
        bool createSurface();

        // Destroy EGL surface.
		/* This method destroy EGL. On some platforms eg. Android, we should call
		this method on each pause, because after resume this surface may not be valid.
		Hovewer this method doesn'r break EGL context. */
        void destroySurface();

        // Create EGL context.
		/* This method create and activate EGL context. */
        bool createContext();

        // Destroy EGL context.
		/* This method destroy EGL context. */
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

