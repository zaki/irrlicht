// Copyright (C) 2009-2010 Amundis
// Heavily based on the OpenGL driver implemented by Nikolaus Gebhardt
// and OpenGL ES driver implemented by Christian Stehno
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#ifndef __C_OGLES2_RENDERER_2D_H_INCLUDED__
#define __C_OGLES2_RENDERER_2D_H_INCLUDED__

#include "COGLES2SLMaterialRenderer.h"

#ifdef _IRR_COMPILE_WITH_OGLES2_

namespace irr
{
namespace io
{
	class IFileSystem;
}
namespace video
{
	class COGLES2Driver;

	class COGLES2Renderer2d : public COGLES2SLMaterialRenderer
	{
	public:
		COGLES2Renderer2d( COGLES2Driver* driver, io::IFileSystem* fs );

		void useTexture( bool param );
		void useAlphaTest( bool param );
		void setAlphaTestValue( float param );
		void setOrthoMatrix( const core::matrix4& matrix );
	private :
		bool UseTexture;
		bool UseAlphaTest;
		float AlphaTestValue;
		core::matrix4 OrthoMatrix;
	private:
		enum SHADER_UNIFORM
		{
			ORTHO_MATRIX = 0,
			USE_TEXTURE,
			TEXTURE_UNIT,
			ALPHA_TEST,
			ALPHA_VALUE,
			UNIFORM_COUNT
		};
		static const char* const sBuiltInShaderUniformNames[];
	};

}
}

#endif
#endif

