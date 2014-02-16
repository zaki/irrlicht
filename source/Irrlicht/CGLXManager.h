// Copyright (C) 2013 Christian Stehno
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#ifndef __C_GLX_MANAGER_H_INCLUDED__
#define __C_GLX_MANAGER_H_INCLUDED__

#include "IrrCompileConfig.h"

#ifdef _IRR_COMPILE_WITH_GLX_MANAGER_

#include "SIrrCreationParameters.h"
#include "SExposedVideoData.h"
#include "IContextManager.h"
#include "SColor.h"
#include <X11/Xlib.h>
#include <X11/Xutil.h>

// we can't include glx.h here, because gl.h has incompatible types with ogl es headers and it
// cause redefinition errors, thats why we use ugly trick with void* types and casts.

namespace irr
{
namespace video
{
    // GLX manager.
    class CGLXManager : public IContextManager
    {
    public:
        //! Constructor.
        CGLXManager(const SIrrlichtCreationParameters& params, const SExposedVideoData& videodata, int screennr);

        //! Destructor
        ~CGLXManager();

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

        // Destroy context.
        void destroyContext();

        //! Get current context
        const SExposedVideoData& getContext() const;

        //! Change render context, disable old and activate new defined by videoData
        bool activateContext(const SExposedVideoData& videoData);

        // Swap buffers.
        bool swapBuffers();

        XVisualInfo* getVisual() const {return visual;} // return XVisualInfo

    private:
        SIrrlichtCreationParameters Params;
        SExposedVideoData PrimaryContext;
        SExposedVideoData CurrentContext;
        XVisualInfo* visual;
        void* glxFBConfig; // GLXFBConfig
        XID glxWin; // GLXWindow
        ECOLOR_FORMAT ColorFormat;
	};
}
}

#endif

#endif

