// Copyright (C) 2014 Patryk Nadrowski
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#include "IrrCompileConfig.h"

#ifdef _IRR_COMPILE_WITH_OGLES2_

#include "irrTypes.h"
#include "COGLES2Texture.h"
#include "COGLES2Driver.h"
#include "os.h"
#include "CImage.h"
#include "CColorConverter.h"
#include "irrString.h"
#include "EVertexAttributes.h"

#if !defined(_IRR_COMPILE_WITH_IPHONE_DEVICE_)
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <EGL/egl.h>
#endif

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
COGLES2Texture::COGLES2Texture(IImage* origImage, const io::path& name, void* mipmapData, COGLES2Driver* driver)
	: ITexture(name, ETT_2D), Pitch(0), ColorFormat(ECF_A8R8G8B8), Driver(driver), Image(0), MipImage(0),
	TextureName(0), TextureType(GL_TEXTURE_2D), InternalFormat(GL_RGBA), PixelFormat(GL_RGBA),
	PixelType(GL_UNSIGNED_BYTE), MipLevelStored(0),
	IsRenderTarget(false), IsCompressed(false), AutomaticMipmapUpdate(false),
	ReadOnlyLock(false), KeepImage(true), LockImage(0)
{
	#ifdef _DEBUG
	setDebugName("COGLES2Texture");
	#endif

#ifndef GL_BGRA
	// whoa, pretty badly implemented extension...
	if (Driver->FeatureAvailable[COGLES2ExtensionHandler::IRR_IMG_texture_format_BGRA8888] ||
		Driver->FeatureAvailable[COGLES2ExtensionHandler::IRR_EXT_texture_format_BGRA8888] ||
		Driver->FeatureAvailable[COGLES2ExtensionHandler::IRR_APPLE_texture_format_BGRA8888])
		GL_BGRA = 0x80E1;
	else
		GL_BGRA = GL_RGBA;
#endif

	HasMipMaps = Driver->getTextureCreationFlag(ETCF_CREATE_MIP_MAPS);
	getImageValues(origImage);

	if (IsCompressed)
	{
		Image.push_back(origImage);
		Image[0]->grab();
		KeepImage = false;
	}
	else if (ImageSize==TextureSize)
	{
		Image.push_back(Driver->createImage(ColorFormat, ImageSize));
		origImage->copyTo(Image[0]);
	}
	else
	{
		Image.push_back(Driver->createImage(ColorFormat, TextureSize));
		origImage->copyToScaling(Image[0]);
	}

	Pitch = Image[0]->getPitch();

	glGenTextures(1, &TextureName);
	uploadTexture(true, 0, true, mipmapData);

	if ( KeepImage )
	{
		KeepImage = Driver->getTextureCreationFlag(ETCF_ALLOW_MEMORY_COPY);
		if ( KeepImage  )
		{
			LockImage = Image[0];
			LockImage->grab();
		}
	}

	Image[0]->drop();
	Image.clear();
}


