// Copyright (C) 2002-2011 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "IrrCompileConfig.h"

#ifdef _IRR_COMPILE_WITH_OPENGL_

#include "COpenGLExtensionHandler.h"
#include "irrString.h"
#include "SMaterial.h" // for MATERIAL_MAX_TEXTURES
#include "fast_atof.h"

namespace irr
{
namespace video
{

COpenGLExtensionHandler::COpenGLExtensionHandler() :
		StencilBuffer(false), MultiTextureExtension(false),
		TextureCompressionExtension(false),
		MaxSupportedTextures(1), MaxTextureUnits(1), MaxLights(1),
		MaxAnisotropy(1), MaxUserClipPlanes(0), MaxAuxBuffers(0),
		MaxMultipleRenderTargets(1), MaxIndices(65535),
		MaxTextureSize(1), MaxGeometryVerticesOut(0),
		MaxTextureLODBias(0.f), Version(0), ShaderLanguageVersion(0),
		OcclusionQuerySupport(false)
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	,pGlActiveTextureARB(0), pGlClientActiveTextureARB(0),
	pGlGenProgramsARB(0), pGlGenProgramsNV(0),
	pGlBindProgramARB(0), pGlBindProgramNV(0),
	pGlDeleteProgramsARB(0), pGlDeleteProgramsNV(0),
	pGlProgramStringARB(0), pGlLoadProgramNV(0),
	pGlProgramLocalParameter4fvARB(0),
	pGlCreateShaderObjectARB(0), pGlShaderSourceARB(0),
	pGlCompileShaderARB(0), pGlCreateProgramObjectARB(0), pGlAttachObjectARB(0),
	pGlLinkProgramARB(0), pGlUseProgramObjectARB(0), pGlDeleteObjectARB(0),
	pGlCreateProgram(0), pGlUseProgram(0),
	pGlDeleteProgram(0), pGlDeleteShader(0),
	pGlGetAttachedObjectsARB(0), pGlGetAttachedShaders(0),
	pGlCreateShader(0), pGlShaderSource(0), pGlCompileShader(0),
	pGlAttachShader(0), pGlLinkProgram(0),
	pGlGetInfoLogARB(0), pGlGetShaderInfoLog(0), pGlGetProgramInfoLog(0),
	pGlGetObjectParameterivARB(0), pGlGetShaderiv(0), pGlGetProgramiv(0),
	pGlGetUniformLocationARB(0), pGlGetUniformLocation(0),
	pGlUniform1ivARB(0), pGlUniform1fvARB(0), pGlUniform2fvARB(0), pGlUniform3fvARB(0), pGlUniform4fvARB(0), pGlUniformMatrix2fvARB(0),
	pGlUniformMatrix3fvARB(0), pGlUniformMatrix4fvARB(0),
	pGlGetActiveUniformARB(0), pGlGetActiveUniform(0),
	pGlPointParameterfARB(0), pGlPointParameterfvARB(0),
	pGlStencilFuncSeparate(0), pGlStencilOpSeparate(0),
	pGlStencilFuncSeparateATI(0), pGlStencilOpSeparateATI(0),
	pGlCompressedTexImage2D(0),
#if defined(GLX_SGI_swap_control)
	pGlxSwapIntervalSGI(0),
#endif
#if defined(GLX_EXT_swap_control)
	pGlxSwapIntervalEXT(0),
#endif
#if defined(GLX_MESA_swap_control)
	pGlxSwapIntervalMESA(0),
#endif
#if defined(WGL_EXT_swap_control)
	pWglSwapIntervalEXT(0),
#endif
	pGlBindFramebufferEXT(0), pGlDeleteFramebuffersEXT(0), pGlGenFramebuffersEXT(0),
	pGlCheckFramebufferStatusEXT(0), pGlFramebufferTexture2DEXT(0),
	pGlBindRenderbufferEXT(0), pGlDeleteRenderbuffersEXT(0), pGlGenRenderbuffersEXT(0),
	pGlRenderbufferStorageEXT(0), pGlFramebufferRenderbufferEXT(0),
	pGlDrawBuffersARB(0), pGlDrawBuffersATI(0),
	pGlGenBuffersARB(0), pGlBindBufferARB(0), pGlBufferDataARB(0), pGlDeleteBuffersARB(0),
	pGlBufferSubDataARB(0), pGlGetBufferSubDataARB(0), pGlMapBufferARB(0), pGlUnmapBufferARB(0),
	pGlIsBufferARB(0), pGlGetBufferParameterivARB(0), pGlGetBufferPointervARB(0),
	pGlProvokingVertexARB(0), pGlProvokingVertexEXT(0),
	pGlColorMaskIndexedEXT(0), pGlEnableIndexedEXT(0), pGlDisableIndexedEXT(0),
	pGlBlendFuncIndexedAMD(0), pGlBlendFunciARB(0),
	pGlProgramParameteriARB(0), pGlProgramParameteriEXT(0),
	pGlGenQueriesARB(0), pGlDeleteQueriesARB(0), pGlIsQueryARB(0),
	pGlBeginQueryARB(0), pGlEndQueryARB(0), pGlGetQueryivARB(0),
	pGlGetQueryObjectivARB(0), pGlGetQueryObjectuivARB(0),
	pGlGenOcclusionQueriesNV(0), pGlDeleteOcclusionQueriesNV(0),
	pGlIsOcclusionQueryNV(0), pGlBeginOcclusionQueryNV(0),
	pGlEndOcclusionQueryNV(0), pGlGetOcclusionQueryivNV(0),
	pGlGetOcclusionQueryuivNV(0)
#endif // _IRR_OPENGL_USE_EXTPOINTER_
{
	for (u32 i=0; i<IRR_OpenGL_Feature_Count; ++i)
		FeatureAvailable[i]=false;
	DimAliasedLine[0]=1.f;
	DimAliasedLine[1]=1.f;
	DimAliasedPoint[0]=1.f;
	DimAliasedPoint[1]=1.f;
	DimSmoothedLine[0]=1.f;
	DimSmoothedLine[1]=1.f;
	DimSmoothedPoint[0]=1.f;
	DimSmoothedPoint[1]=1.f;
}


void COpenGLExtensionHandler::dump() const
{
	for (u32 i=0; i<IRR_OpenGL_Feature_Count; ++i)
		os::Printer::log(OpenGLFeatureStrings[i], FeatureAvailable[i]?" true":" false");
}

void COpenGLExtensionHandler::initExtensions(bool stencilBuffer)
{
	const f32 ogl_ver = core::fast_atof(reinterpret_cast<const c8*>(glGetString(GL_VERSION)));
	Version = static_cast<u16>(core::floor32(ogl_ver)*100+core::round32(core::fract(ogl_ver)*10.0f));
	if ( Version >= 102)
		os::Printer::log("OpenGL driver version is 1.2 or better.", ELL_INFORMATION);
	else
		os::Printer::log("OpenGL driver version is not 1.2 or better.", ELL_WARNING);

	{
		const char* t = reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS));
		size_t len = 0;
		c8 *str = 0;
		if (t)
		{
			len = strlen(t);
			str = new c8[len+1];
		}
		c8* p = str;

