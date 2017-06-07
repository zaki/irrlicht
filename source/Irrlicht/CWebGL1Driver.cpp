// Copyright (C) 2017 Michael Zeilfelder
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#include "CWebGL1Driver.h"

#ifdef _IRR_COMPILE_WITH_WEBGL1_

#include "COpenGLCoreTexture.h"
#include "COpenGLCoreRenderTarget.h"
#include "COpenGLCoreCacheHandler.h"
#include "EVertexAttributes.h"

namespace irr
{
namespace video
{

CWebGL1Driver::CWebGL1Driver(const SIrrlichtCreationParameters& params, io::IFileSystem* io, IContextManager* contextManager) :
	COGLES2Driver(params, io, contextManager)
	, MBTriangleFanSize4(0), MBLinesSize2(0), MBPointsSize1(0)
{
#ifdef _DEBUG
	setDebugName("CWebGL1Driver");
#endif

	// NPOT are not allowed for WebGL in most cases.
	// One can use them when:
	// - The TEXTURE_MIN_FILTER is linear or nearest
	// - no mipmapping is used
	// - no texture wrapping is used (so all texture_wraps have to be CLAMP_TO_EDGE)
	// So users could still enable them for specific cases (usually GUI), but in general better to have it off.
	disableFeature(EVDF_TEXTURE_NPOT);

	MBLinesSize2 = createSimpleMeshBuffer(2, scene::EPT_LINES);
	MBTriangleFanSize4 = createSimpleMeshBuffer(4, scene::EPT_TRIANGLE_FAN);
	MBPointsSize1 = createSimpleMeshBuffer(1, scene::EPT_POINTS);
}

CWebGL1Driver::~CWebGL1Driver()
{
	if ( MBTriangleFanSize4 )
		MBTriangleFanSize4->drop();
	if ( MBLinesSize2 )
		MBLinesSize2->drop();
	if ( MBPointsSize1 )
		MBPointsSize1->drop();
}

//! Returns type of video driver
E_DRIVER_TYPE CWebGL1Driver::getDriverType() const
{
	return EDT_WEBGL1;
}

//! draws a vertex primitive list
void CWebGL1Driver::drawVertexPrimitiveList(const void* vertices, u32 vertexCount,
                             const void* indexList, u32 primitiveCount,
                             E_VERTEX_TYPE vType, scene::E_PRIMITIVE_TYPE pType, E_INDEX_TYPE iType)
{
	if ( !vertices )
	{
		COGLES2Driver::drawVertexPrimitiveList(vertices, vertexCount, indexList, primitiveCount, vType, pType, iType);
	}
	else
	{
		static bool first = true;
		if ( first )
		{
			first = false;
			os::Printer::log("WebGL driver does not support drawVertexPrimitiveList calls without a VBO", ELL_WARNING);
			os::Printer::log(__FILE__, irr::core::stringc(__LINE__).c_str(), ELL_WARNING);
		}
	}
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

void CWebGL1Driver::draw2DImage(const video::ITexture* texture,
		const core::position2d<s32>& destPos,const core::rect<s32>& sourceRect,
		const core::rect<s32>* clipRect, SColor color, bool useAlphaChannelOfTexture)
{
	if (!texture)
		return;

	if (!sourceRect.isValid())
		return;

	core::position2d<s32> targetPos(destPos);
	core::position2d<s32> sourcePos(sourceRect.UpperLeftCorner);
	core::dimension2d<s32> sourceSize(sourceRect.getSize());
	if (clipRect)
	{
		if (targetPos.X < clipRect->UpperLeftCorner.X)
		{
			sourceSize.Width += targetPos.X - clipRect->UpperLeftCorner.X;
			if (sourceSize.Width <= 0)
				return;

			sourcePos.X -= targetPos.X - clipRect->UpperLeftCorner.X;
			targetPos.X = clipRect->UpperLeftCorner.X;
		}

		if (targetPos.X + sourceSize.Width > clipRect->LowerRightCorner.X)
		{
			sourceSize.Width -= (targetPos.X + sourceSize.Width) - clipRect->LowerRightCorner.X;
			if (sourceSize.Width <= 0)
				return;
		}

		if (targetPos.Y < clipRect->UpperLeftCorner.Y)
		{
			sourceSize.Height += targetPos.Y - clipRect->UpperLeftCorner.Y;
			if (sourceSize.Height <= 0)
				return;

			sourcePos.Y -= targetPos.Y - clipRect->UpperLeftCorner.Y;
			targetPos.Y = clipRect->UpperLeftCorner.Y;
		}

		if (targetPos.Y + sourceSize.Height > clipRect->LowerRightCorner.Y)
		{
			sourceSize.Height -= (targetPos.Y + sourceSize.Height) - clipRect->LowerRightCorner.Y;
			if (sourceSize.Height <= 0)
				return;
		}
	}

	// clip these coordinates

	if (targetPos.X < 0)
	{
		sourceSize.Width += targetPos.X;
		if (sourceSize.Width <= 0)
			return;

		sourcePos.X -= targetPos.X;
		targetPos.X = 0;
	}

	const core::dimension2d<u32>& renderTargetSize = getCurrentRenderTargetSize();

	if (targetPos.X + sourceSize.Width > (s32)renderTargetSize.Width)
	{
		sourceSize.Width -= (targetPos.X + sourceSize.Width) - renderTargetSize.Width;
		if (sourceSize.Width <= 0)
			return;
	}

	if (targetPos.Y < 0)
	{
		sourceSize.Height += targetPos.Y;
		if (sourceSize.Height <= 0)
			return;

		sourcePos.Y -= targetPos.Y;
		targetPos.Y = 0;
	}

	if (targetPos.Y + sourceSize.Height > (s32)renderTargetSize.Height)
	{
		sourceSize.Height -= (targetPos.Y + sourceSize.Height) - renderTargetSize.Height;
		if (sourceSize.Height <= 0)
			return;
	}

	// ok, we've clipped everything.
	// now draw it.

	// texcoords need to be flipped horizontally for RTTs
	const bool isRTT = texture->isRenderTarget();
	const core::dimension2d<u32>& ss = texture->getOriginalSize();
	const f32 invW = 1.f / static_cast<f32>(ss.Width);
	const f32 invH = 1.f / static_cast<f32>(ss.Height);
	const core::rect<f32> tcoords(
		sourcePos.X * invW,
		(isRTT ? (sourcePos.Y + sourceSize.Height) : sourcePos.Y) * invH,
		(sourcePos.X + sourceSize.Width) * invW,
		(isRTT ? sourcePos.Y : (sourcePos.Y + sourceSize.Height)) * invH);

	const core::rect<s32> poss(targetPos, sourceSize);

	chooseMaterial2D();
	if ( !setMaterialTexture(0, texture) )
		return;

	setRenderStates2DMode(color.getAlpha() < 255, true, useAlphaChannelOfTexture);
	lockRenderStateMode();

	f32 left = (f32)poss.UpperLeftCorner.X / (f32)renderTargetSize.Width * 2.f - 1.f;
	f32 right = (f32)poss.LowerRightCorner.X / (f32)renderTargetSize.Width * 2.f - 1.f;
	f32 down = 2.f - (f32)poss.LowerRightCorner.Y / (f32)renderTargetSize.Height * 2.f - 1.f;
	f32 top = 2.f - (f32)poss.UpperLeftCorner.Y / (f32)renderTargetSize.Height * 2.f - 1.f;

	MBTriangleFanSize4->Vertices[0] = S3DVertex(left, top, 0, 0, 0, 1, color, tcoords.UpperLeftCorner.X, tcoords.UpperLeftCorner.Y);
	MBTriangleFanSize4->Vertices[1] = S3DVertex(right, top, 0, 0, 0, 1, color, tcoords.LowerRightCorner.X, tcoords.UpperLeftCorner.Y);
	MBTriangleFanSize4->Vertices[2] = S3DVertex(right, down, 0, 0, 0, 1, color, tcoords.LowerRightCorner.X, tcoords.LowerRightCorner.Y);
	MBTriangleFanSize4->Vertices[3] = S3DVertex(left, down, 0, 0, 0, 1, color, tcoords.UpperLeftCorner.X, tcoords.LowerRightCorner.Y);
	MBTriangleFanSize4->setDirty(scene::EBT_VERTEX);

	drawMeshBuffer(MBTriangleFanSize4);

	unlockRenderStateMode();
}

void CWebGL1Driver::draw2DImage(const video::ITexture* texture, const core::rect<s32>& destRect,
	const core::rect<s32>& sourceRect, const core::rect<s32>* clipRect,
	const video::SColor* const colors, bool useAlphaChannelOfTexture)
{
	if (!texture)
		return;

	// texcoords need to be flipped horizontally for RTTs
	const bool isRTT = texture->isRenderTarget();
	const core::dimension2du& ss = texture->getOriginalSize();
	const f32 invW = 1.f / static_cast<f32>(ss.Width);
	const f32 invH = 1.f / static_cast<f32>(ss.Height);
	const core::rect<f32> tcoords(
		sourceRect.UpperLeftCorner.X * invW,
		(isRTT ? sourceRect.LowerRightCorner.Y : sourceRect.UpperLeftCorner.Y) * invH,
		sourceRect.LowerRightCorner.X * invW,
		(isRTT ? sourceRect.UpperLeftCorner.Y : sourceRect.LowerRightCorner.Y) *invH);

	const video::SColor temp[4] =
	{
		0xFFFFFFFF,
		0xFFFFFFFF,
		0xFFFFFFFF,
		0xFFFFFFFF
	};

	const video::SColor* const useColor = colors ? colors : temp;

	chooseMaterial2D();
	if ( !setMaterialTexture(0, texture) )
		return;

	setRenderStates2DMode(useColor[0].getAlpha() < 255 || useColor[1].getAlpha() < 255 ||
		useColor[2].getAlpha() < 255 || useColor[3].getAlpha() < 255,
		true, useAlphaChannelOfTexture);
	lockRenderStateMode();

	const core::dimension2d<u32>& renderTargetSize = getCurrentRenderTargetSize();

	bool useScissorTest = false;
	if (clipRect && clipRect->isValid())
	{
		useScissorTest = true;
		glEnable(GL_SCISSOR_TEST);
		glScissor(clipRect->UpperLeftCorner.X, renderTargetSize.Height - clipRect->LowerRightCorner.Y,
			clipRect->getWidth(), clipRect->getHeight());
	}

	f32 left = (f32)destRect.UpperLeftCorner.X / (f32)renderTargetSize.Width * 2.f - 1.f;
	f32 right = (f32)destRect.LowerRightCorner.X / (f32)renderTargetSize.Width * 2.f - 1.f;
	f32 down = 2.f - (f32)destRect.LowerRightCorner.Y / (f32)renderTargetSize.Height * 2.f - 1.f;
	f32 top = 2.f - (f32)destRect.UpperLeftCorner.Y / (f32)renderTargetSize.Height * 2.f - 1.f;

	MBTriangleFanSize4->Vertices[0] = S3DVertex(left, top, 0, 0, 0, 1, useColor[0], tcoords.UpperLeftCorner.X, tcoords.UpperLeftCorner.Y);
	MBTriangleFanSize4->Vertices[1] = S3DVertex(right, top, 0, 0, 0, 1, useColor[3], tcoords.LowerRightCorner.X, tcoords.UpperLeftCorner.Y);
	MBTriangleFanSize4->Vertices[2] = S3DVertex(right, down, 0, 0, 0, 1, useColor[2], tcoords.LowerRightCorner.X, tcoords.LowerRightCorner.Y);
	MBTriangleFanSize4->Vertices[3] = S3DVertex(left, down, 0, 0, 0, 1, useColor[1], tcoords.UpperLeftCorner.X, tcoords.LowerRightCorner.Y);
	MBTriangleFanSize4->setDirty(scene::EBT_VERTEX);

	drawMeshBuffer(MBTriangleFanSize4);

	if (useScissorTest)
		glDisable(GL_SCISSOR_TEST);

	unlockRenderStateMode();

	testGLError();
}

void CWebGL1Driver::draw2DImage(const video::ITexture* texture, u32 layer, bool flip)
{
	if (!texture )
		return;

	chooseMaterial2D();
	if ( !setMaterialTexture(0, texture) )
		return;

	setRenderStates2DMode(false, true, true);
	lockRenderStateMode();

	MBTriangleFanSize4->Vertices[0].Pos = core::vector3df(-1.f, 1.f, 0.f);
	MBTriangleFanSize4->Vertices[1].Pos = core::vector3df(1.f, 1.f, 0.f);
	MBTriangleFanSize4->Vertices[2].Pos = core::vector3df(1.f, -1.f, 0.f);
	MBTriangleFanSize4->Vertices[3].Pos = core::vector3df(-1.f, -1.f, 0.f);

	f32 modificator = (flip) ? 1.f : 0.f;

	MBTriangleFanSize4->Vertices[0].TCoords = core::vector2df(0.f, 0.f + modificator);
	MBTriangleFanSize4->Vertices[1].TCoords = core::vector2df(1.f, 0.f + modificator);
	MBTriangleFanSize4->Vertices[2].TCoords = core::vector2df(1.f, 1.f - modificator);
	MBTriangleFanSize4->Vertices[3].TCoords = core::vector2df(0.f, 1.f - modificator);

	MBTriangleFanSize4->Vertices[0].Color = SColor(0xFFFFFFFF);
	MBTriangleFanSize4->Vertices[1].Color = SColor(0xFFFFFFFF);
	MBTriangleFanSize4->Vertices[2].Color = SColor(0xFFFFFFFF);
	MBTriangleFanSize4->Vertices[3].Color = SColor(0xFFFFFFFF);

	MBTriangleFanSize4->setDirty(scene::EBT_VERTEX);

	drawMeshBuffer(MBTriangleFanSize4);

	unlockRenderStateMode();
}

void CWebGL1Driver::draw2DImageBatch(const video::ITexture* texture,
				const core::position2d<s32>& pos,
				const core::array<core::rect<s32> >& sourceRects,
				const core::array<s32>& indices, s32 kerningWidth,
				const core::rect<s32>* clipRect,
				SColor color, bool useAlphaChannelOfTexture)
{
	if (!texture)
		return;

	chooseMaterial2D();
	if ( !setMaterialTexture(0, texture) )
		return;

	setRenderStates2DMode(color.getAlpha() < 255, true, useAlphaChannelOfTexture);
	lockRenderStateMode();

	const core::dimension2d<u32>& renderTargetSize = getCurrentRenderTargetSize();

	bool useScissorTest = false;
	if (clipRect && clipRect->isValid())
	{
		useScissorTest = true;
		glEnable(GL_SCISSOR_TEST);
		glScissor(clipRect->UpperLeftCorner.X, renderTargetSize.Height - clipRect->LowerRightCorner.Y,
				clipRect->getWidth(), clipRect->getHeight());
	}

	const core::dimension2du& ss = texture->getOriginalSize();
	core::position2d<s32> targetPos(pos);
	// texcoords need to be flipped horizontally for RTTs
	const bool isRTT = texture->isRenderTarget();
	const f32 invW = 1.f / static_cast<f32>(ss.Width);
	const f32 invH = 1.f / static_cast<f32>(ss.Height);

	for (u32 i = 0; i < indices.size(); ++i)
	{
		const s32 currentIndex = indices[i];
		if (!sourceRects[currentIndex].isValid())
			break;

		const core::rect<f32> tcoords(
			sourceRects[currentIndex].UpperLeftCorner.X * invW,
			(isRTT ? sourceRects[currentIndex].LowerRightCorner.Y : sourceRects[currentIndex].UpperLeftCorner.Y) * invH,
			sourceRects[currentIndex].LowerRightCorner.X * invW,
			(isRTT ? sourceRects[currentIndex].UpperLeftCorner.Y : sourceRects[currentIndex].LowerRightCorner.Y) * invH);

		const core::rect<s32> poss(targetPos, sourceRects[currentIndex].getSize());

		f32 left = (f32)poss.UpperLeftCorner.X / (f32)renderTargetSize.Width * 2.f - 1.f;
		f32 right = (f32)poss.LowerRightCorner.X / (f32)renderTargetSize.Width * 2.f - 1.f;
		f32 down = 2.f - (f32)poss.LowerRightCorner.Y / (f32)renderTargetSize.Height * 2.f - 1.f;
		f32 top = 2.f - (f32)poss.UpperLeftCorner.Y / (f32)renderTargetSize.Height * 2.f - 1.f;

		MBTriangleFanSize4->Vertices[0] = S3DVertex(left, top, 0, 0, 0, 1, color, tcoords.UpperLeftCorner.X, tcoords.UpperLeftCorner.Y);
		MBTriangleFanSize4->Vertices[1] = S3DVertex(right, top, 0, 0, 0, 1, color, tcoords.LowerRightCorner.X, tcoords.UpperLeftCorner.Y);
		MBTriangleFanSize4->Vertices[2] = S3DVertex(right, down, 0, 0, 0, 1, color, tcoords.LowerRightCorner.X, tcoords.LowerRightCorner.Y);
		MBTriangleFanSize4->Vertices[3] = S3DVertex(left, down, 0, 0, 0, 1, color, tcoords.UpperLeftCorner.X, tcoords.LowerRightCorner.Y);
		MBTriangleFanSize4->setDirty(scene::EBT_VERTEX);

		drawMeshBuffer(MBTriangleFanSize4);

		targetPos.X += sourceRects[currentIndex].getWidth();
	}

	if (useScissorTest)
		glDisable(GL_SCISSOR_TEST);

	unlockRenderStateMode();

	testGLError();
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

	chooseMaterial2D();
	if ( !setMaterialTexture(0, texture) )
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

		MBTriangleFanSize4->Vertices[0] = S3DVertex(left, top, 0, 0, 0, 1, color, tcoords.UpperLeftCorner.X, tcoords.UpperLeftCorner.Y);
		MBTriangleFanSize4->Vertices[1] = S3DVertex(right, top, 0, 0, 0, 1, color, tcoords.LowerRightCorner.X, tcoords.UpperLeftCorner.Y);
		MBTriangleFanSize4->Vertices[2] = S3DVertex(right, down, 0, 0, 0, 1, color, tcoords.LowerRightCorner.X, tcoords.LowerRightCorner.Y);
		MBTriangleFanSize4->Vertices[3] = S3DVertex(left, down, 0, 0, 0, 1, color, tcoords.UpperLeftCorner.X, tcoords.LowerRightCorner.Y);
		MBTriangleFanSize4->setDirty(scene::EBT_VERTEX);

		drawMeshBuffer(MBTriangleFanSize4);
	}

	unlockRenderStateMode();
}

//! draw a 2d rectangle
void CWebGL1Driver::draw2DRectangle(SColor color,
		const core::rect<s32>& position,
		const core::rect<s32>* clip)
{
	chooseMaterial2D();
	setMaterialTexture(0, 0);

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

	MBTriangleFanSize4->Vertices[0] = S3DVertex(left, top, 0, 0, 0, 1, color, 0, 0);
	MBTriangleFanSize4->Vertices[1] = S3DVertex(right, top, 0, 0, 0, 1, color, 0, 0);
	MBTriangleFanSize4->Vertices[2] = S3DVertex(right, down, 0, 0, 0, 1, color, 0, 0);
	MBTriangleFanSize4->Vertices[3] = S3DVertex(left, down, 0, 0, 0, 1, color, 0, 0);
	MBTriangleFanSize4->setDirty(scene::EBT_VERTEX);

	drawMeshBuffer(MBTriangleFanSize4);

	unlockRenderStateMode();
}

void CWebGL1Driver::draw2DRectangle(const core::rect<s32>& position,
				SColor colorLeftUp, SColor colorRightUp, SColor colorLeftDown, SColor colorRightDown,
				const core::rect<s32>* clip)
{
	core::rect<s32> pos = position;

	if (clip)
		pos.clipAgainst(*clip);

	if (!pos.isValid())
		return;

	chooseMaterial2D();
	setMaterialTexture(0, 0);

	setRenderStates2DMode(colorLeftUp.getAlpha() < 255 ||
			colorRightUp.getAlpha() < 255 ||
			colorLeftDown.getAlpha() < 255 ||
			colorRightDown.getAlpha() < 255, false, false);
	lockRenderStateMode();

	const core::dimension2d<u32>& renderTargetSize = getCurrentRenderTargetSize();

	f32 left = (f32)pos.UpperLeftCorner.X / (f32)renderTargetSize.Width * 2.f - 1.f;
	f32 right = (f32)pos.LowerRightCorner.X / (f32)renderTargetSize.Width * 2.f - 1.f;
	f32 down = 2.f - (f32)pos.LowerRightCorner.Y / (f32)renderTargetSize.Height * 2.f - 1.f;
	f32 top = 2.f - (f32)pos.UpperLeftCorner.Y / (f32)renderTargetSize.Height * 2.f - 1.f;

	MBTriangleFanSize4->Vertices[0] = S3DVertex(left, top, 0, 0, 0, 1, colorLeftUp, 0, 0);
	MBTriangleFanSize4->Vertices[1] = S3DVertex(right, top, 0, 0, 0, 1, colorRightUp, 0, 0);
	MBTriangleFanSize4->Vertices[2] = S3DVertex(right, down, 0, 0, 0, 1, colorRightDown, 0, 0);
	MBTriangleFanSize4->Vertices[3] = S3DVertex(left, down, 0, 0, 0, 1, colorLeftDown, 0, 0);
	MBTriangleFanSize4->setDirty(scene::EBT_VERTEX);

	drawMeshBuffer(MBTriangleFanSize4);

	unlockRenderStateMode();
}

