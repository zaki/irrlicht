// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "IrrCompileConfig.h"

#ifdef _IRR_COMPILE_WITH_OGLES1_

#include "irrTypes.h"
#include "COGLESTexture.h"
#include "COGLESDriver.h"
#include "os.h"
#include "CImage.h"
#include "CColorConverter.h"

#include "irrString.h"

namespace
{
#ifndef GL_BGRA
// we need to do this for the IMG_BGRA8888 extension
int GL_BGRA=GL_RGBA;
#endif
}

namespace irr
{
namespace video
{

//! constructor for usual textures
COGLES1Texture::COGLES1Texture(IImage* origImage, const io::path& name, COGLES1Driver* driver, void* mipmapData)
	: ITexture(name), Driver(driver), Image(0), MipImage(0),
	TextureName(0), InternalFormat(GL_RGBA), PixelFormat(GL_RGBA),
	// TODO ogl-es
	// PixelFormat(GL_BGRA),
	PixelType(GL_UNSIGNED_BYTE), MipLevelStored(0),
	HasMipMaps(true), IsRenderTarget(false), IsCompressed(false), AutomaticMipmapUpdate(false),
	UseStencil(false), ReadOnlyLock(false), KeepImage(true)
{
	#ifdef _DEBUG
	setDebugName("COGLES1Texture");
	#endif

#ifndef GL_BGRA
	// whoa, pretty badly implemented extension...
	if (Driver->FeatureAvailable[COGLES1ExtensionHandler::IRR_IMG_texture_format_BGRA8888] ||
		Driver->FeatureAvailable[COGLES1ExtensionHandler::IRR_EXT_texture_format_BGRA8888] ||
		Driver->FeatureAvailable[COGLES1ExtensionHandler::IRR_APPLE_texture_format_BGRA8888])
		GL_BGRA = 0x80E1;
	else
		GL_BGRA = GL_RGBA;
#endif

	HasMipMaps = Driver->getTextureCreationFlag(ETCF_CREATE_MIP_MAPS);
	getImageValues(origImage);

	if (IsCompressed)
	{
		Image = origImage;
		Image->grab();
		KeepImage = false;
	}
	else if (ImageSize==TextureSize)
	{
		Image = Driver->createImage(ColorFormat, ImageSize);
		origImage->copyTo(Image);
	}
	else
	{
		Image = Driver->createImage(ColorFormat, TextureSize);
		origImage->copyToScaling(Image);
	}

	glGenTextures(1, &TextureName);
	uploadTexture(true, mipmapData);

	if (!KeepImage)
	{
		Image->drop();
		Image=0;
	}
}


//! constructor for basic setup (only for derived classes)
COGLES1Texture::COGLES1Texture(const io::path& name, COGLES1Driver* driver)
	: ITexture(name), Driver(driver), Image(0), MipImage(0),
	TextureName(0), InternalFormat(GL_RGBA), PixelFormat(GL_RGBA),
	PixelType(GL_UNSIGNED_BYTE), MipLevelStored(0),
	HasMipMaps(true), IsRenderTarget(false), IsCompressed(false), AutomaticMipmapUpdate(false),
	ReadOnlyLock(false), KeepImage(true)
{
	#ifdef _DEBUG
	setDebugName("COGLES1Texture");
	#endif
}


//! destructor
COGLES1Texture::~COGLES1Texture()
{
	if (TextureName)
		glDeleteTextures(1, &TextureName);
	if (Image)
		Image->drop();
}


//! Choose best matching color format, based on texture creation flags
ECOLOR_FORMAT COGLES1Texture::getBestColorFormat(ECOLOR_FORMAT format)
{
	ECOLOR_FORMAT destFormat = ECF_A8R8G8B8;

	if (!IImage::isCompressedFormat(format))
	{
		switch (format)
		{
			case ECF_A1R5G5B5:
				if (!Driver->getTextureCreationFlag(ETCF_ALWAYS_32_BIT))
					destFormat = ECF_A1R5G5B5;
				break;
			case ECF_R5G6B5:
				if (!Driver->getTextureCreationFlag(ETCF_ALWAYS_32_BIT))
					destFormat = ECF_A1R5G5B5;
				break;
			case ECF_A8R8G8B8:
				if (Driver->getTextureCreationFlag(ETCF_ALWAYS_16_BIT) ||
					Driver->getTextureCreationFlag(ETCF_OPTIMIZED_FOR_SPEED))
					destFormat = ECF_A1R5G5B5;
				break;
			case ECF_R8G8B8:
				if (Driver->getTextureCreationFlag(ETCF_ALWAYS_16_BIT) ||
					Driver->getTextureCreationFlag(ETCF_OPTIMIZED_FOR_SPEED))
					destFormat = ECF_A1R5G5B5;
				break;
			default:
				break;
		}
	}
	else
		destFormat = format;

	if (Driver->getTextureCreationFlag(ETCF_NO_ALPHA_CHANNEL))
	{
		switch (destFormat)
		{
			case ECF_A1R5G5B5:
				destFormat = ECF_R5G6B5;
				break;
			case ECF_A8R8G8B8:
				destFormat = ECF_R8G8B8;
				break;
			default:
				break;
		}
	}

	return destFormat;
}


//! Get the OpenGL color format parameters based on the given Irrlicht color format
void COGLES1Texture::getFormatParameters(ECOLOR_FORMAT format, GLint& internalFormat, GLint& filtering,
	GLenum& pixelFormat, GLenum& type, void(*&convert)(const void*, s32, void*))
{
	switch(format)
	{
		case ECF_A1R5G5B5:
			internalFormat = GL_RGBA;
			filtering = GL_LINEAR;
			pixelFormat = GL_RGBA;
			type = GL_UNSIGNED_SHORT_5_5_5_1;
			convert = CColorConverter::convert_A1R5G5B5toR5G5B5A1;
			break;
		case ECF_R5G6B5:
			internalFormat = GL_RGB;
			filtering = GL_LINEAR;
			pixelFormat = GL_RGB;
			type = GL_UNSIGNED_SHORT_5_6_5;
			break;
		case ECF_R8G8B8:
			internalFormat = GL_RGB;
			filtering = GL_LINEAR;
			pixelFormat = GL_RGB;
			type = GL_UNSIGNED_BYTE;
			break;
		case ECF_A8R8G8B8:
			filtering = GL_LINEAR;
			type = GL_UNSIGNED_BYTE;
			if (!Driver->queryOpenGLFeature(COGLES1ExtensionHandler::IRR_IMG_texture_format_BGRA8888) &&
				!Driver->queryOpenGLFeature(COGLES1ExtensionHandler::IRR_EXT_texture_format_BGRA8888) &&
				!Driver->queryOpenGLFeature(COGLES1ExtensionHandler::IRR_APPLE_texture_format_BGRA8888))
			{
				internalFormat = GL_RGBA;
				pixelFormat = GL_RGBA;
				convert = CColorConverter::convert_A8R8G8B8toA8B8G8R8;
			}
			else
			{
				internalFormat = GL_BGRA;
				pixelFormat = GL_BGRA;
			}
			break;
#ifdef GL_EXT_texture_compression_s3tc
		case ECF_DXT1:
			internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
			filtering = GL_LINEAR;
			pixelFormat = GL_BGRA;
			type = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
			break;
#endif
#ifdef GL_EXT_texture_compression_s3tc
		case ECF_DXT2:
		case ECF_DXT3:
			internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
			filtering = GL_LINEAR;
			pixelFormat = GL_BGRA;
			type = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
			break;
#endif
#ifdef GL_EXT_texture_compression_s3tc
		case ECF_DXT4:
		case ECF_DXT5:
			internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
			filtering = GL_LINEAR;
			pixelFormat = GL_BGRA;
			type = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
			break;
#endif
#ifdef GL_IMG_texture_compression_pvrtc
		case ECF_PVRTC_RGB2:
			internalFormat = GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG;
			filtering = GL_LINEAR;
			pixelFormat = GL_RGB;
			type = GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG;
			break;
#endif
#ifdef GL_IMG_texture_compression_pvrtc
		case ECF_PVRTC_ARGB2:
			internalFormat = GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG;
			filtering = GL_LINEAR;
			pixelFormat = GL_RGBA;
			type = GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG;
			break;
#endif
#ifdef GL_IMG_texture_compression_pvrtc
		case ECF_PVRTC_RGB4:
			internalFormat = GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG;
			filtering = GL_LINEAR;
			pixelFormat = GL_RGB;
			type = GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG;
			break;
#endif
#ifdef GL_IMG_texture_compression_pvrtc
		case ECF_PVRTC_ARGB4:
			internalFormat = GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG;
			filtering = GL_LINEAR;
			pixelFormat = GL_RGBA;
			type = GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG;
			break;
#endif
#ifdef GL_IMG_texture_compression_pvrtc2
		case ECF_PVRTC2_ARGB2:
			internalFormat = GL_COMPRESSED_RGBA_PVRTC_2BPPV2_IMG;
			filtering = GL_LINEAR;
			pixelFormat = GL_RGBA;
			type = GL_COMPRESSED_RGBA_PVRTC_2BPPV2_IMG;
			break;
#endif
#ifdef GL_IMG_texture_compression_pvrtc2
		case ECF_PVRTC2_ARGB4:
			internalFormat = GL_COMPRESSED_RGBA_PVRTC_4BPPV2_IMG;
			filtering = GL_LINEAR;
			pixelFormat = GL_RGBA;
			type = GL_COMPRESSED_RGBA_PVRTC_4BPPV2_IMG;
			break;
#endif
#ifdef GL_OES_compressed_ETC1_RGB8_texture
		case ECF_ETC1:
			internalFormat = GL_ETC1_RGB8_OES;
			filtering = GL_LINEAR;
			pixelFormat = GL_RGB;
			type = GL_ETC1_RGB8_OES;
			break;
#endif
#ifdef GL_ES_VERSION_3_0 // TO-DO - fix when extension name will be available
		case ECF_ETC2_RGB:
			internalFormat = GL_COMPRESSED_RGB8_ETC2;
			filtering = GL_LINEAR;
			pixelFormat = GL_RGB;
			type = GL_COMPRESSED_RGB8_ETC2;
			break;
#endif
#ifdef GL_ES_VERSION_3_0 // TO-DO - fix when extension name will be available
		case ECF_ETC2_ARGB:
			internalFormat = GL_COMPRESSED_RGBA8_ETC2_EAC;
			filtering = GL_LINEAR;
			pixelFormat = GL_RGBA;
			type = GL_COMPRESSED_RGBA8_ETC2_EAC;
			break;
#endif
		default:
			os::Printer::log("Unsupported texture format", ELL_ERROR);
			break;
	}

	// Hack for iPhone SDK, which requires a different InternalFormat
#ifdef _IRR_IPHONE_PLATFORM_
	if (internalFormat == GL_BGRA)
		internalFormat = GL_RGBA;
#endif
}


void COGLES1Texture::getImageValues(IImage* image)
{
	if (!image)
	{
		os::Printer::log("No image for OpenGL ES1 texture.", ELL_ERROR);
		return;
	}

	ImageSize = image->getDimension();

	if ( !ImageSize.Width || !ImageSize.Height)
	{
		os::Printer::log("Invalid size of image for OpenGL ES1 Texture.", ELL_ERROR);
		return;
	}

	const f32 ratio = (f32)ImageSize.Width/(f32)ImageSize.Height;
	if ((ImageSize.Width>Driver->MaxTextureSize) && (ratio >= 1.0f))
	{
		ImageSize.Width = Driver->MaxTextureSize;
		ImageSize.Height = (u32)(Driver->MaxTextureSize/ratio);
	}
	else if (ImageSize.Height>Driver->MaxTextureSize)
	{
		ImageSize.Height = Driver->MaxTextureSize;
		ImageSize.Width = (u32)(Driver->MaxTextureSize*ratio);
	}
	TextureSize=ImageSize.getOptimalSize(!Driver->queryFeature(EVDF_TEXTURE_NPOT));

	ColorFormat = getBestColorFormat(image->getColorFormat());

	IsCompressed = IImage::isCompressedFormat(image->getColorFormat());
}


//! copies the the texture into an open gl texture.
void COGLES1Texture::uploadTexture(bool newTexture, void* mipmapData, u32 level)
{
	// check which image needs to be uploaded
	IImage* image = level?MipImage:Image;
	if (!image)
	{
		os::Printer::log("No image for OpenGL ES1 texture to upload", ELL_ERROR);
		return;
	}

	// get correct opengl color data values
	GLint oldInternalFormat = InternalFormat;
	GLint filtering = GL_LINEAR;
	void(*convert)(const void*, s32, void*) = 0;
	getFormatParameters(ColorFormat, InternalFormat, filtering, PixelFormat, PixelType, convert);

	// make sure we don't change the internal format of existing images
	if (!newTexture)
		InternalFormat = oldInternalFormat;

    Driver->setActiveTexture(0, this);

	if (Driver->testGLError())
		os::Printer::log("Could not bind Texture", ELL_ERROR);

	// mipmap handling for main texture
	if (!level && newTexture)
	{
		// auto generate if possible and no mipmap data is given
		if (!IsCompressed && HasMipMaps && !mipmapData && Driver->queryFeature(EVDF_MIP_MAP_AUTO_UPDATE))
		{
			if (Driver->getTextureCreationFlag(ETCF_OPTIMIZED_FOR_SPEED))
				glHint(GL_GENERATE_MIPMAP_HINT, GL_FASTEST);
			else if (Driver->getTextureCreationFlag(ETCF_OPTIMIZED_FOR_QUALITY))
				glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
			else
				glHint(GL_GENERATE_MIPMAP_HINT, GL_DONT_CARE);

            glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
			AutomaticMipmapUpdate=true;
		}

		// enable bilinear filter without mipmaps
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filtering);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtering);
	}

