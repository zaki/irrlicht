// Copyright (C) 2015 Patryk Nadrowski
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_OGLCORE_CACHE_HANDLER_H_INCLUDED__
#define __C_OGLCORE_CACHE_HANDLER_H_INCLUDED__

#include "IrrCompileConfig.h"

#if defined(_IRR_COMPILE_WITH_OPENGL_) || defined(_IRR_COMPILE_WITH_OGLES1_) || defined(_IRR_COMPILE_WITH_OGLES2_)

#include "SMaterial.h"
#include "ITexture.h"

namespace irr
{
namespace video
{

template <class TOGLDriver, class TOGLTexture>
class COGLCoreCacheHandler
{
	class STextureCache
	{
	public:
		STextureCache(COGLCoreCacheHandler* cacheHandler, u32 textureCount) :
			CacheHandler(cacheHandler), DriverType(cacheHandler->getDriverType()), TextureCount(textureCount)
		{
			for (u32 i = 0; i < MATERIAL_MAX_TEXTURES; ++i)
			{
				Texture[i] = 0;
			}
		}

		~STextureCache()
		{
			clear();
		}

		const TOGLTexture* operator[](int index) const
		{
			if (static_cast<u32>(index) < MATERIAL_MAX_TEXTURES)
				return Texture[static_cast<u32>(index)];

			return 0;
		}

		const TOGLTexture* get(u32 index) const
		{
			if (index < MATERIAL_MAX_TEXTURES)
				return Texture[index];

			return 0;
		}

