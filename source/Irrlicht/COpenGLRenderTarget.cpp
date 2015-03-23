// Copyright (C) 2015 Patryk Nadrowski
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "COpenGLRenderTarget.h"

#ifdef _IRR_COMPILE_WITH_OPENGL_

#include "IImage.h"
#include "irrMath.h"

#include "COpenGLDriver.h"
#include "COpenGLTexture.h"

#if !defined(GL_VERSION_3_0) && !defined(GL_ARB_framebuffer_object)
	#ifdef GL_EXT_framebuffer_object
		#define GL_FRAMEBUFFER GL_FRAMEBUFFER_EXT
		#define GL_COLOR_ATTACHMENT0 GL_COLOR_ATTACHMENT0_EXT
		#define GL_DEPTH_ATTACHMENT GL_DEPTH_ATTACHMENT_EXT
		#define GL_STENCIL_ATTACHMENT GL_STENCIL_ATTACHMENT_EXT
		#define GL_FRAMEBUFFER_COMPLETE GL_FRAMEBUFFER_COMPLETE_EXT
		#define GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT
		#define GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT
		#define GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT
		#define GL_FRAMEBUFFER_INCOMPLETE_FORMATS GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT
		#define GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT
		#define GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT
		#define GL_FRAMEBUFFER_UNSUPPORTED GL_FRAMEBUFFER_UNSUPPORTED_EXT
	#else
		#define GL_FRAMEBUFFER 0
		#define GL_COLOR_ATTACHMENT0 0
		#define GL_DEPTH_ATTACHMENT 0
		#define GL_STENCIL_ATTACHMENT 0
		#define GL_FRAMEBUFFER_COMPLETE 0
		#define GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER 1
		#define GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER 2
		#define GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT 3
		#define GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT 4
		#define GL_FRAMEBUFFER_UNSUPPORTED 5
	#endif
#endif

#ifdef GL_EXT_framebuffer_object
	#define GL_FRAMEBUFFER_INCOMPLETE_FORMATS GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT
	#define GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT
#else
	#define GL_FRAMEBUFFER_INCOMPLETE_FORMATS 6
	#define GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS 7
#endif

namespace irr
{
namespace video
{

bool checkFBOStatus(COpenGLDriver* Driver)
{
	GLenum status = Driver->extGlCheckFramebufferStatus(GL_FRAMEBUFFER);

	switch (status)
	{
		case GL_FRAMEBUFFER_COMPLETE:
			return true;
		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
			os::Printer::log("FBO has invalid read buffer", ELL_ERROR);
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
			os::Printer::log("FBO has invalid draw buffer", ELL_ERROR);
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
			os::Printer::log("FBO has one or several incomplete image attachments", ELL_ERROR);
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_FORMATS:
			os::Printer::log("FBO has one or several image attachments with different internal formats", ELL_ERROR);
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
			os::Printer::log("FBO has one or several image attachments with different dimensions", ELL_ERROR);
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
			os::Printer::log("FBO missing an image attachment", ELL_ERROR);
			break;
		case GL_FRAMEBUFFER_UNSUPPORTED:
			os::Printer::log("FBO format unsupported", ELL_ERROR);
			break;
		default:
			os::Printer::log("FBO error", ELL_ERROR);
			break;
	}

	return false;
}

COpenGLRenderTarget::COpenGLRenderTarget(COpenGLDriver* driver) : AssignedTextureCount(0), AssignedDepth(false), AssignedStencil(false),
	TextureUpdate(false), DepthStencilUpdate(false), BufferID(0), SupportForFBO(false), SupportForMRT(false), BridgeCalls(0), Driver(driver)
{
#ifdef _DEBUG
	setDebugName("COpenGLRenderTarget");
#endif

	DriverType = EDT_OPENGL;

	AssignedActiveTextureID.set_used(1);
	AssignedActiveTextureID[0] = 0;

	Size = Driver->getScreenSize();
	BridgeCalls = Driver->getBridgeCalls();

#if defined(GL_VERSION_3_0) || defined(GL_ARB_framebuffer_object) || defined(GL_EXT_framebuffer_object)
	SupportForFBO = Driver->FeatureAvailable[COpenGLDriver::IRR_EXT_framebuffer_object] || Driver->FeatureAvailable[COpenGLDriver::IRR_ARB_framebuffer_object];
#endif

	SupportForMRT = SupportForFBO && Driver->MaxMultipleRenderTargets > 1 && (Driver->Version >= 200 || Driver->FeatureAvailable[COpenGLDriver::IRR_ARB_draw_buffers] ||
		Driver->FeatureAvailable[COpenGLDriver::IRR_ATI_draw_buffers]);

	if (SupportForFBO)
		Driver->extGlGenFramebuffers(1, &BufferID);
}

COpenGLRenderTarget::~COpenGLRenderTarget()
{
	if (SupportForFBO && BufferID != 0)
		Driver->extGlDeleteFramebuffers(1, &BufferID);

	for (u32 i = 0; i < Texture.size(); ++i)
	{
		if (Texture[i])
			Texture[i]->drop();
	}

	if (DepthStencil)
		DepthStencil->drop();
}

void COpenGLRenderTarget::setTexture(const core::array<ITexture*>& texture, ITexture* depthStencil)
{
	TextureUpdate = TextureUpdate || Texture != texture;

	if (Texture != texture)
	{
		for (u32 i = 0; i < Texture.size(); ++i)
		{
			if (Texture[i])
				Texture[i]->drop();
		}

		Texture.set_used(core::min_(texture.size(), static_cast<u32>(Driver->MaxColorAttachments)));

		for (u32 i = 0; i < Texture.size(); ++i)
		{
			GLuint textureID = (texture[i] && texture[i]->getDriverType() == EDT_OPENGL) ? static_cast<COpenGLTexture*>(depthStencil)->getOpenGLTextureName() : 0;

			if (textureID != 0)
			{
				Texture[i] = texture[i];
				Texture[i]->grab();
			}
			else
			{
				Texture[i] = 0;
			}
		}
	}

	DepthStencilUpdate = DepthStencilUpdate || DepthStencil != depthStencil;

	if (DepthStencil != depthStencil)
	{
		GLuint textureID = (depthStencil && depthStencil->getDriverType() == EDT_OPENGL) ? static_cast<COpenGLTexture*>(depthStencil)->getOpenGLTextureName() : 0;
		const ECOLOR_FORMAT textureFormat = (textureID != 0) ? depthStencil->getColorFormat() : ECF_UNKNOWN;

		if (IImage::isDepthFormat(textureFormat))
		{
			DepthStencil = depthStencil;
			DepthStencil->grab();
		}
		else
		{
			if (DepthStencil)
				DepthStencil->drop();

			DepthStencil = 0;
		}
	}
}

void COpenGLRenderTarget::update(const core::array<u32>& id)
{
	if (TextureUpdate || DepthStencilUpdate)
	{
		// Set color attachments.

		if (TextureUpdate)
		{
			const u32 textureSize = Texture.size();
			const u32 stepCount = core::max_(textureSize, AssignedTextureCount);

			for (u32 i = 0; i < stepCount; ++i)
			{
				GLuint textureID = 0;

				if (i < textureSize && Texture[i])
					textureID = static_cast<COpenGLTexture*>(Texture[i])->getOpenGLTextureName();

				if (textureID != 0)
				{
					Driver->extGlFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, textureID, 0);
				}
				else if (i < AssignedTextureCount)
				{
					Driver->extGlFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, 0, 0);
				}
			}

			AssignedTextureCount = textureSize;

			TextureUpdate = false;
		}