	// now get image data and upload to GPU

	u32 compressedImageSize = IImage::getCompressedImageSize(ColorFormat, image->getDimension().Width, image->getDimension().Height);

	void* source = image->lock();

	IImage* tmpImage = 0;

	if (convert)
	{
		tmpImage = new CImage(image->getColorFormat(), image->getDimension());
		void* dest = tmpImage->lock();
		convert(source, image->getDimension().getArea(), dest);
		image->unlock();
		source = dest;
	}

	if (newTexture)
	{
		if (IsCompressed)
		{
			glCompressedTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, image->getDimension().Width,
				image->getDimension().Height, 0, compressedImageSize, source);
		}
		else
			glTexImage2D(GL_TEXTURE_2D, level, InternalFormat, image->getDimension().Width,
				image->getDimension().Height, 0, PixelFormat, PixelType, source);
	}
	else
	{
		if (IsCompressed)
		{
			glCompressedTexSubImage2D(GL_TEXTURE_2D, level, 0, 0, image->getDimension().Width,
				image->getDimension().Height, PixelFormat, compressedImageSize, source);
		}
		else
			glTexSubImage2D(GL_TEXTURE_2D, level, 0, 0, image->getDimension().Width,
				image->getDimension().Height, PixelFormat, PixelType, source);
	}

	if (convert)
	{
		tmpImage->unlock();
		tmpImage->drop();
	}
	else
		image->unlock();

	if (!level && newTexture)
	{
		if (IsCompressed && !mipmapData)
		{
			if (image->hasMipMaps())
				mipmapData = static_cast<u8*>(image->lock())+compressedImageSize;
			else
				HasMipMaps = false;
		}

		regenerateMipMapLevels(mipmapData);

		if (HasMipMaps) // might have changed in regenerateMipMapLevels
		{
			// enable bilinear mipmap filter
			GLint filteringMipMaps = GL_LINEAR_MIPMAP_NEAREST;

			if (filtering != GL_LINEAR)
				filteringMipMaps = GL_NEAREST_MIPMAP_NEAREST;

			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filteringMipMaps);
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtering);
		}
	}

	if (Driver->testGLError())
		os::Printer::log("Could not glTexImage2D", ELL_ERROR);
}