		//! Draws a 2d line.
void CWebGL1Driver::draw2DLine(const core::position2d<s32>& start, const core::position2d<s32>& end, SColor color)
{
	if (start==end)
		drawPixel(start.X, start.Y, color);
	else
	{
		chooseMaterial2D();
		setMaterialTexture(0, 0);

		setRenderStates2DMode(color.getAlpha() < 255, false, false);
		lockRenderStateMode();

		const core::dimension2d<u32>& renderTargetSize = getCurrentRenderTargetSize();

		f32 startX = (f32)start.X / (f32)renderTargetSize.Width * 2.f - 1.f;
		f32 endX = (f32)end.X / (f32)renderTargetSize.Width * 2.f - 1.f;
		f32 startY = 2.f - (f32)start.Y / (f32)renderTargetSize.Height * 2.f - 1.f;
		f32 endY = 2.f - (f32)end.Y / (f32)renderTargetSize.Height * 2.f - 1.f;

		MBLinesSize2->Vertices[0] = S3DVertex(startX, startY, 0, 0, 0, 1, color, 0, 0);
		MBLinesSize2->Vertices[1] = S3DVertex(endX, endY, 0, 0, 0, 1, color, 1, 1);
		MBLinesSize2->setDirty(scene::EBT_VERTEX);

		drawMeshBuffer(MBLinesSize2);

		unlockRenderStateMode();
	}
}

void CWebGL1Driver::drawPixel(u32 x, u32 y, const SColor & color)
{
	const core::dimension2d<u32>& renderTargetSize = getCurrentRenderTargetSize();
	if (x > (u32)renderTargetSize.Width || y > (u32)renderTargetSize.Height)
		return;

	chooseMaterial2D();
	setMaterialTexture(0, 0);

	setRenderStates2DMode(color.getAlpha() < 255, false, false);
	lockRenderStateMode();

	f32 X = (f32)x / (f32)renderTargetSize.Width * 2.f - 1.f;
	f32 Y = 2.f - (f32)y / (f32)renderTargetSize.Height * 2.f - 1.f;

	MBPointsSize1->Vertices[0] = S3DVertex(X, Y, 0, 0, 0, 1, color, 0, 0);
	MBPointsSize1->setDirty(scene::EBT_VERTEX);

	drawMeshBuffer(MBPointsSize1);

	unlockRenderStateMode();
}

void CWebGL1Driver::draw3DLine(const core::vector3df& start, const core::vector3df& end, SColor color)
{
	MBLinesSize2->Vertices[0] = S3DVertex(start.X, start.Y, start.Z, 0, 0, 1, color, 0, 0);
	MBLinesSize2->Vertices[1] = S3DVertex(end.X, end.Y, end.Z, 0, 0, 1, color, 0, 0);
	MBLinesSize2->setDirty(scene::EBT_VERTEX);

	drawMeshBuffer(MBLinesSize2);
}

void CWebGL1Driver::drawStencilShadowVolume(const core::array<core::vector3df>& triangles, bool zfail, u32 debugDataVisible)
{
	static bool first = true;
	if ( first )
	{
		first = false;
		os::Printer::log("WebGL1 driver does not yet support drawStencilShadowVolume", ELL_WARNING);
		os::Printer::log(__FILE__, irr::core::stringc(__LINE__).c_str(), ELL_WARNING);
	}
}

void CWebGL1Driver::drawStencilShadow(bool clearStencilBuffer,
	video::SColor leftUpEdge,
	video::SColor rightUpEdge,
	video::SColor leftDownEdge,
	video::SColor rightDownEdge)
{
	// NOTE: Might work, but untested as drawStencilShadowVolume is not yet supported.

	if (!StencilBuffer)
		return;

	chooseMaterial2D();
	setMaterialTexture(0, 0);

	setRenderStates2DMode(true, false, false);
	lockRenderStateMode();

	CacheHandler->setDepthMask(false);
	CacheHandler->setColorMask(ECP_ALL);

	CacheHandler->setBlend(true);
	CacheHandler->setBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_NOTEQUAL, 0, ~0);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

	MBTriangleFanSize4->Vertices[0] = S3DVertex(-1.f, 1.f, 0.9f, 0, 0, 1, leftDownEdge, 0, 0);
	MBTriangleFanSize4->Vertices[1] = S3DVertex(1.f, 1.f, 0.9f, 0, 0, 1, leftUpEdge, 0, 0);
	MBTriangleFanSize4->Vertices[2] = S3DVertex(1.f, -1.f, 0.9f, 0, 0, 1, rightUpEdge, 0, 0);
	MBTriangleFanSize4->Vertices[3] = S3DVertex(-1.f, -1.f, 0.9f, 0, 0, 1, rightDownEdge, 0, 0);
	MBTriangleFanSize4->setDirty(scene::EBT_VERTEX);

	drawMeshBuffer(MBTriangleFanSize4);

	unlockRenderStateMode();

	if (clearStencilBuffer)
		glClear(GL_STENCIL_BUFFER_BIT);

	glDisable(GL_STENCIL_TEST);
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
