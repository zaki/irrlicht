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

CEGLManager::CEGLManager(const SIrrlichtCreationParameters& params, SExposedVideoData& data) :
    EglSurface(EGL_NO_SURFACE), EglContext(EGL_NO_CONTEXT), EglConfig(0), EglReady(false), Params(params), Data(data)
{
#if defined(_IRR_COMPILE_WITH_WINDOWS_DEVICE_)
	EglWindow = (NativeWindowType)data.OpenGLWin32.HWnd;
	HDc = GetDC((HWND)EglWindow);
	EglDisplay = eglGetDisplay((NativeDisplayType)HDc);
#elif defined(_IRR_COMPILE_WITH_X11_DEVICE_)
	EglWindow = (NativeWindowType)ExposedData.OpenGLLinux.X11Window;
	EglDisplay = eglGetDisplay((NativeDisplayType)ExposedData.OpenGLLinux.X11Display);
#elif defined(_IRR_COMPILE_WITH_ANDROID_DEVICE_)
	EglWindow =	(ANativeWindow*)data.OGLESAndroid.window;
    EglDisplay = eglGetDisplay((NativeDisplayType) EGL_DEFAULT_DISPLAY);
#endif

	if (EglDisplay == EGL_NO_DISPLAY)
		os::Printer::log("Could not get EGL display.");
}

CEGLManager::~CEGLManager()
{
    destroyContext();
    destroyEGL();

#if defined(_IRR_COMPILE_WITH_WINDOWS_DEVICE_)
	if (HDc)
		ReleaseDC((HWND)EglWindow, HDc);
#endif
}

bool CEGLManager::createEGL()
{
    if (EglDisplay == EGL_NO_DISPLAY)
        return false;

    if (EglReady)
        return true;

	EGLint MajorVersion = 0, MinorVersion = 0;

	if (!eglInitialize(EglDisplay, &MajorVersion, &MinorVersion))
		os::Printer::log("Could not initialize EGL display.");
	else
		os::Printer::log("EGL version", core::stringc(MajorVersion+(MinorVersion*0.1f)).c_str());

	EGLint Attribs[] =
	{
#if defined( _IRR_COMPILE_WITH_ANDROID_DEVICE_ )
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_BLUE_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_RED_SIZE, 8,
		EGL_DEPTH_SIZE, Params.ZBufferBits,
		EGL_NONE
#else		
		EGL_RED_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_BLUE_SIZE, 8,
		EGL_ALPHA_SIZE, params.WithAlphaChannel ? 1:0,
		EGL_BUFFER_SIZE, params.Bits,
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_DEPTH_SIZE, params.ZBufferBits,
		EGL_STENCIL_SIZE, params.Stencilbuffer,
		EGL_SAMPLE_BUFFERS, params.AntiAlias ? 1:0,
		EGL_SAMPLES, params.AntiAlias,
#ifdef EGL_VERSION_1_3
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES_BIT,
#endif
		EGL_NONE, 0
#endif		
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
    /* EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
    * guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
    * As soon as we picked a EGLConfig, we can safely reconfigure the
    * ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID. */
    EGLint Format = 0;
    eglGetConfigAttrib(EglDisplay, EglConfig, EGL_NATIVE_VISUAL_ID, &Format);

    ANativeWindow_setBuffersGeometry(EglWindow, 0, 0, Format);
#endif
   
	EglSurface = eglCreateWindowSurface(EglDisplay, EglConfig, EglWindow, 0);
	
	if (EGL_NO_SURFACE == EglSurface)
		EglSurface = eglCreateWindowSurface(EglDisplay, EglConfig, 0, 0);

	if (EGL_NO_SURFACE == EglSurface)
    {
		os::Printer::log("Could not create EGL surface.");
        eglTerminate(EglDisplay);
	}

#ifdef EGL_VERSION_1_2
	if (MinorVersion > 1)
		eglBindAPI(EGL_OPENGL_ES_API);
#endif

    if (Params.Vsync)
		eglSwapInterval(EglDisplay, 1);

    if (EglContext != EGL_NO_CONTEXT)
        eglMakeCurrent(EglDisplay, EglSurface, EglSurface, EglContext);

    EglReady = true;

    return true;
}

bool CEGLManager::destroyEGL()
{
    if (!EglReady)
        return false;

    eglMakeCurrent(EglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

    eglDestroySurface(EglDisplay, EglSurface);
	eglTerminate(EglDisplay);

    EglSurface = EGL_NO_SURFACE;

#if defined(_IRR_COMPILE_WITH_WINDOWS_DEVICE_)
	if (HDc)
		ReleaseDC((HWND)EglWindow, HDc);
#endif

    EglReady = false;

    return true;
}

bool CEGLManager::createContext()
{
    if (EglDisplay == EGL_NO_DISPLAY || !EglReady)
        return false;

    if (EglContext != EGL_NO_CONTEXT)
        return true;

    EGLint ContextAttrib[] =
	{
#ifdef EGL_VERSION_1_3
		EGL_CONTEXT_CLIENT_VERSION, 1,
#endif
		EGL_NONE, 0
	};

	EglContext = eglCreateContext(EglDisplay, EglConfig, EGL_NO_CONTEXT, ContextAttrib);

	if (testEGLError())
		os::Printer::log("Could not create EGL context.");

	eglMakeCurrent(EglDisplay, EglSurface, EglSurface, EglContext);

	if (testEGLError())
		os::Printer::log("Could not make EGL context current.");
}

bool CEGLManager::destroyContext()
{
    if (EglContext == EGL_NO_CONTEXT)
        return false;

    eglMakeCurrent(EglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	eglDestroyContext(EglDisplay, EglContext);

    EglContext = EGL_NO_CONTEXT;

    return true;
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

