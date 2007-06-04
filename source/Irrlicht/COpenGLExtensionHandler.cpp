#include "COpenGLExtensionHandler.h"
#include "os.h"
#include "irrString.h"
#include "SMaterial.h" // for MATERIAL_MAX_TEXTURES

#include "IrrCompileConfig.h"

#ifdef _IRR_COMPILE_WITH_OPENGL_

namespace irr
{
namespace video
{

COpenGLExtensionHandler::COpenGLExtensionHandler() :
		StencilBuffer(false),
		MultiTextureExtension(false), MultiSamplingExtension(false), AnisotropyExtension(false),
		SeparateStencilExtension(false),
		TextureCompressionExtension(false),
		PackedDepthStencilExtension(false),
		MaxTextureUnits(1), MaxLights(1), MaxIndices(1),
		MaxAnisotropy(1),
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
#ifdef _IRR_USE_WINDOWS_DEVICE_
	wglSwapIntervalEXT(0),
#elif defined(GLX_SGI_swap_control)
	glxSwapIntervalSGI(0),
#endif
	pGlBindFramebufferEXT(0), pGlDeleteFramebuffersEXT(0), pGlGenFramebuffersEXT(0),
	pGlCheckFramebufferStatusEXT(0), pGlFramebufferTexture2DEXT(0),
	pGlBindRenderbufferEXT(0), pGlDeleteRenderbuffersEXT(0), pGlGenRenderbuffersEXT(0),
	pGlRenderbufferStorageEXT(0), pGlFramebufferRenderbufferEXT(0)
#endif // _IRR_OPENGL_USE_EXTPOINTER_
{
		for (u32 i=0; i<IRR_OpenGL_Feature_Count; ++i)
			FeatureAvailable[i]=false;
	}


void COpenGLExtensionHandler::dump() const
{
	for (u32 i=0; i<IRR_OpenGL_Feature_Count; ++i)
		os::Printer::log(OpenGLFeatureStrings[i], FeatureAvailable[i]?" true":" false");
}

void COpenGLExtensionHandler::initExtensions(bool stencilBuffer)
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
		for (u32 i=0; i<IRR_OpenGL_Feature_Count; ++i)
			FeatureAvailable[i] = gluCheckExtension((const GLubyte*)OpenGLFeatureStrings[i], t);
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
				for (u32 i=0; i<IRR_OpenGL_Feature_Count; ++i)
					if (strstr(p, OpenGLFeatureStrings[i]))
						FeatureAvailable[i] = true;

				p = p + strlen(p) + 1;
			}
		}

		delete [] str;
	}

	MultiTextureExtension = FeatureAvailable[IRR_ARB_multitexture];
	MultiSamplingExtension = FeatureAvailable[IRR_ARB_multisample];
	AnisotropyExtension = FeatureAvailable[IRR_EXT_texture_filter_anisotropic];
	SeparateStencilExtension = FeatureAvailable[IRR_ATI_separate_stencil];
	TextureCompressionExtension = FeatureAvailable[IRR_ARB_texture_compression];
	PackedDepthStencilExtension = FeatureAvailable[IRR_EXT_packed_depth_stencil];
	SeparateSpecularColorExtension = FeatureAvailable[IRR_EXT_separate_specular_color];
	StencilBuffer=stencilBuffer;

#ifdef _IRR_WINDOWS_API_
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

	// compressed textures
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

	// vsync extension
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

	// compressed textures
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

	// set some properties
	glGetIntegerv(GL_MAX_TEXTURE_UNITS, &MaxTextureUnits);
	glGetIntegerv(GL_MAX_LIGHTS, &MaxLights);
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &MaxAnisotropy);

#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (!pGlActiveTextureARB || !pGlClientActiveTextureARB)
	{
		MultiTextureExtension = false;
//		ExtensionHandler.MultiTextureExtension = false;
		os::Printer::log("Failed to load OpenGL's multitexture extension, proceeding without.", ELL_WARNING);
	}
	else
#endif
	if (MaxTextureUnits < 2)
	{
		MultiTextureExtension = false;
//		ExtensionHandler.MultiTextureExtension = false;
		os::Printer::log("Warning: OpenGL device only has one texture unit. Disabling multitexturing.", ELL_WARNING);
	}
	MaxTextureUnits = core::min_((u32)MaxTextureUnits,MATERIAL_MAX_TEXTURES);
	glGetIntegerv(GL_MAX_ELEMENTS_INDICES, &MaxIndices);

}