//! lock function
void* COGLES1Texture::lock(E_TEXTURE_LOCK_MODE mode, u32 mipmapLevel)
{
	// store info about which image is locked
	IImage* image = (mipmapLevel==0)?Image:MipImage;

	ReadOnlyLock |= (mode==ETLM_READ_ONLY);
	MipLevelStored = mipmapLevel;

	if (!Image)
		Image = new CImage(ECF_A8R8G8B8, ImageSize);
	if (IsRenderTarget)
	{
		u8* pPixels = static_cast<u8*>(Image->lock());
		if (!pPixels)
		{
			return 0;
		}
		// we need to keep the correct texture bound...
		GLint tmpTexture;
		glGetIntegerv(GL_TEXTURE_BINDING_2D, &tmpTexture);
		glBindTexture(GL_TEXTURE_2D, TextureName);

	// TODO ogl-es
	//	glGetTexImage(GL_TEXTURE_2D, 0, GL_BGRA, GL_UNSIGNED_BYTE, pPixels);

		// opengl images are horizontally flipped, so we have to fix that here.
		const u32 pitch=Image->getPitch();
		u8* p2 = pPixels + (ImageSize.Height - 1) * pitch;
		u8* tmpBuffer = new u8[pitch];
		for (u32 i=0; i < ImageSize.Height; i += 2)
		{
			memcpy(tmpBuffer, pPixels, pitch);
			memcpy(pPixels, p2, pitch);
			memcpy(p2, tmpBuffer, pitch);
			pPixels += pitch;
			p2 -= pitch;
		}
		delete [] tmpBuffer;
		Image->unlock();

		//reset old bound texture
		glBindTexture(GL_TEXTURE_2D, tmpTexture);
	}
	return Image->lock();
}


