// Copyright (C) 2013 Patryk Nadrowski
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#include "CEGLManager.h"

#ifdef _IRR_COMPILE_WITH_EGL_MANAGER_

#include "irrString.h"
#include "os.h"

namespace irr
{
namespace video
{

CEGLManager::CEGLManager() : IContextManager(), EglWindow(0), EglDisplay(EGL_NO_DISPLAY),
    EglSurface(EGL_NO_SURFACE), EglContext(EGL_NO_CONTEXT), EglConfig(0), MajorVersion(0), MinorVersion(0)
{
	#ifdef _DEBUG
	setDebugName("CEGLManager");
	#endif
}

CEGLManager::~CEGLManager()
{
    destroyContext();
    destroySurface();
    terminate();
}

bool CEGLManager::initialize(const SIrrlichtCreationParameters& params, const SExposedVideoData& data)
{
	// store new data
	Params=params;
	Data=data;

	if (EglWindow != 0 && EglDisplay != EGL_NO_DISPLAY)
        return true;

	// Window is depend on platform.
#if defined(_IRR_COMPILE_WITH_WINDOWS_DEVICE_)
	EglWindow = (NativeWindowType)Data.OpenGLWin32.HWnd;
	Data.OpenGLWin32.HDc = GetDC((HWND)EglWindow);
	EglDisplay = eglGetDisplay((NativeDisplayType)Data.OpenGLWin32.HDc);
#elif defined(_IRR_COMPILE_WITH_X11_DEVICE_)
	EglWindow = (NativeWindowType)Data.OpenGLLinux.X11Window;
	EglDisplay = eglGetDisplay((NativeDisplayType)Data.OpenGLLinux.X11Display);
#elif defined(_IRR_COMPILE_WITH_ANDROID_DEVICE_)
	EglWindow =	(ANativeWindow*)Data.OGLESAndroid.Window;
	EglDisplay = eglGetDisplay((NativeDisplayType) EGL_DEFAULT_DISPLAY);
#endif

	// We must check if EGL display is valid.
	if (EglDisplay == EGL_NO_DISPLAY)
    {
		os::Printer::log("Could not get EGL display.");
		terminate();
        return false;
    }

	// Initialize EGL here.
	if (!eglInitialize(EglDisplay, &MajorVersion, &MinorVersion))
    {
		os::Printer::log("Could not initialize EGL display.");

        EglDisplay = EGL_NO_DISPLAY;
		terminate();
        return false;
    }
	else
		os::Printer::log("EGL version", core::stringc(MajorVersion+(MinorVersion*0.1f)).c_str());

    return true;
}

void CEGLManager::terminate()
{
    if (EglWindow == 0 && EglDisplay == EGL_NO_DISPLAY)
        return;

	if (EglDisplay != EGL_NO_DISPLAY)
	{
		// We should unbind current EGL context before terminate EGL.
		eglMakeCurrent(EglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

		eglTerminate(EglDisplay);
		EglDisplay = EGL_NO_DISPLAY;
	}

#if defined(_IRR_COMPILE_WITH_WINDOWS_DEVICE_)
	if (Data.OpenGLWin32.HDc)
    {
		ReleaseDC((HWND)EglWindow, (HDC)Data.OpenGLWin32.HDc);
        Data.OpenGLWin32.HDc = 0;
    }
#endif

    MajorVersion = 0;
    MinorVersion = 0;
}

bool CEGLManager::generateSurface()
{
    if (EglDisplay == EGL_NO_DISPLAY)
        return false;

    if (EglSurface != EGL_NO_SURFACE)
        return true;

	// We should assign new WindowID on platforms, where WindowID may change at runtime,
	// at this time only Android support this feature.
	// this needs an update method instead!

#if defined(_IRR_COMPILE_WITH_ANDROID_DEVICE_)
	EglWindow = (ANativeWindow*)Data.OGLESAndroid.Window;
#endif

	EGLint EglOpenGLBIT = 0;

	// We need proper OpenGL BIT.
	switch (Params.DriverType)
	{
	case EDT_OGLES1:
		EglOpenGLBIT = EGL_OPENGL_ES_BIT;
		break;
	case EDT_OGLES2:
		EglOpenGLBIT = EGL_OPENGL_ES2_BIT;
		break;
	default:
		break;
	}

	EGLint Attribs[] =
	{
		EGL_RED_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_BLUE_SIZE, 8,
		EGL_ALPHA_SIZE, Params.WithAlphaChannel ? 1:0,
		EGL_BUFFER_SIZE, Params.Bits,
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_DEPTH_SIZE, Params.ZBufferBits,
		EGL_STENCIL_SIZE, Params.Stencilbuffer,
		EGL_SAMPLE_BUFFERS, Params.AntiAlias ? 1:0,
		EGL_SAMPLES, Params.AntiAlias,
#ifdef EGL_VERSION_1_3
		EGL_RENDERABLE_TYPE, EglOpenGLBIT,
#endif
		EGL_NONE, 0	
	};

	EglConfig = 0;
	EGLint NumConfigs = 0;
	u32 Steps = 5;

	// Choose the best EGL config.
	while (!eglChooseConfig(EglDisplay, Attribs, &EglConfig, 1, &NumConfigs) || !NumConfigs)
	{
		switch (Steps)
		{
		case 5: // samples
			if (Attribs[19] > 2)
				--Attribs[19];
			else
			{
				Attribs[17] = 0;
				Attribs[19] = 0;
				--Steps;
			}
			break;
		case 4: // alpha
			if (Attribs[7])
			{
				Attribs[7] = 0;

				if (Params.AntiAlias)
				{
					Attribs[17] = 1;
					Attribs[19] = Params.AntiAlias;
					Steps = 5;
				}
			}
			else
				--Steps;
			break;
		case 3: // stencil
			if (Attribs[15])
			{
				Attribs[15] = 0;

				if (Params.AntiAlias)
				{
					Attribs[17] = 1;
					Attribs[19] = Params.AntiAlias;
					Steps = 5;
				}
			}
			else
				--Steps;
			break;
		case 2: // depth size
			if (Attribs[13] > 16)
			{
				Attribs[13] -= 8;
			}
			else
				--Steps;
			break;
		case 1: // buffer size
			if (Attribs[9] > 16)
			{
				Attribs[9] -= 8;
			}
			else
				--Steps;
			break;
		default:
			os::Printer::log("Could not get config for EGL display.");
			return false;
		}
	}

	if (Params.AntiAlias && !Attribs[17])
		os::Printer::log("No multisampling.");

	if (Params.WithAlphaChannel && !Attribs[7])
		os::Printer::log("No alpha.");

	if (Params.Stencilbuffer && !Attribs[15])
		os::Printer::log("No stencil buffer.");

	if (Params.ZBufferBits > Attribs[13])
		os::Printer::log("No full depth buffer.");

	if (Params.Bits > Attribs[9])
		os::Printer::log("No full color buffer.");

#if defined(_IRR_COMPILE_WITH_ANDROID_DEVICE_)
    EGLint Format = 0;
    eglGetConfigAttrib(EglDisplay, EglConfig, EGL_NATIVE_VISUAL_ID, &Format);

    ANativeWindow_setBuffersGeometry(EglWindow, 0, 0, Format);
#endif
   
	// Now we are able to create EGL surface.
	EglSurface = eglCreateWindowSurface(EglDisplay, EglConfig, EglWindow, 0);
	
	if (EGL_NO_SURFACE == EglSurface)
		EglSurface = eglCreateWindowSurface(EglDisplay, EglConfig, 0, 0);

	if (EGL_NO_SURFACE == EglSurface)
		os::Printer::log("Could not create EGL surface.");

#ifdef EGL_VERSION_1_2
	if (MinorVersion > 1)
		eglBindAPI(EGL_OPENGL_ES_API);
#endif

    if (Params.Vsync)
		eglSwapInterval(EglDisplay, 1);

    return true;
}

void CEGLManager::destroySurface()
{
    if (EglSurface == EGL_NO_SURFACE)
        return;

	// We should unbind current EGL context before destroy EGL surface.
	eglMakeCurrent(EglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

    eglDestroySurface(EglDisplay, EglSurface);
    EglSurface = EGL_NO_SURFACE;
}

bool CEGLManager::generateContext()
{
    if (EglDisplay == EGL_NO_DISPLAY || EglSurface == EGL_NO_SURFACE)
        return false;

    if (EglContext != EGL_NO_CONTEXT)
        return true;

	EGLint OpenGLESVersion = 0;

	switch (Params.DriverType)
	{
	case EDT_OGLES1:
		OpenGLESVersion = 1;
		break;
	case EDT_OGLES2:
		OpenGLESVersion = 2;
		break;
	default:
		break;
	}

    EGLint ContextAttrib[] =
	{
#ifdef EGL_VERSION_1_3
		EGL_CONTEXT_CLIENT_VERSION, OpenGLESVersion,
#endif
		EGL_NONE, 0
	};

	EglContext = eglCreateContext(EglDisplay, EglConfig, EGL_NO_CONTEXT, ContextAttrib);

	if (testEGLError())
	{
		os::Printer::log("Could not create EGL context.", ELL_ERROR);
		return false;
	}
    return true;
}

void CEGLManager::destroyContext()
{
    if (EglContext == EGL_NO_CONTEXT)
        return;

	// We must unbind current EGL context before destroy it.
	eglMakeCurrent(EglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	eglDestroyContext(EglDisplay, EglContext);

    EglContext = EGL_NO_CONTEXT;
}

bool CEGLManager::activateContext(const SExposedVideoData& videoData)
{
	eglMakeCurrent(EglDisplay, EglSurface, EglSurface, EglContext);

	if (testEGLError())
	{
		os::Printer::log("Could not make EGL context current.");
		return false;
	}
	return true;
}

const SExposedVideoData& CEGLManager::getContext() const
{
	return Data;
}

bool CEGLManager::swapBuffers()
{
    return (eglSwapBuffers(EglDisplay, EglSurface)==EGL_TRUE);
}

bool CEGLManager::testEGLError()
{
#if defined(EGL_VERSION_1_0) && defined(_DEBUG)
	EGLint status = eglGetError();

	switch (status)
	{
		case EGL_SUCCESS:
            return false;
		case EGL_NOT_INITIALIZED :
			os::Printer::log("Not Initialized", ELL_ERROR);
            break;
		case EGL_BAD_ACCESS:
			os::Printer::log("Bad Access", ELL_ERROR);
            break;
		case EGL_BAD_ALLOC:
			os::Printer::log("Bad Alloc", ELL_ERROR);
            break;
		case EGL_BAD_ATTRIBUTE:
			os::Printer::log("Bad Attribute", ELL_ERROR);
            break;
		case EGL_BAD_CONTEXT:
			os::Printer::log("Bad Context", ELL_ERROR);
            break;
		case EGL_BAD_CONFIG:
			os::Printer::log("Bad Config", ELL_ERROR);
            break;
		case EGL_BAD_CURRENT_SURFACE:
			os::Printer::log("Bad Current Surface", ELL_ERROR);
            break;
		case EGL_BAD_DISPLAY:
			os::Printer::log("Bad Display", ELL_ERROR);
            break;
		case EGL_BAD_SURFACE:
			os::Printer::log("Bad Surface", ELL_ERROR);
            break;
		case EGL_BAD_MATCH:
			os::Printer::log("Bad Match", ELL_ERROR);
            break;
		case EGL_BAD_PARAMETER:
			os::Printer::log("Bad Parameter", ELL_ERROR);
            break;
		case EGL_BAD_NATIVE_PIXMAP:
			os::Printer::log("Bad Native Pixmap", ELL_ERROR);
            break;
		case EGL_BAD_NATIVE_WINDOW:
			os::Printer::log("Bad Native Window", ELL_ERROR);
            break;
		case EGL_CONTEXT_LOST:
			os::Printer::log("Context Lost", ELL_ERROR);
            break;
        default:
            break;
	};

	return true;
#else
	return false;
#endif
}

}
}

#endif
