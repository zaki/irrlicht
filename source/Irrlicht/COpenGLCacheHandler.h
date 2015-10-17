// Copyright (C) 2015 Patryk Nadrowski
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#ifndef __C_OPEN_GL_CACHE_HANDLER_H_INCLUDED__
#define __C_OPEN_GL_CACHE_HANDLER_H_INCLUDED__

#include "IrrCompileConfig.h"

#ifdef _IRR_COMPILE_WITH_OPENGL_

#include "COpenGLExtensionHandler.h"
#include "SMaterial.h"

namespace irr
{
namespace video
{
	class COpenGLDriver;
	class COpenGLTexture;

	class COpenGLCacheHandler
	{
		class STextureCache
		{
		public:
			STextureCache();
			STextureCache(COpenGLCacheHandler* cacheHandler, u32 textureCount);
			~STextureCache();

			const COpenGLTexture* operator[](int index) const;

			bool set(u32 index, const ITexture* texture);

			void remove(ITexture* texture);
			void clear();

		private:
			COpenGLCacheHandler* CacheHandler;

			const COpenGLTexture* Texture[MATERIAL_MAX_TEXTURES];
			u32 TextureCount;
		};

	public:
		COpenGLCacheHandler(COpenGLDriver* driver);
		~COpenGLCacheHandler();

		void reset();

		// Alpha calls.

		void setAlphaFunc(GLenum mode, GLclampf ref);

		void setAlphaTest(bool enable);

		// Blending calls.

		void setBlendEquation(GLenum mode);

		void setBlendEquationIndexed(GLuint index, GLenum mode);

		void setBlendFunc(GLenum source, GLenum destination);

		void setBlendFuncSeparate(GLenum sourceRGB, GLenum destinationRGB, GLenum sourceAlpha, GLenum destinationAlpha);

		void setBlendFuncIndexed(GLuint index, GLenum source, GLenum destination);

		void setBlendFuncSeparateIndexed(GLuint index, GLenum sourceRGB, GLenum destinationRGB, GLenum sourceAlpha, GLenum destinationAlpha);

		void setBlend(bool enable);

		void setBlendIndexed(GLuint index, bool enable);

		// Client state calls.

		void setClientState(bool vertex, bool normal, bool color, bool texCoord0);

		// Color Mask.

		void setColorMask(bool red, bool green, bool blue, bool alpha);

		void setColorMaskIndexed(GLuint index, bool red, bool green, bool blue, bool alpha);

		// Cull face calls.

		void setCullFaceFunc(GLenum mode);

		void setCullFace(bool enable);

		// Depth calls.

		void setDepthFunc(GLenum mode);

		void setDepthMask(bool enable);

		void setDepthTest(bool enable);

		// FBO calls.

		void getFBO(GLuint& id) const;

		void setFBO(GLuint id);

		// Matrix calls.

		void setMatrixMode(GLenum mode);

		// Texture calls.

		void setActiveTexture(GLenum texture);

		void setClientActiveTexture(GLenum texture);

		// Viewport calls.

		void setViewport(GLint viewportX, GLint viewportY, GLsizei viewportWidth, GLsizei viewportHeight);

		// Texture cache.

		STextureCache TextureCache;

	private:
		COpenGLDriver* Driver;

		GLuint FrameBufferCount;

		GLenum AlphaMode;
		GLclampf AlphaRef;
		bool AlphaTest;

		GLenum* BlendEquation;
		GLenum* BlendSourceRGB;
		GLenum* BlendDestinationRGB;
		GLenum* BlendSourceAlpha;
		GLenum* BlendDestinationAlpha;
		bool* Blend;

		bool (*ColorMask)[4];

		GLenum CullFaceMode;
		bool CullFace;

		GLenum DepthFunc;
		bool DepthMask;
		bool DepthTest;

		GLuint FrameBufferID;

		GLenum MatrixMode;

		GLenum ActiveTexture;
		GLenum ClientActiveTexture;

		bool ClientStateVertex;
		bool ClientStateNormal;
		bool ClientStateColor;
		bool ClientStateTexCoord0;

		GLint ViewportX;
		GLint ViewportY;
		GLsizei ViewportWidth;
		GLsizei ViewportHeight;
	};

} // end namespace video
} // end namespace irr

#endif // _IRR_COMPILE_WITH_OPENGL_
#endif