bool COpenGLExtensionHandler::queryFeature(E_VIDEO_DRIVER_FEATURE feature) const
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
		return FeatureAvailable[IRR_SGIS_generate_mipmap];
	case EVDF_STENCIL_BUFFER:
		return StencilBuffer;
	case EVDF_ARB_VERTEX_PROGRAM_1:
		return FeatureAvailable[IRR_ARB_vertex_program];
	case EVDF_ARB_FRAGMENT_PROGRAM_1:
		return FeatureAvailable[IRR_ARB_fragment_program];
	case EVDF_ARB_GLSL:
		return FeatureAvailable[IRR_ARB_shading_language_100];
	case EVDF_TEXTURE_NPOT:
		return FeatureAvailable[IRR_ARB_texture_non_power_of_two];
	case EVDF_FRAMEBUFFER_OBJECT:
		return FeatureAvailable[IRR_EXT_framebuffer_object];
	default:
		return false;
	};
}

void COpenGLExtensionHandler::extGlActiveTextureARB(GLenum texture)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (MultiTextureExtension && pGlActiveTextureARB)
//	if (ExtensionHandler.MultiTextureExtension && pGlActiveTextureARB)
		pGlActiveTextureARB(texture);
#else
	if (MultiTextureExtension) glActiveTextureARB(texture);
//	if (ExtensionHandler.MultiTextureExtension) glActiveTextureARB(texture);
#endif
}

void COpenGLExtensionHandler::extGlClientActiveTextureARB(GLenum texture)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (MultiTextureExtension && pGlClientActiveTextureARB)
//	if (ExtensionHandler.MultiTextureExtension && pGlClientActiveTextureARB)
		pGlClientActiveTextureARB(texture);
#else
	if (MultiTextureExtension) glClientActiveTextureARB(texture);
//	if (ExtensionHandler.MultiTextureExtension) glClientActiveTextureARB(texture);
#endif
}

void COpenGLExtensionHandler::extGlGenProgramsARB(GLsizei n, GLuint *programs)
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

void COpenGLExtensionHandler::extGlBindProgramARB(GLenum target, GLuint program)
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

void COpenGLExtensionHandler::extGlProgramStringARB(GLenum target, GLenum format, GLsizei len, const GLvoid *string)
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

void COpenGLExtensionHandler::extGlDeleteProgramsARB(GLsizei n, const GLuint *programs)
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

void COpenGLExtensionHandler::extGlProgramLocalParameter4fvARB(GLenum n, GLuint i, const GLfloat * f)
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

GLhandleARB COpenGLExtensionHandler::extGlCreateShaderObjectARB(GLenum shaderType)
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

void COpenGLExtensionHandler::extGlShaderSourceARB(GLhandleARB shader, int numOfStrings, const char **strings, int *lenOfStrings)
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

void COpenGLExtensionHandler::extGlCompileShaderARB(GLhandleARB shader)
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

GLhandleARB COpenGLExtensionHandler::extGlCreateProgramObjectARB(void)
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

void COpenGLExtensionHandler::extGlAttachObjectARB(GLhandleARB program, GLhandleARB shader)
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

void COpenGLExtensionHandler::extGlLinkProgramARB(GLhandleARB program)
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

void COpenGLExtensionHandler::extGlUseProgramObjectARB(GLhandleARB prog)
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

void COpenGLExtensionHandler::extGlDeleteObjectARB(GLhandleARB object)
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

void COpenGLExtensionHandler::extGlGetInfoLogARB(GLhandleARB object, GLsizei maxLength, GLsizei *length, GLcharARB *infoLog)
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

void COpenGLExtensionHandler::extGlGetObjectParameterivARB(GLhandleARB object, GLenum type, int *param)
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

GLint COpenGLExtensionHandler::extGlGetUniformLocationARB(GLhandleARB program, const char *name)
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

void COpenGLExtensionHandler::extGlUniform4fvARB(GLint location, GLsizei count, const GLfloat *v)
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

void COpenGLExtensionHandler::extGlUniform1ivARB (GLint loc, GLsizei count, const GLint *v)
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

void COpenGLExtensionHandler::extGlUniform1fvARB (GLint loc, GLsizei count, const GLfloat *v)
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

void COpenGLExtensionHandler::extGlUniform2fvARB (GLint loc, GLsizei count, const GLfloat *v)
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

void COpenGLExtensionHandler::extGlUniform3fvARB (GLint loc, GLsizei count, const GLfloat *v)
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

void COpenGLExtensionHandler::extGlUniformMatrix2fvARB (GLint loc, GLsizei count, GLboolean transpose, const GLfloat *v)
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

void COpenGLExtensionHandler::extGlUniformMatrix3fvARB (GLint loc, GLsizei count, GLboolean transpose, const GLfloat *v)
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

void COpenGLExtensionHandler::extGlUniformMatrix4fvARB (GLint loc, GLsizei count, GLboolean transpose, const GLfloat *v)
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

