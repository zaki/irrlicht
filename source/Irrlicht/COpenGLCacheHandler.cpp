// Copyright (C) 2015 Patryk Nadrowski
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#include "COpenGLCacheHandler.h"

#ifdef _IRR_COMPILE_WITH_OPENGL_

#include "COpenGLDriver.h"
#include "COpenGLTexture.h"

namespace irr
{
namespace video
{

/* COpenGLCacheHandler::STextureCache */

COpenGLCacheHandler::STextureCache::STextureCache(COpenGLCacheHandler* cacheHandler, u32 textureCount) :
	CacheHandler(cacheHandler), TextureCount(textureCount)
{
	for (u32 i = 0; i < MATERIAL_MAX_TEXTURES; ++i)
	{
		Texture[i] = 0;
	}
}

COpenGLCacheHandler::STextureCache::~STextureCache()
{
	clear();
}

const COpenGLTexture* COpenGLCacheHandler::STextureCache::operator[](int index) const
{
	if (static_cast<u32>(index) < MATERIAL_MAX_TEXTURES)
		return Texture[static_cast<u32>(index)];

	return 0;
}

bool COpenGLCacheHandler::STextureCache::set(u32 index, const ITexture* texture)
{
	bool status = false;
	
	E_DRIVER_TYPE type = EDT_OPENGL;

	if (index < MATERIAL_MAX_TEXTURES && index < TextureCount)
	{
		CacheHandler->setActiveTexture(GL_TEXTURE0_ARB + index);

		const COpenGLTexture* prevTexture = Texture[index];

		if (texture != prevTexture)
		{
			if (texture)
			{
				type = texture->getDriverType();

				if (type == EDT_OPENGL)
				{
					texture->grab();

					if (!prevTexture)
						glEnable(GL_TEXTURE_2D);

					glBindTexture(GL_TEXTURE_2D, static_cast<const COpenGLTexture*>(texture)->getOpenGLTextureName());
				}
				else
				{
					texture = 0;

					os::Printer::log("Fatal Error: Tried to set a texture not owned by this driver.", ELL_ERROR);
				}
			}
			
			if (!texture)
			{
				glBindTexture(GL_TEXTURE_2D, 0);

				if (prevTexture)
					glDisable(GL_TEXTURE_2D);
			}

			Texture[index] = static_cast<const COpenGLTexture*>(texture);

			if (prevTexture)
				prevTexture->drop();
		}

		status = true;
	}

	return (status && type == EDT_OPENGL);
}

void COpenGLCacheHandler::STextureCache::remove(ITexture* texture)
{
	if (!texture)
		return;

	for (u32 i = 0; i < MATERIAL_MAX_TEXTURES; ++i)
	{
		if (Texture[i] == texture)
		{
			Texture[i] = 0;

			texture->drop();
		}
	}
}

void COpenGLCacheHandler::STextureCache::clear()
{
	for (u32 i = 0; i < MATERIAL_MAX_TEXTURES; ++i)
	{
		if (Texture[i])
		{
			const COpenGLTexture* prevTexture = Texture[i];

			Texture[i] = 0;

			prevTexture->drop();
		}
	}
}

/* COpenGLCacheHandler */

COpenGLCacheHandler::COpenGLCacheHandler(COpenGLDriver* driver) :
	TextureCache(STextureCache(this, driver->MaxSupportedTextures)), Driver(driver), FrameBufferCount(0),
	AlphaMode(GL_ALWAYS), AlphaRef(0.f), AlphaTest(false), BlendEquation(0), BlendSourceRGB(0), BlendDestinationRGB(0),
	BlendSourceAlpha(0), BlendDestinationAlpha(0), Blend(0), ColorMask(0), CullFaceMode(GL_BACK), CullFace(false),
	DepthFunc(GL_LESS), DepthMask(true), DepthTest(false), FrameBufferID(0), MatrixMode(GL_MODELVIEW), ActiveTexture(GL_TEXTURE0_ARB),
	ClientActiveTexture(GL_TEXTURE0_ARB), ClientStateVertex(false), ClientStateNormal(false),ClientStateColor(false),
	ClientStateTexCoord0(false), ViewportX(0), ViewportY(0)
{
	FrameBufferCount = core::max_(static_cast<GLuint>(1), static_cast<GLuint>(Driver->MaxMultipleRenderTargets));

	BlendEquation = new GLenum[FrameBufferCount];
	BlendSourceRGB = new GLenum[FrameBufferCount];
	BlendDestinationRGB = new GLenum[FrameBufferCount];
	BlendSourceAlpha = new GLenum[FrameBufferCount];
	BlendDestinationAlpha = new GLenum[FrameBufferCount];
	Blend = new bool[FrameBufferCount];

	ColorMask = new bool[FrameBufferCount][4];

	// Initial OpenGL values from specification.

	glAlphaFunc(AlphaMode, AlphaRef);
	glDisable(GL_ALPHA_TEST);

	if (Driver->queryFeature(EVDF_BLEND_OPERATIONS))
	{
#if defined(GL_VERSION_1_4)
		Driver->extGlBlendEquation(GL_FUNC_ADD);
#elif defined(GL_EXT_blend_subtract) || defined(GL_EXT_blend_minmax) || defined(GL_EXT_blend_logic_op)
		Driver->extGlBlendEquation(GL_FUNC_ADD_EXT);
#endif
	}

	for (u32 i = 0; i < FrameBufferCount; ++i)
	{
#if defined(GL_VERSION_1_4)
		BlendEquation[i] = GL_FUNC_ADD;
#elif defined(GL_EXT_blend_subtract) || defined(GL_EXT_blend_minmax) || defined(GL_EXT_blend_logic_op)
		BlendEquation[i] = GL_FUNC_ADD_EXT;
#endif

		BlendSourceRGB[i] = GL_ONE;
		BlendDestinationRGB[i] = GL_ZERO;
		BlendSourceAlpha[i] = GL_ONE;
		BlendDestinationAlpha[i] = GL_ZERO;

		Blend[i] = false;

		for (u32 j = 0; j < 4; ++j)
			ColorMask[i][j] = true;
	}

	glBlendFunc(GL_ONE, GL_ZERO);
	glDisable(GL_BLEND);

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	glCullFace(CullFaceMode);
	glDisable(GL_CULL_FACE);

	glDepthFunc(DepthFunc);
	glDepthMask(GL_TRUE);
	glDisable(GL_DEPTH_TEST);

	glMatrixMode(MatrixMode);

	if (Driver->MultiTextureExtension)
	{
		Driver->extGlActiveTexture(ActiveTexture);
		Driver->extGlClientActiveTexture(ClientActiveTexture);
	}

	glDisable(GL_TEXTURE_2D);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	const core::dimension2d<u32> ScreenSize = Driver->getScreenSize();
	ViewportWidth = ScreenSize.Width;
	ViewportHeight = ScreenSize.Height;
	glViewport(ViewportX, ViewportY, ViewportWidth, ViewportHeight);
}

COpenGLCacheHandler::~COpenGLCacheHandler()
{
	delete[] BlendEquation;
	delete[] BlendSourceRGB;
	delete[] BlendDestinationRGB;
    delete[] BlendSourceAlpha;
	delete[] BlendDestinationAlpha;
	delete[] Blend;

	delete[] ColorMask;
}

void COpenGLCacheHandler::setAlphaFunc(GLenum mode, GLclampf ref)
{
	if (AlphaMode != mode || AlphaRef != ref)
	{
		glAlphaFunc(mode, ref);

		AlphaMode = mode;
		AlphaRef = ref;
	}
}

void COpenGLCacheHandler::setAlphaTest(bool enable)
{
	if (AlphaTest != enable)
	{
		if (enable)
			glEnable(GL_ALPHA_TEST);
		else
			glDisable(GL_ALPHA_TEST);
		AlphaTest = enable;
	}
}

void COpenGLCacheHandler::setBlendEquation(GLenum mode)
{
	if (BlendEquation[0] != mode)
	{
		Driver->extGlBlendEquation(mode);

		for (GLuint i = 0; i < FrameBufferCount; ++i)
			BlendEquation[i] = mode;
	}
}

void COpenGLCacheHandler::setBlendEquationIndexed(GLuint index, GLenum mode)
{
	if (index < FrameBufferCount && BlendEquation[index] != mode)
	{
		Driver->extGlBlendEquationIndexed(index, mode);

		BlendEquation[index] = mode;
	}
}

void COpenGLCacheHandler::setBlendFunc(GLenum source, GLenum destination)
{
	if (BlendSourceRGB[0] != source || BlendDestinationRGB[0] != destination ||
        BlendSourceAlpha[0] != source || BlendDestinationAlpha[0] != destination)
	{
		glBlendFunc(source, destination);

		for (GLuint i = 0; i < FrameBufferCount; ++i)
		{
			BlendSourceRGB[i] = source;
			BlendDestinationRGB[i] = destination;
			BlendSourceAlpha[i] = source;
			BlendDestinationAlpha[i] = destination;
		}
	}
}

void COpenGLCacheHandler::setBlendFuncSeparate(GLenum sourceRGB, GLenum destinationRGB, GLenum sourceAlpha, GLenum destinationAlpha)
{
    if (sourceRGB != sourceAlpha || destinationRGB != destinationAlpha)
    {
        if (BlendSourceRGB[0] != sourceRGB || BlendDestinationRGB[0] != destinationRGB ||
            BlendSourceAlpha[0] != sourceAlpha || BlendDestinationAlpha[0] != destinationAlpha)
        {
            Driver->extGlBlendFuncSeparate(sourceRGB, destinationRGB, sourceAlpha, destinationAlpha);

            for (GLuint i = 0; i < FrameBufferCount; ++i)
            {
                BlendSourceRGB[i] = sourceRGB;
                BlendDestinationRGB[i] = destinationRGB;
                BlendSourceAlpha[i] = sourceAlpha;
                BlendDestinationAlpha[i] = destinationAlpha;
            }
        }
    }
    else
    {
        setBlendFunc(sourceRGB, destinationRGB);
    }
}

void COpenGLCacheHandler::setBlendFuncIndexed(GLuint index, GLenum source, GLenum destination)
{
	if (index < FrameBufferCount && (BlendSourceRGB[index] != source || BlendDestinationRGB[index] != destination ||
        BlendSourceAlpha[index] != source || BlendDestinationAlpha[index] != destination))
	{
		Driver->extGlBlendFuncIndexed(index, source, destination);

        BlendSourceRGB[index] = source;
        BlendDestinationRGB[index] = destination;
        BlendSourceAlpha[index] = source;
        BlendDestinationAlpha[index] = destination;
	}
}

void COpenGLCacheHandler::setBlendFuncSeparateIndexed(GLuint index, GLenum sourceRGB, GLenum destinationRGB, GLenum sourceAlpha, GLenum destinationAlpha)
{
    if (sourceRGB != sourceAlpha || destinationRGB != destinationAlpha)
    {
        if (index < FrameBufferCount && (BlendSourceRGB[index] != sourceRGB || BlendDestinationRGB[index] != destinationRGB ||
            BlendSourceAlpha[index] != sourceAlpha || BlendDestinationAlpha[index] != destinationAlpha))
        {
            Driver->extGlBlendFuncSeparateIndexed(index, sourceRGB, destinationRGB, sourceAlpha, destinationAlpha);

            BlendSourceRGB[index] = sourceRGB;
            BlendDestinationRGB[index] = destinationRGB;
            BlendSourceAlpha[index] = sourceAlpha;
            BlendDestinationAlpha[index] = destinationAlpha;
        }
    }
    else
    {
        setBlendFuncIndexed(index, sourceRGB, destinationRGB);
    }
}

void COpenGLCacheHandler::setBlend(bool enable)
{
	if (Blend[0] != enable)
	{
		if (enable)
			glEnable(GL_BLEND);
		else
			glDisable(GL_BLEND);

		for (GLuint i = 0; i < FrameBufferCount; ++i)
			Blend[i] = enable;
	}
}

void COpenGLCacheHandler::setBlendIndexed(GLuint index, bool enable)
{
	if (index < FrameBufferCount && Blend[index] != enable)
	{
		if (enable)
			Driver->extGlEnableIndexed(GL_BLEND, index);
		else
			Driver->extGlDisableIndexed(GL_BLEND, index);

		Blend[index] = enable;
	}
}

void COpenGLCacheHandler::setColorMask(bool red, bool green, bool blue, bool alpha)
{
	if (ColorMask[0][0] != red || ColorMask[0][1] != green || ColorMask[0][2] != blue || ColorMask[0][3] != alpha)
	{
		glColorMask(red, green, blue, alpha);

		for (GLuint i = 0; i < FrameBufferCount; ++i)
		{
			ColorMask[i][0] = red;
			ColorMask[i][1] = green;
			ColorMask[i][2] = blue;
			ColorMask[i][3] = alpha;
		}
	}
}

void COpenGLCacheHandler::setColorMaskIndexed(GLuint index, bool red, bool green, bool blue, bool alpha)
{
	if (index < FrameBufferCount && (ColorMask[index][0] != red || ColorMask[index][1] != green || ColorMask[index][2] != blue || ColorMask[index][3] != alpha))
	{
		Driver->extGlColorMaskIndexed(index, red, green, blue, alpha);

		ColorMask[index][0] = red;
		ColorMask[index][1] = green;
		ColorMask[index][2] = blue;
		ColorMask[index][3] = alpha;
	}
}

void COpenGLCacheHandler::setClientState(bool vertex, bool normal, bool color, bool texCoord0)
{
	if (ClientStateVertex != vertex)
	{
		if (vertex)
			glEnableClientState(GL_VERTEX_ARRAY);
		else
			glDisableClientState(GL_VERTEX_ARRAY);

		ClientStateVertex = vertex;
	}

	if (ClientStateNormal != normal)
	{
		if (normal)
			glEnableClientState(GL_NORMAL_ARRAY);
		else
			glDisableClientState(GL_NORMAL_ARRAY);

		ClientStateNormal = normal;
	}

	if (ClientStateColor != color)
	{
		if (color)
			glEnableClientState(GL_COLOR_ARRAY);
		else
			glDisableClientState(GL_COLOR_ARRAY);

		ClientStateColor = color;
	}

	if (ClientStateTexCoord0 != texCoord0)
	{
		setClientActiveTexture(GL_TEXTURE0_ARB);

		if (texCoord0)
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		else
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);

		ClientStateTexCoord0 = texCoord0;
	}
}

void COpenGLCacheHandler::setCullFaceFunc(GLenum mode)
{
	if (CullFaceMode != mode)
	{
		glCullFace(mode);
		CullFaceMode = mode;
	}
}

void COpenGLCacheHandler::setCullFace(bool enable)
{
	if (CullFace != enable)
	{
		if (enable)
			glEnable(GL_CULL_FACE);
		else
			glDisable(GL_CULL_FACE);
		CullFace = enable;
	}
}

void COpenGLCacheHandler::setDepthFunc(GLenum mode)
{
	if (DepthFunc != mode)
	{
		glDepthFunc(mode);
		DepthFunc = mode;
	}
}

void COpenGLCacheHandler::setDepthMask(bool enable)
{
	if (DepthMask != enable)
	{
		if (enable)
			glDepthMask(GL_TRUE);
		else
			glDepthMask(GL_FALSE);
		DepthMask = enable;
	}
}

void COpenGLCacheHandler::setDepthTest(bool enable)
{
	if (DepthTest != enable)
	{
		if (enable)
			glEnable(GL_DEPTH_TEST);
		else
			glDisable(GL_DEPTH_TEST);
		DepthTest = enable;
	}
}

void COpenGLCacheHandler::getFBO(GLuint& id) const
{
	id = FrameBufferID;
}

void COpenGLCacheHandler::setFBO(GLuint id)
{
	if (FrameBufferID != id)
	{
#if defined(GL_EXT_framebuffer_object)
		Driver->extGlBindFramebuffer(GL_FRAMEBUFFER_EXT, id);
#endif
		FrameBufferID = id;
	}
}

void COpenGLCacheHandler::setMatrixMode(GLenum mode)
{
	if (MatrixMode != mode)
	{
		glMatrixMode(mode);
		MatrixMode = mode;
	}
}

void COpenGLCacheHandler::setActiveTexture(GLenum texture)
{
	if (Driver->MultiTextureExtension && ActiveTexture != texture)
	{
		Driver->extGlActiveTexture(texture);
		ActiveTexture = texture;
	}
}

void COpenGLCacheHandler::setClientActiveTexture(GLenum texture)
{
	if (Driver->MultiTextureExtension && ClientActiveTexture != texture)
	{
		Driver->extGlClientActiveTexture(texture);
		ClientActiveTexture = texture;
	}
}

void COpenGLCacheHandler::setViewport(GLint viewportX, GLint viewportY, GLsizei viewportWidth, GLsizei viewportHeight)
{
	if (ViewportX != viewportX || ViewportY != viewportY || ViewportWidth != viewportWidth || ViewportHeight != viewportHeight)
	{
		glViewport(viewportX, viewportY, viewportWidth, viewportHeight);
		ViewportX = viewportX;
		ViewportY = viewportY;
		ViewportWidth = viewportWidth;
		ViewportHeight = viewportHeight;
	}
}

} // end namespace
} // end namespace

#endif // _IRR_COMPILE_WITH_OPENGL_