//! unlock function
void COGLES1Texture::unlock()
{
	Image->unlock();
	if (!ReadOnlyLock)
		uploadTexture(false);
	ReadOnlyLock = false;
}


//! Returns size of the original image.
const core::dimension2d<u32>& COGLES1Texture::getOriginalSize() const
{
	return ImageSize;
}


//! Returns size of the texture.
const core::dimension2d<u32>& COGLES1Texture::getSize() const
{
	if (Image)
		return Image->getDimension();
	else
		return ImageSize;
}


//! returns driver type of texture, i.e. the driver, which created the texture
E_DRIVER_TYPE COGLES1Texture::getDriverType() const
{
	return EDT_OGLES1;
}


//! returns color format of texture
ECOLOR_FORMAT COGLES1Texture::getColorFormat() const
{
	if (Image)
		return Image->getColorFormat();
	else
		return ECF_A8R8G8B8;
}


//! returns pitch of texture (in bytes)
u32 COGLES1Texture::getPitch() const
{
	if (Image)
		return Image->getPitch();
	else
		return 0;
}


//! return open gl texture name
GLuint COGLES1Texture::getOGLES1TextureName() const
{
	return TextureName;
}


//! Returns whether this texture has mipmaps
bool COGLES1Texture::hasMipMaps() const
{
	return HasMipMaps;
}