void COpenGLExtensionHandler::extGlGetActiveUniformARB (GLhandleARB program, GLuint index, GLsizei maxlength, GLsizei *length, GLint *size, GLenum *type, GLcharARB *name)
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

void COpenGLExtensionHandler::extGlPointParameterfARB (GLint loc, GLfloat f)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlPointParameterfARB)
		pGlPointParameterfARB(loc, f);
#else
	glPointParameterfARB(loc, f);
#endif
}

void COpenGLExtensionHandler::extGlPointParameterfvARB (GLint loc, const GLfloat *v)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlPointParameterfvARB)
		pGlPointParameterfvARB(loc, v);
#else
	glPointParameterfvARB(loc, v);
#endif
}

void COpenGLExtensionHandler::extGlStencilFuncSeparate (GLenum frontfunc, GLenum backfunc, GLint ref, GLuint mask)
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

void COpenGLExtensionHandler::extGlStencilOpSeparate (GLenum face, GLenum fail, GLenum zfail, GLenum zpass)
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

void COpenGLExtensionHandler::extGlCompressedTexImage2D (GLenum target, GLint level, GLenum internalformat, GLsizei width,
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

void COpenGLExtensionHandler::extGlBindFramebufferEXT (GLenum target, GLuint framebuffer)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlBindFramebufferEXT)
		pGlBindFramebufferEXT(target, framebuffer);
#elif defined(GL_EXT_framebuffer_object)
	glBindFramebufferEXT(target, framebuffer);
#endif
}

void COpenGLExtensionHandler::extGlDeleteFramebuffersEXT (GLsizei n, const GLuint *framebuffers)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlDeleteFramebuffersEXT)
		pGlDeleteFramebuffersEXT(n, framebuffers);
#elif defined(GL_EXT_framebuffer_object)
	glDeleteFramebuffersEXT(n, framebuffers);
#endif
}

void COpenGLExtensionHandler::extGlGenFramebuffersEXT (GLsizei n, GLuint *framebuffers)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlGenFramebuffersEXT)
		pGlGenFramebuffersEXT(n, framebuffers);
#elif defined(GL_EXT_framebuffer_object)
	glGenFramebuffersEXT(n, framebuffers);
#endif
}

GLenum COpenGLExtensionHandler::extGlCheckFramebufferStatusEXT (GLenum target)
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

void COpenGLExtensionHandler::extGlFramebufferTexture2DEXT (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlFramebufferTexture2DEXT)
		pGlFramebufferTexture2DEXT(target, attachment, textarget, texture, level);
#elif defined(GL_EXT_framebuffer_object)
	glFramebufferTexture2DEXT(target, attachment, textarget, texture, level);
#endif
}

void COpenGLExtensionHandler::extGlBindRenderbufferEXT (GLenum target, GLuint renderbuffer)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlBindRenderbufferEXT)
		pGlBindRenderbufferEXT(target, renderbuffer);
#elif defined(GL_EXT_framebuffer_object)
	glBindRenderbufferEXT(target, renderbuffer);
#endif
}

void COpenGLExtensionHandler::extGlDeleteRenderbuffersEXT (GLsizei n, const GLuint *renderbuffers)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlDeleteRenderbuffersEXT)
		pGlDeleteRenderbuffersEXT(n, renderbuffers);
#elif defined(GL_EXT_framebuffer_object)
	glDeleteRenderbuffersEXT(n, renderbuffers);
#endif
}

void COpenGLExtensionHandler::extGlGenRenderbuffersEXT (GLsizei n, GLuint *renderbuffers)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlGenRenderbuffersEXT)
		pGlGenRenderbuffersEXT(n, renderbuffers);
#elif defined(GL_EXT_framebuffer_object)
	glGenRenderbuffersEXT(n, renderbuffers);
#endif
}

void COpenGLExtensionHandler::extGlRenderbufferStorageEXT (GLenum target, GLenum internalformat, GLsizei width, GLsizei height)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlRenderbufferStorageEXT)
		pGlRenderbufferStorageEXT(target, internalformat, width, height);
#elif defined(GL_EXT_framebuffer_object)
	glRenderbufferStorageEXT(target, internalformat, width, height);
#endif
}

void COpenGLExtensionHandler::extGlFramebufferRenderbufferEXT (GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlFramebufferRenderbufferEXT)
		pGlFramebufferRenderbufferEXT(target, attachment, renderbuffertarget, renderbuffer);
#elif defined(GL_EXT_framebuffer_object)
	glFramebufferRenderbufferEXT(target, attachment, renderbuffertarget, renderbuffer);
#endif
}


}
}

#endif
