// Copyright (C) 2017 Michael Zeilfelder
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#include "CWebGL1Driver.h"

#ifdef _IRR_COMPILE_WITH_WEBGL1_

#include "COpenGLCoreTexture.h"
#include "COpenGLCoreRenderTarget.h"
#include "COpenGLCoreCacheHandler.h"
#include "EVertexAttributes.h"

// Add as first line to a function to get info is was called once.
#define FIRST_CALL \
	static bool first = true; \
	if ( first ) \
	{\
		first = false; \
		os::Printer::log(__FILE__, irr::core::stringc(__LINE__).c_str(), ELL_ERROR); \
	}

namespace irr
{
namespace video
{

CWebGL1Driver::CWebGL1Driver(const SIrrlichtCreationParameters& params, io::IFileSystem* io, IContextManager* contextManager) :
	COGLES2Driver(params, io, contextManager)
	, MBDraw2DRectangle(0), MBDraw2DImageBatch(0)
{
#ifdef _DEBUG
	setDebugName("CWebGL1Driver");
#endif

	setTextureCreationFlag(ETCF_CREATE_MIP_MAPS, false);	// so far causing errors, have to figure out later
}

CWebGL1Driver::~CWebGL1Driver()
{
	if ( MBDraw2DRectangle )
		MBDraw2DRectangle->drop();
	if ( MBDraw2DImageBatch )
		MBDraw2DImageBatch->drop();
}

//! Returns type of video driver
E_DRIVER_TYPE CWebGL1Driver::getDriverType() const
{
	return EDT_WEBGL1;
}

//! Draws a mesh buffer
void CWebGL1Driver::drawMeshBuffer(const scene::IMeshBuffer* mb)
{
	if ( mb )
	{
		// OK - this is bad and I hope I can find a better solution.
		// Basically casting away a const which shouldn't be cast away.
		// Not a nice surprise for users to see their mesh changes I guess :-(
		scene::IMeshBuffer* mbUglyHack = const_cast<scene::IMeshBuffer*>(mb);

		// We can't allow any buffers which are not bound to some VBO.
		if ( mb->getHardwareMappingHint_Vertex() == scene::EHM_NEVER)
			mbUglyHack->setHardwareMappingHint(scene::EHM_STREAM, scene::EBT_VERTEX);
		if ( mb->getHardwareMappingHint_Index() == scene::EHM_NEVER)
			mbUglyHack->setHardwareMappingHint(scene::EHM_STREAM, scene::EBT_INDEX);

		COGLES2Driver::drawMeshBuffer(mb);
	}
}

//! draw a 2d rectangle
void CWebGL1Driver::draw2DRectangle(SColor color,
		const core::rect<s32>& position,
		const core::rect<s32>* clip)
{
FIRST_CALL;
	chooseMaterial2D();
	Material.TextureLayer[0].Texture = 0;
	CacheHandler->getTextureCache().set(0, 0);

	setRenderStates2DMode(color.getAlpha() < 255, false, false);
	lockRenderStateMode();

	core::rect<s32> pos = position;

	if (clip)
		pos.clipAgainst(*clip);

	if (!pos.isValid())
		return;

	const core::dimension2d<u32>& renderTargetSize = getCurrentRenderTargetSize();

	f32 left = (f32)pos.UpperLeftCorner.X / (f32)renderTargetSize.Width * 2.f - 1.f;
	f32 right = (f32)pos.LowerRightCorner.X / (f32)renderTargetSize.Width * 2.f - 1.f;
	f32 down = 2.f - (f32)pos.LowerRightCorner.Y / (f32)renderTargetSize.Height * 2.f - 1.f;
	f32 top = 2.f - (f32)pos.UpperLeftCorner.Y / (f32)renderTargetSize.Height * 2.f - 1.f;

	if ( !MBDraw2DRectangle )
	{
		MBDraw2DRectangle = createSimpleMeshBuffer(4, scene::EPT_TRIANGLE_FAN);
	}
	MBDraw2DRectangle->Vertices[0] = S3DVertex(left, top, 0, 0, 0, 1, color, 0, 0);
	MBDraw2DRectangle->Vertices[1] = S3DVertex(right, top, 0, 0, 0, 1, color, 0, 0);
	MBDraw2DRectangle->Vertices[2] = S3DVertex(right, down, 0, 0, 0, 1, color, 0, 0);
	MBDraw2DRectangle->Vertices[3] = S3DVertex(left, down, 0, 0, 0, 1, color, 0, 0);
	MBDraw2DRectangle->setDirty(scene::EBT_VERTEX);

	drawMeshBuffer(MBDraw2DRectangle);

	unlockRenderStateMode();
}

void CWebGL1Driver::draw2DImageBatch(const video::ITexture* texture,
		const core::array<core::position2d<s32> >& positions,
		const core::array<core::rect<s32> >& sourceRects,
		const core::rect<s32>* clipRect,
		SColor color, bool useAlphaChannelOfTexture)
{
	if (!texture)
		return;

	const irr::u32 drawCount = core::min_<u32>(positions.size(), sourceRects.size());
	if ( !drawCount )
		return;

	if ( !MBDraw2DImageBatch)
	{
		// We can only allocate for one image, otherwise we would have to re-bind the buffer on every frame as it can change size
		MBDraw2DImageBatch = createSimpleMeshBuffer(4, scene::EPT_TRIANGLE_FAN);
	}

	chooseMaterial2D();
	Material.TextureLayer[0].Texture = const_cast<ITexture*>(texture);
	if (!CacheHandler->getTextureCache().set(0, texture))
		return;

	setRenderStates2DMode(color.getAlpha() < 255, true, useAlphaChannelOfTexture);
	lockRenderStateMode();

	for (u32 i = 0; i < drawCount; i++)
	{
		core::position2d<s32> targetPos = positions[i];
		core::position2d<s32> sourcePos = sourceRects[i].UpperLeftCorner;
		// This needs to be signed as it may go negative.
		core::dimension2d<s32> sourceSize(sourceRects[i].getSize());

		if (clipRect)
		{
			if (targetPos.X < clipRect->UpperLeftCorner.X)
			{
				sourceSize.Width += targetPos.X - clipRect->UpperLeftCorner.X;
				if (sourceSize.Width <= 0)
					continue;

				sourcePos.X -= targetPos.X - clipRect->UpperLeftCorner.X;
				targetPos.X = clipRect->UpperLeftCorner.X;
			}

			if (targetPos.X + (s32)sourceSize.Width > clipRect->LowerRightCorner.X)
			{
				sourceSize.Width -= (targetPos.X + sourceSize.Width) - clipRect->LowerRightCorner.X;
				if (sourceSize.Width <= 0)
					continue;
			}

			if (targetPos.Y < clipRect->UpperLeftCorner.Y)
			{
				sourceSize.Height += targetPos.Y - clipRect->UpperLeftCorner.Y;
				if (sourceSize.Height <= 0)
					continue;

				sourcePos.Y -= targetPos.Y - clipRect->UpperLeftCorner.Y;
				targetPos.Y = clipRect->UpperLeftCorner.Y;
			}

			if (targetPos.Y + (s32)sourceSize.Height > clipRect->LowerRightCorner.Y)
			{
				sourceSize.Height -= (targetPos.Y + sourceSize.Height) - clipRect->LowerRightCorner.Y;
				if (sourceSize.Height <= 0)
					continue;
			}
		}

		// clip these coordinates

		if (targetPos.X < 0)
		{
			sourceSize.Width += targetPos.X;
			if (sourceSize.Width <= 0)
				continue;

			sourcePos.X -= targetPos.X;
			targetPos.X = 0;
		}

		const core::dimension2d<u32>& renderTargetSize = getCurrentRenderTargetSize();

		if (targetPos.X + sourceSize.Width > (s32)renderTargetSize.Width)
		{
			sourceSize.Width -= (targetPos.X + sourceSize.Width) - renderTargetSize.Width;
			if (sourceSize.Width <= 0)
				continue;
		}

		if (targetPos.Y < 0)
		{
			sourceSize.Height += targetPos.Y;
			if (sourceSize.Height <= 0)
				continue;

			sourcePos.Y -= targetPos.Y;
			targetPos.Y = 0;
		}

		if (targetPos.Y + sourceSize.Height > (s32)renderTargetSize.Height)
		{
			sourceSize.Height -= (targetPos.Y + sourceSize.Height) - renderTargetSize.Height;
			if (sourceSize.Height <= 0)
				continue;
		}

		// ok, we've clipped everything.
		// now draw it.

		core::rect<f32> tcoords;
		tcoords.UpperLeftCorner.X = (((f32)sourcePos.X)) / texture->getOriginalSize().Width ;
		tcoords.UpperLeftCorner.Y = (((f32)sourcePos.Y)) / texture->getOriginalSize().Height;
		tcoords.LowerRightCorner.X = tcoords.UpperLeftCorner.X + ((f32)(sourceSize.Width) / texture->getOriginalSize().Width);
		tcoords.LowerRightCorner.Y = tcoords.UpperLeftCorner.Y + ((f32)(sourceSize.Height) / texture->getOriginalSize().Height);

		const core::rect<s32> poss(targetPos, sourceSize);

		f32 left = (f32)poss.UpperLeftCorner.X / (f32)renderTargetSize.Width * 2.f - 1.f;
		f32 right = (f32)poss.LowerRightCorner.X / (f32)renderTargetSize.Width * 2.f - 1.f;
		f32 down = 2.f - (f32)poss.LowerRightCorner.Y / (f32)renderTargetSize.Height * 2.f - 1.f;
		f32 top = 2.f - (f32)poss.UpperLeftCorner.Y / (f32)renderTargetSize.Height * 2.f - 1.f;

		MBDraw2DImageBatch->Vertices[0] = S3DVertex(left, top, 0, 0, 0, 1, color, tcoords.UpperLeftCorner.X, tcoords.UpperLeftCorner.Y);
		MBDraw2DImageBatch->Vertices[1] = S3DVertex(right, top, 0, 0, 0, 1, color, tcoords.LowerRightCorner.X, tcoords.UpperLeftCorner.Y);
		MBDraw2DImageBatch->Vertices[2] = S3DVertex(right, down, 0, 0, 0, 1, color, tcoords.LowerRightCorner.X, tcoords.LowerRightCorner.Y);
		MBDraw2DImageBatch->Vertices[3] = S3DVertex(left, down, 0, 0, 0, 1, color, tcoords.UpperLeftCorner.X, tcoords.LowerRightCorner.Y);
		MBDraw2DImageBatch->setDirty(scene::EBT_VERTEX);

		drawMeshBuffer(MBDraw2DImageBatch);
	}

	unlockRenderStateMode();
}

scene::SMeshBuffer* CWebGL1Driver::createSimpleMeshBuffer(irr::u32 numVertices, scene::E_PRIMITIVE_TYPE primitiveType, scene::E_HARDWARE_MAPPING vertexMappingHint, scene::E_HARDWARE_MAPPING indexMappingHint) const
{
	scene::SMeshBuffer* mbResult = new scene::SMeshBuffer();
	mbResult->Vertices.set_used(numVertices);
	mbResult->Indices.set_used(numVertices);
	for ( irr::u32 i=0; i < numVertices; ++i )
		mbResult->Indices[i] = i;

	mbResult->setPrimitiveType(primitiveType);
	mbResult->setHardwareMappingHint(vertexMappingHint, scene::EBT_VERTEX);
	mbResult->setHardwareMappingHint(indexMappingHint, scene::EBT_INDEX);
	mbResult->setDirty();

	return mbResult;
}

} // end namespace video
} // end namespace irr

#endif // _IRR_COMPILE_WITH_WEBGL1_

namespace irr
{
namespace video
{

#ifndef _IRR_COMPILE_WITH_WEBGL1_
class IVideoDriver;
class IContextManager;
#endif

IVideoDriver* createWebGL1Driver(const SIrrlichtCreationParameters& params, io::IFileSystem* io, IContextManager* contextManager)
{
#ifdef _IRR_COMPILE_WITH_WEBGL1_
	CWebGL1Driver* driver = new CWebGL1Driver(params, io, contextManager);
	driver->genericDriverInit(params.WindowSize, params.Stencilbuffer);	// don't call in constructor, it uses virtual function calls of driver
	return driver;
#else
	return 0;
#endif //  _IRR_COMPILE_WITH_WEBGL1_
}

} // end namespace
} // end namespace
