// Copyright (C) 2002-2007 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "IrrCompileConfig.h"
#include "COpenGLDriver.h"

#ifdef _IRR_COMPILE_WITH_OPENGL_

#include "COpenGLTexture.h"
#include "COpenGLMaterialRenderer.h"
#include "COpenGLShaderMaterialRenderer.h"
#include "COpenGLSLMaterialRenderer.h"
#include "COpenGLNormalMapRenderer.h"
#include "COpenGLParallaxMapRenderer.h"
#include "CImage.h"
#include "os.h"

#ifdef _IRR_USE_SDL_DEVICE_
#include <SDL/SDL.h>
#endif

namespace irr
{
namespace video
{

// -----------------------------------------------------------------------
// WINDOWS CONSTRUCTOR
// -----------------------------------------------------------------------
#ifdef _IRR_USE_WINDOWS_DEVICE_
//! Windows constructor and init code
COpenGLDriver::COpenGLDriver(const core::dimension2d<s32>& screenSize, HWND window, bool fullscreen, bool stencilBuffer, io::IFileSystem* io, bool antiAlias)
: CNullDriver(io, screenSize),
	CurrentRenderMode(ERM_NONE), ResetRenderStates(true), Transformation3DChanged(true),
	StencilBuffer(stencilBuffer), AntiAlias(antiAlias),
	MultiTextureExtension(false), MultiSamplingExtension(false), AnisotropyExtension(false),
	ARBVertexProgramExtension(false), ARBFragmentProgramExtension(false),
	ARBShadingLanguage100Extension(false), SeparateStencilExtension(false),
	GenerateMipmapExtension(false), TextureCompressionExtension(false),
	TextureNPOTExtension(false), FramebufferObjectExtension(false), EXTPackedDepthStencil(false),
	RenderTargetTexture(0), LastSetLight(-1), MaxAnisotropy(1),
	MaxTextureUnits(1), MaxLights(1), CurrentRendertargetSize(0,0),
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	pGlActiveTextureARB(0), pGlClientActiveTextureARB(0),
	pGlGenProgramsARB(0), pGlBindProgramARB(0), pGlProgramStringARB(0),
	pGlDeleteProgramsARB(0), pGlProgramLocalParameter4fvARB(0),
	pGlCreateShaderObjectARB(0), pGlShaderSourceARB(0),
	pGlCompileShaderARB(0), pGlCreateProgramObjectARB(0), pGlAttachObjectARB(0),
	pGlLinkProgramARB(0), pGlUseProgramObjectARB(0), pGlDeleteObjectARB(0),
	pGlGetObjectParameterivARB(0), pGlGetUniformLocationARB(0),
	pGlUniform1ivARB(0), pGlUniform1fvARB(0), pGlUniform2fvARB(0), pGlUniform3fvARB(0), pGlUniform4fvARB(0), pGlUniformMatrix2fvARB(0),
	pGlUniformMatrix3fvARB(0), pGlUniformMatrix4fvARB(0), pGlGetActiveUniformARB(0), pGlPointParameterfARB(0), pGlPointParameterfvARB(0),
	pGlStencilFuncSeparate(0), pGlStencilOpSeparate(0),
	pGlStencilFuncSeparateATI(0), pGlStencilOpSeparateATI(0),
	#ifdef PFNGLCOMPRESSEDTEXIMAGE2DPROC
		pGlCompressedTexImage2D(0),
	#endif
#endif // _IRR_OPENGL_USE_EXTPOINTER_
	wglSwapIntervalEXT(0),
	pGlBindFramebufferEXT(0), pGlDeleteFramebuffersEXT(0), pGlGenFramebuffersEXT(0),
	pGlCheckFramebufferStatusEXT(0), pGlFramebufferTexture2DEXT(0),
	pGlBindRenderbufferEXT(0), pGlDeleteRenderbuffersEXT(0), pGlGenRenderbuffersEXT(0),
	pGlRenderbufferStorageEXT(0), pGlFramebufferRenderbufferEXT(0),
	HDc(0), Window(window), HRc(0)
{
	#ifdef _DEBUG
	setDebugName("COpenGLDriver");
	#endif
}

//! inits the open gl driver
bool COpenGLDriver::initDriver(const core::dimension2d<s32>& screenSize,
				HWND window, u32 bits, bool fullscreen, bool vsync)
{
	static	PIXELFORMATDESCRIPTOR pfd =	{
		sizeof(PIXELFORMATDESCRIPTOR),	// Size Of This Pixel Format Descriptor
		1,				// Version Number
		PFD_DRAW_TO_WINDOW |		// Format Must Support Window
		PFD_SUPPORT_OPENGL |		// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,		// Must Support Double Buffering
		PFD_TYPE_RGBA,			// Request An RGBA Format
		bits,				// Select Our Color Depth
		0, 0, 0, 0, 0, 0,		// Color Bits Ignored
		0,				// No Alpha Buffer
		0,				// Shift Bit Ignored
		0,				// No Accumulation Buffer
		0, 0, 0, 0,			// Accumulation Bits Ignored
		24,				// Z-Buffer (Depth Buffer)
		StencilBuffer ? 1 : 0,		// Stencil Buffer Depth
		0,				// No Auxiliary Buffer
		PFD_MAIN_PLANE,			// Main Drawing Layer
		0,				// Reserved
		0, 0, 0				// Layer Masks Ignored
	};

	for (int i=0; i<5; ++i)
	{
		if (i == 1)
		{
			if (StencilBuffer)
				os::Printer::log("Cannot create a GL device with stencil buffer, disabling stencil shadows.", ELL_WARNING);
			StencilBuffer = false;
			pfd.cStencilBits = 0;
		}
		else
		if (i == 2)
		{
			pfd.cDepthBits = 24;
		}
		if (i == 3) // might be checked twice, but shouldn't matter
		{
			pfd.cDepthBits = 16;
		}
		else
		if (i == 4)
		{
			os::Printer::log("Cannot create a GL device context.", ELL_ERROR);
			return false;
		}

		// get hdc
		if (!(HDc=GetDC(window)))
		{
			os::Printer::log("Cannot create a GL device context.", ELL_ERROR);
			continue;
		}

		GLuint PixelFormat;

		// choose pixelformat
		if (!(PixelFormat = ChoosePixelFormat(HDc, &pfd)))
		{
			os::Printer::log("Cannot find a suitable pixelformat.", ELL_ERROR);
			continue;
		}

		// set pixel format
		if(!SetPixelFormat(HDc, PixelFormat, &pfd))
		{
			os::Printer::log("Cannot set the pixel format.", ELL_ERROR);
			continue;
		}

		// create rendering context
		if (!(HRc=wglCreateContext(HDc)))
		{
			os::Printer::log("Cannot create a GL rendering context.", ELL_ERROR);
			continue;
		}

		// activate rendering context
		if(!wglMakeCurrent(HDc, HRc))
		{
			os::Printer::log("Cannot activate GL rendering context", ELL_ERROR);
			continue;
		}

		break;
	}

	genericDriverInit(screenSize);

	// set vsync
	if (wglSwapIntervalEXT)
		wglSwapIntervalEXT(vsync ? 1 : 0);

	// set exposed data
	ExposedData.OpenGLWin32.HDc = reinterpret_cast<s32>(HDc);
	ExposedData.OpenGLWin32.HRc = reinterpret_cast<s32>(HRc);
	ExposedData.OpenGLWin32.HWnd = reinterpret_cast<s32>(Window);

	return true;
}



//! win32 destructor
COpenGLDriver::~COpenGLDriver()
{
	deleteMaterialRenders();

	// I get a blue screen on my laptop, when I do not delete the
	// textures manually before releasing the dc. Oh how I love this.

	deleteAllTextures();

	if (HRc)
	{
		if (!wglMakeCurrent(0, 0))
			os::Printer::log("Release of dc and rc failed.", ELL_WARNING);

		if (!wglDeleteContext(HRc))
			os::Printer::log("Release of rendering context failed.", ELL_WARNING);

		HRc = 0;
	}

	if (HDc)
		ReleaseDC(Window, HDc);

	HDc = 0;
}
#endif //IRR_USE_WINDOWS_DEVICE_

// -----------------------------------------------------------------------
// MACOSX CONSTRUCTOR
// -----------------------------------------------------------------------
#ifdef MACOSX
//! Windows constructor and init code
COpenGLDriver::COpenGLDriver(const core::dimension2d<s32>& screenSize, bool fullscreen, bool stencilBuffer, CIrrDeviceMacOSX *device, io::IFileSystem* io, bool vsync, bool antiAlias)
: CNullDriver(io, screenSize),
	CurrentRenderMode(ERM_NONE), ResetRenderStates(true), Transformation3DChanged(true),
	StencilBuffer(stencilBuffer), AntiAlias(antiAlias),
	MultiTextureExtension(false), MultiSamplingExtension(false), AnisotropyExtension(false),
	ARBVertexProgramExtension(false), ARBFragmentProgramExtension(false),
	ARBShadingLanguage100Extension(false), SeparateStencilExtension(false),
	GenerateMipmapExtension(false), TextureCompressionExtension(false),
	TextureNPOTExtension(false), FramebufferObjectExtension(false), EXTPackedDepthStencil(false),
	RenderTargetTexture(0), LastSetLight(-1), MaxAnisotropy(1),
	MaxTextureUnits(1), MaxLights(1),
	CurrentRendertargetSize(0,0), _device(device)
{
	#ifdef _DEBUG
	setDebugName("COpenGLDriver");
	#endif
	genericDriverInit(screenSize);
}

COpenGLDriver::~COpenGLDriver()
{
	deleteAllTextures();
}

#endif

// -----------------------------------------------------------------------
// LINUX CONSTRUCTOR
// -----------------------------------------------------------------------
#ifdef _IRR_USE_LINUX_DEVICE_
//! Linux constructor and init code
COpenGLDriver::COpenGLDriver(const core::dimension2d<s32>& screenSize, bool fullscreen, bool stencilBuffer, io::IFileSystem* io, bool vsync, bool antiAlias)
: CNullDriver(io, screenSize),
	CurrentRenderMode(ERM_NONE), ResetRenderStates(true), Transformation3DChanged(true),
	StencilBuffer(stencilBuffer), AntiAlias(antiAlias),
	MultiTextureExtension(false), MultiSamplingExtension(false), AnisotropyExtension(false),
	ARBVertexProgramExtension(false), ARBFragmentProgramExtension(false),
	ARBShadingLanguage100Extension(false), SeparateStencilExtension(false),
	GenerateMipmapExtension(false), TextureCompressionExtension(false),
	TextureNPOTExtension(false), FramebufferObjectExtension(false), EXTPackedDepthStencil(false),
	RenderTargetTexture(0), LastSetLight(-1), MaxAnisotropy(1),
	MaxTextureUnits(1), MaxLights(1), CurrentRendertargetSize(0,0)
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	,pGlActiveTextureARB(0), pGlClientActiveTextureARB(0),
	pGlGenProgramsARB(0), pGlBindProgramARB(0), pGlProgramStringARB(0),
	pGlDeleteProgramsARB(0), pGlProgramLocalParameter4fvARB(0)
		#ifdef PFNGLCOMPRESSEDTEXIMAGE2DPROC
			,pGlCompressedTexImage2D(0)
		#endif
#ifdef GLX_SGI_swap_control
	,glxSwapIntervalSGI(0)
#endif
    ,pGlBindFramebufferEXT(0), pGlDeleteFramebuffersEXT(0), pGlGenFramebuffersEXT(0),
    pGlCheckFramebufferStatusEXT(0), pGlFramebufferTexture2DEXT(0),
    pGlBindRenderbufferEXT(0), pGlDeleteRenderbuffersEXT(0), pGlGenRenderbuffersEXT(0),
    pGlRenderbufferStorageEXT(0), pGlFramebufferRenderbufferEXT(0)
#endif
{
	#ifdef _DEBUG
	setDebugName("COpenGLDriver");
	#endif
	XWindow = glXGetCurrentDrawable();
	XDisplay = glXGetCurrentDisplay();
	ExposedData.OpenGLLinux.X11Display = XDisplay;
	ExposedData.OpenGLLinux.X11Window = XWindow;
	genericDriverInit(screenSize);

	// set vsync
#ifdef GLX_SGI_swap_control
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (vsync && glxSwapIntervalSGI)
		glxSwapIntervalSGI(1);
#else
	if (vsync)
		glXSwapIntervalSGI(1);
#endif
#endif
}

//! linux destructor
COpenGLDriver::~COpenGLDriver()
{
	deleteAllTextures();
}

#endif // _IRR_USE_LINUX_DEVICE_


// -----------------------------------------------------------------------
// SDL CONSTRUCTOR
// -----------------------------------------------------------------------
#ifdef _IRR_USE_SDL_DEVICE_
//! SDL constructor and init code
COpenGLDriver::COpenGLDriver(const core::dimension2d<s32>& screenSize, bool fullscreen, bool stencilBuffer, io::IFileSystem* io, bool vsync, bool antiAlias)
: CNullDriver(io, screenSize),
	CurrentRenderMode(ERM_NONE), ResetRenderStates(true), Transformation3DChanged(true),
	StencilBuffer(stencilBuffer), AntiAlias(antiAlias),
	MultiTextureExtension(false), MultiSamplingExtension(false), AnisotropyExtension(false),
	ARBVertexProgramExtension(false), ARBFragmentProgramExtension(false),
	ARBShadingLanguage100Extension(false), SeparateStencilExtension(false),
	GenerateMipmapExtension(false), TextureCompressionExtension(false),
	TextureNPOTExtension(false), FramebufferObjectExtension(false), EXTPackedDepthStencil(false),
	RenderTargetTexture(0), LastSetLight(-1), MaxAnisotropy(1),
	MaxTextureUnits(1), MaxLights(1), CurrentRendertargetSize(0,0)
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	,pGlActiveTextureARB(0), pGlClientActiveTextureARB(0),
	pGlGenProgramsARB(0), pGlBindProgramARB(0), pGlProgramStringARB(0),
	pGlDeleteProgramsARB(0), pGlProgramLocalParameter4fvARB(0)
		#ifdef PFNGLCOMPRESSEDTEXIMAGE2DPROC
			,pGlCompressedTexImage2D(0)
		#endif
    ,pGlBindFramebufferEXT(0), pGlDeleteFramebuffersEXT(0), pGlGenFramebuffersEXT(0),
    pGlCheckFramebufferStatusEXT(0), pGlFramebufferTexture2DEXT(0),
    pGlBindRenderbufferEXT(0), pGlDeleteRenderbuffersEXT(0), pGlGenRenderbuffersEXT(0),
    pGlRenderbufferStorageEXT(0), pGlFramebufferRenderbufferEXT(0)
#endif
{
	#ifdef _DEBUG
	setDebugName("COpenGLDriver");
	#endif

	genericDriverInit(screenSize);
}

//! SDL destructor
COpenGLDriver::~COpenGLDriver()
{
	deleteAllTextures();
}

#endif // _IRR_USE_SDL_DEVICE_



// -----------------------------------------------------------------------
// METHODS
// -----------------------------------------------------------------------

bool COpenGLDriver::genericDriverInit(const core::dimension2d<s32>& screenSize)
{
	Name=L"OpenGL ";
	Name.append(glGetString(GL_VERSION));
	s32 pos=Name.findNext(L' ', 7);
	if (pos != -1)
		Name=Name.subString(0, pos);
	printVersion();

	// print renderer information
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* vendor = glGetString(GL_VENDOR);
	if (renderer && vendor)
	{
		os::Printer::log((const c8*)renderer, (const c8*)vendor, ELL_INFORMATION);
	}

	for (u32 i=0; i<MATERIAL_MAX_TEXTURES; ++i)
		CurrentTexture[i]=0;
	// load extensions
	loadExtensions();

	glViewport(0, 0, screenSize.Width, screenSize.Height); // Reset The Current Viewport
	setAmbientLight(SColorf(0.0f,0.0f,0.0f,0.0f));
#ifdef GL_EXT_separate_specular_color
	if (EXTSeparateSpecularColor)
		glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);
#endif
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 1);
	glClearDepth(1.0);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glDepthFunc(GL_LEQUAL);
	glFrontFace( GL_CW );
// currently disabled, because often in software, and thus very slow
//	glHint(GL_POINT_SMOOTH_HINT, GL_FASTEST);
//	glHint(GL_LINE_SMOOTH_HINT, GL_FASTEST);
//	glEnable(GL_POINT_SMOOTH);
//	glEnable(GL_LINE_SMOOTH);

