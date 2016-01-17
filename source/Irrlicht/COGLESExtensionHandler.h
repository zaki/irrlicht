// Copyright (C) 2008 Christian Stehno
// Heavily based on the OpenGL driver implemented by Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#ifndef __C_OGLES_EXTENSION_HANDLER_H_INCLUDED__
#define __C_OGLES_EXTENSION_HANDLER_H_INCLUDED__

#include "IrrCompileConfig.h"

#ifdef _IRR_COMPILE_WITH_OGLES1_

#include "EDriverFeatures.h"
#include "irrTypes.h"
#include "os.h"

#include "COGLESCommon.h"

#include "COpenGLCoreFeature.h"

namespace irr
{
namespace video
{

	class COGLES1ExtensionHandler
	{
	public:
		enum EOGLES1Features
		{
			IRR_AMD_compressed_3DC_texture = 0, //39
			IRR_AMD_compressed_ATC_texture, //40
			IRR_AMD_performance_monitor, //50
			IRR_AMD_program_binary_Z400, //48
			IRR_ANGLE_framebuffer_blit, // 84
			IRR_ANGLE_framebuffer_multisample, // 85
			IRR_APPLE_copy_texture_levels, // 123
			IRR_APPLE_framebuffer_multisample, // 79
			IRR_APPLE_rgb_422, // 77
			IRR_APPLE_sync, // 124
			IRR_APPLE_texture_2D_limited_npot, // 59
			IRR_APPLE_texture_format_BGRA8888, // 80
			IRR_APPLE_texture_max_level, // 81
			IRR_ARB_texture_env_combine, //ogl, IMG simulator
			IRR_ARB_texture_env_dot3, //ogl, IMG simulator
			IRR_ARM_mali_shader_binary, // 82
			IRR_ARM_rgba8, // 83
			IRR_DMP_shader_binary, // 89
			IRR_EXT_blend_minmax, // 65
			IRR_EXT_discard_framebuffer, // 64
			IRR_EXT_frag_depth, // 87
			IRR_EXT_map_buffer_range, // 121
			IRR_EXT_multisampled_render_to_texture, // 106
			IRR_EXT_multi_draw_arrays, // 69
			IRR_EXT_robustness, // 107
			IRR_EXT_read_format_bgra, // 66
			IRR_EXT_shader_texture_lod, // 78
			IRR_EXT_sRGB, // 105
			IRR_EXT_texture_compression_dxt1, //49
			IRR_EXT_texture_filter_anisotropic, //41
			IRR_EXT_texture_format_BGRA8888, //51
			IRR_EXT_texture_lod_bias, // 60
			IRR_EXT_texture_storage, // 108
			IRR_EXT_texture_type_2_10_10_10_REV, //42
			IRR_IMG_multisampled_render_to_texture, // 75
			IRR_IMG_program_binary, // 67
			IRR_IMG_read_format, //53
			IRR_IMG_shader_binary, // 68
			IRR_IMG_texture_compression_pvrtc, //54
			IRR_IMG_texture_env_enhanced_fixed_function, // 58
			IRR_IMG_texture_format_BGRA8888, // replaced by EXT version
			IRR_IMG_user_clip_plane, // 57, was clip_planes
			IRR_IMG_vertex_program, // non-standard
			IRR_NV_coverage_sample, // 73
			IRR_NV_depth_nonlinear, // 74
			IRR_NV_fence, //52
			IRR_OES_blend_equation_separate, //1
			IRR_OES_blend_func_separate, //2
			IRR_OES_blend_subtract, //3
			IRR_OES_byte_coordinates, //4
			IRR_OES_compressed_ETC1_RGB8_texture, //5
			IRR_OES_compressed_paletted_texture, //6
			IRR_OES_depth24, //24
			IRR_OES_depth32, //25
			IRR_OES_depth_texture, //43
			IRR_OES_draw_texture, //7
			IRR_OES_EGL_image, //23
			IRR_OES_EGL_image_external, // 88
			IRR_OES_EGL_sync, // 76
			IRR_OES_element_index_uint, //26
			IRR_OES_extended_matrix_palette, //8
			IRR_OES_fbo_render_mipmap, //27
			IRR_OES_fixed_point, //9
			IRR_OES_fragment_precision_high, //28
			IRR_OES_framebuffer_object, //10
			IRR_OES_get_program_binary, //47
			IRR_OES_mapbuffer, //29
			IRR_OES_matrix_get, //11
			IRR_OES_matrix_palette, //12
			IRR_OES_packed_depth_stencil, //44
			IRR_OES_point_size_array, //14
			IRR_OES_point_sprite, //15
			IRR_OES_query_matrix, //16
			IRR_OES_read_format, //17
			IRR_OES_required_internalformat, // 115
			IRR_OES_rgb8_rgba8, //30
			IRR_OES_single_precision, //18
			IRR_OES_standard_derivatives, //45
			IRR_OES_stencil1, //31
			IRR_OES_stencil4, //32
			IRR_OES_stencil8, //33
			IRR_OES_stencil_wrap, //19
			IRR_OES_texture_3D, //34
			IRR_OES_texture_cube_map, //20
			IRR_OES_texture_env_crossbar, //21
			IRR_OES_texture_float, //36
			IRR_OES_texture_float_linear, //35
			IRR_OES_texture_half_float, //36
			IRR_OES_texture_half_float_linear, //35
			IRR_OES_texture_mirrored_repeat, //22
			IRR_OES_texture_npot, //37
			IRR_OES_vertex_array_object, // 72
			IRR_OES_vertex_half_float, //38
			IRR_OES_vertex_type_10_10_10_2, //46
			IRR_QCOM_driver_control, //55
			IRR_QCOM_extended_get, // 62
			IRR_QCOM_extended_get2, // 63
			IRR_QCOM_performance_monitor_global_mode, //56
			IRR_QCOM_tiled_rendering, // 71
			IRR_QCOM_writeonly_rendering, // 61
			IRR_SUN_multi_draw_arrays, // 70
			IRR_VIV_shader_binary, // 86