//! constructor for cube textures
COGLES2Texture::COGLES2Texture(const io::path& name, IImage* posXImage, IImage* negXImage, IImage* posYImage,
	IImage* negYImage, IImage* posZImage, IImage* negZImage, COGLES2Driver* driver)
		: ITexture(name, ETT_CUBE), Pitch(0), ColorFormat(ECF_A8R8G8B8), Driver(driver), Image(0), MipImage(0),
		TextureName(0), TextureType(GL_TEXTURE_CUBE_MAP), InternalFormat(GL_RGBA), PixelFormat(GL_RGBA),
		PixelType(GL_UNSIGNED_BYTE), MipLevelStored(0), IsRenderTarget(false), IsCompressed(false),
		AutomaticMipmapUpdate(false), ReadOnlyLock(false), KeepImage(false), LockImage(0)
{
	#ifdef _DEBUG
	setDebugName("COpenGLTexture");
	#endif

#ifndef GL_BGRA
	// whoa, pretty badly implemented extension...
	if (Driver->FeatureAvailable[COGLES2ExtensionHandler::IRR_IMG_texture_format_BGRA8888] ||
		Driver->FeatureAvailable[COGLES2ExtensionHandler::IRR_EXT_texture_format_BGRA8888] ||
		Driver->FeatureAvailable[COGLES2ExtensionHandler::IRR_APPLE_texture_format_BGRA8888])
		GL_BGRA = 0x80E1;
	else
		GL_BGRA = GL_RGBA;
#endif

	HasMipMaps = Driver->getTextureCreationFlag(ETCF_CREATE_MIP_MAPS);
	getImageValues(posXImage);

	if (IsCompressed)
	{
		Image.push_back(posXImage);
		Image.push_back(negXImage);
		Image.push_back(posYImage);
		Image.push_back(negYImage);
		Image.push_back(posZImage);
		Image.push_back(negZImage);

		for (u32 i = 0; i < 6; ++i)
			Image[i]->grab();
	}
	else if (ImageSize==TextureSize)
	{
		for (u32 i = 0; i < 6; ++i)
			Image.push_back(Driver->createImage(ColorFormat, ImageSize));

		posXImage->copyTo(Image[0]);
		negXImage->copyTo(Image[1]);
		posYImage->copyTo(Image[2]);
		negYImage->copyTo(Image[3]);
		posZImage->copyTo(Image[4]);
		negZImage->copyTo(Image[5]);
	}
	else
	{
		for (u32 i = 0; i < 6; ++i)
			Image.push_back(Driver->createImage(ColorFormat, ImageSize));

		posXImage->copyToScaling(Image[0]);
		negXImage->copyToScaling(Image[1]);
		posYImage->copyToScaling(Image[2]);
		negYImage->copyToScaling(Image[3]);
		posZImage->copyToScaling(Image[4]);
		negZImage->copyToScaling(Image[5]);
	}

	Pitch = Image[0]->getPitch();

	glGenTextures(1, &TextureName);

	for (u32 i = 0; i < 5; ++i)
		uploadTexture(true, i, false);

	uploadTexture(true, 5, true);

	for (u32 i = 0; i < Image.size(); ++i)
		Image[i]->drop();

	Image.clear();
}


//! constructor for basic setup (only for derived classes)
COGLES2Texture::COGLES2Texture(const io::path& name, COGLES2Driver* driver)
	: ITexture(name, ETT_2D), Pitch(0), ColorFormat(ECF_A8R8G8B8), Driver(driver), Image(0), MipImage(0),
	TextureName(0), TextureType(GL_TEXTURE_2D), InternalFormat(GL_RGBA), PixelFormat(GL_RGBA),
	PixelType(GL_UNSIGNED_BYTE), MipLevelStored(0), HasMipMaps(true),
	IsRenderTarget(false), IsCompressed(false), AutomaticMipmapUpdate(false),
	ReadOnlyLock(false), KeepImage(false), LockImage(0)
{
	#ifdef _DEBUG
	setDebugName("COGLES2Texture");
	#endif
}


//! destructor
COGLES2Texture::~COGLES2Texture()
{
	if (TextureName)
		glDeleteTextures(1, &TextureName);

	if (LockImage)
		LockImage->drop();
}