	if (AntiAlias && MultiSamplingExtension)
		glEnable(GL_MULTISAMPLE_ARB);

	// create material renderers
	createMaterialRenderers();

	// set the renderstates
	ResetRenderStates = true;
	setRenderStates3DMode();

	// set fog mode
	setFog(FogColor, LinearFog, FogStart, FogEnd, FogDensity, PixelFog, RangeFog);

	return true;
}


void COpenGLDriver::createMaterialRenderers()
{
	// create OpenGL material renderers

	addAndDropMaterialRenderer(new COpenGLMaterialRenderer_SOLID( this));
	addAndDropMaterialRenderer(new COpenGLMaterialRenderer_SOLID_2_LAYER( this));

	// add the same renderer for all lightmap types
	COpenGLMaterialRenderer_LIGHTMAP* lmr = new COpenGLMaterialRenderer_LIGHTMAP( this);
	addMaterialRenderer(lmr); // for EMT_LIGHTMAP:
	addMaterialRenderer(lmr); // for EMT_LIGHTMAP_ADD:
	addMaterialRenderer(lmr); // for EMT_LIGHTMAP_M2:
	addMaterialRenderer(lmr); // for EMT_LIGHTMAP_M4:
	addMaterialRenderer(lmr); // for EMT_LIGHTMAP_LIGHTING:
	addMaterialRenderer(lmr); // for EMT_LIGHTMAP_LIGHTING_M2:
	addMaterialRenderer(lmr); // for EMT_LIGHTMAP_LIGHTING_M4:
	lmr->drop();

	// add remaining material renderer
	addAndDropMaterialRenderer(new COpenGLMaterialRenderer_DETAIL_MAP( this));
	addAndDropMaterialRenderer(new COpenGLMaterialRenderer_SPHERE_MAP( this));
	addAndDropMaterialRenderer(new COpenGLMaterialRenderer_REFLECTION_2_LAYER( this));
	addAndDropMaterialRenderer(new COpenGLMaterialRenderer_TRANSPARENT_ADD_COLOR( this));
	addAndDropMaterialRenderer(new COpenGLMaterialRenderer_TRANSPARENT_ALPHA_CHANNEL( this));
	addAndDropMaterialRenderer(new COpenGLMaterialRenderer_TRANSPARENT_ALPHA_CHANNEL_REF( this));
	addAndDropMaterialRenderer(new COpenGLMaterialRenderer_TRANSPARENT_VERTEX_ALPHA( this));
	addAndDropMaterialRenderer(new COpenGLMaterialRenderer_TRANSPARENT_REFLECTION_2_LAYER( this));

	// add normal map renderers
	s32 tmp = 0;
	video::IMaterialRenderer* renderer = 0;
	renderer = new COpenGLNormalMapRenderer(this, tmp, MaterialRenderers[EMT_SOLID].Renderer);
	renderer->drop();
	renderer = new COpenGLNormalMapRenderer(this, tmp, MaterialRenderers[EMT_TRANSPARENT_ADD_COLOR].Renderer);
	renderer->drop();
	renderer = new COpenGLNormalMapRenderer(this, tmp, MaterialRenderers[EMT_TRANSPARENT_VERTEX_ALPHA].Renderer);
	renderer->drop();

	// add parallax map renderers
	renderer = new COpenGLParallaxMapRenderer(this, tmp, MaterialRenderers[EMT_SOLID].Renderer);
	renderer->drop();
	renderer = new COpenGLParallaxMapRenderer(this, tmp, MaterialRenderers[EMT_TRANSPARENT_ADD_COLOR].Renderer);
	renderer->drop();
	renderer = new COpenGLParallaxMapRenderer(this, tmp, MaterialRenderers[EMT_TRANSPARENT_VERTEX_ALPHA].Renderer);
	renderer->drop();

	// add basic 1 texture blending
	addAndDropMaterialRenderer(new COpenGLMaterialRenderer_ONETEXTURE_BLEND(this));
}

void COpenGLDriver::loadExtensions()
{
	if (atof((c8*)glGetString(GL_VERSION)) >= 1.2)
		os::Printer::log("OpenGL driver version is 1.2 or better.", ELL_INFORMATION);
	else
		os::Printer::log("OpenGL driver version is not 1.2 or better.", ELL_WARNING);

	const GLubyte* t = glGetString(GL_EXTENSIONS);
//	os::Printer::log((const c8*)t, ELL_INFORMATION);
	#ifdef GLU_VERSION_1_3
	const GLubyte* gluVersion = gluGetString(GLU_VERSION);

	if (gluVersion[0]>1 || gluVersion[3]>2)
	{
		MultiTextureExtension = gluCheckExtension((const GLubyte*)"GL_ARB_multitexture", t);
		MultiSamplingExtension = gluCheckExtension((const GLubyte*)"GL_ARB_multisample", t);
		ARBVertexProgramExtension = gluCheckExtension((const GLubyte*)"GL_ARB_vertex_program", t);
		ARBFragmentProgramExtension = gluCheckExtension((const GLubyte*)"GL_ARB_fragment_program", t);
		ARBShadingLanguage100Extension = gluCheckExtension((const GLubyte*)"GL_ARB_shading_language_100", t);
		AnisotropyExtension = gluCheckExtension((const GLubyte*)"GL_EXT_texture_filter_anisotropic", t);
		SeparateStencilExtension = gluCheckExtension((const GLubyte*)"GL_ATI_separate_stencil", t);
		SeparateStencilExtension = SeparateStencilExtension || gluCheckExtension((const GLubyte*)"GL_ARB_separate_stencil", t);
		GenerateMipmapExtension = gluCheckExtension((const GLubyte*)"GL_SGIS_generate_mipmap", t);
		TextureCompressionExtension = gluCheckExtension((const GLubyte*)"GL_ARB_texture_compression", t);
		TextureNPOTExtension = gluCheckExtension((const GLubyte*)"GL_ARB_texture_non_power_of_two", t);
		FramebufferObjectExtension = gluCheckExtension((const GLubyte*)"GL_EXT_framebuffer_object", t);
		EXTPackedDepthStencil = gluCheckExtension((const GLubyte*)"GL_EXT_packed_depth_stencil", t);
		EXTSeparateSpecularColor = gluCheckExtension((const GLubyte*)"GL_EXT_separate_specular_color", t);
	}
	else
	#endif
	{
		s32 len = (s32)strlen((const char*)t);
		c8 *str = new c8[len+1];
		c8* p = str;

		for (s32 i=0; i<len; ++i)
		{
			str[i] = (char)t[i];

			if (str[i] == ' ')
			{
				str[i] = 0;
				if (strstr(p, "GL_ARB_multitexture"))
					MultiTextureExtension = true;
				else
				if (strstr(p, "GL_ARB_multisample"))
					MultiSamplingExtension = true;
				else
				if (strstr(p, "GL_ARB_vertex_program"))
					ARBVertexProgramExtension = true;
				else
				if (strstr(p, "GL_ARB_fragment_program"))
					ARBFragmentProgramExtension = true;
				else
				if (strstr(p, "GL_ARB_shading_language_100"))
					ARBShadingLanguage100Extension = true;
				else
				if (strstr(p, "GL_EXT_texture_filter_anisotropic"))
					AnisotropyExtension = true;
				else
				if (strstr(p, "GL_ATI_separate_stencil"))
					SeparateStencilExtension = true;
				else
				if (strstr(p, "GL_ARB_separate_stencil"))
					SeparateStencilExtension = true;
				else
				if (strstr(p, "GL_SGIS_generate_mipmap"))
					GenerateMipmapExtension = true;
				else
				if (strstr(p, "GL_ARB_texture_compression"))
					TextureCompressionExtension = true;
				else
				if (strstr(p, "GL_ARB_texture_non_power_of_two"))
					TextureNPOTExtension = true;
				else
				if (strstr(p, "GL_EXT_framebuffer_object"))
					FramebufferObjectExtension = true;
				else
				if (strstr(p, "GL_EXT_packed_depth_stencil"))
					EXTPackedDepthStencil = true;
				else
				if (strstr(p, "GL_EXT_separate_specular_color"))
					EXTSeparateSpecularColor = true;

				p = p + strlen(p) + 1;
			}
		}

		delete [] str;
	}

	if (MultiTextureExtension)
	{
		#ifdef _IRR_WINDOWS_API_

		// Windows
		// get multitexturing function pointers

		pGlActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC) wglGetProcAddress("glActiveTextureARB");
		pGlClientActiveTextureARB = (PFNGLCLIENTACTIVETEXTUREARBPROC) wglGetProcAddress("glClientActiveTextureARB");

		// get fragment and vertex program function pointers
		pGlGenProgramsARB = (PFNGLGENPROGRAMSARBPROC) wglGetProcAddress("glGenProgramsARB");
		pGlBindProgramARB = (PFNGLBINDPROGRAMARBPROC) wglGetProcAddress("glBindProgramARB");
		pGlProgramStringARB = (PFNGLPROGRAMSTRINGARBPROC) wglGetProcAddress("glProgramStringARB");
		pGlDeleteProgramsARB = (PFNGLDELETEPROGRAMSNVPROC) wglGetProcAddress("glDeleteProgramsARB");
		pGlProgramLocalParameter4fvARB = (PFNGLPROGRAMLOCALPARAMETER4FVARBPROC) wglGetProcAddress("glProgramLocalParameter4fvARB");
		pGlCreateShaderObjectARB = (PFNGLCREATESHADEROBJECTARBPROC) wglGetProcAddress("glCreateShaderObjectARB");
		pGlShaderSourceARB = (PFNGLSHADERSOURCEARBPROC) wglGetProcAddress("glShaderSourceARB");
		pGlCompileShaderARB = (PFNGLCOMPILESHADERARBPROC) wglGetProcAddress("glCompileShaderARB");
		pGlCreateProgramObjectARB = (PFNGLCREATEPROGRAMOBJECTARBPROC) wglGetProcAddress("glCreateProgramObjectARB");
		pGlAttachObjectARB = (PFNGLATTACHOBJECTARBPROC) wglGetProcAddress("glAttachObjectARB");
		pGlLinkProgramARB = (PFNGLLINKPROGRAMARBPROC) wglGetProcAddress("glLinkProgramARB");
		pGlUseProgramObjectARB = (PFNGLUSEPROGRAMOBJECTARBPROC) wglGetProcAddress("glUseProgramObjectARB");
		pGlDeleteObjectARB = (PFNGLDELETEOBJECTARBPROC) wglGetProcAddress("glDeleteObjectARB");
		pGlGetInfoLogARB = (PFNGLGETINFOLOGARBPROC) wglGetProcAddress("glGetInfoLogARB");
		pGlGetObjectParameterivARB = (PFNGLGETOBJECTPARAMETERIVARBPROC) wglGetProcAddress("glGetObjectParameterivARB");
		pGlGetUniformLocationARB = (PFNGLGETUNIFORMLOCATIONARBPROC) wglGetProcAddress("glGetUniformLocationARB");
		pGlUniform4fvARB = (PFNGLUNIFORM4FVARBPROC) wglGetProcAddress("glUniform4fvARB");
		pGlUniform1ivARB = (PFNGLUNIFORM1IVARBPROC) wglGetProcAddress("glUniform1ivARB");
		pGlUniform1fvARB = (PFNGLUNIFORM1FVARBPROC) wglGetProcAddress("glUniform1fvARB");
		pGlUniform2fvARB = (PFNGLUNIFORM2FVARBPROC) wglGetProcAddress("glUniform2fvARB");
		pGlUniform3fvARB = (PFNGLUNIFORM3FVARBPROC) wglGetProcAddress("glUniform3fvARB");
		pGlUniformMatrix2fvARB = (PFNGLUNIFORMMATRIX2FVARBPROC) wglGetProcAddress("glUniformMatrix2fvARB");
		pGlUniformMatrix3fvARB = (PFNGLUNIFORMMATRIX3FVARBPROC) wglGetProcAddress("glUniformMatrix3fvARB");
		pGlUniformMatrix4fvARB = (PFNGLUNIFORMMATRIX4FVARBPROC) wglGetProcAddress("glUniformMatrix4fvARB");
		pGlGetActiveUniformARB = (PFNGLGETACTIVEUNIFORMARBPROC) wglGetProcAddress("glGetActiveUniformARB");

		// get point parameter extension
		pGlPointParameterfARB = (PFNGLPOINTPARAMETERFARBPROC) wglGetProcAddress("glPointParameterfARB");
		pGlPointParameterfvARB = (PFNGLPOINTPARAMETERFVARBPROC) wglGetProcAddress("glPointParameterfvARB");

		// get stencil extension
		pGlStencilFuncSeparate = (PFNGLSTENCILFUNCSEPARATEPROC) wglGetProcAddress("glStencilFuncSeparate");
		pGlStencilOpSeparate = (PFNGLSTENCILOPSEPARATEPROC) wglGetProcAddress("glStencilOpSeparate");
		pGlStencilFuncSeparateATI = (PFNGLSTENCILFUNCSEPARATEATIPROC) wglGetProcAddress("glStencilFuncSeparateATI");
		pGlStencilOpSeparateATI = (PFNGLSTENCILOPSEPARATEATIPROC) wglGetProcAddress("glStencilOpSeparateATI");

		#ifdef PFNGLCOMPRESSEDTEXIMAGE2DPROC
		pGlCompressedTexImage2D = (PFNGLCOMPRESSEDTEXIMAGE2DPROC) wglGetProcAddress("glCompressedTexImage2D");
		#endif

        // FrameBufferObjects
        pGlBindFramebufferEXT = (PFNGLBINDFRAMEBUFFEREXTPROC) wglGetProcAddress("glBindFramebufferEXT");
        pGlDeleteFramebuffersEXT = (PFNGLDELETEFRAMEBUFFERSEXTPROC) wglGetProcAddress("glDeleteFramebuffersEXT");
        pGlGenFramebuffersEXT = (PFNGLGENFRAMEBUFFERSEXTPROC) wglGetProcAddress("glGenFramebuffersEXT");
        pGlCheckFramebufferStatusEXT = (PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC) wglGetProcAddress("glCheckFramebufferStatusEXT");
        pGlFramebufferTexture2DEXT = (PFNGLFRAMEBUFFERTEXTURE2DEXTPROC) wglGetProcAddress("glFramebufferTexture2DEXT");
        pGlBindRenderbufferEXT = (PFNGLBINDRENDERBUFFEREXTPROC) wglGetProcAddress("glBindRenderbufferEXT");
        pGlDeleteRenderbuffersEXT = (PFNGLDELETERENDERBUFFERSEXTPROC) wglGetProcAddress("glDeleteRenderbuffersEXT");
        pGlGenRenderbuffersEXT = (PFNGLGENRENDERBUFFERSEXTPROC) wglGetProcAddress("glGenRenderbuffersEXT");
        pGlRenderbufferStorageEXT = (PFNGLRENDERBUFFERSTORAGEEXTPROC) wglGetProcAddress("glRenderbufferStorageEXT");
        pGlFramebufferRenderbufferEXT = (PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC) wglGetProcAddress("glFramebufferRenderbufferEXT");

		// get vsync extension
		wglSwapIntervalEXT = (PFNWGLSWAPINTERVALFARPROC)wglGetProcAddress( "wglSwapIntervalEXT" );

		#elif defined(_IRR_USE_LINUX_DEVICE_) || defined (_IRR_USE_SDL_DEVICE_)
			#ifdef _IRR_OPENGL_USE_EXTPOINTER_

			#ifdef _IRR_USE_SDL_DEVICE_
				#define IRR_OGL_LOAD_EXTENSION(x) SDL_GL_GetProcAddress(reinterpret_cast<const char*>(x))
			#else
			// Accessing the correct function is quite complex
			// All libraries should support the ARB version, however
			// since GLX 1.4 the non-ARB version is the official one
			// So we have to check the runtime environment and
			// choose the proper symbol
			// In case you still have problems please enable the
			// next line by uncommenting it
			// #define _IRR_GETPROCADDRESS_WORKAROUND_

			#ifndef _IRR_GETPROCADDRESS_WORKAROUND_
			__GLXextFuncPtr (*IRR_OGL_LOAD_EXTENSION)(const GLubyte*)=0;
			#ifdef GLX_VERSION_1_4
				int major,minor;
				glXQueryVersion(XDisplay, &major, &minor);
				if ((major>1) || (minor>3))
					IRR_OGL_LOAD_EXTENSION=glXGetProcAddress;
				else
			#endif
					IRR_OGL_LOAD_EXTENSION=glXGetProcAddressARB;
			#else
				#define IRR_OGL_LOAD_EXTENSION glXGetProcAddressARB
			#endif
			#endif

			pGlActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC)
				IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glActiveTextureARB"));

			pGlClientActiveTextureARB = (PFNGLCLIENTACTIVETEXTUREARBPROC)
				IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glClientActiveTextureARB"));

			// get fragment and vertex program function pointers
			pGlGenProgramsARB = (PFNGLGENPROGRAMSARBPROC)
				IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glGenProgramsARB"));

			pGlBindProgramARB = (PFNGLBINDPROGRAMARBPROC)
				IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glBindProgramARB"));

			pGlProgramStringARB = (PFNGLPROGRAMSTRINGARBPROC)
				IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glProgramStringARB"));

			pGlDeleteProgramsARB = (PFNGLDELETEPROGRAMSNVPROC)
				IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glDeleteProgramsARB"));

			pGlProgramLocalParameter4fvARB = (PFNGLPROGRAMLOCALPARAMETER4FVARBPROC)
				IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glProgramLocalParameter4fvARB"));

			pGlCreateShaderObjectARB = (PFNGLCREATESHADEROBJECTARBPROC)
				IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glCreateShaderObjectARB"));

			pGlShaderSourceARB = (PFNGLSHADERSOURCEARBPROC)
				IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glShaderSourceARB"));

			pGlCompileShaderARB = (PFNGLCOMPILESHADERARBPROC)
				IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glCompileShaderARB"));

			pGlCreateProgramObjectARB = (PFNGLCREATEPROGRAMOBJECTARBPROC)
				IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glCreateProgramObjectARB"));

			pGlAttachObjectARB = (PFNGLATTACHOBJECTARBPROC)
				IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glAttachObjectARB"));

			pGlLinkProgramARB = (PFNGLLINKPROGRAMARBPROC)
				IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glLinkProgramARB"));

			pGlUseProgramObjectARB = (PFNGLUSEPROGRAMOBJECTARBPROC)
				IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glUseProgramObjectARB"));

			pGlDeleteObjectARB = (PFNGLDELETEOBJECTARBPROC)
				IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glDeleteObjectARB"));

			pGlGetInfoLogARB = (PFNGLGETINFOLOGARBPROC)
				IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glGetInfoLogARB"));

			pGlGetObjectParameterivARB = (PFNGLGETOBJECTPARAMETERIVARBPROC)
				IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glGetObjectParameterivARB"));

			pGlGetUniformLocationARB = (PFNGLGETUNIFORMLOCATIONARBPROC)
				IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glGetUniformLocationARB"));

			pGlUniform4fvARB = (PFNGLUNIFORM4FVARBPROC)
				IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glUniform4fvARB"));

			pGlUniform1ivARB = (PFNGLUNIFORM1IVARBPROC)
				IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glUniform1ivARB"));

			pGlUniform1fvARB = (PFNGLUNIFORM1FVARBPROC)
				IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glUniform1fvARB"));

			pGlUniform2fvARB = (PFNGLUNIFORM2FVARBPROC)
				IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glUniform2fvARB"));

			pGlUniform3fvARB = (PFNGLUNIFORM3FVARBPROC)
				IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glUniform3fvARB"));

			pGlUniform4fvARB = (PFNGLUNIFORM4FVARBPROC)
				IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glUniform4fvARB"));

			pGlUniformMatrix2fvARB = (PFNGLUNIFORMMATRIX2FVARBPROC)
				IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glUniformMatrix2fvARB"));

			pGlUniformMatrix3fvARB = (PFNGLUNIFORMMATRIX3FVARBPROC)
				IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glUniformMatrix3fvARB"));

			pGlUniformMatrix4fvARB = (PFNGLUNIFORMMATRIX4FVARBPROC)
				IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glUniformMatrix4fvARB"));

			pGlGetActiveUniformARB = (PFNGLGETACTIVEUNIFORMARBPROC)
				IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glGetActiveUniformARB"));

			// get point parameter extension
			pGlPointParameterfARB = (PFNGLPOINTPARAMETERFARBPROC)
				IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glPointParameterfARB"));
			pGlPointParameterfvARB = (PFNGLPOINTPARAMETERFVARBPROC)
				IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glPointParameterfvARB"));

			// get stencil extension
			pGlStencilFuncSeparate = (PFNGLSTENCILFUNCSEPARATEPROC)
				IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glStencilFuncSeparate"));
			pGlStencilOpSeparate = (PFNGLSTENCILOPSEPARATEPROC)
				IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glStencilOpSeparate"));
			pGlStencilFuncSeparateATI = (PFNGLSTENCILFUNCSEPARATEATIPROC)
				IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glStencilFuncSeparateATI"));
			pGlStencilOpSeparateATI = (PFNGLSTENCILOPSEPARATEATIPROC)
				IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glStencilOpSeparateATI"));

			#ifdef PFNGLCOMPRESSEDTEXIMAGE2DPROC
			pGlCompressedTexImage2D = (PFNGLCOMPRESSEDTEXIMAGE2DPROC)
				IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glCompressedTexImage2D"));
			#endif

