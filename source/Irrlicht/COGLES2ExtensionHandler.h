// Copyright (C) 2009-2010 Amundis
// Heavily based on the OpenGL driver implemented by Nikolaus Gebhardt
// and OpenGL ES driver implemented by Christian Stehno
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#ifndef __C_OGLES2_EXTENSION_HANDLER_H_INCLUDED__
#define __C_OGLES2_EXTENSION_HANDLER_H_INCLUDED__

#include "IrrCompileConfig.h"

#ifdef _IRR_COMPILE_WITH_OGLES2_
#include <EGL/egl.h>
#include <GLES2/gl2.h>
// seems to be missing...
typedef char GLchar;
#if defined(_IRR_OGLES2_USE_EXTPOINTER_)
#include "gles2-ext.h"
#endif
#include "os.h"
#include "EDriverFeatures.h"

namespace irr
{
namespace video
{
	class COGLES2Driver;
	class COGLES2ExtensionHandler
	{
	public:
		enum EOGLES2Features
		{
			IRR_AMD_compressed_3DC_texture = 0, //39
			IRR_AMD_compressed_ATC_texture, //40
			IRR_AMD_performance_monitor, //50
			IRR_AMD_program_binary_Z400, //48
			IRR_ANGLE_framebuffer_blit, // 84
			IRR_ANGLE_framebuffer_multisample, // 85
			IRR_APPLE_framebuffer_multisample, // 79
			IRR_APPLE_rgb_422, // 77
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
			IRR_EXT_multi_draw_arrays, // 69
			IRR_EXT_read_format_bgra, // 66
			IRR_EXT_shader_texture_lod, // 78
			IRR_EXT_texture_compression_dxt1, //49
			IRR_EXT_texture_filter_anisotropic, //41
			IRR_EXT_texture_format_BGRA8888, //51
			IRR_EXT_texture_lod_bias, // 60
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

			IRR_OGLES2_Feature_Count
		};

		//! queries the features of the driver, returns true if feature is available
		bool queryOpenGLFeature(EOGLES2Features feature) const
		{
			return FeatureAvailable[feature];
		}


	protected:
		COGLES2ExtensionHandler();

		bool queryFeature(video::E_VIDEO_DRIVER_FEATURE feature) const
		{
			switch (feature)
			{
				case EVDF_RENDER_TO_TARGET:
				case EVDF_HARDWARE_TL:
					return true;
				case EVDF_MULTITEXTURE:
					return MultiTextureExtension;
				case EVDF_BILINEAR_FILTER:
				case EVDF_MIP_MAP:
				case EVDF_MIP_MAP_AUTO_UPDATE:
					return true;
				case EVDF_STENCIL_BUFFER:
					return StencilBuffer;
				case EVDF_TEXTURE_NSQUARE:
					return true; // non-square is always supported
				case EVDF_ARB_GLSL:
				case EVDF_TEXTURE_NPOT:
				case EVDF_FRAMEBUFFER_OBJECT:
				case EVDF_VERTEX_BUFFER_OBJECT:
				case EVDF_ALPHA_TO_COVERAGE:
				case EVDF_COLOR_MASK:
				case EVDF_POLYGON_OFFSET:
				case EVDF_BLEND_OPERATIONS:
					return true;
				case EVDF_MULTIPLE_RENDER_TARGETS:
				case EVDF_MRT_BLEND:
				case EVDF_MRT_COLOR_MASK:
				case EVDF_MRT_BLEND_FUNC:
				case EVDF_GEOMETRY_SHADER:
				case EVDF_OCCLUSION_QUERY:
					return false;
				default:
					return false;
			}
		}

		void dump() const;

		void initExtensions(COGLES2Driver* driver,
				EGLDisplay display, bool withStencil);

	protected:
		u16 EGLVersion;
		u16 Version;
		u8 MaxTextureUnits;
		u8 MaxSupportedTextures;
		u8 MaxLights;
		u8 MaxAnisotropy;
		u8 MaxUserClipPlanes;
		u32 MaxTextureSize;
		u32 MaxIndices;
		f32 MaxTextureLODBias;

		bool MultiTextureExtension;
		bool MultiSamplingExtension;
		bool StencilBuffer;
		bool FeatureAvailable[IRR_OGLES2_Feature_Count];
	};

} // end namespace video
} // end namespace irr


#endif // _IRR_COMPILE_WITH_OGLES2_
#endif