//! Regenerates the mip map levels of the texture.
void COGLES1Texture::regenerateMipMapLevels(void* mipmapData)
{
	// texture require mipmaps?
	if (!HasMipMaps || AutomaticMipmapUpdate)
		return;

	// we don't use custom data for mipmaps.
	if (!mipmapData)
	{
		// compressed textures require custom data for prepare mipmaps.
		if (IsCompressed)
			return;

		// hardware doesn't support generate mipmaps for certain texture but image data doesn't exist or is wrong.
		if (!Image || (Image && ((Image->getDimension().Width==1) && (Image->getDimension().Height==1))))
			return;
	}

	// Manually create mipmaps or use prepared version
	u32 compressedImageSize = 0;
	u32 width=Image->getDimension().Width;
	u32 height=Image->getDimension().Height;
	u32 i=0;
	u8* target = static_cast<u8*>(mipmapData);
	do
	{
		if (width>1)
			width>>=1;
		if (height>1)
			height>>=1;

		++i;

		if (!target)
			target = new u8[width*height*Image->getBytesPerPixel()];

		// create scaled version if no mipdata available
		if (!mipmapData)
			Image->copyToScaling(target, width, height, Image->getColorFormat());

		if (IsCompressed)
		{
			compressedImageSize = IImage::getCompressedImageSize(ColorFormat, width, height);

			glCompressedTexImage2D(GL_TEXTURE_2D, i, InternalFormat, width,
				height, 0, compressedImageSize, target);
		}
		else
			glTexImage2D(GL_TEXTURE_2D, i, InternalFormat, width, height,
					0, PixelFormat, PixelType, target);

		// get next prepared mipmap data if available
		if (mipmapData)
		{
			if (IsCompressed)
				mipmapData = static_cast<u8*>(mipmapData)+compressedImageSize;
			else
				mipmapData = static_cast<u8*>(mipmapData)+width*height*Image->getBytesPerPixel();

			target = static_cast<u8*>(mipmapData);
		}
	}
	while (width!=1 || height!=1);
	// cleanup
	if (!mipmapData)
		delete [] target;
}