#if defined(GLX_SGI_swap_control) && !defined(_IRR_USE_SDL_DEVICE_)
			// get vsync extension
			glxSwapIntervalSGI = (PFNGLXSWAPINTERVALSGIPROC)IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glXSwapIntervalSGI"));
#endif

            // FrameBufferObjects
            pGlBindFramebufferEXT = (PFNGLBINDFRAMEBUFFEREXTPROC)
                IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glBindFramebufferEXT"));

            pGlDeleteFramebuffersEXT = (PFNGLDELETEFRAMEBUFFERSEXTPROC)
                IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glDeleteFramebuffersEXT"));

            pGlGenFramebuffersEXT = (PFNGLGENFRAMEBUFFERSEXTPROC)
                IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glGenFramebuffersEXT"));

            pGlCheckFramebufferStatusEXT = (PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC)
                IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glCheckFramebufferStatusEXT"));

            pGlFramebufferTexture2DEXT = (PFNGLFRAMEBUFFERTEXTURE2DEXTPROC)
                IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glFramebufferTexture2DEXT"));

            pGlBindRenderbufferEXT = (PFNGLBINDRENDERBUFFEREXTPROC)
                IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glBindRenderbufferEXT"));

            pGlDeleteRenderbuffersEXT = (PFNGLDELETERENDERBUFFERSEXTPROC)
                IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glDeleteRenderbuffersEXT"));

            pGlGenRenderbuffersEXT = (PFNGLGENRENDERBUFFERSEXTPROC)
                IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glGenRenderbuffersEXT"));

            pGlRenderbufferStorageEXT = (PFNGLRENDERBUFFERSTORAGEEXTPROC)
                IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glRenderbufferStorageEXT"));

            pGlFramebufferRenderbufferEXT = (PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC)
                IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glFramebufferRenderbufferEXT"));

			#endif // _IRR_OPENGL_USE_EXTPOINTER_
		#endif // _IRR_WINDOWS_API_

		// load common extensions

		glGetIntegerv(GL_MAX_TEXTURE_UNITS, &MaxTextureUnits);
		glGetIntegerv(GL_MAX_LIGHTS, &MaxLights);
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &MaxAnisotropy);
	}

#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (!pGlActiveTextureARB || !pGlClientActiveTextureARB)
	{
		MultiTextureExtension = false;
		os::Printer::log("Failed to load OpenGL's multitexture extension, proceeding without.", ELL_WARNING);
	}
	else
#endif
	if (MaxTextureUnits < 2)
	{
		MultiTextureExtension = false;
		os::Printer::log("Warning: OpenGL device only has one texture unit. Disabling multitexturing.", ELL_WARNING);
	}
	MaxTextureUnits = core::min_((u32)MaxTextureUnits,MATERIAL_MAX_TEXTURES);
	glGetIntegerv(GL_MAX_ELEMENTS_INDICES, &MaxIndices);
}



//! presents the rendered scene on the screen, returns false if failed
bool COpenGLDriver::endScene( s32 windowId, core::rect<s32>* sourceRect )
{
	CNullDriver::endScene( windowId );

#ifdef _IRR_USE_WINDOWS_DEVICE_
	return SwapBuffers(HDc) == TRUE;
#elif defined(_IRR_USE_LINUX_DEVICE_)
	glXSwapBuffers(XDisplay, XWindow);
	return true;
#elif defined(MACOSX)
	_device->flush();
	return true;
#elif defined(_IRR_USE_SDL_DEVICE_)
	SDL_GL_SwapBuffers();
	return true;
#else
	return false;
#endif
}



//! clears the zbuffer
bool COpenGLDriver::beginScene(bool backBuffer, bool zBuffer, SColor color)
{
	CNullDriver::beginScene(backBuffer, zBuffer, color);

	GLbitfield mask = 0;

	if (backBuffer)
	{
		f32 inv = 1.0f / 255.0f;
		glClearColor(color.getRed() * inv, color.getGreen() * inv,
				color.getBlue() * inv, color.getAlpha() * inv);

		mask |= GL_COLOR_BUFFER_BIT;
	}

	if (zBuffer)
	{
		glDepthMask(GL_TRUE);
		mask |= GL_DEPTH_BUFFER_BIT;
	}

	glClear(mask);
	return true;
}



//! Returns the transformation set by setTransform
const core::matrix4& COpenGLDriver::getTransform(E_TRANSFORMATION_STATE state)
{
	return Matrices[state];
}



//! sets transformation
void COpenGLDriver::setTransform(E_TRANSFORMATION_STATE state, const core::matrix4& mat)
{
	GLfloat glmat[16];
	Matrices[state] = mat;
	Transformation3DChanged = true;

	switch(state)
	{
	case ETS_VIEW:
	case ETS_WORLD:
		// OpenGL only has a model matrix, view and world is not existent. so lets fake these two.
		createGLMatrix(glmat, Matrices[ETS_VIEW] * Matrices[ETS_WORLD]);
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(glmat);
		break;
	case ETS_PROJECTION:
		createGLMatrix(glmat, mat);
		// flip z to compensate OpenGLs right-hand coordinate system
		glmat[12] *= -1.0f;
		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf(glmat);
		break;
	case ETS_TEXTURE_0:
	case ETS_TEXTURE_1:
	case ETS_TEXTURE_2:
	case ETS_TEXTURE_3:
		if (mat.isIdentity())
			return;

		if (MultiTextureExtension)
			extGlActiveTextureARB(GL_TEXTURE0_ARB + ( state - ETS_TEXTURE_0 ));

		glMatrixMode(GL_TEXTURE);
		createGLTextureMatrix(glmat, mat );
		glLoadMatrixf(glmat);
		break;
	default:
		break;
	}
}



