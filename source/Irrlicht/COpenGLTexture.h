// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_OPEN_GL_TEXTURE_H_INCLUDED__
#define __C_OPEN_GL_TEXTURE_H_INCLUDED__

#include "ITexture.h"
#include "IImage.h"
#include "SMaterialLayer.h"
#include "irrArray.h"

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_OPENGL_

#if defined(_IRR_OPENGL_USE_EXTPOINTER_)
	#define GL_GLEXT_LEGACY 1
#else
	#define GL_GLEXT_PROTOTYPES 1
#endif
#ifdef _IRR_WINDOWS_API_
	// include windows headers for HWND
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#include <GL/gl.h>
#ifdef _MSC_VER
	#pragma comment(lib, "OpenGL32.lib")
#endif
#elif defined(_IRR_OSX_PLATFORM_)
	#include <OpenGL/gl.h>
#elif defined(_IRR_COMPILE_WITH_SDL_DEVICE_)
	#define NO_SDL_GLEXT
	#include <SDL/SDL_video.h>
	#include <SDL/SDL_opengl.h>
#else
	#if defined(_IRR_OSX_PLATFORM_)
		#include <OpenGL/gl.h>
	#else
		#include <GL/gl.h>
	#endif
#endif


namespace irr
{
namespace video
{

class COpenGLDriver;

//! OpenGL texture.
class COpenGLTexture : public ITexture
{
public:

	//! Cache structure.
	struct SStatesCache
	{
		SStatesCache() : WrapU(ETC_REPEAT), WrapV(ETC_REPEAT),
				LODBias(0), AnisotropicFilter(0),
				BilinearFilter(false), TrilinearFilter(false),
				MipMapStatus(false), IsCached(false)
		{
		}

		u8 WrapU;
		u8 WrapV;
		s8 LODBias;
		u8 AnisotropicFilter;
		bool BilinearFilter;
		bool TrilinearFilter;
		bool MipMapStatus;
		bool IsCached;
	};

	//! constructor
	COpenGLTexture(IImage* surface, const io::path& name, void* mipmapData=0, COpenGLDriver* driver=0);

	//! constructor
	COpenGLTexture(const io::path& name, IImage* posXImage, IImage* negXImage, IImage* posYImage,
		IImage* negYImage, IImage* posZImage, IImage* negZImage, COpenGLDriver* driver=0);

	//! destructor
	virtual ~COpenGLTexture();

	//! lock function
	virtual void* lock(E_TEXTURE_LOCK_MODE mode=ETLM_READ_WRITE, u32 mipmapLevel=0) _IRR_OVERRIDE_;

	//! unlock function
	virtual void unlock() _IRR_OVERRIDE_;

	//! Returns original size of the texture (image).
	virtual const core::dimension2d<u32>& getOriginalSize() const _IRR_OVERRIDE_;

	//! Returns size of the texture.
	virtual const core::dimension2d<u32>& getSize() const _IRR_OVERRIDE_;

	//! returns driver type of texture (=the driver, that created it)
	virtual E_DRIVER_TYPE getDriverType() const _IRR_OVERRIDE_;

	//! returns color format of texture
	virtual ECOLOR_FORMAT getColorFormat() const _IRR_OVERRIDE_;

	//! returns pitch of texture (in bytes)
	virtual u32 getPitch() const _IRR_OVERRIDE_;

	//! return open gl texture name
	GLuint getOpenGLTextureName() const;

	//! return open gl texture type
	GLenum getOpenGLTextureType() const;

	//! return whether this texture has mipmaps
	virtual bool hasMipMaps() const _IRR_OVERRIDE_;

	//! Regenerates the mip map levels of the texture.
	/** Useful after locking and modifying the texture
	\param mipmapData Pointer to raw mipmap data, including all necessary mip levels, in the same format as the main texture image. If not set the mipmaps are derived from the main image. */
	virtual void regenerateMipMapLevels(void* mipmapData=0) _IRR_OVERRIDE_;

	//! Is it a render target?
	virtual bool isRenderTarget() const _IRR_OVERRIDE_;