		// Set depth and stencil attachments.

		if (DepthStencilUpdate)
		{
			const ECOLOR_FORMAT textureFormat = (DepthStencil) ? DepthStencil->getColorFormat() : ECF_UNKNOWN;

			if (IImage::isDepthFormat(textureFormat))
			{
				GLuint textureID = static_cast<COpenGLTexture*>(DepthStencil)->getOpenGLTextureName();

				Driver->extGlFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, textureID, 0);

				if (textureFormat == ECF_D24S8)
				{
					Driver->extGlFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, textureID, 0);

					AssignedStencil = true;
				}
				else
				{
					if (AssignedStencil)
						Driver->extGlFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, 0, 0);

					AssignedStencil = false;
				}

				AssignedDepth = true;
			}
			else
			{
				if (AssignedDepth)
					Driver->extGlFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0);

				if (AssignedStencil)
					Driver->extGlFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, 0, 0);

				AssignedDepth = false;
				AssignedStencil = false;
			}

			DepthStencilUpdate = false;
		}

		// Set size required for a viewport.

		ITexture* firstTexture = getTexture();

		if (firstTexture)
			Size = firstTexture->getSize();
		else
		{
			if (DepthStencil)
				Size = DepthStencil->getSize();
			else
				Size = Driver->getScreenSize();
		}

#ifdef _DEBUG
		checkFBOStatus(Driver);
#endif
	}

	if ((AssignedActiveTextureID != id) && SupportForFBO && BufferID != 0)
	{
		const u32 size = id.size();

		if (size == 0)
			glDrawBuffer(GL_NONE);
		else if (size == 1 || !SupportForMRT)
			glDrawBuffer(GL_COLOR_ATTACHMENT0 + id[0]);
		else
		{
			GLenum* target = new GLenum[Driver->MaxMultipleRenderTargets];

			for (u32 i = 0; i < Driver->MaxMultipleRenderTargets; ++i)
				target[i] = GL_NONE;

			const u32 mrtSize = core::min_(size, static_cast<u32>(Driver->MaxMultipleRenderTargets));

			for (u32 i = 0; i < mrtSize; ++i)
				target[i + id[i]] = GL_COLOR_ATTACHMENT0 + id[i];

			Driver->extGlDrawBuffers(mrtSize, target);

			delete[] target;
		}

		AssignedActiveTextureID = id;
	}
}

GLuint COpenGLRenderTarget::getBufferID() const
{
	return BufferID;
}

const core::dimension2d<u32>& COpenGLRenderTarget::getSize() const
{
	return Size;
}

ITexture* COpenGLRenderTarget::getTexture() const
{
	for (u32 i = 0; i < Texture.size(); ++i)
	{
		if (Texture[i])
			return Texture[i];
	}

	return 0;
}

}
}

#endif