//! draws a vertex primitive list
void COpenGLDriver::drawVertexPrimitiveList(const void* vertices, u32 vertexCount, const u16* indexList, u32 primitiveCount, E_VERTEX_TYPE vType, scene::E_PRIMITIVE_TYPE pType)
{
	if (!primitiveCount || !vertexCount)
		return;

	if (!checkPrimitiveCount(primitiveCount))
		return;

	CNullDriver::drawVertexPrimitiveList(vertices, vertexCount, indexList, primitiveCount, vType, pType);

	// convert colors to gl color format.
	vertexCount *= 4; //reused as color component count
	ColorBuffer.set_used(vertexCount);
	u32 i;

	switch (vType)
	{
		case EVT_STANDARD:
		{
			const S3DVertex* p = (const S3DVertex*)vertices;
			for ( i=0; i<vertexCount; i+=4)
			{
				p->Color.toOpenGLColor(&ColorBuffer[i]);
				++p;
			}
		}
		break;
		case EVT_2TCOORDS:
		{
			const S3DVertex2TCoords* p = (const S3DVertex2TCoords*)vertices;
			for ( i=0; i<vertexCount; i+=4)
			{
				p->Color.toOpenGLColor(&ColorBuffer[i]);
				++p;
			}
		}
		break;
		case EVT_TANGENTS:
		{
			const S3DVertexTangents* p = (const S3DVertexTangents*)vertices;
			for ( i=0; i<vertexCount; i+=4)
			{
				p->Color.toOpenGLColor(&ColorBuffer[i]);
				++p;
			}
		}
		break;
	}

	// draw everything

	setRenderStates3DMode();

	if (MultiTextureExtension)
		extGlClientActiveTextureARB(GL_TEXTURE0_ARB);

	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);
	if (pType!=scene::EPT_POINTS)
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	if (pType!=scene::EPT_POINTS)
		glEnableClientState(GL_NORMAL_ARRAY);

	glColorPointer(4, GL_UNSIGNED_BYTE, 0, &ColorBuffer[0]);
	switch (vType)
	{
		case EVT_STANDARD:
			glVertexPointer(3, GL_FLOAT, sizeof(S3DVertex), &((S3DVertex*)vertices)[0].Pos);
			glNormalPointer(GL_FLOAT, sizeof(S3DVertex), &((S3DVertex*)vertices)[0].Normal);
			glTexCoordPointer(2, GL_FLOAT, sizeof(S3DVertex), &((S3DVertex*)vertices)[0].TCoords);
			break;
		case EVT_2TCOORDS:
			glVertexPointer(3, GL_FLOAT, sizeof(S3DVertex2TCoords), &((S3DVertex2TCoords*)vertices)[0].Pos);
			glNormalPointer(GL_FLOAT, sizeof(S3DVertex2TCoords), &((S3DVertex2TCoords*)vertices)[0].Normal);
			// texture coordinates
			if (MultiTextureExtension)
			{
				extGlClientActiveTextureARB(GL_TEXTURE1_ARB);
				glEnableClientState ( GL_TEXTURE_COORD_ARRAY );
				glTexCoordPointer(2, GL_FLOAT, sizeof(S3DVertex2TCoords), &((S3DVertex2TCoords*)vertices)[0].TCoords2);
				extGlClientActiveTextureARB(GL_TEXTURE0_ARB);
			}
			glTexCoordPointer(2, GL_FLOAT, sizeof(S3DVertex2TCoords), &((S3DVertex2TCoords*)vertices)[0].TCoords);
			break;
		case EVT_TANGENTS:
			glVertexPointer(3, GL_FLOAT, sizeof(S3DVertexTangents), &((S3DVertexTangents*)vertices)[0].Pos);
			glNormalPointer(GL_FLOAT, sizeof(S3DVertexTangents), &((S3DVertexTangents*)vertices)[0].Normal);
			// texture coordinates
			if (MultiTextureExtension)
			{
				extGlClientActiveTextureARB(GL_TEXTURE1_ARB);
				glEnableClientState ( GL_TEXTURE_COORD_ARRAY );
				glTexCoordPointer(3, GL_FLOAT, sizeof(S3DVertexTangents), &((S3DVertexTangents*)vertices)[0].Tangent);

				extGlClientActiveTextureARB(GL_TEXTURE2_ARB);
				glEnableClientState ( GL_TEXTURE_COORD_ARRAY );
				glTexCoordPointer(3, GL_FLOAT, sizeof(S3DVertexTangents), &((S3DVertexTangents*)vertices)[0].Binormal);

				extGlClientActiveTextureARB(GL_TEXTURE0_ARB);
			}
			glTexCoordPointer(2, GL_FLOAT, sizeof(S3DVertexTangents), &((S3DVertexTangents*)vertices)[0].TCoords);
			break;
	}

	switch (pType)
	{
		case scene::EPT_POINTS:
			glDrawArrays(GL_POINTS, 0, primitiveCount);
			break;
		case scene::EPT_LINE_STRIP:
			glDrawElements(GL_LINE_STRIP, primitiveCount+1, GL_UNSIGNED_SHORT, indexList);
			break;
		case scene::EPT_LINE_LOOP:
			glDrawElements(GL_LINE_LOOP, primitiveCount, GL_UNSIGNED_SHORT, indexList);
			break;
		case scene::EPT_LINES:
			glDrawElements(GL_LINES, primitiveCount*2, GL_UNSIGNED_SHORT, indexList);
			break;
		case scene::EPT_TRIANGLE_STRIP:
			glDrawElements(GL_TRIANGLE_STRIP, primitiveCount+2, GL_UNSIGNED_SHORT, indexList);
			break;
		case scene::EPT_TRIANGLE_FAN:
			glDrawElements(GL_TRIANGLE_FAN, primitiveCount+2, GL_UNSIGNED_SHORT, indexList);
			break;
		case scene::EPT_TRIANGLES:
			glDrawElements(GL_TRIANGLES, primitiveCount*3, GL_UNSIGNED_SHORT, indexList);
			break;
		case scene::EPT_QUAD_STRIP:
			glDrawElements(GL_QUAD_STRIP, primitiveCount*2+2, GL_UNSIGNED_SHORT, indexList);
			break;
		case scene::EPT_QUADS:
			glDrawElements(GL_QUADS, primitiveCount*4, GL_UNSIGNED_SHORT, indexList);
			break;
		case scene::EPT_POLYGON:
			glDrawElements(GL_POLYGON, primitiveCount, GL_UNSIGNED_SHORT, indexList);
			break;
	}

	glFlush();

	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	if (MultiTextureExtension)
	{
		if (vType==EVT_TANGENTS)
		{
			extGlClientActiveTextureARB(GL_TEXTURE2_ARB);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		}
		if (vType!=EVT_STANDARD && MultiTextureExtension)
		{
			extGlClientActiveTextureARB(GL_TEXTURE1_ARB);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		}
		extGlClientActiveTextureARB(GL_TEXTURE0_ARB);
	}
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}



//! draws a 2d image, using a color and the alpha channel of the texture if
//! desired. The image is drawn at pos, clipped against clipRect (if != 0).
//! Only the subtexture defined by sourceRect is used.
void COpenGLDriver::draw2DImage(video::ITexture* texture,
				const core::position2d<s32>& pos,
				const core::rect<s32>& sourceRect,
				const core::rect<s32>* clipRect, SColor color,
				bool useAlphaChannelOfTexture)
{
	if (!texture)
		return;

	if (!sourceRect.isValid())
		return;

	core::position2d<s32> targetPos(pos);
	core::position2d<s32> sourcePos(sourceRect.UpperLeftCorner);
	core::dimension2d<s32> sourceSize(sourceRect.getSize());
	const core::dimension2d<s32>& renderTargetSize = getCurrentRenderTargetSize();
	if (clipRect)
	{
		if (targetPos.X < clipRect->UpperLeftCorner.X)
		{
			sourceSize.Width += targetPos.X - clipRect->UpperLeftCorner.X;
			if (sourceSize.Width <= 0)
				return;

			sourcePos.X -= targetPos.X - clipRect->UpperLeftCorner.X;
			targetPos.X = clipRect->UpperLeftCorner.X;
		}

		if (targetPos.X + sourceSize.Width > clipRect->LowerRightCorner.X)
		{
			sourceSize.Width -= (targetPos.X + sourceSize.Width) - clipRect->LowerRightCorner.X;
			if (sourceSize.Width <= 0)
				return;
		}

		if (targetPos.Y < clipRect->UpperLeftCorner.Y)
		{
			sourceSize.Height += targetPos.Y - clipRect->UpperLeftCorner.Y;
			if (sourceSize.Height <= 0)
				return;

			sourcePos.Y -= targetPos.Y - clipRect->UpperLeftCorner.Y;
			targetPos.Y = clipRect->UpperLeftCorner.Y;
		}

		if (targetPos.Y + sourceSize.Height > clipRect->LowerRightCorner.Y)
		{
			sourceSize.Height -= (targetPos.Y + sourceSize.Height) - clipRect->LowerRightCorner.Y;
			if (sourceSize.Height <= 0)
				return;
		}
	}

	// clip these coordinates

	if (targetPos.X<0)
	{
		sourceSize.Width += targetPos.X;
		if (sourceSize.Width <= 0)
			return;

		sourcePos.X -= targetPos.X;
		targetPos.X = 0;
	}

	if (targetPos.X + sourceSize.Width > renderTargetSize.Width)
	{
		sourceSize.Width -= (targetPos.X + sourceSize.Width) - renderTargetSize.Width;
		if (sourceSize.Width <= 0)
			return;
	}

	if (targetPos.Y<0)
	{
		sourceSize.Height += targetPos.Y;
		if (sourceSize.Height <= 0)
			return;

		sourcePos.Y -= targetPos.Y;
		targetPos.Y = 0;
	}

	if (targetPos.Y + sourceSize.Height > renderTargetSize.Height)
	{
		sourceSize.Height -= (targetPos.Y + sourceSize.Height) - renderTargetSize.Height;
		if (sourceSize.Height <= 0)
			return;
	}

	// ok, we've clipped everything.
	// now draw it.

	const core::dimension2d<s32>& ss = texture->getOriginalSize();
	core::rect<f32> tcoords;

	tcoords.UpperLeftCorner.X = (f32)sourcePos.X / (f32)ss.Width;
	tcoords.UpperLeftCorner.Y = (f32)sourcePos.Y / (f32)ss.Height;
	tcoords.LowerRightCorner.X = ((f32)sourcePos.X +(f32)sourceSize.Width) / (f32)ss.Width;
	tcoords.LowerRightCorner.Y = ((f32)sourcePos.Y + (f32)sourceSize.Height) / (f32)ss.Height;

	core::rect<s32> poss(targetPos, sourceSize);
	core::rect<f32> npos;
	f32 xFact = 2.0f / ( renderTargetSize.Width );
	f32 yFact = 2.0f / ( renderTargetSize.Height );

	npos.UpperLeftCorner.X = ( poss.UpperLeftCorner.X * xFact ) - 1.0f;
	npos.UpperLeftCorner.Y = 1.0f - ( poss.UpperLeftCorner.Y * yFact );
	npos.LowerRightCorner.X = ( poss.LowerRightCorner.X * xFact ) - 1.0f;
	npos.LowerRightCorner.Y = 1.0f - ( poss.LowerRightCorner.Y * yFact );

	setRenderStates2DMode(color.getAlpha()<255, true, useAlphaChannelOfTexture);
	disableTextures(1);
	if (!setTexture(0, texture))
		return;

	glColor4ub(color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha());
	glBegin(GL_QUADS);

	glTexCoord2f(tcoords.UpperLeftCorner.X, tcoords.UpperLeftCorner.Y);
	glVertex2f(npos.UpperLeftCorner.X, npos.UpperLeftCorner.Y);

	glTexCoord2f(tcoords.LowerRightCorner.X, tcoords.UpperLeftCorner.Y);
	glVertex2f(npos.LowerRightCorner.X, npos.UpperLeftCorner.Y);

	glTexCoord2f(tcoords.LowerRightCorner.X, tcoords.LowerRightCorner.Y);
	glVertex2f(npos.LowerRightCorner.X, npos.LowerRightCorner.Y);

	glTexCoord2f(tcoords.UpperLeftCorner.X, tcoords.LowerRightCorner.Y);
	glVertex2f(npos.UpperLeftCorner.X, npos.LowerRightCorner.Y);

	glEnd();
}



//! draws a set of 2d images, using a color and the alpha channel of the
//! texture if desired. The images are drawn beginning at pos and concatenated
//! in one line. All drawings are clipped against clipRect (if != 0).
//! The subtextures are defined by the array of sourceRects and are chosen
//! by the indices given.
void COpenGLDriver::draw2DImage(video::ITexture* texture,
				const core::position2d<s32>& pos,
				const core::array<core::rect<s32> >& sourceRects,
				const core::array<s32>& indices,
				const core::rect<s32>* clipRect, SColor color,
				bool useAlphaChannelOfTexture)
{
	if (!texture)
		return;

	const core::dimension2d<s32>& renderTargetSize = getCurrentRenderTargetSize();
	setRenderStates2DMode(color.getAlpha()<255, true, useAlphaChannelOfTexture);
	disableTextures(1);
	if (!setTexture(0, texture))
		return;

	glColor4ub(color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha());
	if (clipRect)
	{
		glEnable(GL_SCISSOR_TEST);
		glScissor(clipRect->UpperLeftCorner.X,renderTargetSize.Height-clipRect->LowerRightCorner.Y,
			clipRect->getWidth(),clipRect->getHeight());
	}

	const core::dimension2d<s32>& ss = texture->getOriginalSize();
	core::position2d<s32> targetPos(pos);
	core::position2d<s32> sourcePos;
	core::dimension2d<s32> sourceSize;
	core::rect<f32> tcoords;
	f32 xFact = 2.0f / ( renderTargetSize.Width );
	f32 yFact = 2.0f / ( renderTargetSize.Height );

	for (u32 i=0; i<indices.size(); ++i)
	{
		s32 currentIndex(indices[i]);
		if (!sourceRects[currentIndex].isValid())
			break;
		sourcePos=sourceRects[currentIndex].UpperLeftCorner;
		sourceSize=sourceRects[currentIndex].getSize();

		tcoords.UpperLeftCorner.X = (f32)sourceRects[currentIndex].UpperLeftCorner.X / (f32)ss.Width;
		tcoords.UpperLeftCorner.Y = (f32)sourceRects[currentIndex].UpperLeftCorner.Y / (f32)ss.Height;
		tcoords.LowerRightCorner.X = (f32)sourceRects[currentIndex].LowerRightCorner.X / (f32)ss.Width;
		tcoords.LowerRightCorner.Y = (f32)sourceRects[currentIndex].LowerRightCorner.Y / (f32)ss.Height;

		core::rect<s32> poss(targetPos, sourceSize);
		core::rect<f32> npos;

		npos.UpperLeftCorner.X = ( poss.UpperLeftCorner.X * xFact ) - 1.0f;
		npos.UpperLeftCorner.Y = 1.0f - ( poss.UpperLeftCorner.Y * yFact );

		npos.LowerRightCorner.X = ( poss.LowerRightCorner.X * xFact ) - 1.0f;
		npos.LowerRightCorner.Y = 1.0f - ( poss.LowerRightCorner.Y * yFact ); 

		glBegin(GL_QUADS);

		glTexCoord2f(tcoords.UpperLeftCorner.X, tcoords.UpperLeftCorner.Y);
		glVertex2f(npos.UpperLeftCorner.X, npos.UpperLeftCorner.Y);

		glTexCoord2f(tcoords.LowerRightCorner.X, tcoords.UpperLeftCorner.Y);
		glVertex2f(npos.LowerRightCorner.X, npos.UpperLeftCorner.Y);

		glTexCoord2f(tcoords.LowerRightCorner.X, tcoords.LowerRightCorner.Y);
		glVertex2f(npos.LowerRightCorner.X, npos.LowerRightCorner.Y);

		glTexCoord2f(tcoords.UpperLeftCorner.X, tcoords.LowerRightCorner.Y);
		glVertex2f(npos.UpperLeftCorner.X, npos.LowerRightCorner.Y);

		glEnd();
		targetPos.X += sourceRects[currentIndex].getWidth();
	}
	if (clipRect)
		glDisable(GL_SCISSOR_TEST);
}