	//! Is it a FrameBufferObject?
	virtual bool isFrameBufferObject() const;

	//! Bind RenderTargetTexture
	virtual void bindRTT();

	//! Unbind RenderTargetTexture
	virtual void unbindRTT();

	//! sets whether this texture is intended to be used as a render target.
	void setIsRenderTarget(bool isTarget);

	//! Get an access to texture states cache.
	SStatesCache& getStatesCache() const;

protected:

	//! protected constructor with basic setup, no GL texture name created, for derived classes
	COpenGLTexture(const io::path& name, COpenGLDriver* driver);

	//! get the desired color format based on texture creation flags and the input format.
	ECOLOR_FORMAT getBestColorFormat(ECOLOR_FORMAT format);

	//! Get the OpenGL color format parameters based on the given Irrlicht color format
	void getFormatParameters(ECOLOR_FORMAT format, GLint& internalFormat, GLint& filtering,
		GLenum& pixelFormat, GLenum& type);

	//! get important numbers of the image and hw texture
	void getImageValues(IImage* image);

	//! copies the texture into an OpenGL texture.
	/** \param newTexture True if method is called for a newly created texture for the first time. Otherwise call with false to improve memory handling.
	\param imageNumber Inform which image should be used for upload.
	\param regMipmap Inform if regenerate mipmap should be call.
	\param mipmapData Pointer to raw mipmap data, including all necessary mip levels, in the same format as the main texture image.
	\param mipLevel If set to non-zero, only that specific miplevel is updated, using the MipImage member. */
	void uploadTexture(bool newTexture=false, u32 imageNumber=0, bool regMipmap = false, void* mipmapData=0, u32 mipLevel=0);

	core::dimension2d<u32> ImageSize;
	core::dimension2d<u32> TextureSize;
	ECOLOR_FORMAT ColorFormat;
	COpenGLDriver* Driver;
	core::array<IImage*> Image;
	IImage* MipImage;

	GLuint TextureName;
	GLenum TextureType;
	GLint InternalFormat;
	GLenum PixelFormat;
	GLenum PixelType;

	u8 MipLevelStored;
	bool HasMipMaps;
	bool MipmapLegacyMode;
	bool IsRenderTarget;
	bool IsCompressed;
	bool AutomaticMipmapUpdate;
	bool ReadOnlyLock;
	bool KeepImage;

	mutable SStatesCache StatesCache;
};

//! OpenGL FBO texture.
class COpenGLFBOTexture : public COpenGLTexture
{
public:

	//! FrameBufferObject constructor
	COpenGLFBOTexture(const core::dimension2d<u32>& size, const io::path& name,
		COpenGLDriver* driver = 0, ECOLOR_FORMAT format = ECF_UNKNOWN);

	//! destructor
	virtual ~COpenGLFBOTexture();

	//! Is it a FrameBufferObject?
	virtual bool isFrameBufferObject() const _IRR_OVERRIDE_;

	//! Bind RenderTargetTexture
	virtual void bindRTT() _IRR_OVERRIDE_;

	//! Unbind RenderTargetTexture
	virtual void unbindRTT() _IRR_OVERRIDE_;

	ITexture* DepthTexture;
protected:
	GLuint ColorFrameBuffer;
};


//! OpenGL FBO depth texture.
class COpenGLFBODepthTexture : public COpenGLTexture
{
public:
	//! FrameBufferObject depth constructor
	COpenGLFBODepthTexture(const core::dimension2d<u32>& size, const io::path& name, COpenGLDriver* driver=0, bool useStencil=false);

	//! destructor
	virtual ~COpenGLFBODepthTexture();

	//! Bind RenderTargetTexture
	virtual void bindRTT() _IRR_OVERRIDE_;

	//! Unbind RenderTargetTexture
	virtual void unbindRTT() _IRR_OVERRIDE_;

	bool attach(ITexture*);

protected:
	GLuint DepthRenderBuffer;
	GLuint StencilRenderBuffer;
	bool UseStencil;
};


} // end namespace video
} // end namespace irr

#endif
#endif // _IRR_COMPILE_WITH_OPENGL_