//! Choose best matching color format, based on texture creation flags
ECOLOR_FORMAT COGLES2Texture::getBestColorFormat(ECOLOR_FORMAT format)
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
void COGLES2Texture::getFormatParameters(ECOLOR_FORMAT format, GLint& internalFormat, GLint& filtering,
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
			if (!Driver->queryOpenGLFeature(COGLES2ExtensionHandler::IRR_IMG_texture_format_BGRA8888) &&
				!Driver->queryOpenGLFeature(COGLES2ExtensionHandler::IRR_EXT_texture_format_BGRA8888) &&
				!Driver->queryOpenGLFeature(COGLES2ExtensionHandler::IRR_APPLE_texture_format_BGRA8888))
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


// prepare values ImageSize, TextureSize, and ColorFormat based on image
void COGLES2Texture::getImageValues(IImage* image)
{
	if (!image)
	{
		os::Printer::log("No image for OpenGL ES2 texture.", ELL_ERROR);
		return;
	}

	ImageSize = image->getDimension();

	if ( !ImageSize.Width || !ImageSize.Height)
	{
		os::Printer::log("Invalid size of image for OpenGL ES2 Texture.", ELL_ERROR);
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
	TextureSize=ImageSize.getOptimalSize(false);

	ColorFormat = getBestColorFormat(image->getColorFormat());

	IsCompressed = IImage::isCompressedFormat(image->getColorFormat());
}


//! copies the the texture into an open gl texture.
void COGLES2Texture::uploadTexture(bool newTexture, u32 imageNumber, bool regMipmap, void* mipmapData, u32 level)
{
	// check which image needs to be uploaded
	IImage* image = LockImage?LockImage:level?MipImage:Image[imageNumber];

	if (!image)
	{
		os::Printer::log("No image for OpenGL ES2 texture to upload", ELL_ERROR);
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

	GLenum origTextureType = GL_TEXTURE_2D;
	GLuint origTextureName = 0;

	Driver->getBridgeCalls()->setActiveTexture(0);
	Driver->getBridgeCalls()->getTexture(origTextureType, origTextureName);

	glBindTexture(TextureType, TextureName);

#ifdef _DEBUG
	if (Driver->testGLError())
		os::Printer::log("Could not bind Texture", ELL_ERROR);
#endif

	// mipmap handling for main texture
	if (!level && newTexture)
	{
		// auto generate if possible and no mipmap data is given
		if (!IsCompressed && HasMipMaps && !mipmapData && Driver->queryFeature(EVDF_MIP_MAP_AUTO_UPDATE) && regMipmap)
		{
			if (Driver->getTextureCreationFlag(ETCF_OPTIMIZED_FOR_SPEED))
				glHint(GL_GENERATE_MIPMAP_HINT, GL_FASTEST);
			else if (Driver->getTextureCreationFlag(ETCF_OPTIMIZED_FOR_QUALITY))
				glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
			else
				glHint(GL_GENERATE_MIPMAP_HINT, GL_DONT_CARE);

			AutomaticMipmapUpdate=true;
		}

		// enable bilinear filter without mipmaps
		if (filtering == GL_LINEAR)
			StatesCache.BilinearFilter = true;
		else
			StatesCache.BilinearFilter = false;

		StatesCache.TrilinearFilter = false;
		StatesCache.MipMapStatus = false;

		glTexParameteri(TextureType, GL_TEXTURE_MIN_FILTER, filtering);
		glTexParameteri(TextureType, GL_TEXTURE_MAG_FILTER, filtering);
	}

	// get texture type

	GLenum tmpTextureType = GL_TEXTURE_2D;

	if (TextureType == GL_TEXTURE_CUBE_MAP)
	{
		switch(imageNumber)
		{
		case 0:
			tmpTextureType = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
			break;
		case 1:
			tmpTextureType = GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
			break;
		case 2:
			tmpTextureType = GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
			break;
		case 3:
			tmpTextureType = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
			break;
		case 4:
			tmpTextureType = GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
			break;
		case 5:
			tmpTextureType = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
			break;
		default:
			break;
		}
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
			glCompressedTexImage2D(tmpTextureType, 0, InternalFormat, image->getDimension().Width,
				image->getDimension().Height, 0, compressedImageSize, source);
		}
		else
			glTexImage2D(tmpTextureType, level, InternalFormat, image->getDimension().Width,
				image->getDimension().Height, 0, PixelFormat, PixelType, source);
	}
	else
	{
		if (IsCompressed)
		{
			glCompressedTexSubImage2D(tmpTextureType, 0, 0, 0, image->getDimension().Width,
				image->getDimension().Height, PixelFormat, compressedImageSize, source);
		}
		else
			glTexSubImage2D(tmpTextureType, level, 0, 0, image->getDimension().Width,
				image->getDimension().Height, PixelFormat, PixelType, source);
	}

	if (convert)
	{
		tmpImage->unlock();
		tmpImage->drop();
	}
	else
		image->unlock();

	if (!level && newTexture && regMipmap)
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

			if (filtering == GL_LINEAR)
				StatesCache.BilinearFilter = true;
			else
			{
				StatesCache.BilinearFilter = false;
				filteringMipMaps = GL_NEAREST_MIPMAP_NEAREST;
			}

			StatesCache.TrilinearFilter = false;
			StatesCache.MipMapStatus = false;

			glTexParameteri(TextureType, GL_TEXTURE_MIN_FILTER, filteringMipMaps);
			glTexParameteri(TextureType, GL_TEXTURE_MAG_FILTER, filtering);
		}
	}

#ifdef _DEBUG
	if (Driver->testGLError())
		os::Printer::log("Could not glTexImage2D", ELL_ERROR);
#endif

	glBindTexture(origTextureType, origTextureName);
}


