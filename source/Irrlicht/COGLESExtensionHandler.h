// Copyright (C) 2008 Christian Stehno
// Heavily based on the OpenGL driver implemented by Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#ifndef __C_OGLES_EXTENSION_HANDLER_H_INCLUDED__
#define __C_OGLES_EXTENSION_HANDLER_H_INCLUDED__

#include "IrrCompileConfig.h"

#ifdef _IRR_COMPILE_WITH_OGLES1_
#include <GLES/egl.h>
#include <GLES/gl.h>
#include "gles-ext.h"
#include "os.h"
#include "EDriverFeatures.h"

#ifndef GL_BGRA
// we need to do this for the IMG_BGRA8888 extension
extern int GL_BGRA;
#endif

namespace irr
{
namespace video
{

	class COGLES1ExtensionHandler
	{
	protected:
	enum EOGLESFeatures {
		IRR_AMD_compressed_3DC_texture,
		IRR_AMD_compressed_ATC_texture,
		IRR_ARB_texture_env_combine,
		IRR_ARB_texture_env_dot3,
		IRR_EXT_multi_draw_arrays,
		IRR_EXT_texture_filter_anisotropic,
		IRR_IMG_read_format,
		IRR_IMG_texture_compression_pvrtc,
		IRR_IMG_texture_env_enhanced_fixed_function,
		IRR_IMG_texture_format_BGRA8888,
		IRR_IMG_user_clip_planes,
		IRR_IMG_vertex_program,
		IRR_OES_blend_equation_separate,
		IRR_OES_blend_func_separate,
		IRR_OES_blend_subtract,
		IRR_OES_byte_coordinates,
		IRR_OES_compressed_ETC1_RGB8_texture,
		IRR_OES_compressed_paletted_texture,
		IRR_OES_depth24,
		IRR_OES_depth32,
		IRR_OES_draw_texture,
		IRR_OES_EGL_image,
		IRR_OES_element_index_uint,
		IRR_OES_extended_matrix_palette,
		IRR_OES_fbo_render_mipmap,
		IRR_OES_fixed_point,
		IRR_OES_framebuffer_object,
		IRR_OES_mapbuffer,
		IRR_OES_matrix_get,
		IRR_OES_matrix_palette,
		IRR_OES_point_size_array,
		IRR_OES_point_sprite,
		IRR_OES_query_matrix,
		IRR_OES_read_format,
		IRR_OES_rgb8_rgba8,
		IRR_OES_single_precision,
		IRR_OES_stencil1,
		IRR_OES_stencil4,
		IRR_OES_stencil8,
		IRR_OES_stencil_wrap,
		IRR_OES_texture_cube_map,
		IRR_OES_texture_env_crossbar,
		IRR_OES_texture_mirrored_repeat,

		IRR_OGLES_Feature_Count
	};

		COGLES1ExtensionHandler();

		bool queryFeature(video::E_VIDEO_DRIVER_FEATURE feature) const
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
					return Version>100; // Supported in version 1.1
				case EVDF_STENCIL_BUFFER:
					return StencilBuffer;
				case EVDF_TEXTURE_NSQUARE:
					return true; // non-square is always supported
				default:
					return false;
			}
		}

		void dump() const;

		void initExtensions(EGLDisplay display, bool withStencil);

	public:
		void extGlDrawTex(GLfloat X, GLfloat Y, GLfloat Z, GLfloat W, GLfloat H)
		{
#if defined(_IRR_OPENGL_USE_EXTPOINTER_)
			if (pGlDrawTexfOES)
				pGlDrawTexfOES(X, Y, Z, W, H);
#elif defined(GL_OES_draw_texture)
			glDrawTexfOES(X, Y, Z, W, H);
#else
			os::Printer::log("glDrawTexture not supported", ELL_ERROR);
#endif
		}

		void extGlDrawTex(GLint X, GLint Y, GLint Z, GLint W, GLint H)
		{
#if defined(_IRR_OPENGL_USE_EXTPOINTER_)
			if (pGlDrawTexiOES)
				pGlDrawTexiOES(X, Y, Z, W, H);
#elif defined(GL_OES_draw_texture)
			glDrawTexiOES(X, Y, Z, W, H);
#else
			os::Printer::log("glDrawTexture not supported", ELL_ERROR);
#endif
		}

		void extGlDrawTex(GLfloat* coords)
		{
#if defined(_IRR_OPENGL_USE_EXTPOINTER_)
			if (pGlDrawTexfvOES)
				pGlDrawTexfvOES(coords);
#elif defined(GL_OES_draw_texture)
			glDrawTexfvOES(coords);
#else
			os::Printer::log("glDrawTexture not supported", ELL_ERROR);
#endif
		}

		void extGlDrawTex(GLint* coords)
		{
#if defined(_IRR_OPENGL_USE_EXTPOINTER_)
			if (pGlDrawTexivOES)
				pGlDrawTexivOES(coords);
#elif defined(GL_OES_draw_texture)
			glDrawTexivOES(coords);
#else
			os::Printer::log("glDrawTexture not supported", ELL_ERROR);
#endif
		}

		// we need to implement some methods which have been extensions in the original OpenGL driver
		void extGlActiveTexture(GLenum texture)
		{
			glActiveTexture(texture);
		}
		void extGlClientActiveTexture(GLenum texture)
		{
			glClientActiveTexture(texture);
		}
		void extGlGenBuffers(GLsizei n, GLuint *buffers)
		{
			glGenBuffers(n, buffers);
		}
		void extGlBindBuffer(GLenum target, GLuint buffer)
		{
			glBindBuffer(target, buffer);
		}
		void extGlBufferData(GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage)
		{
			glBufferData(target, size, data, usage);
		}
		void extGlBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid *data)
		{
			glBufferSubData(target, offset, size, data);
		}
		void extGlDeleteBuffers(GLsizei n, const GLuint *buffers)
		{
			glDeleteBuffers(n, buffers);
		}
		void extGlPointParameterf(GLint loc, GLfloat f)
		{
			glPointParameterf(loc, f);
		}
		void extGlPointParameterfv(GLint loc, const GLfloat *v)
		{
			glPointParameterfv(loc, v);
		}

	private:
#if defined(_IRR_OPENGL_USE_EXTPOINTER_)
		typedef void (APIENTRY * PFNGLDRAWTEXIOES) (GLint x, GLint y, GLint z, GLint width, GLint height);
		typedef void (APIENTRY * PFNGLDRAWTEXIVOES) (const GLint* coords);
		typedef void (APIENTRY * PFNGLDRAWTEXFOES) (GLfloat x, GLfloat y, GLfloat z, GLfloat width, GLfloat height);
		typedef void (APIENTRY * PFNGLDRAWTEXFVOES) (const GLfloat* coords);
		PFNGLDRAWTEXIOES pGlDrawTexiOES;
		PFNGLDRAWTEXFOES pGlDrawTexfOES;
		PFNGLDRAWTEXIVOES pGlDrawTexivOES;
		PFNGLDRAWTEXFVOES pGlDrawTexfvOES;
#endif

	protected:
		bool FeatureAvailable[IRR_OGLES_Feature_Count];
		s32 Version;
		u32 MaxUserClipPlanes;
		u32 MaxTextureUnits;
		s32 MaxLights;
		bool MultiTextureExtension;
		bool MultiSamplingExtension;
		bool StencilBuffer;
	};

} // end namespace video
} // end namespace irr


#endif // _IRR_COMPILE_WITH_OPENGL_
#endif