void COpenGLDriver::draw2DImage(video::ITexture* texture, const core::rect<s32>& destRect,
		const core::rect<s32>& sourceRect, const core::rect<s32>* clipRect,
		video::SColor* colors, bool useAlphaChannelOfTexture)
{
	if (!texture)
		return;

	const core::dimension2d<s32>& ss = texture->getOriginalSize();
	core::rect<f32> tcoords;
	tcoords.UpperLeftCorner.X = (f32)sourceRect.UpperLeftCorner.X / (f32)ss.Width;
	tcoords.UpperLeftCorner.Y = (f32)sourceRect.UpperLeftCorner.Y / (f32)ss.Height;
	tcoords.LowerRightCorner.X = (f32)sourceRect.LowerRightCorner.X / (f32)ss.Width;
	tcoords.LowerRightCorner.Y = (f32)sourceRect.LowerRightCorner.Y / (f32)ss.Height;

	const core::dimension2d<s32>& renderTargetSize = getCurrentRenderTargetSize();
	core::rect<f32> npos;
	f32 xFact = 2.0f / ( renderTargetSize.Width );
	f32 yFact = 2.0f / ( renderTargetSize.Height );
	npos.UpperLeftCorner.X = ( destRect.UpperLeftCorner.X * xFact ) - 1.0f;
	npos.UpperLeftCorner.Y = 1.0f - ( destRect.UpperLeftCorner.Y * yFact );
	npos.LowerRightCorner.X = ( destRect.LowerRightCorner.X * xFact ) - 1.0f;
	npos.LowerRightCorner.Y = 1.0f - ( destRect.LowerRightCorner.Y * yFact ); 

	video::SColor temp[4] =
	{
		0xFFFFFFFF,
		0xFFFFFFFF,
		0xFFFFFFFF,
		0xFFFFFFFF
	};

	video::SColor* useColor = colors ? colors : temp;

	setRenderStates2DMode(useColor[0].getAlpha()<255 || useColor[1].getAlpha()<255 || useColor[2].getAlpha()<255 || useColor[3].getAlpha()<255, true, useAlphaChannelOfTexture);

	disableTextures(1);
	setTexture(0, texture);

	if (clipRect)
	{
		glEnable(GL_SCISSOR_TEST);
		glScissor(clipRect->UpperLeftCorner.X,renderTargetSize.Height-clipRect->LowerRightCorner.Y,
			clipRect->getWidth(),clipRect->getHeight());
	}

	glBegin(GL_QUADS);

	glColor4ub(useColor[0].getRed(), useColor[0].getGreen(), useColor[0].getBlue(), useColor[0].getAlpha());
	glTexCoord2f(tcoords.UpperLeftCorner.X, tcoords.UpperLeftCorner.Y);
	glVertex2f(npos.UpperLeftCorner.X, npos.UpperLeftCorner.Y);

	glColor4ub(useColor[3].getRed(), useColor[3].getGreen(), useColor[3].getBlue(), useColor[3].getAlpha());
	glTexCoord2f(tcoords.LowerRightCorner.X, tcoords.UpperLeftCorner.Y);
	glVertex2f(npos.LowerRightCorner.X, npos.UpperLeftCorner.Y);

	glColor4ub(useColor[2].getRed(), useColor[2].getGreen(), useColor[2].getBlue(), useColor[2].getAlpha());
	glTexCoord2f(tcoords.LowerRightCorner.X, tcoords.LowerRightCorner.Y);
	glVertex2f(npos.LowerRightCorner.X, npos.LowerRightCorner.Y);

	glColor4ub(useColor[1].getRed(), useColor[1].getGreen(), useColor[1].getBlue(), useColor[1].getAlpha());
	glTexCoord2f(tcoords.UpperLeftCorner.X, tcoords.LowerRightCorner.Y);
	glVertex2f(npos.UpperLeftCorner.X, npos.LowerRightCorner.Y);

	glEnd();

	if (clipRect)
		glDisable(GL_SCISSOR_TEST);
}



//! draw a 2d rectangle
void COpenGLDriver::draw2DRectangle(SColor color, const core::rect<s32>& position,
		const core::rect<s32>* clip)
{
	setRenderStates2DMode(color.getAlpha() < 255, false, false);
	disableTextures();

	core::rect<s32> pos = position;

	if (clip)
		pos.clipAgainst(*clip);

	if (!pos.isValid())
		return;

	const core::dimension2d<s32>& renderTargetSize = getCurrentRenderTargetSize();
	s32 xPlus = renderTargetSize.Width>>1;
	f32 xFact = 1.0f / (renderTargetSize.Width>>1);

	s32 yPlus = renderTargetSize.Height-(renderTargetSize.Height>>1);
	f32 yFact = 1.0f / (renderTargetSize.Height>>1);

	glColor4ub(color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha());
	glRectf((pos.UpperLeftCorner.X-xPlus) * xFact,
		(yPlus-pos.UpperLeftCorner.Y) * yFact,
		(pos.LowerRightCorner.X-xPlus) * xFact,
		(yPlus-pos.LowerRightCorner.Y) * yFact);
}



//! draw an 2d rectangle
void COpenGLDriver::draw2DRectangle(const core::rect<s32>& position,
			SColor colorLeftUp, SColor colorRightUp, SColor colorLeftDown, SColor colorRightDown,
			const core::rect<s32>* clip)
{
	core::rect<s32> pos = position;

	if (clip)
		pos.clipAgainst(*clip);

	if (!pos.isValid())
		return;

	const core::dimension2d<s32>& renderTargetSize = getCurrentRenderTargetSize();
	s32 xPlus = renderTargetSize.Width>>1;
	f32 xFact = 1.0f / (renderTargetSize.Width>>1);

	s32 yPlus = renderTargetSize.Height-(renderTargetSize.Height>>1);
	f32 yFact = 1.0f / (renderTargetSize.Height>>1);

	core::rect<f32> npos;
	npos.UpperLeftCorner.X = (f32)(pos.UpperLeftCorner.X-xPlus) * xFact;
	npos.UpperLeftCorner.Y = (f32)(yPlus-pos.UpperLeftCorner.Y) * yFact;
	npos.LowerRightCorner.X = (f32)(pos.LowerRightCorner.X-xPlus) * xFact;
	npos.LowerRightCorner.Y = (f32)(yPlus-pos.LowerRightCorner.Y) * yFact;

	setRenderStates2DMode(colorLeftUp.getAlpha() < 255 ||
		colorRightUp.getAlpha() < 255 ||
		colorLeftDown.getAlpha() < 255 ||
		colorRightDown.getAlpha() < 255, false, false);

	disableTextures();

	glBegin(GL_QUADS);
	glColor4ub(colorLeftUp.getRed(), colorLeftUp.getGreen(),
		colorLeftUp.getBlue(), colorLeftUp.getAlpha());
	glVertex2f(npos.UpperLeftCorner.X, npos.UpperLeftCorner.Y);

	glColor4ub(colorRightUp.getRed(), colorRightUp.getGreen(),
		colorRightUp.getBlue(), colorRightUp.getAlpha());
	glVertex2f(npos.LowerRightCorner.X, npos.UpperLeftCorner.Y);

	glColor4ub(colorRightDown.getRed(), colorRightDown.getGreen(),
		colorRightDown.getBlue(), colorRightDown.getAlpha());
	glVertex2f(npos.LowerRightCorner.X, npos.LowerRightCorner.Y);

	glColor4ub(colorLeftDown.getRed(), colorLeftDown.getGreen(),
		colorLeftDown.getBlue(), colorLeftDown.getAlpha());
	glVertex2f(npos.UpperLeftCorner.X, npos.LowerRightCorner.Y);

	glEnd();
}



//! Draws a 2d line.
void COpenGLDriver::draw2DLine(const core::position2d<s32>& start,
				const core::position2d<s32>& end,
				SColor color)
{
	// thanks to Vash TheStampede who sent in his implementation

	const core::dimension2d<s32>& renderTargetSize = getCurrentRenderTargetSize();
	const s32 xPlus = renderTargetSize.Width>>1;
	const f32 xFact = 1.0f / (renderTargetSize.Width>>1);

	const s32 yPlus =
	renderTargetSize.Height-(renderTargetSize.Height>>1);
	const f32 yFact = 1.0f / (renderTargetSize.Height>>1);

	core::position2d<f32> npos_start;
	npos_start.X  = (f32)(start.X - xPlus) * xFact;
	npos_start.Y  = (f32)(yPlus - start.Y) * yFact;

	core::position2d<f32> npos_end;
	npos_end.X  = (f32)(end.X - xPlus) * xFact;
	npos_end.Y  = (f32)(yPlus - end.Y) * yFact;

	setRenderStates2DMode(color.getAlpha() < 255, false, false);
	disableTextures();

	glBegin(GL_LINES);
	glColor4ub(color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha());
	glVertex2f(npos_start.X, npos_start.Y);
	glVertex2f(npos_end.X,   npos_end.Y);
	glEnd();
}



//! queries the features of the driver, returns true if feature is available
bool COpenGLDriver::queryFeature(E_VIDEO_DRIVER_FEATURE feature)
{
	switch (feature)
	{
	case EVDF_RENDER_TO_TARGET:
		return true;
	case EVDF_MULTITEXTURE:
		return MultiTextureExtension;
	case EVDF_BILINEAR_FILTER:
		return true;
	case EVDF_MIP_MAP:
		return true;
	case EVDF_MIP_MAP_AUTO_UPDATE:
		return GenerateMipmapExtension;
	case EVDF_STENCIL_BUFFER:
		return StencilBuffer;
	case EVDF_ARB_VERTEX_PROGRAM_1:
		return ARBVertexProgramExtension;
	case EVDF_ARB_FRAGMENT_PROGRAM_1:
		return ARBFragmentProgramExtension;
	case EVDF_ARB_GLSL:
		return ARBShadingLanguage100Extension;
	case EVDF_TEXTURE_NPOT:
		return TextureNPOTExtension;
	case EVDF_FRAMEBUFFER_OBJECT:
		return FramebufferObjectExtension;
	default:
		return false;
	};
}



//! sets the current Texture
bool COpenGLDriver::setTexture(s32 stage, video::ITexture* texture)
{
	if (stage >= MaxTextureUnits)
		return false;

// Not working, texture confusion in menu of Meshviewer.
	if (CurrentTexture[stage]==texture)
		return true;

	if (MultiTextureExtension)
		extGlActiveTextureARB(GL_TEXTURE0_ARB + stage);

	CurrentTexture[stage]=texture;

	if (texture == 0)
	{
		glDisable(GL_TEXTURE_2D);
		return true;
	}
	else
	{
		if (texture->getDriverType() != EDT_OPENGL)
		{
			glDisable(GL_TEXTURE_2D);
			os::Printer::log("Fatal Error: Tried to set a texture not owned by this driver.", ELL_ERROR);
			return false;
		}

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D,
			((COpenGLTexture*)texture)->getOpenGLTextureName());
	}
	return true;
}



//! disables all textures beginning with the optional fromStage parameter. Otherwise all texture stages are disabled.
//! Returns whether disabling was successful or not.
bool COpenGLDriver::disableTextures(s32 fromStage)
{
	bool result=true;
	for (s32 i=fromStage; i<MaxTextureUnits; ++i)
		result &= setTexture(i, 0);
	return result;
}



//! creates a matrix in supplied GLfloat array to pass to OpenGL
inline void COpenGLDriver::createGLMatrix(GLfloat gl_matrix[16], const core::matrix4& m)
{
	memcpy ( gl_matrix, m.pointer(), 16 * sizeof(f32) );
}


//! creates a opengltexturematrix from a D3D style texture matrix
inline void COpenGLDriver::createGLTextureMatrix(GLfloat *o, const core::matrix4& m)
{
	o[0] = m[0];
	o[1] = m[1];
	o[2] = 0.f;
	o[3] = 0.f;

	o[4] = m[4];
	o[5] = m[5];
	o[6] = 0.f;
	o[7] = 0.f;

	o[8] = 0.f;
	o[9] = 0.f;
	o[10] = 1.f;
	o[11] = 0.f;

	o[12] = m[8];
	o[13] = m[9];
	o[14] = 0.f;
	o[15] = 1.f;
}


//! returns a device dependent texture from a software surface (IImage)
video::ITexture* COpenGLDriver::createDeviceDependentTexture(IImage* surface, const char* name)
{
	return new COpenGLTexture(surface, name, this);
}



//! Sets a material. All 3d drawing functions draw geometry now
//! using this material.
//! \param material: Material to be used from now on.
void COpenGLDriver::setMaterial(const SMaterial& material)
{
	Material = material;

	for (s32 i = 0; i < MaxTextureUnits; ++i)
	{
		setTransform ((E_TRANSFORMATION_STATE) ( ETS_TEXTURE_0 + i ),
				material.getTextureMatrix(i));
	}
}



//! prints error if an error happened.
bool COpenGLDriver::testGLError()
{
#ifdef _DEBUG
	GLenum g = glGetError();
	switch(g)
	{
	case GL_NO_ERROR:
		return false;
	case GL_INVALID_ENUM:
		os::Printer::log("GL_INVALID_ENUM", ELL_ERROR); break;
	case GL_INVALID_VALUE:
		os::Printer::log("GL_INVALID_VALUE", ELL_ERROR); break;
	case GL_INVALID_OPERATION:
		os::Printer::log("GL_INVALID_OPERATION", ELL_ERROR); break;
	case GL_STACK_OVERFLOW:
		os::Printer::log("GL_STACK_OVERFLOW", ELL_ERROR); break;
	case GL_STACK_UNDERFLOW:
		os::Printer::log("GL_STACK_UNDERFLOW", ELL_ERROR); break;
	case GL_OUT_OF_MEMORY:
		os::Printer::log("GL_OUT_OF_MEMORY", ELL_ERROR); break;
	case GL_TABLE_TOO_LARGE:
		os::Printer::log("GL_TABLE_TOO_LARGE", ELL_ERROR); break;
	};
	return true;
#endif
	return false;
}



//! sets the needed renderstates
void COpenGLDriver::setRenderStates3DMode()
{
	if (CurrentRenderMode != ERM_3D)
	{
		// Reset Texture Stages
		glTexEnvi( GL_TEXTURE_ENV, GL_COMBINE_ALPHA_ARB, GL_MODULATE );
		glDisable( GL_BLEND );
		glBlendFunc( GL_ONE, GL_ONE_MINUS_SRC_COLOR );

		// switch back the matrices
		GLfloat glmat[16];

		createGLMatrix(glmat, Matrices[ETS_VIEW] * Matrices[ETS_WORLD]);
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(glmat);

		createGLMatrix(glmat, Matrices[ETS_PROJECTION]);
		glmat[12] *= -1.0f;
		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf(glmat);

		ResetRenderStates = true;
	}

	if ( ResetRenderStates || LastMaterial != Material)
	{
		// unset old material

		if (LastMaterial.MaterialType != Material.MaterialType &&
			LastMaterial.MaterialType >= 0 && LastMaterial.MaterialType < (s32)MaterialRenderers.size())
			MaterialRenderers[LastMaterial.MaterialType].Renderer->OnUnsetMaterial();

		// set new material.
		if (Material.MaterialType >= 0 && Material.MaterialType < (s32)MaterialRenderers.size())
			MaterialRenderers[Material.MaterialType].Renderer->OnSetMaterial(
				Material, LastMaterial, ResetRenderStates, this);

		LastMaterial = Material;
		ResetRenderStates = false;
	}

	if (Material.MaterialType >= 0 && Material.MaterialType < (s32)MaterialRenderers.size())
		MaterialRenderers[Material.MaterialType].Renderer->OnRender(this, video::EVT_STANDARD);

	CurrentRenderMode = ERM_3D;
}