//! lock function
void* COGLES2Texture::lock(E_TEXTURE_LOCK_MODE mode, u32 mipmapLevel)
{
	if (IsCompressed || IsRenderTarget || Type != ETT_2D || mipmapLevel > 0) // TO-DO
		return 0;

	ReadOnlyLock |= (mode==ETLM_READ_ONLY);
	MipLevelStored = mipmapLevel;

	if (LockImage)
		return LockImage->lock();

	IImage* tmpImage = 0;

	if (mipmapLevel)
	{
		u32 i=0;
		u32 width = TextureSize.Width;
		u32 height = TextureSize.Height;

		do
		{
			if (width>1)
				width>>=1;
			if (height>1)
				height>>=1;

			++i;
		}
		while (i != mipmapLevel);

		LockImage = Driver->createImage(ColorFormat, core::dimension2du(width,height));
	}
	else
	{
		LockImage = Driver->createImage(ColorFormat, ImageSize);
	}

	if (!LockImage)
		return 0;

	tmpImage = Driver->createImage(ECF_A8R8G8B8, LockImage->getDimension());

	u8* pPixels = static_cast<u8*>(tmpImage->lock());

	if (!pPixels)
		return 0;

	// Prepare FBO and texture.

	const core::dimension2d<u32> imageSize = LockImage->getDimension();

	COGLES2Texture* origRT = static_cast<COGLES2Texture*>(Driver->getRenderTargetTexture());
	core::rect<s32> origViewport = Driver->getBridgeCalls()->getViewport();

	GLenum origTextureType = GL_TEXTURE_2D;
	GLuint origTextureName = 0;

	Driver->getBridgeCalls()->setActiveTexture(0);
	Driver->getBridgeCalls()->getTexture(origTextureType, origTextureName);

	GLuint tmpTexture = 0;
	glGenTextures(1, &tmpTexture);
	glBindTexture(GL_TEXTURE_2D, tmpTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, PixelFormat, ImageSize.Width, ImageSize.Height, 0, PixelFormat, PixelType, 0);
	glBindTexture(origTextureType, origTextureName);

	GLuint tmpFBO = 0;
	glGenFramebuffers(1, &tmpFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, tmpFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tmpTexture, 0);

	Driver->getBridgeCalls()->setViewport(core::rect<s32>(0, 0, imageSize.Width, imageSize.Height));

	// Draw 2D image.

	Driver->chooseMaterial2D();
	Driver->Material.TextureLayer[0].Texture = this;
	Driver->Material.TextureLayer[0].TextureWrapU = ETC_CLAMP_TO_EDGE;
	Driver->Material.TextureLayer[0].TextureWrapV = ETC_CLAMP_TO_EDGE;

	Driver->setRenderStates2DMode(false, true, false);

	const core::vector3df normal(0.f, 0.f, 1.f);
	const SColor color(255, 255, 255, 255);

	u16 indices[] = {0, 1, 2, 3};
	S3DVertex vertices[4];

	vertices[0] = S3DVertex(-1.f, 1.f, 0.f, normal.X, normal.Y, normal.Z, color, 0.f, 1.f);
	vertices[1] = S3DVertex(1.f, 1.f, 0.f, normal.X, normal.Y, normal.Z, color, 1.f, 1.f);
	vertices[2] = S3DVertex(1.f, -1.f, 0.f, normal.X, normal.Y, normal.Z, color, 1.f, 0.f);
	vertices[3] = S3DVertex(-1.f, -1.f, 0.f, normal.X, normal.Y, normal.Z, color, 0.f, 0.f);

	glEnableVertexAttribArray(EVA_POSITION);
	glEnableVertexAttribArray(EVA_COLOR);
	glEnableVertexAttribArray(EVA_TCOORD0);
	glVertexAttribPointer(EVA_POSITION, 3, GL_FLOAT, false, sizeof(S3DVertex), &(static_cast<const S3DVertex*>(vertices))[0].Pos);
	glVertexAttribPointer(EVA_COLOR, 4, GL_UNSIGNED_BYTE, true, sizeof(S3DVertex), &(static_cast<const S3DVertex*>(vertices))[0].Color);
	glVertexAttribPointer(EVA_TCOORD0, 2, GL_FLOAT, false, sizeof(S3DVertex), &(static_cast<const S3DVertex*>(vertices))[0].TCoords);
	glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_SHORT, indices);
	glDisableVertexAttribArray(EVA_TCOORD0);
	glDisableVertexAttribArray(EVA_COLOR);
	glDisableVertexAttribArray(EVA_POSITION);

	// Download data from FBO.

	glReadPixels(0, 0, imageSize.Width, imageSize.Height, GL_RGBA, GL_UNSIGNED_BYTE, pPixels);