			IRR_OGLES1_Feature_Count
		};

		COGLES1ExtensionHandler();

		void dump() const;

		void initExtensions();

		const COpenGLCoreFeature& getFeature() const;

		bool queryFeature(video::E_VIDEO_DRIVER_FEATURE feature) const
		{
			switch (feature)
			{
			case EVDF_RENDER_TO_TARGET:
			case EVDF_HARDWARE_TL:
			case EVDF_MULTITEXTURE:
			case EVDF_BILINEAR_FILTER:
			case EVDF_MIP_MAP:
			case EVDF_TEXTURE_NSQUARE:
			case EVDF_STENCIL_BUFFER:
			case EVDF_ALPHA_TO_COVERAGE:
			case EVDF_COLOR_MASK:
			case EVDF_POLYGON_OFFSET:
			case EVDF_TEXTURE_MATRIX:
				return true;
			case EVDF_TEXTURE_NPOT:
				return FeatureAvailable[IRR_APPLE_texture_2D_limited_npot] || FeatureAvailable[IRR_OES_texture_npot];
			case EVDF_MIP_MAP_AUTO_UPDATE:
				return Version>100;
			case EVDF_BLEND_OPERATIONS:
				return FeatureAvailable[IRR_OES_blend_subtract];
			case EVDF_BLEND_SEPARATE:
				return FeatureAvailable[IRR_OES_blend_func_separate];
			case EVDF_FRAMEBUFFER_OBJECT:
				return FeatureAvailable[IRR_OES_framebuffer_object];
			case EVDF_VERTEX_BUFFER_OBJECT:
				return Version>100;
			case EVDF_TEXTURE_COMPRESSED_DXT:
				return false; // NV Tegra need improvements here
			case EVDF_TEXTURE_COMPRESSED_PVRTC:
				return FeatureAvailable[IRR_IMG_texture_compression_pvrtc];
			case EVDF_TEXTURE_COMPRESSED_ETC1:
				return FeatureAvailable[IRR_OES_compressed_ETC1_RGB8_texture];
			case EVDF_TEXTURE_CUBEMAP:
				return FeatureAvailable[IRR_OES_texture_cube_map];
			default:
				return true;
			};
		}

		bool queryOpenGLFeature(EOGLES1Features feature) const
		{
			return FeatureAvailable[feature];
		}

		inline void irrGlActiveTexture(GLenum texture)
		{
			glActiveTexture(texture);
		}

		inline void irrGlCompressedTexImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border,
			GLsizei imageSize, const void* data)
		{
			glCompressedTexImage2D(target, level, internalformat, width, height, border, imageSize, data);
		}

		inline void irrGlCompressedTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
			GLenum format, GLsizei imageSize, const void* data)
		{
			glCompressedTexSubImage2D(target, level, xoffset, yoffset, width, height, format, imageSize, data);
		}

		inline void irrGlUseProgram(GLuint prog)
		{
		}

		inline void irrGlBindFramebuffer(GLenum target, GLuint framebuffer)
		{
#ifdef _IRR_OGLES1_USE_EXTPOINTER_
			if (pGlBindFramebufferOES)
				pGlBindFramebufferOES(target, framebuffer);
#elif defined(GL_OES_framebuffer_object)
			glBindFramebufferOES(target, framebuffer);
#endif
		}

		inline void irrGlDeleteFramebuffers(GLsizei n, const GLuint *framebuffers)
		{
#ifdef _IRR_OGLES1_USE_EXTPOINTER_
			if (pGlDeleteFramebuffersOES)
				pGlDeleteFramebuffersOES(n, framebuffers);
#elif defined(GL_OES_framebuffer_object)
			glDeleteFramebuffersOES(n, framebuffers);
#endif
		}