bool COGLES1Texture::isRenderTarget() const
{
    return IsRenderTarget;
}


bool COGLES1Texture::isFrameBufferObject() const
{
	return false;
}


void COGLES1Texture::setIsRenderTarget(bool isTarget)
{
    IsRenderTarget = isTarget;
}


//! Bind Render Target Texture
void COGLES1Texture::bindRTT()
{
	glViewport(0, 0, getSize().Width, getSize().Height);
}


//! Unbind Render Target Texture
void COGLES1Texture::unbindRTT()
{
	glBindTexture(GL_TEXTURE_2D, getOGLES1TextureName());

	// Copy Our ViewPort To The Texture
	glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, getSize().Width, getSize().Height);
}

/* FBO Textures */

#ifdef GL_OES_framebuffer_object
// helper function for render to texture
static bool checkOGLES1FBOStatus(COGLES1Driver* Driver);
#endif


//! RTT ColorFrameBuffer constructor
COGLES1FBOTexture::COGLES1FBOTexture(const core::dimension2d<u32>& size,
					const io::path& name, COGLES1Driver* driver,
					ECOLOR_FORMAT format)
	: COGLES1Texture(name, driver), DepthTexture(0), ColorFrameBuffer(0)
{
	#ifdef _DEBUG
	setDebugName("COGLES1Texture_FBO");
	#endif

	ImageSize = size;
	TextureSize = size;
	HasMipMaps = false;
	IsRenderTarget = true;
	ColorFormat = getBestColorFormat(format);

	switch (ColorFormat)
	{
	case ECF_A8R8G8B8:
		InternalFormat = GL_RGBA;
		PixelFormat = GL_RGBA;
		PixelType = GL_UNSIGNED_BYTE;
		break;
	case ECF_R8G8B8:
		InternalFormat = GL_RGB;
		PixelFormat = GL_RGB;
		PixelType = GL_UNSIGNED_BYTE;
		break;
	case ECF_A1R5G5B5:
		InternalFormat = GL_RGBA;
		PixelFormat = GL_RGBA;
		PixelType = GL_UNSIGNED_SHORT_5_5_5_1;
		break;
	case ECF_R5G6B5:
		InternalFormat = GL_RGB;
		PixelFormat = GL_RGB;
		PixelType = GL_UNSIGNED_SHORT_5_6_5;
		break;
	default:
		os::Printer::log( "color format not handled", ELL_WARNING );
		break;
	}

#ifdef GL_OES_framebuffer_object
	// generate frame buffer
	Driver->extGlGenFramebuffers(1, &ColorFrameBuffer);
	bindRTT();

	// generate color texture
	glGenTextures(1, &TextureName);
    glBindTexture(GL_TEXTURE_2D, TextureName);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, ImageSize.Width, ImageSize.Height, 0, PixelFormat, PixelType, 0);

#ifdef _DEBUG
	Driver->testGLError();
#endif

	// attach color texture to frame buffer
	Driver->extGlFramebufferTexture2D(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_TEXTURE_2D, TextureName, 0);
#ifdef _DEBUG
	checkOGLES1FBOStatus(Driver);
#endif

	glBindTexture(GL_TEXTURE_2D, TextureName);
	unbindRTT();
#endif
}


//! destructor
COGLES1FBOTexture::~COGLES1FBOTexture()
{
	if (DepthTexture)
		if (DepthTexture->drop())
			Driver->removeDepthTexture(DepthTexture);
#ifdef GL_OES_framebuffer_object
	if (ColorFrameBuffer)
		Driver->extGlDeleteFramebuffers(1, &ColorFrameBuffer);
#endif
}