#ifdef _DEBUG
	if (Driver->testGLError())
		os::Printer::log("Could not read pixels", ELL_ERROR);
#endif

	// Clean resources.

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &tmpFBO);
	glDeleteTextures(1, &tmpTexture);

	Driver->getBridgeCalls()->setViewport(origViewport);

	if (origRT)
		origRT->bindRTT();

	tmpImage->unlock();

	// convert from RGBA8.

	void* src = tmpImage->lock();
	void* dest = LockImage->lock();

	bool passed = true;

	switch(ColorFormat)
	{
		case ECF_A1R5G5B5:
			CColorConverter::convert_A8R8G8B8toA1B5G5R5(src, tmpImage->getDimension().getArea(), dest);
			break;
		case ECF_R5G6B5:
			CColorConverter::convert_A8R8G8B8toR5G6B5(src, tmpImage->getDimension().getArea(), dest);
			break;
		case ECF_R8G8B8:
			CColorConverter::convert_A8R8G8B8toB8G8R8(src, tmpImage->getDimension().getArea(), dest);
			break;
		case ECF_A8R8G8B8:
			CColorConverter::convert_A8R8G8B8toA8B8G8R8(src, tmpImage->getDimension().getArea(), dest);
			break;
		default:
			passed = false;
			break;
	}

	LockImage->unlock();
	tmpImage->unlock();

	tmpImage->drop();

	if (!passed)
	{
		LockImage->drop();
		LockImage = 0;
	}

	return LockImage ? LockImage->lock() : 0;
}


//! unlock function
void COGLES2Texture::unlock()
{
	if (!LockImage)
		return;

	LockImage->unlock();

	if (!ReadOnlyLock)
		uploadTexture(false, 0, true, 0, MipLevelStored);

	ReadOnlyLock = false;

	if ( !KeepImage )
	{
		LockImage->drop();
		LockImage = 0;
	}
}


//! Returns size of the original image.
const core::dimension2d<u32>& COGLES2Texture::getOriginalSize() const
{
	return ImageSize;
}


//! Returns size of the texture.
const core::dimension2d<u32>& COGLES2Texture::getSize() const
{
	return TextureSize;
}


//! returns driver type of texture, i.e. the driver, which created the texture
E_DRIVER_TYPE COGLES2Texture::getDriverType() const
{
	return EDT_OGLES2;
}


//! returns color format of texture
ECOLOR_FORMAT COGLES2Texture::getColorFormat() const
{
	return ColorFormat;
}


//! returns pitch of texture (in bytes)
u32 COGLES2Texture::getPitch() const
{
	return Pitch;
}


//! return open gl texture name
GLuint COGLES2Texture::getOpenGLTextureName() const
{
	return TextureName;
}


//! return open gl texture type
GLenum COGLES2Texture::getOpenGLTextureType() const
{
	return TextureType;
}


//! Returns whether this texture has mipmaps
bool COGLES2Texture::hasMipMaps() const
{
	return HasMipMaps;
}