		for (size_t i=0; i<len; ++i)
		{
			str[i] = static_cast<char>(t[i]);

			if (str[i] == ' ')
			{
				str[i] = 0;
				for (u32 j=0; j<IRR_OpenGL_Feature_Count; ++j)
				{
					if (!strcmp(OpenGLFeatureStrings[j], p))
					{
						FeatureAvailable[j] = true;
						break;
					}
				}

				p = p + strlen(p) + 1;
			}
		}

		delete [] str;
	}

	MultiTextureExtension = FeatureAvailable[IRR_ARB_multitexture];
	TextureCompressionExtension = FeatureAvailable[IRR_ARB_texture_compression];
	StencilBuffer=stencilBuffer;

#ifdef _IRR_WINDOWS_API_
	// get multitexturing function pointers
	pGlActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC) wglGetProcAddress("glActiveTextureARB");
	pGlClientActiveTextureARB = (PFNGLCLIENTACTIVETEXTUREARBPROC) wglGetProcAddress("glClientActiveTextureARB");

	// get fragment and vertex program function pointers
	pGlGenProgramsARB = (PFNGLGENPROGRAMSARBPROC) wglGetProcAddress("glGenProgramsARB");
	pGlGenProgramsNV = (PFNGLGENPROGRAMSNVPROC) wglGetProcAddress("glGenProgramsNV");
	pGlBindProgramARB = (PFNGLBINDPROGRAMARBPROC) wglGetProcAddress("glBindProgramARB");
	pGlBindProgramNV = (PFNGLBINDPROGRAMNVPROC) wglGetProcAddress("glBindProgramNV");
	pGlProgramStringARB = (PFNGLPROGRAMSTRINGARBPROC) wglGetProcAddress("glProgramStringARB");
	pGlLoadProgramNV = (PFNGLLOADPROGRAMNVPROC) wglGetProcAddress("glLoadProgramNV");
	pGlDeleteProgramsARB = (PFNGLDELETEPROGRAMSARBPROC) wglGetProcAddress("glDeleteProgramsARB");
	pGlDeleteProgramsNV = (PFNGLDELETEPROGRAMSNVPROC) wglGetProcAddress("glDeleteProgramsNV");
	pGlProgramLocalParameter4fvARB = (PFNGLPROGRAMLOCALPARAMETER4FVARBPROC) wglGetProcAddress("glProgramLocalParameter4fvARB");
	pGlCreateShaderObjectARB = (PFNGLCREATESHADEROBJECTARBPROC) wglGetProcAddress("glCreateShaderObjectARB");
	pGlCreateShader = (PFNGLCREATESHADERPROC) wglGetProcAddress("glCreateShader");
	pGlShaderSourceARB = (PFNGLSHADERSOURCEARBPROC) wglGetProcAddress("glShaderSourceARB");
	pGlShaderSource = (PFNGLSHADERSOURCEPROC) wglGetProcAddress("glShaderSource");
	pGlCompileShaderARB = (PFNGLCOMPILESHADERARBPROC) wglGetProcAddress("glCompileShaderARB");
	pGlCompileShader = (PFNGLCOMPILESHADERPROC) wglGetProcAddress("glCompileShader");
	pGlCreateProgramObjectARB = (PFNGLCREATEPROGRAMOBJECTARBPROC) wglGetProcAddress("glCreateProgramObjectARB");
	pGlCreateProgram = (PFNGLCREATEPROGRAMPROC) wglGetProcAddress("glCreateProgram");
	pGlAttachObjectARB = (PFNGLATTACHOBJECTARBPROC) wglGetProcAddress("glAttachObjectARB");
	pGlAttachShader = (PFNGLATTACHSHADERPROC) wglGetProcAddress("glAttachShader");
	pGlLinkProgramARB = (PFNGLLINKPROGRAMARBPROC) wglGetProcAddress("glLinkProgramARB");
	pGlLinkProgram = (PFNGLLINKPROGRAMPROC) wglGetProcAddress("glLinkProgram");
	pGlUseProgramObjectARB = (PFNGLUSEPROGRAMOBJECTARBPROC) wglGetProcAddress("glUseProgramObjectARB");
	pGlUseProgram = (PFNGLUSEPROGRAMPROC) wglGetProcAddress("glUseProgram");
	pGlDeleteObjectARB = (PFNGLDELETEOBJECTARBPROC) wglGetProcAddress("glDeleteObjectARB");
	pGlDeleteProgram = (PFNGLDELETEPROGRAMPROC) wglGetProcAddress("glDeleteProgram");
	pGlDeleteShader = (PFNGLDELETESHADERPROC) wglGetProcAddress("glDeleteShader");
	pGlGetAttachedShaders = (PFNGLGETATTACHEDSHADERSPROC) wglGetProcAddress("glGetAttachedShaders");
	pGlGetAttachedObjectsARB = (PFNGLGETATTACHEDOBJECTSARBPROC) wglGetProcAddress("glGetAttachedObjectsARB");
	pGlGetInfoLogARB = (PFNGLGETINFOLOGARBPROC) wglGetProcAddress("glGetInfoLogARB");
	pGlGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC) wglGetProcAddress("glGetShaderInfoLog");
	pGlGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC) wglGetProcAddress("glGetProgramInfoLog");
	pGlGetObjectParameterivARB = (PFNGLGETOBJECTPARAMETERIVARBPROC) wglGetProcAddress("glGetObjectParameterivARB");
	pGlGetShaderiv = (PFNGLGETSHADERIVPROC) wglGetProcAddress("glGetShaderiv");
	pGlGetProgramiv = (PFNGLGETPROGRAMIVPROC) wglGetProcAddress("glGetProgramiv");
	pGlGetUniformLocationARB = (PFNGLGETUNIFORMLOCATIONARBPROC) wglGetProcAddress("glGetUniformLocationARB");
	pGlGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC) wglGetProcAddress("glGetUniformLocation");
	pGlUniform4fvARB = (PFNGLUNIFORM4FVARBPROC) wglGetProcAddress("glUniform4fvARB");
	pGlUniform1ivARB = (PFNGLUNIFORM1IVARBPROC) wglGetProcAddress("glUniform1ivARB");
	pGlUniform1fvARB = (PFNGLUNIFORM1FVARBPROC) wglGetProcAddress("glUniform1fvARB");
	pGlUniform2fvARB = (PFNGLUNIFORM2FVARBPROC) wglGetProcAddress("glUniform2fvARB");
	pGlUniform3fvARB = (PFNGLUNIFORM3FVARBPROC) wglGetProcAddress("glUniform3fvARB");
	pGlUniformMatrix2fvARB = (PFNGLUNIFORMMATRIX2FVARBPROC) wglGetProcAddress("glUniformMatrix2fvARB");
	pGlUniformMatrix3fvARB = (PFNGLUNIFORMMATRIX3FVARBPROC) wglGetProcAddress("glUniformMatrix3fvARB");
	pGlUniformMatrix4fvARB = (PFNGLUNIFORMMATRIX4FVARBPROC) wglGetProcAddress("glUniformMatrix4fvARB");
	pGlGetActiveUniformARB = (PFNGLGETACTIVEUNIFORMARBPROC) wglGetProcAddress("glGetActiveUniformARB");
	pGlGetActiveUniform = (PFNGLGETACTIVEUNIFORMPROC) wglGetProcAddress("glGetActiveUniform");

	// get point parameter extension
	pGlPointParameterfARB = (PFNGLPOINTPARAMETERFARBPROC) wglGetProcAddress("glPointParameterfARB");
	pGlPointParameterfvARB = (PFNGLPOINTPARAMETERFVARBPROC) wglGetProcAddress("glPointParameterfvARB");

	// get stencil extension
	pGlStencilFuncSeparate = (PFNGLSTENCILFUNCSEPARATEPROC) wglGetProcAddress("glStencilFuncSeparate");
	pGlStencilOpSeparate = (PFNGLSTENCILOPSEPARATEPROC) wglGetProcAddress("glStencilOpSeparate");
	pGlStencilFuncSeparateATI = (PFNGLSTENCILFUNCSEPARATEATIPROC) wglGetProcAddress("glStencilFuncSeparateATI");
	pGlStencilOpSeparateATI = (PFNGLSTENCILOPSEPARATEATIPROC) wglGetProcAddress("glStencilOpSeparateATI");

	// compressed textures
	pGlCompressedTexImage2D = (PFNGLCOMPRESSEDTEXIMAGE2DPROC) wglGetProcAddress("glCompressedTexImage2D");

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
	pGlDrawBuffersARB = (PFNGLDRAWBUFFERSARBPROC) wglGetProcAddress("glDrawBuffersARB");
	pGlDrawBuffersATI = (PFNGLDRAWBUFFERSATIPROC) wglGetProcAddress("glDrawBuffersATI");

	// get vertex buffer extension
	pGlGenBuffersARB = (PFNGLGENBUFFERSARBPROC) wglGetProcAddress("glGenBuffersARB");
	pGlBindBufferARB = (PFNGLBINDBUFFERARBPROC) wglGetProcAddress("glBindBufferARB");
	pGlBufferDataARB = (PFNGLBUFFERDATAARBPROC) wglGetProcAddress("glBufferDataARB");
	pGlDeleteBuffersARB = (PFNGLDELETEBUFFERSARBPROC) wglGetProcAddress("glDeleteBuffersARB");
	pGlBufferSubDataARB= (PFNGLBUFFERSUBDATAARBPROC) wglGetProcAddress("glBufferSubDataARB");
	pGlGetBufferSubDataARB= (PFNGLGETBUFFERSUBDATAARBPROC)wglGetProcAddress("glGetBufferSubDataARB");
	pGlMapBufferARB= (PFNGLMAPBUFFERARBPROC) wglGetProcAddress("glMapBufferARB");
	pGlUnmapBufferARB= (PFNGLUNMAPBUFFERARBPROC) wglGetProcAddress("glUnmapBufferARB");
	pGlIsBufferARB= (PFNGLISBUFFERARBPROC) wglGetProcAddress("glIsBufferARB");
	pGlGetBufferParameterivARB= (PFNGLGETBUFFERPARAMETERIVARBPROC) wglGetProcAddress("glGetBufferParameterivARB");
	pGlGetBufferPointervARB= (PFNGLGETBUFFERPOINTERVARBPROC) wglGetProcAddress("glGetBufferPointervARB");
	pGlProvokingVertexARB= (PFNGLPROVOKINGVERTEXPROC) wglGetProcAddress("glProvokingVertex");
	pGlProvokingVertexEXT= (PFNGLPROVOKINGVERTEXEXTPROC) wglGetProcAddress("glProvokingVertexEXT");
	pGlColorMaskIndexedEXT= (PFNGLCOLORMASKINDEXEDEXTPROC) wglGetProcAddress("glColorMaskIndexedEXT");
	pGlEnableIndexedEXT= (PFNGLENABLEINDEXEDEXTPROC) wglGetProcAddress("glEnableIndexedEXT");
	pGlDisableIndexedEXT= (PFNGLDISABLEINDEXEDEXTPROC) wglGetProcAddress("glDisableIndexedEXT");
	pGlBlendFuncIndexedAMD= (PFNGLBLENDFUNCINDEXEDAMDPROC) wglGetProcAddress("glBlendFuncIndexedAMD");
	pGlBlendFunciARB= (PFNGLBLENDFUNCIPROC) wglGetProcAddress("glBlendFunciARB");
	pGlProgramParameteriARB= (PFNGLPROGRAMPARAMETERIARBPROC) wglGetProcAddress("glProgramParameteriARB");
	pGlProgramParameteriEXT= (PFNGLPROGRAMPARAMETERIEXTPROC) wglGetProcAddress("glProgramParameteriEXT");

	// occlusion query
	pGlGenQueriesARB = (PFNGLGENQUERIESARBPROC) wglGetProcAddress("glGenQueriesARB");
	pGlDeleteQueriesARB = (PFNGLDELETEQUERIESARBPROC) wglGetProcAddress("glDeleteQueriesARB");
	pGlIsQueryARB = (PFNGLISQUERYARBPROC) wglGetProcAddress("glIsQueryARB");
	pGlBeginQueryARB = (PFNGLBEGINQUERYARBPROC) wglGetProcAddress("glBeginQueryARB");
	pGlEndQueryARB = (PFNGLENDQUERYARBPROC) wglGetProcAddress("glEndQueryARB");
	pGlGetQueryivARB = (PFNGLGETQUERYIVARBPROC) wglGetProcAddress("glGetQueryivARB");
	pGlGetQueryObjectivARB = (PFNGLGETQUERYOBJECTIVARBPROC) wglGetProcAddress("glGetQueryObjectivARB");
	pGlGetQueryObjectuivARB = (PFNGLGETQUERYOBJECTUIVARBPROC) wglGetProcAddress("glGetQueryObjectuivARB");
	pGlGenOcclusionQueriesNV = (PFNGLGENOCCLUSIONQUERIESNVPROC) wglGetProcAddress("glGenOcclusionQueriesNV");
	pGlDeleteOcclusionQueriesNV = (PFNGLDELETEOCCLUSIONQUERIESNVPROC) wglGetProcAddress("glDeleteOcclusionQueriesNV");
	pGlIsOcclusionQueryNV = (PFNGLISOCCLUSIONQUERYNVPROC) wglGetProcAddress("glIsOcclusionQueryNV");
	pGlBeginOcclusionQueryNV = (PFNGLBEGINOCCLUSIONQUERYNVPROC) wglGetProcAddress("glBeginOcclusionQueryNV");
	pGlEndOcclusionQueryNV = (PFNGLENDOCCLUSIONQUERYNVPROC) wglGetProcAddress("glEndOcclusionQueryNV");
	pGlGetOcclusionQueryivNV = (PFNGLGETOCCLUSIONQUERYIVNVPROC) wglGetProcAddress("glGetOcclusionQueryivNV");
	pGlGetOcclusionQueryuivNV = (PFNGLGETOCCLUSIONQUERYUIVNVPROC) wglGetProcAddress("glGetOcclusionQueryuivNV");
	#if defined(WGL_EXT_swap_control) && !defined(_IRR_COMPILE_WITH_SDL_DEVICE_)
		// get vsync extension
		pWglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC) wglGetProcAddress("wglSwapIntervalEXT");
	#endif