bool COGLES1FBOTexture::isFrameBufferObject() const
{
	return true;
}


//! Bind Render Target Texture
void COGLES1FBOTexture::bindRTT()
{
#ifdef GL_OES_framebuffer_object
	if (ColorFrameBuffer != 0)
		Driver->extGlBindFramebuffer(GL_FRAMEBUFFER_OES, ColorFrameBuffer);
#endif
}


//! Unbind Render Target Texture
void COGLES1FBOTexture::unbindRTT()
{
#ifdef GL_OES_framebuffer_object
	if (ColorFrameBuffer != 0)
		Driver->extGlBindFramebuffer(GL_FRAMEBUFFER_OES, 0);
#endif
}


/* FBO Depth Textures */

//! RTT DepthBuffer constructor
COGLES1FBODepthTexture::COGLES1FBODepthTexture(
		const core::dimension2d<u32>& size,
		const io::path& name,
		COGLES1Driver* driver,
		bool useStencil)
	: COGLES1Texture(name, driver), DepthRenderBuffer(0),
	StencilRenderBuffer(0), UseStencil(useStencil)
{
#ifdef _DEBUG
	setDebugName("COGLES1TextureFBO_Depth");
#endif

	ImageSize = size;
	TextureSize = size;
	InternalFormat = GL_RGBA;
	PixelFormat = GL_RGBA;
	PixelType = GL_UNSIGNED_BYTE;
	HasMipMaps = false;

#if defined(GL_OES_framebuffer_object)
	GLenum internalFormatDepth = GL_DEPTH_COMPONENT16_OES;

#if defined(GL_OES_depth24)
	if (Driver->queryOpenGLFeature(COGLES1ExtensionHandler::IRR_OES_depth24))
		internalFormatDepth = GL_DEPTH_COMPONENT24_OES;
#endif

	bool stencilSupported = true;
	GLenum internalFormatStencil = 0;

#if defined(GL_OES_packed_depth_stencil)
	if (Driver->queryOpenGLFeature(COGLES1ExtensionHandler::IRR_OES_packed_depth_stencil))
		internalFormatStencil = GL_DEPTH24_STENCIL8_OES;
	else
#endif
#if defined(GL_OES_stencil8)
	if (Driver->queryOpenGLFeature(COGLES1ExtensionHandler::IRR_OES_stencil8))
		internalFormatStencil = GL_OES_stencil8;
	else
#endif
#if defined(GL_OES_stencil4)
	if (Driver->queryOpenGLFeature(COGLES1ExtensionHandler::IRR_OES_stencil4))
		internalFormatStencil = GL_OES_stencil4;
	else
#endif
#if defined(GL_OES_stencil1)
	if (Driver->queryOpenGLFeature(COGLES1ExtensionHandler::IRR_OES_stencil1))
		internalFormatStencil = GL_OES_stencil1;
	else
#endif
		stencilSupported = false;

	if (useStencil && stencilSupported)
	{
		Driver->extGlGenRenderbuffers(1, &DepthRenderBuffer);
		Driver->extGlBindRenderbuffer(GL_RENDERBUFFER_OES, DepthRenderBuffer);
#if defined(GL_OES_packed_depth_stencil)
		if (Driver->queryOpenGLFeature(COGLES1ExtensionHandler::IRR_OES_packed_depth_stencil))
		{
			// generate packed depth stencil buffer
			Driver->extGlRenderbufferStorage(GL_RENDERBUFFER_OES, internalFormatStencil, ImageSize.Width, ImageSize.Height);
			StencilRenderBuffer = DepthRenderBuffer; // stencil is packed with depth
		}
		else // generate separate stencil and depth textures
#endif
		{
			Driver->extGlRenderbufferStorage(GL_RENDERBUFFER_OES, internalFormatDepth, ImageSize.Width, ImageSize.Height);

			Driver->extGlGenRenderbuffers(1, &StencilRenderBuffer);
			Driver->extGlBindRenderbuffer(GL_RENDERBUFFER_OES, StencilRenderBuffer);
			Driver->extGlRenderbufferStorage(GL_RENDERBUFFER_OES, internalFormatStencil, ImageSize.Width, ImageSize.Height);
		}
	}
	else
	{
		// generate depth buffer
		Driver->extGlGenRenderbuffers(1, &DepthRenderBuffer);
		Driver->extGlBindRenderbuffer(GL_RENDERBUFFER_OES, DepthRenderBuffer);
		Driver->extGlRenderbufferStorage(GL_RENDERBUFFER_OES, internalFormatDepth, ImageSize.Width, ImageSize.Height);
	}
#endif
}


