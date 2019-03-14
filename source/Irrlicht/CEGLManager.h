// Copyright (C) 2013 Patryk Nadrowski
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#ifndef __C_EGL_MANAGER_H_INCLUDED__
#define __C_EGL_MANAGER_H_INCLUDED__

#include "IrrCompileConfig.h"

#ifdef _IRR_COMPILE_WITH_EGL_MANAGER_

#if defined(_IRR_COMPILE_WITH_ANDROID_DEVICE_) || defined(_IRR_COMPILE_WITH_FB_DEVICE_) || defined(_IRR_COMPILE_WITH_WINDOWS_DEVICE_) || defined(__EMSCRIPTEN__)
#include <EGL/egl.h>
#else
#include <GLES/egl.h>
#endif

#include "SIrrCreationParameters.h"
#include "SExposedVideoData.h"
#include "IContextManager.h"

#ifdef _MSC_VER
#pragma comment(lib, "libEGL.lib")
#endif

namespace irr
{
namespace video
{
	// EGL manager.
	class CEGLManager : public IContextManager
	{
	public:
		//! Constructor.
		CEGLManager();

		//! Destructor.
		virtual ~CEGLManager();

		// Initialize EGL.
		/* This method initialize EGLand create EGL display, anyway surface and context
		aren't create. */
		virtual bool initialize(const SIrrlichtCreationParameters& params, const SExposedVideoData& data) _IRR_OVERRIDE_;

		// Terminate EGL.
		/* Terminate EGL context. This method break both existed surface and context. */
		virtual void terminate() _IRR_OVERRIDE_;

		// Create EGL surface.
		/* This method create EGL surface. On some platforms eg. Android, we must
		recreate surface on each resume, because WindowID may change, so existed
		surface may not be valid. If EGL context already exist, this method
		automatically activates it. */
		virtual bool generateSurface() _IRR_OVERRIDE_;

		// Destroy EGL surface.
		/* This method destroy EGL. On some platforms eg. Android, we should call
		this method on each pause, because after resume this surface may not be valid.
		Hovewer this method doesn'r break EGL context. */
		virtual void destroySurface() _IRR_OVERRIDE_;

		// Create EGL context.
		/* This method create and activate EGL context. */
		virtual bool generateContext() _IRR_OVERRIDE_;

		// Destroy EGL context.
		/* This method destroy EGL context. */
		virtual void destroyContext() _IRR_OVERRIDE_;

		virtual const SExposedVideoData& getContext() const _IRR_OVERRIDE_;

		virtual bool activateContext(const SExposedVideoData& videoData, bool restorePrimaryOnZero) _IRR_OVERRIDE_;

		// Swap buffers.
		virtual bool swapBuffers() _IRR_OVERRIDE_;

	protected:
		enum EConfigStyle
		{
			//! Get first result of eglChooseConfigs and if that fails try again by requesting simpler attributes
			ECS_EGL_CHOOSE_FIRST_LOWER_EXPECTATIONS,

			//! We select our own best fit and avoid using eglChooseConfigs
			ECS_IRR_CHOOSE,
		};

		EGLConfig chooseConfig(EConfigStyle confStyle);

		//! Check how close this config is to the parameters we requested
		//! returns 0 is perfect, larger values are worse and < 0 is unusable.
		irr::s32 rateConfig(EGLConfig config, EGLint eglOpenGLBIT, bool log=false);

		// Helper to sort EGLConfig's. (because we got no std::pair....)
		struct SConfigRating
		{
			EGLConfig config;
			irr::s32 rating;
			bool operator<(const SConfigRating& other) const
			{
				return rating < other.rating;
			}
		};

	private:
		bool testEGLError();

		NativeWindowType EglWindow;
		EGLDisplay EglDisplay;
		EGLSurface EglSurface;
		EGLContext EglContext;

		EGLConfig EglConfig;

		SIrrlichtCreationParameters Params;
		SExposedVideoData Data;

		EGLint MajorVersion;
		EGLint MinorVersion;
	};
}
}
#endif
#endif