//! Can be called by an IMaterialRenderer to make its work easier.
void COpenGLDriver::setBasicRenderStates(const SMaterial& material, const SMaterial& lastmaterial,
	bool resetAllRenderStates)
{
	if (resetAllRenderStates ||
		lastmaterial.AmbientColor != material.AmbientColor ||
		lastmaterial.DiffuseColor != material.DiffuseColor ||
		lastmaterial.SpecularColor != material.SpecularColor ||
		lastmaterial.EmissiveColor != material.EmissiveColor ||
		lastmaterial.Shininess != material.Shininess)
	{
		GLfloat color[4];

		const f32 inv = 1.0f / 255.0f;

		color[0] = material.AmbientColor.getRed() * inv;
		color[1] = material.AmbientColor.getGreen() * inv;
		color[2] = material.AmbientColor.getBlue() * inv;
		color[3] = material.AmbientColor.getAlpha() * inv;
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, color);

		color[0] = material.DiffuseColor.getRed() * inv;
		color[1] = material.DiffuseColor.getGreen() * inv;
		color[2] = material.DiffuseColor.getBlue() * inv;
		color[3] = material.DiffuseColor.getAlpha() * inv;
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, color);

		// disable Specular colors if no shininess is set
		if (material.Shininess != 0.0f)
		{
#ifdef GL_EXT_separate_specular_color
			if (EXTSeparateSpecularColor)
				glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);
#endif
			glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, material.Shininess);
			color[0] = material.SpecularColor.getRed() * inv;
			color[1] = material.SpecularColor.getGreen() * inv;
			color[2] = material.SpecularColor.getBlue() * inv;
			color[3] = material.SpecularColor.getAlpha() * inv;
			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, color);
		}
#ifdef GL_EXT_separate_specular_color
		else
			if (EXTSeparateSpecularColor)
				glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SINGLE_COLOR);
#endif

		color[0] = material.EmissiveColor.getRed() * inv;
		color[1] = material.EmissiveColor.getGreen() * inv;
		color[2] = material.EmissiveColor.getBlue() * inv;
		color[3] = material.EmissiveColor.getAlpha() * inv;
		glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, color);
	}

	// Texture filter
	// Has to be checked always because it depends on the textures
	// Filtering has to be set for each texture layer
	for (s32 i=0; i<MaxTextureUnits; ++i)
	{
		if (!material.Textures[i])
			continue;
		if (MultiTextureExtension)
			extGlActiveTextureARB(GL_TEXTURE0_ARB + i);
		else if (i>0)
			break;

		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
			(material.BilinearFilter || material.TrilinearFilter) ? GL_LINEAR : GL_NEAREST);

		if (material.Textures[i] && material.Textures[i]->hasMipMaps())
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
				material.TrilinearFilter ? GL_LINEAR_MIPMAP_LINEAR : material.BilinearFilter ? GL_LINEAR_MIPMAP_NEAREST : GL_NEAREST_MIPMAP_NEAREST );
		else
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
				(material.BilinearFilter || material.TrilinearFilter) ? GL_LINEAR : GL_NEAREST);

		if (AnisotropyExtension)
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT,
				material.AnisotropicFilter ? MaxAnisotropy : 1.0f );
	}

	// fillmode
	if (resetAllRenderStates || lastmaterial.Wireframe != material.Wireframe || lastmaterial.PointCloud != material.PointCloud)
		glPolygonMode(GL_FRONT_AND_BACK, material.Wireframe ? GL_LINE : material.PointCloud? GL_POINT : GL_FILL);

	// shademode

	if (resetAllRenderStates || lastmaterial.GouraudShading != material.GouraudShading)
	{
		if (material.GouraudShading)
			glShadeModel(GL_SMOOTH);
		else
			glShadeModel(GL_FLAT);
	}

	// lighting

	if (resetAllRenderStates || lastmaterial.Lighting != material.Lighting)
	{
		if (material.Lighting)
			glEnable(GL_LIGHTING);
		else
			glDisable(GL_LIGHTING);
	}

	// zbuffer
	if (resetAllRenderStates || lastmaterial.ZBuffer != material.ZBuffer)
	{
		switch (material.ZBuffer)
		{
			case 0:
				glDisable(GL_DEPTH_TEST);
				break;
			case 1:
				glEnable(GL_DEPTH_TEST);
				glDepthFunc ( GL_LEQUAL );
				break;
			case 2:
				glEnable(GL_DEPTH_TEST);
				glDepthFunc ( GL_EQUAL );
				break;
		}
	}

	// zwrite
	if (resetAllRenderStates || lastmaterial.ZWriteEnable != material.ZWriteEnable)
	{
		if (material.ZWriteEnable)
		{
			glDepthMask(GL_TRUE);
		}
		else
			glDepthMask(GL_FALSE);
	}

	// back face culling

	if (resetAllRenderStates || lastmaterial.BackfaceCulling != material.BackfaceCulling)
	{
		if (material.BackfaceCulling)
			glEnable(GL_CULL_FACE);
		else
			glDisable(GL_CULL_FACE);
	}

	// fog
	if (resetAllRenderStates || lastmaterial.FogEnable != material.FogEnable)
	{
		if (material.FogEnable)
			glEnable(GL_FOG);
		else
			glDisable(GL_FOG);
	}

	// normalization
	if (resetAllRenderStates || lastmaterial.NormalizeNormals != material.NormalizeNormals)
	{
		if (material.NormalizeNormals)
			glEnable(GL_NORMALIZE);
		else
			glDisable(GL_NORMALIZE);
	}

	// thickness
	if (resetAllRenderStates || lastmaterial.Thickness != material.Thickness)
	{
		glPointSize(material.Thickness);
		glLineWidth(material.Thickness);
	}

	// texture address mode
	for (s32 u=0; u<MaxTextureUnits; ++u)
	{
		if (resetAllRenderStates || lastmaterial.TextureWrap[u] != material.TextureWrap[u])
		{
			GLint mode=GL_REPEAT;
			switch (material.TextureWrap[u])
			{
				case ETC_REPEAT:
					mode=GL_REPEAT;
					break;
				case ETC_CLAMP:
					mode=GL_CLAMP;
					break;
				case ETC_CLAMP_TO_EDGE:
					mode=GL_CLAMP_TO_EDGE;
					break;
				case ETC_CLAMP_TO_BORDER:
					mode=GL_CLAMP_TO_BORDER;
					break;
				case ETC_MIRROR:
					mode=GL_MIRRORED_REPEAT;
					break;
			}
	
			if (MultiTextureExtension)
				extGlActiveTextureARB(GL_TEXTURE0_ARB + u);
			else if (u>0)
				break;
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, mode);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, mode);
		}
	}
}



//! sets the needed renderstates
void COpenGLDriver::setRenderStates2DMode(bool alpha, bool texture, bool alphaChannel)
{
	if (CurrentRenderMode != ERM_2D || Transformation3DChanged)
	{
		// unset last 3d material
		if (CurrentRenderMode == ERM_3D && Material.MaterialType >= 0 &&
				Material.MaterialType < (s32)MaterialRenderers.size())
			MaterialRenderers[Material.MaterialType].Renderer->OnUnsetMaterial();

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();

		Transformation3DChanged = false;

		glDisable(GL_DEPTH_TEST);
		glDisable(GL_FOG);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDisable(GL_LIGHTING);

		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);

		glDisable(GL_ALPHA_TEST);
		glCullFace(GL_BACK);
	}

	if (texture)
	{
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		if (alphaChannel)
		{
			if (alpha)
			{
				glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_ALPHA_EXT, GL_MODULATE);
				glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA_EXT, GL_TEXTURE);
				glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE1_ALPHA_EXT, GL_PRIMARY_COLOR_EXT);
			}
			else
			{
				glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_ALPHA_EXT, GL_REPLACE);
				glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA_EXT, GL_TEXTURE);
			}

			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glEnable(GL_BLEND);

			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);
			glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_MODULATE);
			glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE0_RGB_EXT, GL_TEXTURE);
			glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE1_RGB_EXT, GL_PRIMARY_COLOR_EXT);
		}
		else
		{
			if (alpha)
			{
				glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);
				glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_ALPHA_EXT, GL_REPLACE);
				glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA_EXT, GL_PRIMARY_COLOR_EXT);
				glDisable(GL_ALPHA_TEST);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glEnable(GL_BLEND);
			}
			else
			{
				glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
				glDisable(GL_ALPHA_TEST);
				glDisable(GL_BLEND);
			}
		}

	}
	else
	{
		if (alpha)
		{
			glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_ALPHA_EXT, GL_REPLACE);
			glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA_EXT, GL_PRIMARY_COLOR_EXT);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			glDisable(GL_ALPHA_TEST);
		}
		else
		{
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			glDisable(GL_BLEND);
			glDisable(GL_ALPHA_TEST);
		}
	}

	CurrentRenderMode = ERM_2D;
}


//! \return Returns the name of the video driver. Example: In case of the Direct3D8
//! driver, it would return "Direct3D8.1".
const wchar_t* COpenGLDriver::getName()
{
	return Name.c_str();
}


//! deletes all dynamic lights there are
void COpenGLDriver::deleteAllDynamicLights()
{
	for (s32 i=0; i<LastSetLight+1; ++i)
		glDisable(GL_LIGHT0 + i);

	LastSetLight = -1;

	CNullDriver::deleteAllDynamicLights();
}


//! adds a dynamic light
void COpenGLDriver::addDynamicLight(const SLight& light)
{
	if (LastSetLight == MaxLights-1)
		return;

	setTransform(ETS_WORLD, core::matrix4());

	++LastSetLight;
	CNullDriver::addDynamicLight(light);

	s32 lidx = GL_LIGHT0 + LastSetLight;
	GLfloat data[4];

	if( light.Type == video::ELT_DIRECTIONAL )
	{
		// set direction
		data[0] = -light.Position.X;
		data[1] = -light.Position.Y;
		data[2] = -light.Position.Z;

		data[3] = 0.0f;
		glLightfv(lidx, GL_POSITION, data);

		data[3] = 1.0f;
		glLightfv(lidx, GL_SPOT_DIRECTION, data);

		glLightf(lidx, GL_SPOT_CUTOFF, 180.0f);
		glLightf(lidx, GL_SPOT_EXPONENT, 0.0f);
	}
	else
	{
		// set position
		data[0] = light.Position.X;
		data[1] = light.Position.Y;
		data[2] = light.Position.Z;
		data[3] = 1.0f;
		glLightfv(lidx, GL_POSITION, data);
	}

	// set diffuse color
	data[0] = light.DiffuseColor.r;
	data[1] = light.DiffuseColor.g;
	data[2] = light.DiffuseColor.b;
	data[3] = light.DiffuseColor.a;
	glLightfv(lidx, GL_DIFFUSE, data);

	// set specular color
	data[0] = light.SpecularColor.r;
	data[1] = light.SpecularColor.g;
	data[2] = light.SpecularColor.b;
	data[3] = light.SpecularColor.a;
	glLightfv(lidx, GL_SPECULAR, data);

	// set ambient color
	data[0] = light.AmbientColor.r;
	data[1] = light.AmbientColor.g;
	data[2] = light.AmbientColor.b;
	data[3] = light.AmbientColor.a;
	glLightfv(lidx, GL_AMBIENT, data);

	// 1.0f / (constant + linar * d + quadratic*(d*d);

	// set attenuation
	glLightf(lidx, GL_CONSTANT_ATTENUATION, 0.0f);
	glLightf(lidx, GL_LINEAR_ATTENUATION, 1.0f / light.Radius);
	glLightf(lidx, GL_QUADRATIC_ATTENUATION, 0.0f);

	glEnable(lidx);
}


//! returns the maximal amount of dynamic lights the device can handle
u32 COpenGLDriver::getMaximalDynamicLightAmount()
{
	return MaxLights;
}


//! Sets the dynamic ambient light color. The default color is
//! (0,0,0,0) which means it is dark.
//! \param color: New color of the ambient light.
void COpenGLDriver::setAmbientLight(const SColorf& color)
{
	GLfloat data[4] = {color.r, color.g, color.b, color.a};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, data);
}


// this code was sent in by Oliver Klems, thank you! (I modified the glViewport
// method just a bit.
void COpenGLDriver::setViewPort(const core::rect<s32>& area)
{
	core::rect<s32> vp = area;
	core::rect<s32> rendert(0,0, getCurrentRenderTargetSize().Width, getCurrentRenderTargetSize().Height);
	vp.clipAgainst(rendert);

	if (vp.getHeight()>0 && vp.getWidth()>0)
		glViewport(vp.UpperLeftCorner.X,
		           getCurrentRenderTargetSize().Height - vp.UpperLeftCorner.Y - vp.getHeight(),
			   vp.getWidth(), vp.getHeight());

	ViewPort = vp;
}

//! Draws a shadow volume into the stencil buffer. To draw a stencil shadow, do
//! this: First, draw all geometry. Then use this method, to draw the shadow
//! volume. Next use IVideoDriver::drawStencilShadow() to visualize the shadow.
void COpenGLDriver::drawStencilShadowVolume(const core::vector3df* triangles, s32 count, bool zfail)
{
	if (!StencilBuffer || !count)
		return;

	// unset last 3d material
	if (CurrentRenderMode == ERM_3D &&
	Material.MaterialType >= 0 && Material.MaterialType < (s32)MaterialRenderers.size())
	{
		MaterialRenderers[Material.MaterialType].Renderer->OnUnsetMaterial();
		ResetRenderStates = true;
	}

	// store current OpenGL state
	glPushAttrib(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_ENABLE_BIT |
		GL_POLYGON_BIT | GL_STENCIL_BUFFER_BIT);

	glDisable(GL_LIGHTING);
	glDisable(GL_FOG);
	glDepthMask(GL_FALSE); // no depth buffer writing
	glDepthFunc(GL_LEQUAL);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE ); // no color buffer drawing
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS, 0, 0);
	glEnable(GL_CULL_FACE);

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3,GL_FLOAT,sizeof(core::vector3df),&triangles[0]);

	if (!zfail)
	{
		// ZPASS Method

		glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
		glCullFace(GL_BACK);
		glDrawArrays(GL_TRIANGLES,0,count);

		glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
		glCullFace(GL_FRONT);
		glDrawArrays(GL_TRIANGLES,0,count);
	}
	else
	{
		// ZFAIL Method

		glStencilOp(GL_KEEP, GL_INCR, GL_KEEP);
		glCullFace(GL_FRONT);
		glDrawArrays(GL_TRIANGLES,0,count);

		glStencilOp(GL_KEEP, GL_DECR, GL_KEEP);
		glCullFace(GL_BACK);
		glDrawArrays(GL_TRIANGLES,0,count);
	}

	glDisableClientState(GL_VERTEX_ARRAY); //not stored on stack
	glPopAttrib();
}



void COpenGLDriver::drawStencilShadow(bool clearStencilBuffer, video::SColor leftUpEdge,
	video::SColor rightUpEdge, video::SColor leftDownEdge, video::SColor rightDownEdge)
{
	if (!StencilBuffer)
		return;

	disableTextures();

	// store attributes
	glPushAttrib( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_ENABLE_BIT | GL_POLYGON_BIT | GL_STENCIL_BUFFER_BIT );

	glDisable( GL_LIGHTING );
	glDisable(GL_FOG);
	glDepthMask(GL_FALSE);
	glDepthFunc( GL_LEQUAL );

	glFrontFace( GL_CCW );
	glShadeModel( GL_FLAT );
	glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable( GL_STENCIL_TEST );
	glStencilFunc(GL_NOTEQUAL, 0, 0xFFFFFFFFL);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

	// draw a shadow rectangle covering the entire screen using stencil buffer
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glBegin(GL_TRIANGLE_STRIP);

	glColor4ub (leftUpEdge.getRed(), leftUpEdge.getGreen(), leftUpEdge.getBlue(), leftUpEdge.getAlpha() );
	glVertex3f(-1.1f, 1.1f,0.9f);

	glColor4ub (leftDownEdge.getRed(), leftDownEdge.getGreen(), leftDownEdge.getBlue(), leftDownEdge.getAlpha() );
	glVertex3f(-1.1f,-1.1f,0.9f);

	glColor4ub (rightUpEdge.getRed(), rightUpEdge.getGreen(), rightUpEdge.getBlue(), rightUpEdge.getAlpha() );
	glVertex3f( 1.1f, 1.1f,0.9f);

	glColor4ub (rightDownEdge.getRed(), rightDownEdge.getGreen(), rightDownEdge.getBlue(), rightDownEdge.getAlpha() );
	glVertex3f( 1.1f,-1.1f,0.9f);

	glEnd();

	if (clearStencilBuffer)
		glClear(GL_STENCIL_BUFFER_BIT);

	// restore settings
	glPopMatrix();
	glPopAttrib();
}


