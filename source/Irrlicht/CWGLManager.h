// Copyright (C) 2013 Christian Stehno
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#ifndef __C_WGL_MANAGER_H_INCLUDED__
#define __C_WGL_MANAGER_H_INCLUDED__

#include "IrrCompileConfig.h"

#ifdef _IRR_COMPILE_WITH_WGL_MANAGER_

#include "SIrrCreationParameters.h"
#include "SExposedVideoData.h"
#include "IContextManager.h"
#include "SColor.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace irr
{
namespace video
{
    // WGL manager.
    class CWGLManager : public IContextManager
    {
    public:
        //! Constructor.
        CWGLManager();

		//! Destructor
		~CWGLManager();

        // Initialize
        bool initialize(const SIrrlichtCreationParameters& params, const SExposedVideoData& data);

        // Terminate
        void terminate();

        // Create surface.
        bool generateSurface();

        // Destroy surface.
        void destroySurface();

        // Create context.
        bool generateContext();

        // Destroy EGL context.
        void destroyContext();

		//! Get current context
		const SExposedVideoData& getContext() const;

		//! Change render context, disable old and activate new defined by videoData
		bool activateContext(const SExposedVideoData& videoData);

        // Swap buffers.
        bool swapBuffers();

    private:
        SIrrlichtCreationParameters Params;
		SExposedVideoData PrimaryContext;
        SExposedVideoData CurrentContext;
		s32 PixelFormat;
		PIXELFORMATDESCRIPTOR pfd;
		ECOLOR_FORMAT ColorFormat;
	};
}
}

#endif

#endif