//! Regenerates the mip map levels of the texture. Useful after locking and
//! modifying the texture
void COGLES2Texture::regenerateMipMapLevels(void* mipmapData)
{
	// texture require mipmaps?
	if (!HasMipMaps)
		return;

	// we don't use custom data for mipmaps.
	if (!mipmapData)
	{
		// compressed textures require custom data for prepare mipmaps.
		if (IsCompressed)
			return;

		// hardware doesn't support generate mipmaps for certain texture but image data doesn't exist or is wrong.
		if (!AutomaticMipmapUpdate && (Image.size() == 0 || (Image.size() > 0 && ((Image[0]->getDimension().Width==1) && (Image[0]->getDimension().Height==1)))))
			return;
	}

	// hardware moethods for generate mipmaps.
	if (!mipmapData && AutomaticMipmapUpdate)
	{
		GLenum origTextureType = GL_TEXTURE_2D;
		GLuint origTextureName = 0;

		Driver->getBridgeCalls()->setActiveTexture(0);
		Driver->getBridgeCalls()->getTexture(origTextureType, origTextureName);

		glBindTexture(TextureType, TextureName);
		glGenerateMipmap(TextureType);

		glBindTexture(origTextureType, origTextureName);

		return;
	}

	// only 2D textures are supported in manual creation mipmaps process.
	if (Type != ETT_2D)
		return;

	GLenum origTextureType = GL_TEXTURE_2D;
	GLuint origTextureName = 0;

	Driver->getBridgeCalls()->setActiveTexture(0);
	Driver->getBridgeCalls()->getTexture(origTextureType, origTextureName);

	glBindTexture(TextureType, TextureName);

 	// Manually create mipmaps or use prepared version
	u32 compressedImageSize = 0;
	u32 width=Image[0]->getDimension().Width;
	u32 height=Image[0]->getDimension().Height;
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
			target = new u8[width*height*Image[0]->getBytesPerPixel()];

		// create scaled version if no mipdata available
		if (!mipmapData)
			Image[0]->copyToScaling(target, width, height, Image[0]->getColorFormat());

		if (IsCompressed)
		{
			compressedImageSize = IImage::getCompressedImageSize(ColorFormat, width, height);

			glCompressedTexImage2D(TextureType, i, InternalFormat, width,
				height, 0, compressedImageSize, target);
		}
		else
			glTexImage2D(TextureType, i, InternalFormat, width, height,
					0, PixelFormat, PixelType, target);

		// get next prepared mipmap data if available
		if (mipmapData)
		{
			if (IsCompressed)
				mipmapData = static_cast<u8*>(mipmapData)+compressedImageSize;
			else
				mipmapData = static_cast<u8*>(mipmapData)+width*height*Image[0]->getBytesPerPixel();

			target = static_cast<u8*>(mipmapData);
		}
	}
	while (width!=1 || height!=1);
	// cleanup
	if (!mipmapData)
		delete [] target;

	glBindTexture(origTextureType, origTextureName);
}


bool COGLES2Texture::isRenderTarget() const
{
	return IsRenderTarget;
}


void COGLES2Texture::setIsRenderTarget(bool isTarget)
{
	IsRenderTarget = isTarget;
}


bool COGLES2Texture::isFrameBufferObject() const
{
	return false;
}


//! Bind Render Target Texture
void COGLES2Texture::bindRTT()
{
}


//! Unbind Render Target Texture
void COGLES2Texture::unbindRTT()
{
}


//! Get an access to texture states cache.
COGLES2Texture::SStatesCache& COGLES2Texture::getStatesCache() const
{
	return StatesCache;
}


/* FBO Textures */

// helper function for render to texture
static bool checkOGLES2FBOStatus(COGLES2Driver* Driver);

//! RTT ColorFrameBuffer constructor
COGLES2FBOTexture::COGLES2FBOTexture(const core::dimension2d<u32>& size,
					const io::path& name, COGLES2Driver* driver,
					ECOLOR_FORMAT format)
	: COGLES2Texture(name, driver), DepthTexture(0), ColorFrameBuffer(0)
{
	#ifdef _DEBUG
	setDebugName("COGLES2Texture_FBO");
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

	// generate frame buffer
	glGenFramebuffers(1, &ColorFrameBuffer);
	bindRTT();

	// generate color texture
	glGenTextures(1, &TextureName);

    GLenum origTextureType = GL_TEXTURE_2D;
	GLuint origTextureName = 0;

	Driver->getBridgeCalls()->setActiveTexture(0);
	Driver->getBridgeCalls()->getTexture(origTextureType, origTextureName);

	glBindTexture(TextureType, TextureName);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	StatesCache.BilinearFilter = true;
    StatesCache.WrapU = ETC_CLAMP_TO_EDGE;
    StatesCache.WrapV = ETC_CLAMP_TO_EDGE;

	glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, ImageSize.Width, ImageSize.Height, 0, PixelFormat, PixelType, 0);

#ifdef _DEBUG
	Driver->testGLError();
#endif

	// attach color texture to frame buffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, TextureName, 0);
#ifdef _DEBUG
	checkOGLES2FBOStatus(Driver);
#endif

	glBindTexture(origTextureType, origTextureName);

	unbindRTT();
}


//! destructor
COGLES2FBOTexture::~COGLES2FBOTexture()
{
	if (DepthTexture)
		if (DepthTexture->drop())
			Driver->removeDepthTexture(DepthTexture);
	if (ColorFrameBuffer)
		glDeleteFramebuffers(1, &ColorFrameBuffer);
}


bool COGLES2FBOTexture::isFrameBufferObject() const
{
	return true;
}