//! Sets the fog mode.
void COpenGLDriver::setFog(SColor c, bool linearFog, f32 start,
			f32 end, f32 density, bool pixelFog, bool rangeFog)
{
	CNullDriver::setFog(c, linearFog, start, end, density, pixelFog, rangeFog);

	glFogi(GL_FOG_MODE, linearFog ? GL_LINEAR : GL_EXP);
#ifdef GL_EXT_fog_coord
	glFogi(GL_FOG_COORDINATE_SOURCE, GL_FRAGMENT_DEPTH);
#endif

	if(linearFog)
	{
		glFogf(GL_FOG_START, start);
		glFogf(GL_FOG_END, end);
	}
	else
		glFogf(GL_FOG_DENSITY, density);

	SColorf color(c);
	GLfloat data[4] = {color.r, color.g, color.b, color.a};
	glFogfv(GL_FOG_COLOR, data);
}



//! Draws a 3d line.
void COpenGLDriver::draw3DLine(const core::vector3df& start,
				const core::vector3df& end, SColor color)
{
	setRenderStates3DMode();

	glBegin(GL_LINES);
	glColor4ub(color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha());
	glVertex3f(start.X, start.Y, start.Z);

	glColor4ub(color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha());
	glVertex3f(end.X, end.Y, end.Z);
	glEnd();
}



//! Only used by the internal engine. Used to notify the driver that
//! the window was resized.
void COpenGLDriver::OnResize(const core::dimension2d<s32>& size)
{
	CNullDriver::OnResize(size);
	glViewport(0, 0, size.Width, size.Height);
}



//! Returns type of video driver
E_DRIVER_TYPE COpenGLDriver::getDriverType()
{
	return EDT_OPENGL;
}



void COpenGLDriver::extGlActiveTextureARB(GLenum texture)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (MultiTextureExtension && pGlActiveTextureARB)
		pGlActiveTextureARB(texture);
#else
	if (MultiTextureExtension) glActiveTextureARB(texture);
#endif
}

void COpenGLDriver::extGlClientActiveTextureARB(GLenum texture)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (MultiTextureExtension && pGlClientActiveTextureARB)
		pGlClientActiveTextureARB(texture);
#else
	if (MultiTextureExtension) glClientActiveTextureARB(texture);
#endif
}

void COpenGLDriver::extGlGenProgramsARB(GLsizei n, GLuint *programs)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlGenProgramsARB)
		pGlGenProgramsARB(n, programs);
#elif defined(GL_ARB_vertex_program)
	glGenProgramsARB(n,programs);
#else
	os::Printer::log("glGenProgramsARB not supported", ELL_ERROR);
#endif
}

void COpenGLDriver::extGlBindProgramARB(GLenum target, GLuint program)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlBindProgramARB)
		pGlBindProgramARB(target, program);
#elif defined(GL_ARB_vertex_program)
	glBindProgramARB(target, program);
#else
	os::Printer::log("glBindProgramARB not supported", ELL_ERROR);
#endif
}

void COpenGLDriver::extGlProgramStringARB(GLenum target, GLenum format, GLsizei len, const GLvoid *string)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlProgramStringARB)
		pGlProgramStringARB(target, format, len, string);
#elif defined(GL_ARB_vertex_program)
	glProgramStringARB(target,format,len,string);
#else
	os::Printer::log("glProgramStringARB not supported", ELL_ERROR);
#endif
}

void COpenGLDriver::extGlDeleteProgramsARB(GLsizei n, const GLuint *programs)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlDeleteProgramsARB)
		pGlDeleteProgramsARB(n, programs);
#elif defined(GL_ARB_vertex_program)
	glDeleteProgramsARB(n,programs);
#else
	os::Printer::log("glDeleteProgramsARB not supported", ELL_ERROR);
#endif
}

void COpenGLDriver::extGlProgramLocalParameter4fvARB(GLenum n, GLuint i, const GLfloat * f)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlProgramLocalParameter4fvARB)
		pGlProgramLocalParameter4fvARB(n,i,f);
#elif defined(GL_ARB_vertex_program)
	glProgramLocalParameter4fvARB(n,i,f);
#else
	os::Printer::log("glProgramLocalParameter4fvARB not supported", ELL_ERROR);
#endif
}

GLhandleARB COpenGLDriver::extGlCreateShaderObjectARB(GLenum shaderType)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlCreateShaderObjectARB)
		return pGlCreateShaderObjectARB(shaderType);
#elif defined(GL_ARB_shader_objects)
	return glCreateShaderObjectARB(shaderType);
#else
	os::Printer::log("glCreateShaderObjectARB not supported", ELL_ERROR);
#endif
	return 0;
}

void COpenGLDriver::extGlShaderSourceARB(GLhandleARB shader, int numOfStrings, const char **strings, int *lenOfStrings)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlShaderSourceARB)
		pGlShaderSourceARB(shader, numOfStrings, strings, lenOfStrings);
#elif defined(GL_ARB_shader_objects)
	glShaderSourceARB(shader, numOfStrings, strings, (GLint *)lenOfStrings);
#else
	os::Printer::log("glShaderSourceARB not supported", ELL_ERROR);
#endif
}

void COpenGLDriver::extGlCompileShaderARB(GLhandleARB shader)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlCompileShaderARB)
		pGlCompileShaderARB(shader);
#elif defined(GL_ARB_shader_objects)
	glCompileShaderARB(shader);
#else
	os::Printer::log("glCompileShaderARB not supported", ELL_ERROR);
#endif
}

GLhandleARB COpenGLDriver::extGlCreateProgramObjectARB(void)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlCreateProgramObjectARB)
		return pGlCreateProgramObjectARB();
#elif defined(GL_ARB_shader_objects)
	return glCreateProgramObjectARB();
#else
	os::Printer::log("glCreateProgramObjectARB not supported", ELL_ERROR);
#endif
	return 0;
}

void COpenGLDriver::extGlAttachObjectARB(GLhandleARB program, GLhandleARB shader)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlAttachObjectARB)
		pGlAttachObjectARB(program, shader);
#elif defined(GL_ARB_shader_objects)
	glAttachObjectARB(program, shader);
#else
	os::Printer::log("glAttachObjectARB not supported", ELL_ERROR);
#endif
}

void COpenGLDriver::extGlLinkProgramARB(GLhandleARB program)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlLinkProgramARB)
		pGlLinkProgramARB(program);
#elif defined(GL_ARB_shader_objects)
	glLinkProgramARB(program);
#else
	os::Printer::log("glLinkProgramARB not supported", ELL_ERROR);
#endif
}

void COpenGLDriver::extGlUseProgramObjectARB(GLhandleARB prog)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlUseProgramObjectARB)
		pGlUseProgramObjectARB(prog);
#elif defined(GL_ARB_shader_objects)
	glUseProgramObjectARB(prog);
#else
	os::Printer::log("glUseProgramObjectARB not supported", ELL_ERROR);
#endif
}

void COpenGLDriver::extGlDeleteObjectARB(GLhandleARB object)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlDeleteObjectARB)
		pGlDeleteObjectARB(object);
#elif defined(GL_ARB_shader_objects)
	glDeleteObjectARB(object);
#else
	os::Printer::log("gldeleteObjectARB not supported", ELL_ERROR);
#endif
}

void COpenGLDriver::extGlGetInfoLogARB(GLhandleARB object, GLsizei maxLength, GLsizei *length, GLcharARB *infoLog)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlGetInfoLogARB)
		pGlGetInfoLogARB(object, maxLength, length, infoLog);
#elif defined(GL_ARB_shader_objects)
	glGetInfoLogARB(object, maxLength, length, infoLog);
#else
	os::Printer::log("glGetInfoLogARB not supported", ELL_ERROR);
#endif
}

void COpenGLDriver::extGlGetObjectParameterivARB(GLhandleARB object, GLenum type, int *param)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlGetObjectParameterivARB)
		pGlGetObjectParameterivARB(object, type, param);
#elif defined(GL_ARB_shader_objects)
	glGetObjectParameterivARB(object, type, (GLint *)param);
#else
	os::Printer::log("glGetObjectParameterivARB not supported", ELL_ERROR);
#endif
}

GLint COpenGLDriver::extGlGetUniformLocationARB(GLhandleARB program, const char *name)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlGetUniformLocationARB)
		return pGlGetUniformLocationARB(program, name);
#elif defined(GL_ARB_shader_objects)
	return glGetUniformLocationARB(program, name);
#else
	os::Printer::log("glGetUniformLocationARB not supported", ELL_ERROR);
#endif
	return 0;
}

void COpenGLDriver::extGlUniform4fvARB(GLint location, GLsizei count, const GLfloat *v)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlUniform4fvARB)
		pGlUniform4fvARB(location, count, v);
#elif defined(GL_ARB_shader_objects)
	glUniform4fvARB(location, count, v);
#else
	os::Printer::log("glUniform4fvARB not supported", ELL_ERROR);
#endif
}

void COpenGLDriver::extGlUniform1ivARB (GLint loc, GLsizei count, const GLint *v)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlUniform1ivARB)
		pGlUniform1ivARB(loc, count, v);
#elif defined(GL_ARB_shader_objects)
	glUniform1ivARB(loc, count, v);
#else
	os::Printer::log("glUniform1ivARB not supported", ELL_ERROR);
#endif
}

void COpenGLDriver::extGlUniform1fvARB (GLint loc, GLsizei count, const GLfloat *v)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlUniform1fvARB)
		pGlUniform1fvARB(loc, count, v);
#elif defined(GL_ARB_shader_objects)
	glUniform1fvARB(loc, count, v);
#else
	os::Printer::log("glUniform1fvARB not supported", ELL_ERROR);
#endif
}

void COpenGLDriver::extGlUniform2fvARB (GLint loc, GLsizei count, const GLfloat *v)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlUniform2fvARB)
		pGlUniform2fvARB(loc, count, v);
#elif defined(GL_ARB_shader_objects)
	glUniform2fvARB(loc, count, v);
#else
	os::Printer::log("glUniform2fvARB not supported", ELL_ERROR);
#endif
}

void COpenGLDriver::extGlUniform3fvARB (GLint loc, GLsizei count, const GLfloat *v)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlUniform3fvARB)
		pGlUniform3fvARB(loc, count, v);
#elif defined(GL_ARB_shader_objects)
	glUniform3fvARB(loc, count, v);
#else
	os::Printer::log("glUniform3fvARB not supported", ELL_ERROR);
#endif
}

void COpenGLDriver::extGlUniformMatrix2fvARB (GLint loc, GLsizei count, GLboolean transpose, const GLfloat *v)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlUniformMatrix2fvARB)
		pGlUniformMatrix2fvARB(loc, count, transpose, v);
#elif defined(GL_ARB_shader_objects)
	glUniformMatrix2fvARB(loc, count, transpose, v);
#else
	os::Printer::log("glUniformMatrix2fvARB not supported", ELL_ERROR);
#endif
}

void COpenGLDriver::extGlUniformMatrix3fvARB (GLint loc, GLsizei count, GLboolean transpose, const GLfloat *v)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlUniformMatrix3fvARB)
		pGlUniformMatrix3fvARB(loc, count, transpose, v);
#elif defined(GL_ARB_shader_objects)
	glUniformMatrix3fvARB(loc, count, transpose, v);
#else
	os::Printer::log("glUniformMatrix3fvARB not supported", ELL_ERROR);
#endif
}

void COpenGLDriver::extGlUniformMatrix4fvARB (GLint loc, GLsizei count, GLboolean transpose, const GLfloat *v)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlUniformMatrix4fvARB)
		pGlUniformMatrix4fvARB(loc, count, transpose, v);
#elif defined(GL_ARB_shader_objects)
	glUniformMatrix4fvARB(loc, count, transpose, v);
#else
	os::Printer::log("glUniformMatrix4fvARB not supported", ELL_ERROR);
#endif
}

void COpenGLDriver::extGlGetActiveUniformARB (GLhandleARB program, GLuint index, GLsizei maxlength, GLsizei *length, GLint *size, GLenum *type, GLcharARB *name)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlGetActiveUniformARB)
		pGlGetActiveUniformARB(program, index, maxlength, length, size, type, name);
#elif defined(GL_ARB_shader_objects)
	glGetActiveUniformARB(program, index, maxlength, length, size, type, name);
#else
	os::Printer::log("glGetActiveUniformARB not supported", ELL_ERROR);
#endif
}

void COpenGLDriver::extGlPointParameterfARB (GLint loc, GLfloat f)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlPointParameterfARB)
		pGlPointParameterfARB(loc, f);
#else
	glPointParameterfARB(loc, f);
#endif
}

void COpenGLDriver::extGlPointParameterfvARB (GLint loc, const GLfloat *v)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlPointParameterfvARB)
		pGlPointParameterfvARB(loc, v);
#else
	glPointParameterfvARB(loc, v);
#endif
}

void COpenGLDriver::extGlStencilFuncSeparate (GLenum frontfunc, GLenum backfunc, GLint ref, GLuint mask)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlStencilFuncSeparate)
		pGlStencilFuncSeparate(frontfunc, backfunc, ref, mask);
	else if (pGlStencilFuncSeparateATI)
		pGlStencilFuncSeparateATI(frontfunc, backfunc, ref, mask);
#elif defined(GL_VERSION_2_0)
	glStencilFuncSeparate(frontfunc, backfunc, ref, mask);
#elif defined(GL_ATI_separate_stencil)
	glStencilFuncSeparateATI(frontfunc, backfunc, ref, mask);
#else
	os::Printer::log("glStencilFuncSeparate not supported", ELL_ERROR);
#endif
}

void COpenGLDriver::extGlStencilOpSeparate (GLenum face, GLenum fail, GLenum zfail, GLenum zpass)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlStencilOpSeparate)
		pGlStencilOpSeparate(face, fail, zfail, zpass);
	else if (pGlStencilOpSeparateATI)
		pGlStencilOpSeparateATI(face, fail, zfail, zpass);
#elif defined(GL_VERSION_2_0)
	glStencilOpSeparate(face, fail, zfail, zpass);
#elif defined(GL_ATI_separate_stencil)
	glStencilOpSeparateATI(face, fail, zfail, zpass);
#else
	os::Printer::log("glStencilOpSeparate not supported", ELL_ERROR);
#endif
}