		inline void irrGlGenFramebuffers(GLsizei n, GLuint *framebuffers)
		{
#ifdef _IRR_OGLES1_USE_EXTPOINTER_
			if (pGlGenFramebuffersOES)
				pGlGenFramebuffersOES(n, framebuffers);
#elif defined(GL_OES_framebuffer_object)
			glGenFramebuffersOES(n, framebuffers);
#endif
		}

		inline GLenum irrGlCheckFramebufferStatus(GLenum target)
		{
#ifdef _IRR_OGLES1_USE_EXTPOINTER_
			if (pGlCheckFramebufferStatusOES)
				return pGlCheckFramebufferStatusOES(target);
			else
				return 0;
#elif defined(GL_OES_framebuffer_object)
			return glCheckFramebufferStatusOES(target);
#else
			return 0;
#endif
		}

		inline void irrGlFramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
		{
#ifdef _IRR_OGLES1_USE_EXTPOINTER_
			if (pGlFramebufferTexture2DOES)
				pGlFramebufferTexture2DOES(target, attachment, textarget, texture, level);
#elif defined(GL_OES_framebuffer_object)
			glFramebufferTexture2DOES(target, attachment, textarget, texture, level);
#endif
		}

		inline void irrGlGenerateMipmap(GLenum target)
		{
#ifdef _IRR_OGLES1_USE_EXTPOINTER_
			if (pGlGenerateMipmapOES)
				pGlGenerateMipmapOES(target);
#elif defined(GL_OES_framebuffer_object)
			glGenerateMipmapOES(target);
#endif
		}

		inline void irrGlActiveStencilFace(GLenum face)
		{
		}

		inline void irrGlDrawBuffer(GLenum mode)
		{
		}

		inline void irrGlDrawBuffers(GLsizei n, const GLenum *bufs)
		{
		}

		inline void irrGlBlendFuncSeparate(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha)
		{
#ifdef _IRR_OGLES1_USE_EXTPOINTER_
			if (pGlBlendFuncSeparateOES)
				pGlBlendFuncSeparateOES(srcRGB, dstRGB, srcAlpha, dstAlpha);
#elif defined(GL_OES_blend_func_separate)
			glBlendFuncSeparateOES(srcRGB, dstRGB, srcAlpha, dstAlpha);
#endif
		}

		inline void irrGlBlendEquation(GLenum mode)
		{
#ifdef _IRR_OGLES1_USE_EXTPOINTER_
			if (pGlBlendEquationOES)
				pGlBlendEquationOES(mode);
#elif defined(GL_OES_blend_subtract)
			glBlendEquationOES(mode);
#endif
		}

		inline void irrGlEnableIndexed(GLenum target, GLuint index)
		{
		}

		inline void irrGlDisableIndexed(GLenum target, GLuint index)
		{
		}

		inline void irrGlColorMaskIndexed(GLuint buf, GLboolean r, GLboolean g, GLboolean b, GLboolean a)
		{
		}

		inline void irrGlBlendFuncIndexed(GLuint buf, GLenum src, GLenum dst)
		{
		}

		inline void irrGlBlendFuncSeparateIndexed(GLuint buf, GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha)
		{
		}

		inline void irrGlBlendEquationIndexed(GLuint buf, GLenum mode)
		{
		}

		inline void irrGlBlendEquationSeparateIndexed(GLuint buf, GLenum modeRGB, GLenum modeAlpha)
		{
		}

	protected:
		COpenGLCoreFeature Feature;

		u16 Version;
		u8 MaxUserClipPlanes;
		u8 MaxLights;
		u8 MaxAnisotropy;
		u32 MaxIndices;
		u32 MaxTextureSize;
		f32 MaxTextureLODBias;
		//! Minimal and maximal supported thickness for lines without smoothing
		GLfloat DimAliasedLine[2];
		//! Minimal and maximal supported thickness for points without smoothing
		GLfloat DimAliasedPoint[2];
		bool StencilBuffer;
		bool FeatureAvailable[IRR_OGLES1_Feature_Count];

#if defined(_IRR_OGLES1_USE_EXTPOINTER_)
		PFNGLBLENDEQUATIONOESPROC pGlBlendEquationOES;
		PFNGLBLENDFUNCSEPARATEOESPROC pGlBlendFuncSeparateOES;
		PFNGLBINDFRAMEBUFFEROESPROC pGlBindFramebufferOES;
		PFNGLDELETEFRAMEBUFFERSOESPROC pGlDeleteFramebuffersOES;
		PFNGLGENFRAMEBUFFERSOESPROC pGlGenFramebuffersOES;
		PFNGLCHECKFRAMEBUFFERSTATUSOESPROC pGlCheckFramebufferStatusOES;
		PFNGLFRAMEBUFFERTEXTURE2DOESPROC pGlFramebufferTexture2DOES;
		PFNGLGENERATEMIPMAPOESPROC pGlGenerateMipmapOES;
#endif
	};

}
}

#endif
#endif