//! Bind Render Target Texture
void COGLES2FBOTexture::bindRTT()
{
	if (ColorFrameBuffer != 0)
		glBindFramebuffer(GL_FRAMEBUFFER, ColorFrameBuffer);
}


//! Unbind Render Target Texture
void COGLES2FBOTexture::unbindRTT()
{
	if (ColorFrameBuffer != 0)
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


/* FBO Depth Textures */

//! RTT DepthBuffer constructor
COGLES2FBODepthTexture::COGLES2FBODepthTexture(
		const core::dimension2d<u32>& size,
		const io::path& name,
		COGLES2Driver* driver,
		bool useStencil)
	: COGLES2Texture(name, driver), DepthRenderBuffer(0),
	StencilRenderBuffer(0), UseStencil(useStencil)
{
#ifdef _DEBUG
	setDebugName("COGLES2TextureFBO_Depth");
#endif

	ImageSize = size;
	TextureSize = size;
	InternalFormat = GL_RGBA;
	PixelFormat = GL_RGBA;
	PixelType = GL_UNSIGNED_BYTE;
	HasMipMaps = false;

	if (useStencil)
	{
		glGenRenderbuffers(1, &DepthRenderBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, DepthRenderBuffer);
#ifdef GL_OES_packed_depth_stencil
		if (Driver->queryOpenGLFeature(COGLES2ExtensionHandler::IRR_OES_packed_depth_stencil))
		{
			// generate packed depth stencil buffer
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8_OES, ImageSize.Width, ImageSize.Height);
			StencilRenderBuffer = DepthRenderBuffer; // stencil is packed with depth
		}
		else // generate separate stencil and depth textures
#endif
		{
			glRenderbufferStorage(GL_RENDERBUFFER, Driver->getZBufferBits(), ImageSize.Width, ImageSize.Height);

			glGenRenderbuffers(1, &StencilRenderBuffer);
			glBindRenderbuffer(GL_RENDERBUFFER, StencilRenderBuffer);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8, ImageSize.Width, ImageSize.Height);
		}
	}
	else
	{
		// generate depth buffer
		glGenRenderbuffers(1, &DepthRenderBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, DepthRenderBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, Driver->getZBufferBits(), ImageSize.Width, ImageSize.Height);
	}
}


//! destructor
COGLES2FBODepthTexture::~COGLES2FBODepthTexture()
{
	if (DepthRenderBuffer)
		glDeleteRenderbuffers(1, &DepthRenderBuffer);

	if (StencilRenderBuffer && StencilRenderBuffer != DepthRenderBuffer)
		glDeleteRenderbuffers(1, &StencilRenderBuffer);
}


//combine depth texture and rtt
bool COGLES2FBODepthTexture::attach(ITexture* renderTex)
{
	if (!renderTex)
		return false;
	COGLES2FBOTexture* rtt = static_cast<COGLES2FBOTexture*>(renderTex);
	rtt->bindRTT();

	// attach stencil texture to stencil buffer
	if (UseStencil)
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, StencilRenderBuffer);

	// attach depth renderbuffer to depth buffer
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, DepthRenderBuffer);

	// check the status
	if (!checkOGLES2FBOStatus(Driver))
	{
		os::Printer::log("FBO incomplete");
		return false;
	}
	rtt->DepthTexture=this;
	grab(); // grab the depth buffer, not the RTT
	rtt->unbindRTT();
	return true;
}


//! Bind Render Target Texture
void COGLES2FBODepthTexture::bindRTT()
{
}


//! Unbind Render Target Texture
void COGLES2FBODepthTexture::unbindRTT()
{
}


bool checkOGLES2FBOStatus(COGLES2Driver* Driver)
{
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	switch (status)
	{
		case GL_FRAMEBUFFER_COMPLETE:
			return true;

		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
			os::Printer::log("FBO has one or several incomplete image attachments", ELL_ERROR);
			break;

		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
			os::Printer::log("FBO missing an image attachment", ELL_ERROR);
			break;

		case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
			os::Printer::log("FBO has one or several image attachments with different dimensions", ELL_ERROR);
			break;

		case GL_FRAMEBUFFER_UNSUPPORTED:
			os::Printer::log("FBO format unsupported", ELL_ERROR);
			break;

		default:
			break;
	}

	os::Printer::log("FBO error", ELL_ERROR);

	return false;
}


} // end namespace video
} // end namespace irr

#endif // _IRR_COMPILE_WITH_OGLES2_