void COpenGLDriver::extGlCompressedTexImage2D (GLenum target, GLint level, GLenum internalformat, GLsizei width,
		GLsizei height, GLint border, GLsizei imageSize, const void* data)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	#ifdef PFNGLCOMPRESSEDTEXIMAGE2DPROC
		if (pGlCompressedTexImage2D)
			pGlCompressedTexImage2D(target, level, internalformat, width, height, border, imageSize, data);
	#endif
#else
	glCompressedTexImage2D(target, level, internalformat, width, height, border, imageSize, data);
#endif
}

void COpenGLDriver::extGlBindFramebufferEXT (GLenum target, GLuint framebuffer)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlBindFramebufferEXT)
		pGlBindFramebufferEXT(target, framebuffer);
#elif defined(GL_EXT_framebuffer_object)
	glBindFramebufferEXT(target, framebuffer);
#endif
}

void COpenGLDriver::extGlDeleteFramebuffersEXT (GLsizei n, const GLuint *framebuffers)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlDeleteFramebuffersEXT)
		pGlDeleteFramebuffersEXT(n, framebuffers);
#elif defined(GL_EXT_framebuffer_object)
	glDeleteFramebuffersEXT(n, framebuffers);
#endif
}

void COpenGLDriver::extGlGenFramebuffersEXT (GLsizei n, GLuint *framebuffers)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlGenFramebuffersEXT)
		pGlGenFramebuffersEXT(n, framebuffers);
#elif defined(GL_EXT_framebuffer_object)
	glGenFramebuffersEXT(n, framebuffers);
#endif
}

GLenum COpenGLDriver::extGlCheckFramebufferStatusEXT (GLenum target)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlCheckFramebufferStatusEXT)
		return pGlCheckFramebufferStatusEXT(target);
	else
		return 0;
#elif defined(GL_EXT_framebuffer_object)
	return glCheckFramebufferStatusEXT(target);
#else
	return 0;
#endif
}

void COpenGLDriver::extGlFramebufferTexture2DEXT (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlFramebufferTexture2DEXT)
		pGlFramebufferTexture2DEXT(target, attachment, textarget, texture, level);
#elif defined(GL_EXT_framebuffer_object)
	glFramebufferTexture2DEXT(target, attachment, textarget, texture, level);
#endif
}

void COpenGLDriver::extGlBindRenderbufferEXT (GLenum target, GLuint renderbuffer)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlBindRenderbufferEXT)
		pGlBindRenderbufferEXT(target, renderbuffer);
#elif defined(GL_EXT_framebuffer_object)
	glBindRenderbufferEXT(target, renderbuffer);
#endif
}

void COpenGLDriver::extGlDeleteRenderbuffersEXT (GLsizei n, const GLuint *renderbuffers)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlDeleteRenderbuffersEXT)
		pGlDeleteRenderbuffersEXT(n, renderbuffers);
#elif defined(GL_EXT_framebuffer_object)
	glDeleteRenderbuffersEXT(n, renderbuffers);
#endif
}

void COpenGLDriver::extGlGenRenderbuffersEXT (GLsizei n, GLuint *renderbuffers)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlGenRenderbuffersEXT)
		pGlGenRenderbuffersEXT(n, renderbuffers);
#elif defined(GL_EXT_framebuffer_object)
	glGenRenderbuffersEXT(n, renderbuffers);
#endif
}

void COpenGLDriver::extGlRenderbufferStorageEXT (GLenum target, GLenum internalformat, GLsizei width, GLsizei height)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlRenderbufferStorageEXT)
		pGlRenderbufferStorageEXT(target, internalformat, width, height);
#elif defined(GL_EXT_framebuffer_object)
	glRenderbufferStorageEXT(target, internalformat, width, height);
#endif
}

void COpenGLDriver::extGlFramebufferRenderbufferEXT (GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlFramebufferRenderbufferEXT)
		pGlFramebufferRenderbufferEXT(target, attachment, renderbuffertarget, renderbuffer);
#elif defined(GL_EXT_framebuffer_object)
	glFramebufferRenderbufferEXT(target, attachment, renderbuffertarget, renderbuffer);
#endif
}

//! Sets a vertex shader constant.
void COpenGLDriver::setVertexShaderConstant(const f32* data, s32 startRegister, s32 constantAmount)
{
#ifdef GL_ARB_vertex_program
	for (int i=0; i<constantAmount; ++i)
		extGlProgramLocalParameter4fvARB(GL_VERTEX_PROGRAM_ARB, startRegister+i, &data[i*4]);
#endif
}

//! Sets a pixel shader constant.
void COpenGLDriver::setPixelShaderConstant(const f32* data, s32 startRegister, s32 constantAmount)
{
#ifdef GL_ARB_fragment_program
	for (int i=0; i<constantAmount; ++i)
		extGlProgramLocalParameter4fvARB(GL_FRAGMENT_PROGRAM_ARB, startRegister+i, &data[i*4]);
#endif
}

//! Sets a constant for the vertex shader based on a name.
bool COpenGLDriver::setVertexShaderConstant(const c8* name, const f32* floats, int count)
{
	//pass this along, as in GLSL the same routine is used for both vertex and fragment shaders
	return setPixelShaderConstant(name, floats, count);
}

//! Sets a constant for the pixel shader based on a name.
bool COpenGLDriver::setPixelShaderConstant(const c8* name, const f32* floats, int count)
{
	os::Printer::log("Error: Please call services->setPixelShaderConstant(), not VideoDriver->setPixelShaderConstant().");
	return false;
}


//! Adds a new material renderer to the VideoDriver, using pixel and/or
//! vertex shaders to render geometry.
s32 COpenGLDriver::addShaderMaterial(const c8* vertexShaderProgram,
	const c8* pixelShaderProgram,
	IShaderConstantSetCallBack* callback,
	E_MATERIAL_TYPE baseMaterial, s32 userData)
{
	s32 nr = -1;
	COpenGLShaderMaterialRenderer* r = new COpenGLShaderMaterialRenderer(
		this, nr, vertexShaderProgram, pixelShaderProgram,
		callback, getMaterialRenderer(baseMaterial), userData);

	r->drop();
	return nr;
}

//! Adds a new material renderer to the VideoDriver, using GLSL to render geometry.
s32 COpenGLDriver::addHighLevelShaderMaterial(
	const c8* vertexShaderProgram,
	const c8* vertexShaderEntryPointName,
	E_VERTEX_SHADER_TYPE vsCompileTarget,
	const c8* pixelShaderProgram,
	const c8* pixelShaderEntryPointName,
	E_PIXEL_SHADER_TYPE psCompileTarget,
	IShaderConstantSetCallBack* callback,
	E_MATERIAL_TYPE baseMaterial,
	s32 userData)
{
	s32 nr = -1;

	COpenGLSLMaterialRenderer* r = new COpenGLSLMaterialRenderer(
		this, nr, vertexShaderProgram, vertexShaderEntryPointName,
		vsCompileTarget, pixelShaderProgram, pixelShaderEntryPointName, psCompileTarget,
		callback,getMaterialRenderer(baseMaterial), userData);

	r->drop();
	return nr;
}

//! Returns a pointer to the IVideoDriver interface. (Implementation for
//! IMaterialRendererServices)
IVideoDriver* COpenGLDriver::getVideoDriver()
{
	return this;
}


//! Returns pointer to the IGPUProgrammingServices interface.
IGPUProgrammingServices* COpenGLDriver::getGPUProgrammingServices()
{
	return this;
}

ITexture* COpenGLDriver::createRenderTargetTexture(const core::dimension2d<s32>& size)
{
	//disable mip-mapping
	bool generateMipLevels = getTextureCreationFlag(ETCF_CREATE_MIP_MAPS);
	setTextureCreationFlag(ETCF_CREATE_MIP_MAPS, false);

	video::ITexture* rtt = 0;
#if defined(GL_EXT_framebuffer_object)
	// if driver supports FrameBufferObjects, use them
	if (FramebufferObjectExtension)
        	rtt = new COpenGLTexture(size, EXTPackedDepthStencil, "rt", this);
	else
#endif
	{
	        rtt = addTexture(size, "rt");
		if (rtt)
			rtt->grab();
	}

	//restore mip-mapping
	setTextureCreationFlag(ETCF_CREATE_MIP_MAPS, generateMipLevels);

	return rtt;
}


//! Returns the maximum amount of primitives (mostly vertices) which
//! the device is able to render with one drawIndexedTriangleList
//! call.
u32 COpenGLDriver::getMaximalPrimitiveCount()
{
	return 65535;// TODO: Fix all loaders to auto-split and then return the correct value: MaxIndices;
}


//! checks triangle count and print warning if wrong
bool COpenGLDriver::setRenderTarget(video::ITexture* texture, bool clearBackBuffer,
								 bool clearZBuffer, SColor color)
{
	// check for right driver type

	if (texture && texture->getDriverType() != EDT_OPENGL)
	{
		os::Printer::log("Fatal Error: Tried to set a texture not owned by this driver.", ELL_ERROR);
		return false;
	}

	// check if we should set the previous RT back

	bool ret = true;

	setTexture(0, 0);
	ResetRenderStates=true;
    if (RenderTargetTexture!=0)
    {
        if (RenderTargetTexture->isFrameBufferObject())
        {
            RenderTargetTexture->unbindFrameBufferObject();
        }
        else
        {
            glBindTexture(GL_TEXTURE_2D, RenderTargetTexture->getOpenGLTextureName());

            // Copy Our ViewPort To The Texture
            glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0,
                RenderTargetTexture->getSize().Width, RenderTargetTexture->getSize().Height);
        }
    }

    if (texture)
    {
		// we want to set a new target. so do this.
		glViewport(0, 0, texture->getSize().Width, texture->getSize().Height);
		RenderTargetTexture = (COpenGLTexture*)texture;
		CurrentRendertargetSize = texture->getSize();

        if (RenderTargetTexture->isFrameBufferObject())
        {
            RenderTargetTexture->bindFrameBufferObject();
        }
	}
	else
	{
        glViewport(0,0,ScreenSize.Width,ScreenSize.Height);
        RenderTargetTexture = 0;
        CurrentRendertargetSize = core::dimension2d<s32>(0,0);
	}

	GLbitfield mask = 0;
	if (clearBackBuffer)
	{
		f32 inv = 1.0f / 255.0f;
		glClearColor(color.getRed() * inv, color.getGreen() * inv,
				color.getBlue() * inv, color.getAlpha() * inv);

		mask |= GL_COLOR_BUFFER_BIT;
	}
	if (clearZBuffer)
	{
		glDepthMask(GL_TRUE);
		mask |= GL_DEPTH_BUFFER_BIT;
	}

	glClear(mask);

	return ret;
}


// returns the current size of the screen or rendertarget
core::dimension2d<s32> COpenGLDriver::getCurrentRenderTargetSize()
{
	if ( CurrentRendertargetSize.Width == 0 )
		return ScreenSize;
	else
		return CurrentRendertargetSize;
}

//! Clears the ZBuffer.
void COpenGLDriver::clearZBuffer()
{
   GLboolean enabled = GL_TRUE;
   glGetBooleanv(GL_DEPTH_WRITEMASK, &enabled);

   glDepthMask(GL_TRUE);
   glClear(GL_DEPTH_BUFFER_BIT);

   glDepthMask(enabled);
} 

//! Returns an image created from the last rendered frame.
IImage* COpenGLDriver::createScreenShot()
{
	IImage* newImage = new CImage(ECF_R8G8B8, ScreenSize);

	u8* pPixels = (u8*)newImage->lock();
	if (!pPixels)
	{
		newImage->drop();
		return 0;
	}

	glReadPixels(0, 0, ScreenSize.Width, ScreenSize.Height, GL_RGB, GL_UNSIGNED_BYTE, pPixels);

	// opengl images are inverted, so we have to fix that here.
	s32 pitch=newImage->getPitch();
	u8* p2 = pPixels + (ScreenSize.Height - 1) * pitch;
	u8* tmpBuffer = new u8[pitch];
	for (s32 i=0; i < ScreenSize.Height; i += 2)
	{
		memcpy(tmpBuffer, pPixels, pitch);
		memcpy(pPixels, p2, pitch);
		memcpy(p2, tmpBuffer, pitch);
		pPixels += pitch;
		p2 -= pitch;
	}
	delete [] tmpBuffer;

	newImage->unlock();

	if (testGLError())
	{
		newImage->drop();
		return 0;
	}

	return newImage;
}


} // end namespace
} // end namespace


namespace irr
{
namespace video
{


// -----------------------------------
// WINDOWS VERSION
// -----------------------------------
#ifdef _IRR_USE_WINDOWS_DEVICE_
IVideoDriver* createOpenGLDriver(const core::dimension2d<s32>& screenSize,
	HWND window, u32 bits, bool fullscreen, bool stencilBuffer, io::IFileSystem* io, bool vsync, bool antiAlias)
{
#ifdef _IRR_COMPILE_WITH_OPENGL_
	COpenGLDriver* ogl =  new COpenGLDriver(screenSize, window, fullscreen, stencilBuffer, io, antiAlias);
	if (!ogl->initDriver(screenSize, window, bits, fullscreen, vsync))
	{
		ogl->drop();
		ogl = 0;
	}
	return ogl;
#else
	return 0;
#endif // _IRR_COMPILE_WITH_OPENGL_
}
#endif // _IRR_USE_WINDOWS_DEVICE_

// -----------------------------------
// MACOSX VERSION
// -----------------------------------
#ifdef MACOSX
IVideoDriver* createOpenGLDriver(const core::dimension2d<s32>& screenSize,
	CIrrDeviceMacOSX *device, bool fullscreen, bool stencilBuffer,
	io::IFileSystem* io, bool vsync, bool antiAlias)
{
#ifdef _IRR_COMPILE_WITH_OPENGL_
	return new COpenGLDriver(screenSize, fullscreen, stencilBuffer,
		device, io, vsync, antiAlias);
#else
	return 0;
#endif //  _IRR_COMPILE_WITH_OPENGL_
}
#endif // MACOSX

// -----------------------------------
// LINUX VERSION
// -----------------------------------
#ifdef _IRR_USE_LINUX_DEVICE_
IVideoDriver* createOpenGLDriver(const core::dimension2d<s32>& screenSize,
		bool fullscreen, bool stencilBuffer, io::IFileSystem* io, bool vsync, bool antiAlias)
{
#ifdef _IRR_COMPILE_WITH_OPENGL_
	return new COpenGLDriver(screenSize, fullscreen, stencilBuffer,
		io, vsync, antiAlias);
#else
	return 0;
#endif //  _IRR_COMPILE_WITH_OPENGL_
}
#endif // _IRR_USE_LINUX_DEVICE_

// -----------------------------------
// SDL VERSION
// -----------------------------------
#ifdef _IRR_USE_SDL_DEVICE_
IVideoDriver* createOpenGLDriver(const core::dimension2d<s32>& screenSize,
		bool fullscreen, bool stencilBuffer, io::IFileSystem* io, bool vsync, bool antiAlias)
{
#ifdef _IRR_COMPILE_WITH_OPENGL_
	return new COpenGLDriver(screenSize, fullscreen, stencilBuffer,
		io, vsync, antiAlias);
#else
	return 0;
#endif //  _IRR_COMPILE_WITH_OPENGL_
}
#endif // _IRR_USE_SDL_DEVICE_

} // end namespace
} // end namespace


#endif // _IRR_COMPILE_WITH_OPENGL_