//! destructor
COGLES1FBODepthTexture::~COGLES1FBODepthTexture()
{
#ifdef GL_OES_framebuffer_object
	if (DepthRenderBuffer)
		Driver->extGlDeleteRenderbuffers(1, &DepthRenderBuffer);

	if (StencilRenderBuffer && StencilRenderBuffer != DepthRenderBuffer)
		Driver->extGlDeleteRenderbuffers(1, &StencilRenderBuffer);
#endif
}


//combine depth texture and rtt
bool COGLES1FBODepthTexture::attach(ITexture* renderTex)
{
#ifdef GL_OES_framebuffer_object
	if (!renderTex)
		return false;
	COGLES1FBOTexture* rtt = static_cast<COGLES1FBOTexture*>(renderTex);
	rtt->bindRTT();

	// attach stencil texture to stencil buffer
	if (UseStencil)
		Driver->extGlFramebufferRenderbuffer(GL_FRAMEBUFFER_OES, GL_STENCIL_ATTACHMENT_OES, GL_RENDERBUFFER_OES, StencilRenderBuffer);

	// attach depth renderbuffer to depth buffer
	Driver->extGlFramebufferRenderbuffer(GL_FRAMEBUFFER_OES, GL_DEPTH_ATTACHMENT_OES, GL_RENDERBUFFER_OES, DepthRenderBuffer);

	// check the status
	if (!checkOGLES1FBOStatus(Driver))
	{
		os::Printer::log("FBO incomplete");
		return false;
	}
	rtt->DepthTexture=this;
	grab(); // grab the depth buffer, not the RTT
	rtt->unbindRTT();
	return true;
#else
	return false;
#endif
}


//! Bind Render Target Texture
void COGLES1FBODepthTexture::bindRTT()
{
}


//! Unbind Render Target Texture
void COGLES1FBODepthTexture::unbindRTT()
{
}


#ifdef GL_OES_framebuffer_object
bool checkOGLES1FBOStatus(COGLES1Driver* Driver)
{
	GLenum status = Driver->extGlCheckFramebufferStatus(GL_FRAMEBUFFER_OES);

	switch (status)
	{
		//Our FBO is perfect, return true
		case GL_FRAMEBUFFER_COMPLETE_OES:
			return true;

		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_OES:
			os::Printer::log("FBO has one or several incomplete image attachments", ELL_ERROR);
			break;

		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_OES:
			os::Printer::log("FBO missing an image attachment", ELL_ERROR);
			break;

		case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_OES:
			os::Printer::log("FBO has one or several image attachments with different dimensions", ELL_ERROR);
			break;

		case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_OES:
			os::Printer::log("FBO has one or several image attachments with different internal formats", ELL_ERROR);
			break;

// not part of all implementations
#ifdef GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_OES
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_OES:
			os::Printer::log("FBO has invalid draw buffer", ELL_ERROR);
			break;
#endif

// not part of all implementations
#ifdef GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_OES
		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_OES:
			os::Printer::log("FBO has invalid read buffer", ELL_ERROR);
			break;
#endif

// not part of fbo_object anymore, but won't harm as it is just a return value
#ifdef GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_OES
		case GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_OES:
			os::Printer::log("FBO has a duplicate image attachment", ELL_ERROR);
			break;
#endif

		case GL_FRAMEBUFFER_UNSUPPORTED_OES:
			os::Printer::log("FBO format unsupported", ELL_ERROR);
			break;

		default:
			break;
	}
	os::Printer::log("FBO error", ELL_ERROR);
	return false;
}
#endif

} // end namespace video
} // end namespace irr

#endif // _IRR_COMPILE_WITH_OGLES1_

