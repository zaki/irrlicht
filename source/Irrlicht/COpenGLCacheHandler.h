// Copyright (C) 2015 Patryk Nadrowski
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#ifndef __C_OPENGL_CACHE_HANDLER_H_INCLUDED__
#define __C_OPENGL_CACHE_HANDLER_H_INCLUDED__

#include "IrrCompileConfig.h"

#ifdef _IRR_COMPILE_WITH_OPENGL_

#include "COpenGLExtensionHandler.h"
#include "COGLCoreCacheHandler.h"

namespace irr
{
namespace video
{
	class COpenGLTexture;
	class COpenGLDriver;

	class COpenGLCacheHandler : public COGLCoreCacheHandler<COpenGLDriver, COpenGLTexture>
	{
	public:
		COpenGLCacheHandler(COpenGLDriver* driver);
		virtual ~COpenGLCacheHandler();

		// Alpha calls.

		void setAlphaFunc(GLenum mode, GLclampf ref);

		void setAlphaTest(bool enable);

		// Client state calls.

		void setClientState(bool vertex, bool normal, bool color, bool texCoord0);

		// Depth calls.

		void setDepthTest(bool enable);

		// Matrix calls.

		void setMatrixMode(GLenum mode);

		// Texture calls.

		void setClientActiveTexture(GLenum texture);

	protected:
		GLenum AlphaMode;
		GLclampf AlphaRef;
		bool AlphaTest;

		bool DepthTest;

		GLenum MatrixMode;

		GLenum ClientActiveTexture;

		bool ClientStateVertex;
		bool ClientStateNormal;
		bool ClientStateColor;
		bool ClientStateTexCoord0;
	};

} // end namespace video
} // end namespace irr

#endif // _IRR_COMPILE_WITH_OPENGL_
#endif
