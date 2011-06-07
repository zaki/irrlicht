// Copyright (C) 2009-2010 Amundis
// Heavily based on the OpenGL driver implemented by Nikolaus Gebhardt
// and OpenGL ES driver implemented by Christian Stehno
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#ifndef __C_OGLES2_NORMAL_MAP_RENDERER_H_INCLUDED__
#define __C_OGLES2_NORMAL_MAP_RENDERER_H_INCLUDED__

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_OGLES2_

#include "COGLES2SLMaterialRenderer.h"
#include "IShaderConstantSetCallBack.h"

namespace irr
{
namespace video
{

	//! Class for rendering normal maps with OGLES2
	class COGLES2NormalMapRenderer : public COGLES2SLMaterialRenderer, public IShaderConstantSetCallBack
	{
	public:

		//! Constructor
		COGLES2NormalMapRenderer(video::COGLES2Driver* driver, io::IFileSystem* fs,
				s32& outMaterialTypeNr, IMaterialRenderer* baseMaterial);

		//! Destructor
		virtual ~COGLES2NormalMapRenderer();

		//! Called by the engine when the vertex and/or pixel shader constants for an
		//! material renderer should be set.
		virtual void OnSetConstants(IMaterialRendererServices* services, s32 userData);

		//! Returns the render capability of the material.
		virtual s32 getRenderCapability() const;

	protected:

		bool CompiledShaders;

	private:
		enum SHADER_UNIFORM
		{
			MVP_MATRIX = 0,
			LIGHT_POSITION,
			LIGHT_COLOR,
			TEXTURE_UNIT0,
			TEXTURE_UNIT1,
			UNIFORM_COUNT
		};
		static const char* const sBuiltInShaderUniformNames[];
	};


} // end namespace video
} // end namespace irr

#endif
#endif