#elif defined(_IRR_COMPILE_WITH_X11_DEVICE_) || defined (_IRR_COMPILE_WITH_SDL_DEVICE_)
	#ifdef _IRR_OPENGL_USE_EXTPOINTER_

	#if defined(_IRR_COMPILE_WITH_SDL_DEVICE_) && !defined(_IRR_COMPILE_WITH_X11_DEVICE_)
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
		int major=0,minor=0;
		if (glXGetCurrentDisplay())
			glXQueryVersion(glXGetCurrentDisplay(), &major, &minor);
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

	pGlGenProgramsNV = (PFNGLGENPROGRAMSNVPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glGenProgramsNV"));

	pGlBindProgramARB = (PFNGLBINDPROGRAMARBPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glBindProgramARB"));

	pGlBindProgramNV = (PFNGLBINDPROGRAMNVPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glBindProgramNV"));

	pGlDeleteProgramsARB = (PFNGLDELETEPROGRAMSARBPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glDeleteProgramsARB"));

	pGlDeleteProgramsNV = (PFNGLDELETEPROGRAMSNVPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glDeleteProgramsNV"));

	pGlProgramStringARB = (PFNGLPROGRAMSTRINGARBPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glProgramStringARB"));

	pGlLoadProgramNV = (PFNGLLOADPROGRAMNVPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glLoadProgramNV"));

	pGlProgramLocalParameter4fvARB = (PFNGLPROGRAMLOCALPARAMETER4FVARBPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glProgramLocalParameter4fvARB"));

	pGlCreateShaderObjectARB = (PFNGLCREATESHADEROBJECTARBPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glCreateShaderObjectARB"));

	pGlCreateShader = (PFNGLCREATESHADERPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glCreateShader"));

	pGlShaderSourceARB = (PFNGLSHADERSOURCEARBPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glShaderSourceARB"));

	pGlShaderSource = (PFNGLSHADERSOURCEPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glShaderSource"));

	pGlCompileShaderARB = (PFNGLCOMPILESHADERARBPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glCompileShaderARB"));

	pGlCompileShader = (PFNGLCOMPILESHADERPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glCompileShader"));

	pGlCreateProgramObjectARB = (PFNGLCREATEPROGRAMOBJECTARBPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glCreateProgramObjectARB"));

	pGlCreateProgram = (PFNGLCREATEPROGRAMPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glCreateProgram"));

	pGlAttachObjectARB = (PFNGLATTACHOBJECTARBPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glAttachObjectARB"));

	pGlAttachShader = (PFNGLATTACHSHADERPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glAttachShader"));

	pGlLinkProgramARB = (PFNGLLINKPROGRAMARBPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glLinkProgram"));

	pGlLinkProgram = (PFNGLLINKPROGRAMPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glLinkProgramARB"));

	pGlUseProgramObjectARB = (PFNGLUSEPROGRAMOBJECTARBPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glUseProgramObjectARB"));

	pGlUseProgram = (PFNGLUSEPROGRAMPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glUseProgram"));

	pGlDeleteObjectARB = (PFNGLDELETEOBJECTARBPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glDeleteObjectARB"));

	pGlDeleteProgram = (PFNGLDELETEPROGRAMPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glDeleteProgram"));

	pGlDeleteShader = (PFNGLDELETESHADERPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glDeleteShader"));

	pGlGetAttachedObjectsARB = (PFNGLGETATTACHEDOBJECTSARBPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glGetAttachedObjectsARB"));

	pGlGetAttachedShaders = (PFNGLGETATTACHEDSHADERSPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glGetAttachedShaders"));

	pGlGetInfoLogARB = (PFNGLGETINFOLOGARBPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glGetInfoLogARB"));

	pGlGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glGetShaderInfoLog"));

	pGlGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glGetProgramInfoLog"));

	pGlGetObjectParameterivARB = (PFNGLGETOBJECTPARAMETERIVARBPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glGetObjectParameterivARB"));

	pGlGetShaderiv = (PFNGLGETSHADERIVPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glGetShaderiv"));

	pGlGetProgramiv = (PFNGLGETPROGRAMIVPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glGetProgramiv"));

	pGlGetUniformLocationARB = (PFNGLGETUNIFORMLOCATIONARBPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glGetUniformLocationARB"));

	pGlGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glGetUniformLocation"));

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

	pGlGetActiveUniform = (PFNGLGETACTIVEUNIFORMPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glGetActiveUniform"));

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

	// compressed textures
	pGlCompressedTexImage2D = (PFNGLCOMPRESSEDTEXIMAGE2DPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glCompressedTexImage2D"));

	// get vsync extension
	#if defined(GLX_SGI_swap_control) && !defined(_IRR_COMPILE_WITH_SDL_DEVICE_)
		pGlxSwapIntervalSGI = (PFNGLXSWAPINTERVALSGIPROC)IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glXSwapIntervalSGI"));
	#endif
	#if defined(GLX_EXT_swap_control) && !defined(_IRR_COMPILE_WITH_SDL_DEVICE_)
		pGlxSwapIntervalEXT = (PFNGLXSWAPINTERVALEXTPROC)IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glXSwapIntervalEXT"));
	#endif
	#if defined(GLX_MESA_swap_control) && !defined(_IRR_COMPILE_WITH_SDL_DEVICE_)
		pGlxSwapIntervalMESA = (PFNGLXSWAPINTERVALMESAPROC)IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glXSwapIntervalMESA"));
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

	pGlDrawBuffersARB = (PFNGLDRAWBUFFERSARBPROC)
	IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glDrawBuffersARB"));

	pGlDrawBuffersATI = (PFNGLDRAWBUFFERSATIPROC)
	IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glDrawBuffersATI"));

	pGlGenBuffersARB = (PFNGLGENBUFFERSARBPROC)
	IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glGenBuffersARB"));

	pGlBindBufferARB = (PFNGLBINDBUFFERARBPROC)
	IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glBindBufferARB"));

	pGlBufferDataARB = (PFNGLBUFFERDATAARBPROC)
	IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glBufferDataARB"));

	pGlDeleteBuffersARB = (PFNGLDELETEBUFFERSARBPROC)
	IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glDeleteBuffersARB"));

	pGlBufferSubDataARB = (PFNGLBUFFERSUBDATAARBPROC)
	IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glBufferSubDataARB"));

	pGlGetBufferSubDataARB = (PFNGLGETBUFFERSUBDATAARBPROC)
	IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glGetBufferSubDataARB"));

	pGlMapBufferARB = (PFNGLMAPBUFFERARBPROC)
	IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glMapBufferARB"));

	pGlUnmapBufferARB = (PFNGLUNMAPBUFFERARBPROC)
	IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glUnmapBufferARB"));

	pGlIsBufferARB = (PFNGLISBUFFERARBPROC)
	IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glIsBufferARB"));

	pGlGetBufferParameterivARB = (PFNGLGETBUFFERPARAMETERIVARBPROC)
	IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glGetBufferParameterivARB"));

	pGlGetBufferPointervARB = (PFNGLGETBUFFERPOINTERVARBPROC)
	IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glGetBufferPointervARB"));
	pGlProvokingVertexARB= (PFNGLPROVOKINGVERTEXPROC)
	IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glProvokingVertex"));
	pGlProvokingVertexEXT= (PFNGLPROVOKINGVERTEXEXTPROC)
	IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glProvokingVertexEXT"));
	pGlColorMaskIndexedEXT= (PFNGLCOLORMASKINDEXEDEXTPROC)
	IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glColorMaskIndexedEXT"));
	pGlEnableIndexedEXT= (PFNGLENABLEINDEXEDEXTPROC)
	IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glEnableIndexedEXT"));
	pGlDisableIndexedEXT= (PFNGLDISABLEINDEXEDEXTPROC)
	IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glDisableIndexedEXT"));
	pGlBlendFuncIndexedAMD= (PFNGLBLENDFUNCINDEXEDAMDPROC)
	IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glBlendFuncIndexedAMD"));
	pGlBlendFunciARB= (PFNGLBLENDFUNCIPROC)
	IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glBlendFunciARB"));
	pGlProgramParameteriARB = (PFNGLPROGRAMPARAMETERIARBPROC)
	IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glProgramParameteriARB"));
	pGlProgramParameteriEXT = (PFNGLPROGRAMPARAMETERIEXTPROC)
	IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glProgramParameteriEXT"));

	// occlusion query
	pGlGenQueriesARB = (PFNGLGENQUERIESARBPROC)
	IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glGenQueriesARB"));
 
	pGlDeleteQueriesARB = (PFNGLDELETEQUERIESARBPROC)
	IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glDeleteQueriesARB"));
 
	pGlIsQueryARB = (PFNGLISQUERYARBPROC)
	IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glIsQueryARB"));
 
	pGlBeginQueryARB = (PFNGLBEGINQUERYARBPROC)
	IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glBeginQueryARB"));

	pGlEndQueryARB = (PFNGLENDQUERYARBPROC)
	IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glEndQueryARB"));

	pGlGetQueryivARB = (PFNGLGETQUERYIVARBPROC)
	IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glGetQueryivARB"));

	pGlGetQueryObjectivARB = (PFNGLGETQUERYOBJECTIVARBPROC)
	IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glGetQueryObjectivARB"));

	pGlGetQueryObjectuivARB = (PFNGLGETQUERYOBJECTUIVARBPROC)
	IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glGetQueryObjectuivARB"));

	pGlGenOcclusionQueriesNV = (PFNGLGENOCCLUSIONQUERIESNVPROC)
	IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glGenOcclusionQueriesNV"));

	pGlDeleteOcclusionQueriesNV = (PFNGLDELETEOCCLUSIONQUERIESNVPROC)
	IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glDeleteOcclusionQueriesNV"));

	pGlIsOcclusionQueryNV = (PFNGLISOCCLUSIONQUERYNVPROC)
	IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glIsOcclusionQueryNV"));

	pGlBeginOcclusionQueryNV = (PFNGLBEGINOCCLUSIONQUERYNVPROC)
	IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glBeginOcclusionQueryNV"));

	pGlEndOcclusionQueryNV = (PFNGLENDOCCLUSIONQUERYNVPROC)
	IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glEndOcclusionQueryNV"));

	pGlGetOcclusionQueryivNV = (PFNGLGETOCCLUSIONQUERYIVNVPROC)
	IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glGetOcclusionQueryivNV"));

	pGlGetOcclusionQueryuivNV = (PFNGLGETOCCLUSIONQUERYUIVNVPROC)
	IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glGetOcclusionQueryuivNV"));

	#endif // _IRR_OPENGL_USE_EXTPOINTER_
#endif // _IRR_WINDOWS_API_

	GLint num;
	// set some properties
#if defined(GL_ARB_multitexture) || defined(GL_VERSION_1_3)
	if (Version>102 || FeatureAvailable[IRR_ARB_multitexture])
	{
		glGetIntegerv(GL_MAX_TEXTURE_UNITS, &num);
		MaxSupportedTextures=static_cast<u8>(num);
	}
#endif
	glGetIntegerv(GL_MAX_LIGHTS, &num);
	MaxLights=static_cast<u8>(num);
#ifdef GL_EXT_texture_filter_anisotropic
	if (FeatureAvailable[IRR_EXT_texture_filter_anisotropic])
	{
		glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &num);
		MaxAnisotropy=static_cast<u8>(num);
	}
#endif
#ifdef GL_VERSION_1_2
	if (Version>101)
	{
		glGetIntegerv(GL_MAX_ELEMENTS_INDICES, &num);
		MaxIndices=num;
	}
#endif
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &num);
	MaxTextureSize=static_cast<u32>(num);
	if (queryFeature(EVDF_GEOMETRY_SHADER))
	{
#if defined(GL_ARB_geometry_shader4) || defined(GL_EXT_geometry_shader4) || defined(GL_NV_geometry_shader4)
		glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_VERTICES_EXT, &num);
		MaxGeometryVerticesOut=static_cast<u32>(num);
#elif defined(GL_NV_geometry_program4)
		extGlGetProgramiv(GEOMETRY_PROGRAM_NV, GL_MAX_PROGRAM_OUTPUT_VERTICES_NV, &num);
		MaxGeometryVerticesOut=static_cast<u32>(num);
#endif
	}
#ifdef GL_EXT_texture_lod_bias
	if (FeatureAvailable[IRR_EXT_texture_lod_bias])
		glGetFloatv(GL_MAX_TEXTURE_LOD_BIAS_EXT, &MaxTextureLODBias);
#endif
	glGetIntegerv(GL_MAX_CLIP_PLANES, &num);
	MaxUserClipPlanes=static_cast<u8>(num);
	glGetIntegerv(GL_AUX_BUFFERS, &num);
	MaxAuxBuffers=static_cast<u8>(num);
#ifdef GL_ARB_draw_buffers
	if (FeatureAvailable[IRR_ARB_draw_buffers])
	{
		glGetIntegerv(GL_MAX_DRAW_BUFFERS_ARB, &num);
		MaxMultipleRenderTargets = static_cast<u8>(num);
	}
#endif
#if defined(GL_ATI_draw_buffers)
#ifdef GL_ARB_draw_buffers
	else
#endif
	if (FeatureAvailable[IRR_ATI_draw_buffers])
	{
		glGetIntegerv(GL_MAX_DRAW_BUFFERS_ATI, &num);
		MaxMultipleRenderTargets = static_cast<u8>(num);
	}
#endif
	glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, DimAliasedLine);
	glGetFloatv(GL_ALIASED_POINT_SIZE_RANGE, DimAliasedPoint);
	glGetFloatv(GL_SMOOTH_LINE_WIDTH_RANGE, DimSmoothedLine);
	glGetFloatv(GL_SMOOTH_POINT_SIZE_RANGE, DimSmoothedPoint);
#if defined(GL_ARB_shading_language_100) || defined (GL_VERSION_2_0)
	if (FeatureAvailable[IRR_ARB_shading_language_100] || Version>=200)
	{
		glGetError(); // clean error buffer
#ifdef GL_SHADING_LANGUAGE_VERSION
		const GLubyte* shaderVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
#else
		const GLubyte* shaderVersion = glGetString(GL_SHADING_LANGUAGE_VERSION_ARB);
#endif
		if (glGetError() == GL_INVALID_ENUM)
			ShaderLanguageVersion = 100;
		else
		{
			const f32 sl_ver = core::fast_atof(reinterpret_cast<const c8*>(shaderVersion));
			ShaderLanguageVersion = static_cast<u16>(core::floor32(sl_ver)*100+core::round32(core::fract(sl_ver)*10.0f));
		}
	}
#endif

#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (!pGlActiveTextureARB || !pGlClientActiveTextureARB)
	{
		MultiTextureExtension = false;
		os::Printer::log("Failed to load OpenGL's multitexture extension, proceeding without.", ELL_WARNING);
	}
	else
#endif
	MaxTextureUnits = core::min_(MaxSupportedTextures, static_cast<u8>(MATERIAL_MAX_TEXTURES));
	if (MaxTextureUnits < 2)
	{
		MultiTextureExtension = false;
		os::Printer::log("Warning: OpenGL device only has one texture unit. Disabling multitexturing.", ELL_WARNING);
	}

#ifdef GL_ARB_occlusion_query
	if (FeatureAvailable[IRR_ARB_occlusion_query])
	{
		extGlGetQueryiv(GL_SAMPLES_PASSED_ARB,GL_QUERY_COUNTER_BITS_ARB,
						&num);
		OcclusionQuerySupport=(num>0);
	}
	else
#endif
#ifdef GL_NV_occlusion_query
	if (FeatureAvailable[IRR_NV_occlusion_query])
	{
		glGetIntegerv(GL_PIXEL_COUNTER_BITS_NV, &num);
		OcclusionQuerySupport=(num>0);
	}
	else
#endif
		OcclusionQuerySupport=false;

#ifdef _DEBUG
	if (FeatureAvailable[IRR_NVX_gpu_memory_info])
	{
		// undocumented flags, so use the RAW values
		GLint val;
		glGetIntegerv(0x9047, &val);
		os::Printer::log("Dedicated video memory (kB)", core::stringc(val));
		glGetIntegerv(0x9048, &val);
		os::Printer::log("Total video memory (kB)", core::stringc(val));
		glGetIntegerv(0x9049, &val);
		os::Printer::log("Available video memory (kB)", core::stringc(val));
	}
#ifdef GL_ATI_meminfo
	if (FeatureAvailable[IRR_ATI_meminfo])
	{
		GLint val[4];
		glGetIntegerv(GL_TEXTURE_FREE_MEMORY_ATI, val);
		os::Printer::log("Free texture memory (kB)", core::stringc(val[0]));
		glGetIntegerv(GL_VBO_FREE_MEMORY_ATI, val);
		os::Printer::log("Free VBO memory (kB)", core::stringc(val[0]));
		glGetIntegerv(GL_RENDERBUFFER_FREE_MEMORY_ATI, val);
		os::Printer::log("Free render buffer memory (kB)", core::stringc(val[0]));
	}
#endif
#endif
}

bool COpenGLExtensionHandler::queryFeature(E_VIDEO_DRIVER_FEATURE feature) const
{
	switch (feature)
	{
	case EVDF_RENDER_TO_TARGET:
		return true;
	case EVDF_HARDWARE_TL:
		return true; // we cannot tell other things
	case EVDF_MULTITEXTURE:
		return MultiTextureExtension;
	case EVDF_BILINEAR_FILTER:
		return true;
	case EVDF_MIP_MAP:
		return true;
	case EVDF_MIP_MAP_AUTO_UPDATE:
		return FeatureAvailable[IRR_SGIS_generate_mipmap];
	case EVDF_STENCIL_BUFFER:
		return StencilBuffer;
	case EVDF_VERTEX_SHADER_1_1:
	case EVDF_ARB_VERTEX_PROGRAM_1:
		return FeatureAvailable[IRR_ARB_vertex_program] || FeatureAvailable[IRR_NV_vertex_program1_1];
	case EVDF_PIXEL_SHADER_1_1: 
	case EVDF_PIXEL_SHADER_1_2: 
	case EVDF_ARB_FRAGMENT_PROGRAM_1:
		return FeatureAvailable[IRR_ARB_fragment_program] || FeatureAvailable[IRR_NV_fragment_program];
	case EVDF_PIXEL_SHADER_2_0:
	case EVDF_VERTEX_SHADER_2_0:
	case EVDF_ARB_GLSL:
		return (FeatureAvailable[IRR_ARB_shading_language_100]||Version>=200);
	case EVDF_TEXTURE_NSQUARE:
		return true; // non-square is always supported
	case EVDF_TEXTURE_NPOT:
		// Some ATI cards seem to have only SW support in OpenGL 2.0
		// drivers if the extension is not exposed, so we skip this
		// extra test for now!
		// return (FeatureAvailable[IRR_ARB_texture_non_power_of_two]||Version>=200);
		return (FeatureAvailable[IRR_ARB_texture_non_power_of_two]);
	case EVDF_FRAMEBUFFER_OBJECT:
		return FeatureAvailable[IRR_EXT_framebuffer_object];
	case EVDF_VERTEX_BUFFER_OBJECT:
		return FeatureAvailable[IRR_ARB_vertex_buffer_object];
	case EVDF_COLOR_MASK:
		return true;
	case EVDF_ALPHA_TO_COVERAGE:
		return FeatureAvailable[IRR_ARB_multisample];
	case EVDF_GEOMETRY_SHADER:
		return FeatureAvailable[IRR_ARB_geometry_shader4] || FeatureAvailable[IRR_EXT_geometry_shader4] || FeatureAvailable[IRR_NV_geometry_program4] || FeatureAvailable[IRR_NV_geometry_shader4];
	case EVDF_MULTIPLE_RENDER_TARGETS:
		return FeatureAvailable[IRR_ARB_draw_buffers] || FeatureAvailable[IRR_ATI_draw_buffers];
	case EVDF_MRT_BLEND:
	case EVDF_MRT_COLOR_MASK:
		return FeatureAvailable[IRR_EXT_draw_buffers2];
	case EVDF_MRT_BLEND_FUNC:
		return FeatureAvailable[IRR_ARB_draw_buffers_blend] || FeatureAvailable[IRR_AMD_draw_buffers_blend];
	case EVDF_OCCLUSION_QUERY:
		return FeatureAvailable[IRR_ARB_occlusion_query] && OcclusionQuerySupport;
	case EVDF_POLYGON_OFFSET:
		// both features supported with OpenGL 1.1
		return Version>=110;
	default:
		return false;
	};
}


}
}

#endif
