// Copyright (C) 2013 Patryk Nadrowski
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#include "CEGLManager.h"

#include "irrString.h"
#include "os.h"

namespace irr
{
namespace video
{

CEGLManager::CEGLManager(const SIrrlichtCreationParameters& params, SExposedVideoData* data) : EglWindow(0), EglDisplay(EGL_NO_DISPLAY),
    EglSurface(EGL_NO_SURFACE), EglContext(EGL_NO_CONTEXT), EglConfig(0), Params(params), Data(data), MajorVersion(0), MinorVersion(0)
{
#ifdef _IRR_COMPILE_WITH_WINDOWS_DEVICE_
    HDc = 0;
#endif
}

CEGLManager::~CEGLManager()
{
    destroyContext();
    destroySurface();
    terminateEGL();
}

bool CEGLManager::initializeEGL()
{
    if (EglWindow != 0 && EglDisplay != EGL_NO_DISPLAY)
        return true;

#if defined(_IRR_COMPILE_WITH_WINDOWS_DEVICE_)
	EglWindow = (NativeWindowType)Data->OpenGLWin32.HWnd;
	HDc = GetDC((HWND)EglWindow);
	EglDisplay = eglGetDisplay((NativeDisplayType)HDc);
#elif defined(_IRR_COMPILE_WITH_X11_DEVICE_)
	EglWindow = (NativeWindowType)Data->OpenGLLinux.X11Window;
	EglDisplay = eglGetDisplay((NativeDisplayType)Data->OpenGLLinux.X11Display);
#elif defined(_IRR_COMPILE_WITH_ANDROID_DEVICE_)
	EglWindow =	(ANativeWindow*)Data->OGLESAndroid.window;
    EglDisplay = eglGetDisplay((NativeDisplayType) EGL_DEFAULT_DISPLAY);
#endif

	if (EglDisplay == EGL_NO_DISPLAY)
    {
		os::Printer::log("Could not get EGL display.");

#if defined(_IRR_COMPILE_WITH_WINDOWS_DEVICE_)
	    if (HDc)
        {
		    ReleaseDC((HWND)EglWindow, HDc);
            HDc = 0;
        }
#endif

        return false;
    }

	if (!eglInitialize(EglDisplay, &MajorVersion, &MinorVersion))
    {
		os::Printer::log("Could not initialize EGL display.");

        EglDisplay = EGL_NO_DISPLAY;

#if defined(_IRR_COMPILE_WITH_WINDOWS_DEVICE_)
	    if (HDc)
        {
		    ReleaseDC((HWND)EglWindow, HDc);
            HDc = 0;
        }
#endif

        return false;
    }
	else
		os::Printer::log("EGL version", core::stringc(MajorVersion+(MinorVersion*0.1f)).c_str());

    return true;
}

void CEGLManager::terminateEGL()
{
    if (EglWindow == 0 && EglDisplay == EGL_NO_DISPLAY)
        return;

    eglMakeCurrent(EglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

	eglTerminate(EglDisplay);
    EglDisplay = EGL_NO_DISPLAY;

#if defined(_IRR_COMPILE_WITH_WINDOWS_DEVICE_)
    if (HDc)
    {
        ReleaseDC((HWND)EglWindow, HDc);
        HDc = 0;
    }
#endif

    MajorVersion = 0;
    MinorVersion = 0;
}

bool CEGLManager::createSurface()
{
    if (EglDisplay == EGL_NO_DISPLAY)
        return false;

    if (EglSurface != EGL_NO_SURFACE)
        return true;

#if defined(_IRR_COMPILE_WITH_ANDROID_DEVICE_)
	EglWindow = (ANativeWindow*)Data->OGLESAndroid.window;
#endif

	EGLint EglOpenGLBIT = 0;

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

    if (EglContext != EGL_NO_CONTEXT)
        eglMakeCurrent(EglDisplay, EglSurface, EglSurface, EglContext);

    return true;
}

void CEGLManager::destroySurface()
{
    if (EglSurface == EGL_NO_SURFACE)
        return;

    eglMakeCurrent(EglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

    eglDestroySurface(EglDisplay, EglSurface);
    EglSurface = EGL_NO_SURFACE;
}

bool CEGLManager::createContext()
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
		os::Printer::log("Could not create EGL context.");

	eglMakeCurrent(EglDisplay, EglSurface, EglSurface, EglContext);

	if (testEGLError())
		os::Printer::log("Could not make EGL context current.");

    return true;
}

void CEGLManager::destroyContext()
{
    if (EglContext == EGL_NO_CONTEXT)
        return;

    eglMakeCurrent(EglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	eglDestroyContext(EglDisplay, EglContext);

    EglContext = EGL_NO_CONTEXT;
}

void CEGLManager::swapBuffers()
{
    eglSwapBuffers(EglDisplay, EglSurface);
}

bool CEGLManager::testEGLError()
{
#if defined(EGL_VERSION_1_0) && defined(_DEBUG)
	EGLint Status = eglGetError();

	switch (Status)
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

