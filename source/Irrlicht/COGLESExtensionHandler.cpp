// Copyright (C) 2008 Christian Stehno
// Heavily based on the OpenGL driver implemented by Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#include "IrrCompileConfig.h"

#ifdef _IRR_COMPILE_WITH_OGLES1_

#include "COGLESExtensionHandler.h"
#include "fast_atof.h"
#include "irrString.h"

namespace irr
{
namespace video
{

static const char* const OGLESFeatureStrings[] =
{
	"GL_AMD_compressed_3DC_texture",
	"GL_AMD_compressed_ATC_texture",
	"GL_ARB_texture_env_combine",
	"GL_ARB_texture_env_dot3",
	"GL_EXT_multi_draw_arrays",
	"GL_EXT_texture_filter_anisotropic",
	"GL_IMG_read_format",
	"GL_IMG_texture_compression_pvrtc",
	"GL_IMG_texture_env_enhanced_fixed_function",
	"GL_IMG_texture_format_BGRA8888",
	"GL_IMG_user_clip_planes",
	"GL_IMG_vertex_program",
	"GL_OES_blend_equation_separate",
	"GL_OES_blend_func_separate",
	"GL_OES_blend_subtract",
	"GL_OES_byte_coordinates",
	"GL_OES_compressed_ETC1_RGB8_texture",
	"GL_OES_compressed_paletted_texture",
	"GL_OES_depth24",
	"GL_OES_depth32",
	"GL_OES_draw_texture",
	"GL_OES_EGL_image",
	"GL_OES_element_index_uint",
	"GL_OES_extended_matrix_palette",
	"GL_OES_fbo_render_mipmap",
	"GL_OES_fixed_point",
	"GL_OES_framebuffer_object",
	"GL_OES_mapbuffer",
	"GL_OES_matrix_get",
	"GL_OES_matrix_palette",
	"GL_OES_point_size_array",
	"GL_OES_point_sprite",
	"GL_OES_query_matrix",
	"GL_OES_read_format",
	"GL_OES_rgb8_rgba8",
	"GL_OES_single_precision",
	"GL_OES_stencil1",
	"GL_OES_stencil4",
	"GL_OES_stencil8",
	"GL_OES_stencil_wrap",
	"GL_OES_texture_cube_map",
	"GL_OES_texture_env_crossbar",
	"GL_OES_texture_mirrored_repeat"
};


COGLES1ExtensionHandler::COGLES1ExtensionHandler() : 
#if defined(_IRR_OPENGL_USE_EXTPOINTER_)
		pGlDrawTexiOES(0), pGlDrawTexfOES(0),
		pGlDrawTexivOES(0), pGlDrawTexfvOES(0),
#endif
	Version(0), MaxUserClipPlanes(0), MaxTextureUnits(0), MaxLights(0),
	MultiTextureExtension(false), MultiSamplingExtension(false),
	StencilBuffer(false)
{
	for (u32 i=0; i<IRR_OGLES_Feature_Count; ++i)
		FeatureAvailable[i]=false;
}


void COGLES1ExtensionHandler::dump() const
{
	for (u32 i=0; i<IRR_OGLES_Feature_Count; ++i)
		os::Printer::log(OGLESFeatureStrings[i], FeatureAvailable[i]?" true":" false");
}


void COGLES1ExtensionHandler::initExtensions(EGLDisplay display, bool withStencil)
{
	const f32 ogl_ver = core::fast_atof(reinterpret_cast<const c8*>(eglQueryString(display, EGL_VERSION)));
	Version = core::floor32(ogl_ver)*100+core::ceil32(core::fract(ogl_ver)*10.0f);
	core::stringc eglExtensions = eglQueryString(display, EGL_EXTENSIONS);
	core::stringc extensions = glGetString(GL_EXTENSIONS);
	os::Printer::log(extensions.c_str());
	{
		c8* str = new c8[extensions.size()+1];
		strncpy(str, extensions.c_str(), extensions.size()+1);
		c8* p = str;

		for (u32 i=0; i<extensions.size(); ++i)
		{
			if (str[i] == ' ')
			{
				str[i] = 0;
				if (*p)
				for (u32 j=0; j<IRR_OGLES_Feature_Count; ++j)
				{
					if (!strcmp(OGLESFeatureStrings[j], p))
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
#ifndef GL_BGRA
	// whoa, pretty badly implemented extension...
	if (FeatureAvailable[IRR_IMG_texture_format_BGRA8888])
		GL_BGRA=0x80E1;
	else
		GL_BGRA=GL_RGBA;
#endif

	GLint val=0;
//	glGetIntegerv(GL_MAX_TEXTURES, &val);
	MaxTextureUnits = 2;
	MultiTextureExtension = true;
	glGetIntegerv(GL_MAX_CLIP_PLANES, &val);
	MaxUserClipPlanes=val;
	glGetIntegerv(GL_MAX_LIGHTS, &val);
	MaxLights = val;

#if defined(_IRR_OPENGL_USE_EXTPOINTER_)
	if (FeatureAvailable[IRR_OES_draw_texture])
	{
		pGlDrawTexiOES = (PFNGLDRAWTEXIOES) eglGetProcAddress("glDrawTexiOES");
		pGlDrawTexfOES = (PFNGLDRAWTEXFOES) eglGetProcAddress("glDrawTexfOES");
		pGlDrawTexivOES = (PFNGLDRAWTEXIVOES) eglGetProcAddress("glDrawTexivOES");
		pGlDrawTexfvOES = (PFNGLDRAWTEXFVOES) eglGetProcAddress("glDrawTexfvOES");
	}
#endif

}

} // end namespace video
} // end namespace irr


#endif // _IRR_COMPILE_WITH_OGLES1_