		bool set(u32 index, const ITexture* texture)
		{
			bool status = false;

			E_DRIVER_TYPE type = DriverType;

			if (index < MATERIAL_MAX_TEXTURES && index < TextureCount)
			{
				CacheHandler->setActiveTexture(GL_TEXTURE0 + index);

				const TOGLTexture* prevTexture = Texture[index];

				if (texture != prevTexture)
				{
					if (texture)
					{
						type = texture->getDriverType();

						if (type == DriverType)
						{
							texture->grab();

#if defined(IRR_OPENGL_VERSION) && IRR_OPENGL_VERSION < 20
							if (!prevTexture)
								glEnable(GL_TEXTURE_2D);
#endif

							glBindTexture(GL_TEXTURE_2D, static_cast<const TOGLTexture*>(texture)->getOpenGLTextureName());
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

#if defined(IRR_OPENGL_VERSION) && IRR_OPENGL_VERSION < 20
						if (prevTexture)
							glDisable(GL_TEXTURE_2D);
#endif
					}

					Texture[index] = static_cast<const TOGLTexture*>(texture);

					if (prevTexture)
						prevTexture->drop();
				}

				status = true;
			}

			return (status && type == DriverType);
		}

		void remove(ITexture* texture)
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

		void clear()
		{
			for (u32 i = 0; i < MATERIAL_MAX_TEXTURES; ++i)
			{
				if (Texture[i])
				{
					const TOGLTexture* prevTexture = Texture[i];

					Texture[i] = 0;

					prevTexture->drop();
				}
			}
		}

	private:
		COGLCoreCacheHandler* CacheHandler;

		E_DRIVER_TYPE DriverType;

		const TOGLTexture* Texture[MATERIAL_MAX_TEXTURES];
		u32 TextureCount;
	};

public:
	COGLCoreCacheHandler(TOGLDriver* driver) :
		Driver(driver), TextureCache(STextureCache(this, Driver->getFeature().TextureUnit)), FrameBufferCount(0),
		BlendEquation(0), BlendSourceRGB(0), BlendDestinationRGB(0), BlendSourceAlpha(0), BlendDestinationAlpha(0),
		Blend(0), ColorMask(0), CullFaceMode(GL_BACK), CullFace(false), DepthFunc(GL_LESS), DepthMask(true),
		FrameBufferID(0), ProgramID(0), ActiveTexture(GL_TEXTURE0), ViewportX(0), ViewportY(0)
	{
		const COGLCoreFeature& feature = Driver->getFeature();

		FrameBufferCount = core::max_(static_cast<GLuint>(1), static_cast<GLuint>(feature.MultipleRenderTarget));

		BlendEquation = new GLenum[FrameBufferCount];
		BlendSourceRGB = new GLenum[FrameBufferCount];
		BlendDestinationRGB = new GLenum[FrameBufferCount];
		BlendSourceAlpha = new GLenum[FrameBufferCount];
		BlendDestinationAlpha = new GLenum[FrameBufferCount];
		Blend = new bool[FrameBufferCount];

		ColorMask = new bool[FrameBufferCount][4];

		// Initial OpenGL values from specification.

		if (feature.BlendOperation)
		{
			Driver->irrGlBlendEquation(GL_FUNC_ADD);
		}

		for (u32 i = 0; i < FrameBufferCount; ++i)
		{
			BlendEquation[i] = GL_FUNC_ADD;

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

		Driver->irrGlActiveTexture(ActiveTexture);

#if defined(IRR_OPENGL_VERSION) && IRR_OPENGL_VERSION < 20
		glDisable(GL_TEXTURE_2D);
#endif

		const core::dimension2d<u32> ScreenSize = Driver->getScreenSize();
		ViewportWidth = ScreenSize.Width;
		ViewportHeight = ScreenSize.Height;
		glViewport(ViewportX, ViewportY, ViewportWidth, ViewportHeight);
	}

	virtual ~COGLCoreCacheHandler()
	{
		delete[] BlendEquation;
		delete[] BlendSourceRGB;
		delete[] BlendDestinationRGB;
		delete[] BlendSourceAlpha;
		delete[] BlendDestinationAlpha;
		delete[] Blend;

		delete[] ColorMask;
	}

	E_DRIVER_TYPE getDriverType() const
	{
		return Driver->getDriverType();
	}

	STextureCache& getTextureCache()
	{
		return TextureCache;
	}

	// Blending calls.

	void setBlendEquation(GLenum mode)
	{
		if (BlendEquation[0] != mode)
		{
			Driver->irrGlBlendEquation(mode);

			for (GLuint i = 0; i < FrameBufferCount; ++i)
				BlendEquation[i] = mode;
		}
	}

	void setBlendEquationIndexed(GLuint index, GLenum mode)
	{
		if (index < FrameBufferCount && BlendEquation[index] != mode)
		{
			Driver->irrGlBlendEquationIndexed(index, mode);

			BlendEquation[index] = mode;
		}
	}

	void setBlendFunc(GLenum source, GLenum destination)
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

	void setBlendFuncSeparate(GLenum sourceRGB, GLenum destinationRGB, GLenum sourceAlpha, GLenum destinationAlpha)
	{
		if (sourceRGB != sourceAlpha || destinationRGB != destinationAlpha)
		{
			if (BlendSourceRGB[0] != sourceRGB || BlendDestinationRGB[0] != destinationRGB ||
				BlendSourceAlpha[0] != sourceAlpha || BlendDestinationAlpha[0] != destinationAlpha)
			{
				Driver->irrGlBlendFuncSeparate(sourceRGB, destinationRGB, sourceAlpha, destinationAlpha);

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

	void setBlendFuncIndexed(GLuint index, GLenum source, GLenum destination)
	{
		if (index < FrameBufferCount && (BlendSourceRGB[index] != source || BlendDestinationRGB[index] != destination ||
			BlendSourceAlpha[index] != source || BlendDestinationAlpha[index] != destination))
		{
			Driver->irrGlBlendFuncIndexed(index, source, destination);

			BlendSourceRGB[index] = source;
			BlendDestinationRGB[index] = destination;
			BlendSourceAlpha[index] = source;
			BlendDestinationAlpha[index] = destination;
		}
	}

	void setBlendFuncSeparateIndexed(GLuint index, GLenum sourceRGB, GLenum destinationRGB, GLenum sourceAlpha, GLenum destinationAlpha)
	{
		if (sourceRGB != sourceAlpha || destinationRGB != destinationAlpha)
		{
			if (index < FrameBufferCount && (BlendSourceRGB[index] != sourceRGB || BlendDestinationRGB[index] != destinationRGB ||
				BlendSourceAlpha[index] != sourceAlpha || BlendDestinationAlpha[index] != destinationAlpha))
			{
				Driver->irrGlBlendFuncSeparateIndexed(index, sourceRGB, destinationRGB, sourceAlpha, destinationAlpha);

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

	void setBlend(bool enable)
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

	void setBlendIndexed(GLuint index, bool enable)
	{
		if (index < FrameBufferCount && Blend[index] != enable)
		{
			if (enable)
				Driver->irrGlEnableIndexed(GL_BLEND, index);
			else
				Driver->irrGlDisableIndexed(GL_BLEND, index);

			Blend[index] = enable;
		}
	}

	// Color Mask.

	void setColorMask(bool red, bool green, bool blue, bool alpha)
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

	void setColorMaskIndexed(GLuint index, bool red, bool green, bool blue, bool alpha)
	{
		if (index < FrameBufferCount && (ColorMask[index][0] != red || ColorMask[index][1] != green || ColorMask[index][2] != blue || ColorMask[index][3] != alpha))
		{
			Driver->irrGlColorMaskIndexed(index, red, green, blue, alpha);

			ColorMask[index][0] = red;
			ColorMask[index][1] = green;
			ColorMask[index][2] = blue;
			ColorMask[index][3] = alpha;
		}
	}

	// Cull face calls.

	void setCullFaceFunc(GLenum mode)
	{
		if (CullFaceMode != mode)
		{
			glCullFace(mode);
			CullFaceMode = mode;
		}
	}

	void setCullFace(bool enable)
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

	// Depth calls.

	void setDepthFunc(GLenum mode)
	{
		if (DepthFunc != mode)
		{
			glDepthFunc(mode);
			DepthFunc = mode;
		}
	}

	void setDepthMask(bool enable)
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

	// FBO calls.

	void getFBO(GLuint& frameBufferID) const
	{
		frameBufferID = FrameBufferID;
	}

	void setFBO(GLuint frameBufferID)
	{
		if (FrameBufferID != frameBufferID)
		{
			Driver->irrGlBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);
			FrameBufferID = frameBufferID;
		}
	}

	// Shaders calls.

	void getProgram(GLuint& programID) const
	{
		programID = ProgramID;
	}

	void setProgram(GLuint programID)
	{
		if (ProgramID != programID)
		{
			Driver->irrGlUseProgram(programID);
			ProgramID = programID;
		}
	}

	// Texture calls.

	void getActiveTexture(GLenum& texture) const
	{
		texture = ActiveTexture;
	}

	void setActiveTexture(GLenum texture)
	{
		if (ActiveTexture != texture)
		{
			Driver->irrGlActiveTexture(texture);
			ActiveTexture = texture;
		}
	}

	// Viewport calls.

	void getViewport(GLint& viewportX, GLint& viewportY, GLsizei& viewportWidth, GLsizei& viewportHeight) const
	{
		viewportX = ViewportX;
		viewportY = ViewportY;
		viewportWidth = ViewportWidth;
		viewportHeight = ViewportHeight;
	}

	void setViewport(GLint viewportX, GLint viewportY, GLsizei viewportWidth, GLsizei viewportHeight)
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

protected:
	TOGLDriver* Driver;

	STextureCache TextureCache;

	GLuint FrameBufferCount;

	GLenum* BlendEquation;
	GLenum* BlendSourceRGB;
	GLenum* BlendDestinationRGB;
	GLenum* BlendSourceAlpha;
	GLenum* BlendDestinationAlpha;
	bool* Blend;

	bool(*ColorMask)[4];

	GLenum CullFaceMode;
	bool CullFace;

	GLenum DepthFunc;
	bool DepthMask;

	GLuint FrameBufferID;

	GLuint ProgramID;

	GLenum ActiveTexture;

	GLint ViewportX;
	GLint ViewportY;
	GLsizei ViewportWidth;
	GLsizei ViewportHeight;
};

}
}

#endif
#endif
