// Copyright (C) 2017 Michael Zeilfelder
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#ifndef __C_WEBGL1_DRIVER_H_INCLUDED__
#define __C_WEBGL1_DRIVER_H_INCLUDED__

#include "IrrCompileConfig.h"

#include "SIrrCreationParameters.h"

#ifdef _IRR_COMPILE_WITH_WEBGL1_

#include "COGLES2Driver.h"
#include "CMeshBuffer.h"
#include "EHardwareBufferFlags.h"

namespace irr
{
namespace video
{
	//! WebGL friendly subset of OGL ES 2.0.
	//! Written for use with emscripten
	class CWebGL1Driver : public COGLES2Driver
	{
		friend class COpenGLCoreTexture<CWebGL1Driver>;
		friend IVideoDriver* createWebGL1Driver(const SIrrlichtCreationParameters& params, io::IFileSystem* io, IContextManager* contextManager);

	protected:
		//! constructor
		CWebGL1Driver(const SIrrlichtCreationParameters& params, io::IFileSystem* io, IContextManager* contextManager);

	public:

		//! destructor
		virtual ~CWebGL1Driver();

		//! Returns type of video driver
		virtual E_DRIVER_TYPE getDriverType() const _IRR_OVERRIDE_;

		//! is vbo recommended on this mesh?
		virtual bool isHardwareBufferRecommend(const scene::IMeshBuffer* mb) _IRR_OVERRIDE_
		{
			return true;	// All buffers must be bound, WebGL doesn't allow sending unbound buffers at all.
		}

		//! Draws a mesh buffer
		virtual void drawMeshBuffer(const scene::IMeshBuffer* mb) _IRR_OVERRIDE_;

		//! draw an 2d rectangle
		virtual void draw2DRectangle(SColor color, const core::rect<s32>& pos,
				const core::rect<s32>* clip = 0) _IRR_OVERRIDE_;

		void draw2DImageBatch(const video::ITexture* texture,
				const core::array<core::position2d<s32> >& positions,
				const core::array<core::rect<s32> >& sourceRects,
				const core::rect<s32>* clipRect,
				SColor color,
				bool useAlphaChannelOfTexture) _IRR_OVERRIDE_;

	protected:
		// create a meshbuffer which has as many vertices as indices
		scene::SMeshBuffer* createSimpleMeshBuffer(irr::u32 numVertices, scene::E_PRIMITIVE_TYPE primitiveType, scene::E_HARDWARE_MAPPING vertexMappingHint=scene::EHM_STREAM, scene::E_HARDWARE_MAPPING indexMappingHint=scene::EHM_STATIC) const;

	private:
		// Because we can't have unbound buffers in webgl we give all drawing functions bound buffers
		// which they can use.
		scene::SMeshBuffer* MBDraw2DRectangle;
		scene::SMeshBuffer* MBDraw2DImageBatch;
	};

} // end namespace video
} // end namespace irr

#endif // _IRR_COMPILE_WITH_WEBGL1_

#endif // __C_WEBGL1_DRIVER_H_INCLUDED__
