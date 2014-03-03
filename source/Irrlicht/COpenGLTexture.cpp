// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "IrrCompileConfig.h"

#ifdef _IRR_COMPILE_WITH_OPENGL_

#include "irrTypes.h"
#include "COpenGLTexture.h"
#include "COpenGLDriver.h"
#include "os.h"
#include "CColorConverter.h"

#include "irrString.h"

namespace irr
{
namespace video
{

//! constructor for usual textures
COpenGLTexture::COpenGLTexture(IImage* origImage, const io::path& name, void* mipmapData, COpenGLDriver* driver)
	: ITexture(name, ETT_2D), ColorFormat(ECF_A8R8G8B8), Driver(driver), Image(0), MipImage(0),
	TextureName(0), TextureType(GL_TEXTURE_2D), InternalFormat(GL_RGBA), PixelFormat(GL_BGRA_EXT),
	PixelType(GL_UNSIGNED_BYTE), MipLevelStored(0), MipmapLegacyMode(true),
	IsRenderTarget(false), IsCompressed(false), AutomaticMipmapUpdate(false),
	ReadOnlyLock(false), KeepImage(true)
{
	#ifdef _DEBUG
	setDebugName("COpenGLTexture");
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

	glGenTextures(1, &TextureName);
	uploadTexture(true, 0, true, mipmapData);

	if (!KeepImage)
	{
		Image[0]->drop();

		Image.clear();
	}
}


//! constructor for cube textures
COpenGLTexture::COpenGLTexture(const io::path& name, IImage* posXImage, IImage* negXImage, IImage* posYImage,
	IImage* negYImage, IImage* posZImage, IImage* negZImage, COpenGLDriver* driver)
		: ITexture(name, ETT_CUBE), ColorFormat(ECF_A8R8G8B8), Driver(driver), Image(0), MipImage(0),
		TextureName(0), TextureType(GL_TEXTURE_CUBE_MAP), InternalFormat(GL_RGBA), PixelFormat(GL_BGRA_EXT),
		PixelType(GL_UNSIGNED_BYTE), MipLevelStored(0), MipmapLegacyMode(true),
		IsRenderTarget(false), IsCompressed(false), AutomaticMipmapUpdate(false),
		ReadOnlyLock(false), KeepImage(true)
{
	#ifdef _DEBUG
	setDebugName("COpenGLTexture");
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

		KeepImage = false;
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

	glGenTextures(1, &TextureName);

	for (u32 i = 0; i < 5; ++i)
		uploadTexture(true, i, false);

	uploadTexture(true, 5, true);

	if (!KeepImage)
	{
		for (u32 i = 0; i < Image.size(); ++i)
			Image[i]->drop();

		Image.clear();
	}
}


//! constructor for basic setup (only for derived classes)
COpenGLTexture::COpenGLTexture(const io::path& name, COpenGLDriver* driver)
	: ITexture(name, ETT_2D), ColorFormat(ECF_A8R8G8B8), Driver(driver), Image(0), MipImage(0),
	TextureName(0), TextureType(GL_TEXTURE_2D), InternalFormat(GL_RGBA), PixelFormat(GL_BGRA_EXT),
	PixelType(GL_UNSIGNED_BYTE), MipLevelStored(0), HasMipMaps(true),
	MipmapLegacyMode(true), IsRenderTarget(false), IsCompressed(false),
	AutomaticMipmapUpdate(false), ReadOnlyLock(false), KeepImage(true)
{
	#ifdef _DEBUG
	setDebugName("COpenGLTexture");
	#endif
}


//! destructor
COpenGLTexture::~COpenGLTexture()
{
	for (u32 i = 0; i < Driver->MaxSupportedTextures; ++i)
		if (Driver->CurrentTexture[i] == this)
		{
			Driver->setActiveTexture(i, 0);
			Driver->getBridgeCalls()->setTexture(i, TextureType, true);
		}

	// Remove this texture from active materials as well

	for (u32 i = 0; i < MATERIAL_MAX_TEXTURES; ++i)
	{
		if (Driver->Material.TextureLayer[i].Texture == this)
			Driver->Material.TextureLayer[i].Texture = 0;

		if (Driver->LastMaterial.TextureLayer[i].Texture == this)
			Driver->LastMaterial.TextureLayer[i].Texture = 0;
	}

	if (TextureName)
		glDeleteTextures(1, &TextureName);
	for (u32 i = 0; i < Image.size(); ++i)
		Image[i]->drop();
}


//! Choose best matching color format, based on texture creation flags
ECOLOR_FORMAT COpenGLTexture::getBestColorFormat(ECOLOR_FORMAT format)
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
void COpenGLTexture::getFormatParameters(ECOLOR_FORMAT format, GLint& internalFormat, GLint& filtering,
	GLenum& pixelFormat, GLenum& type)
{
	switch(format)
	{
		case ECF_A1R5G5B5:
			internalFormat = GL_RGBA;
			filtering = GL_LINEAR;
			pixelFormat = GL_BGRA_EXT;
			type = GL_UNSIGNED_SHORT_1_5_5_5_REV;
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
			pixelFormat = GL_BGR;
			type = GL_UNSIGNED_BYTE;
			break;
		case ECF_A8R8G8B8:
			internalFormat = GL_RGBA;
			filtering = GL_LINEAR;
			pixelFormat = GL_BGRA_EXT;
			if (Driver->Version > 101)
				type = GL_UNSIGNED_INT_8_8_8_8_REV;
			else
				type = GL_UNSIGNED_BYTE;
			break;
#ifdef GL_EXT_texture_compression_s3tc
		case ECF_DXT1:
			internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
			filtering = GL_LINEAR;
			pixelFormat = GL_BGRA_EXT;
			type = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
			break;
#endif
#ifdef GL_EXT_texture_compression_s3tc
		case ECF_DXT2:
		case ECF_DXT3:
			internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
			filtering = GL_LINEAR;
			pixelFormat = GL_BGRA_EXT;
			type = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
			break;
#endif
#ifdef GL_EXT_texture_compression_s3tc
		case ECF_DXT4:
		case ECF_DXT5:
			internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
			filtering = GL_LINEAR;
			pixelFormat = GL_BGRA_EXT;
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
#ifdef GL_ARB_ES3_compatibility
		case ECF_ETC2_RGB:
			internalFormat = GL_COMPRESSED_RGB8_ETC2;
			filtering = GL_LINEAR;
			pixelFormat = GL_RGB;
			type = GL_COMPRESSED_RGB8_ETC2;
			break;
#endif
#ifdef GL_ARB_ES3_compatibility
		case ECF_ETC2_ARGB:
			internalFormat = GL_COMPRESSED_RGBA8_ETC2_EAC;
			filtering = GL_LINEAR;
			pixelFormat = GL_RGBA;
			type = GL_COMPRESSED_RGBA8_ETC2_EAC;
			break;
#endif
#ifdef GL_ARB_texture_rg
		case ECF_R16F:
			internalFormat = GL_R16F;
			filtering = GL_NEAREST;
			pixelFormat = GL_RED;
			type = GL_FLOAT;
			break;
		case ECF_G16R16F:
			internalFormat = GL_RG16F;
			filtering = GL_NEAREST;
			pixelFormat = GL_RG;
			type = GL_FLOAT;
			break;
		case ECF_R32F:
			internalFormat = GL_R32F;
			filtering = GL_NEAREST;
			pixelFormat = GL_RED;
			type = GL_FLOAT;
			break;
		case ECF_G32R32F:
			internalFormat = GL_RG32F;
			filtering = GL_NEAREST;
			pixelFormat = GL_RG;
			type = GL_FLOAT;
			break;
#endif
#ifdef GL_ARB_texture_float
		case ECF_A16B16G16R16F:
			internalFormat = GL_RGBA16F_ARB;
			filtering = GL_NEAREST;
			pixelFormat = GL_RGBA;
			type = GL_FLOAT;
			break;
		case ECF_A32B32G32R32F:
			internalFormat = GL_RGBA32F_ARB;
			filtering = GL_NEAREST;
			pixelFormat = GL_RGBA;
			type = GL_FLOAT;
			break;
#endif
		default:
			os::Printer::log("Unsupported texture format", ELL_ERROR);
			break;
	}

#if defined(GL_ARB_framebuffer_sRGB) || defined(GL_EXT_framebuffer_sRGB)
	if (Driver->Params.HandleSRGB)
	{
		if (internalFormat == GL_RGBA)
			internalFormat = GL_SRGB_ALPHA_EXT;
		else if (internalFormat == GL_RGB)
			internalFormat = GL_SRGB_EXT;
#ifdef GL_ARB_ES3_compatibility
		else if (internalFormat == GL_COMPRESSED_RGBA8_ETC2_EAC)
			internalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC;
		else if (internalFormat == GL_COMPRESSED_RGB8_ETC2)
			internalFormat = GL_COMPRESSED_SRGB8_ETC2;
#endif
	}
#endif
}


// prepare values ImageSize, TextureSize, and ColorFormat based on image
void COpenGLTexture::getImageValues(IImage* image)
{
	if (!image)
	{
		os::Printer::log("No image for OpenGL texture.", ELL_ERROR);
		return;
	}

	ImageSize = image->getDimension();

	if ( !ImageSize.Width || !ImageSize.Height)
	{
		os::Printer::log("Invalid size of image for OpenGL Texture.", ELL_ERROR);
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
void COpenGLTexture::uploadTexture(bool newTexture, u32 imageNumber, bool regMipmap, void* mipmapData, u32 level)
{
	// check which image needs to be uploaded
	IImage* image = level?MipImage:Image[imageNumber];
	if (!image)
	{
		os::Printer::log("No image for OpenGL texture to upload", ELL_ERROR);
		return;
	}

	// get correct opengl color data values
	GLint oldInternalFormat = InternalFormat;
	GLint filtering = GL_LINEAR;
	getFormatParameters(ColorFormat, InternalFormat, filtering, PixelFormat, PixelType);

	// make sure we don't change the internal format of existing images
	if (!newTexture)
		InternalFormat = oldInternalFormat;

    Driver->setActiveTexture(0, this);
	Driver->getBridgeCalls()->setTexture(0, TextureType, true);

	if (Driver->testGLError())
	{
		os::Printer::log("Could not bind Texture", getName(), ELL_ERROR);
	}

	// mipmap handling for main texture
	if (!level && newTexture)
	{
		// auto generate if possible and no mipmap data is given
		if (!IsCompressed && HasMipMaps && !mipmapData && Driver->queryFeature(EVDF_MIP_MAP_AUTO_UPDATE) && regMipmap)
		{
			if (!Driver->queryFeature(EVDF_FRAMEBUFFER_OBJECT))
			{
#ifdef GL_SGIS_generate_mipmap
				if (Driver->getTextureCreationFlag(ETCF_OPTIMIZED_FOR_SPEED))
					glHint(GL_GENERATE_MIPMAP_HINT_SGIS, GL_FASTEST);
				else if (Driver->getTextureCreationFlag(ETCF_OPTIMIZED_FOR_QUALITY))
					glHint(GL_GENERATE_MIPMAP_HINT_SGIS, GL_NICEST);
				else
					glHint(GL_GENERATE_MIPMAP_HINT_SGIS, GL_DONT_CARE);

				glTexParameteri(TextureType, GL_GENERATE_MIPMAP, GL_TRUE );
				MipmapLegacyMode=true;
				AutomaticMipmapUpdate=true;
#endif
			}
			else
			{
				if (Driver->getTextureCreationFlag(ETCF_OPTIMIZED_FOR_SPEED))
					glHint(GL_GENERATE_MIPMAP_HINT, GL_FASTEST);
				else if (Driver->getTextureCreationFlag(ETCF_OPTIMIZED_FOR_QUALITY))
					glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
				else
					glHint(GL_GENERATE_MIPMAP_HINT, GL_DONT_CARE);

				MipmapLegacyMode=false;
				AutomaticMipmapUpdate=true;
			}
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
	if (newTexture)
	{
		if (IsCompressed)
		{
			Driver->extGlCompressedTexImage2D(tmpTextureType, 0, InternalFormat, image->getDimension().Width,
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
			Driver->extGlCompressedTexSubImage2D(tmpTextureType, 0, 0, 0, image->getDimension().Width,
				image->getDimension().Height, PixelFormat, compressedImageSize, source);
		}
		else
			glTexSubImage2D(tmpTextureType, level, 0, 0, image->getDimension().Width,
				image->getDimension().Height, PixelFormat, PixelType, source);
	}
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

	if (Driver->testGLError())
		os::Printer::log("Could not glTexImage2D", ELL_ERROR);

    Driver->setActiveTexture(0, 0);
	Driver->getBridgeCalls()->setTexture(0, TextureType, true);
}


//! lock function
void* COpenGLTexture::lock(E_TEXTURE_LOCK_MODE mode, u32 mipmapLevel)
{
	if (IsCompressed || Type != ETT_2D) // TO-DO
		return 0;

	// store info about which image is locked
	IImage* image = 0;

	if (mipmapLevel==0)
	{
		if (Image.size() > 0)
			image = Image[0];
	}
	else
	{
		image = MipImage;
	}

	ReadOnlyLock |= (mode==ETLM_READ_ONLY);
	MipLevelStored = mipmapLevel;
	if (!ReadOnlyLock && mipmapLevel)
	{
#ifdef GL_SGIS_generate_mipmap
		if (Driver->queryFeature(EVDF_MIP_MAP_AUTO_UPDATE))
		{
			// do not automatically generate and update mipmaps
			glTexParameteri(TextureType, GL_GENERATE_MIPMAP, GL_FALSE);
		}
#endif
		AutomaticMipmapUpdate=false;
	}

	// if data not available or might have changed on GPU download it
	if (!image || IsRenderTarget)
	{
		// prepare the data storage if necessary
		if (!image)
		{
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
				MipImage = image = Driver->createImage(ECF_A8R8G8B8, core::dimension2du(width,height));
			}
			else
			{
				image = Driver->createImage(ECF_A8R8G8B8, ImageSize);

				if (Image.size() == 0)
					Image.push_back(image);
				else
					Image[0] = image;
			}
			ColorFormat = ECF_A8R8G8B8;
		}
		if (!image)
			return 0;

		if (mode != ETLM_WRITE_ONLY)
		{
			u8* pixels = static_cast<u8*>(image->lock());
			if (!pixels)
				return 0;

			// we need to keep the correct texture bound later on
			const COpenGLTexture* tmpTexture = static_cast<const COpenGLTexture*>(Driver->CurrentTexture[0]);
			GLuint tmpTextureType = GL_TEXTURE_2D;
			GLint tmpTextureName = (tmpTexture) ? tmpTexture->getOpenGLTextureName() : 0;
			bool tmpFixedPipeline = false;
			Driver->getBridgeCalls()->getTexture(0, tmpTextureType, tmpFixedPipeline);
			glBindTexture(TextureType, TextureName);

			// we need to flip textures vertical
			// however, it seems that this does not hold for mipmap
			// textures, for unknown reasons.

			// allows to read pixels in top-to-bottom order
#ifdef GL_MESA_pack_invert
			if (!mipmapLevel && Driver->queryOpenGLFeature(COpenGLExtensionHandler::IRR_MESA_pack_invert))
				glPixelStorei(GL_PACK_INVERT_MESA, GL_TRUE);
#endif

			// download GPU data as ARGB8 to pixels;
			glGetTexImage(TextureType, mipmapLevel, GL_BGRA_EXT, GL_UNSIGNED_BYTE, pixels);

			if (!mipmapLevel)
			{
#ifdef GL_MESA_pack_invert
				if (Driver->queryOpenGLFeature(COpenGLExtensionHandler::IRR_MESA_pack_invert))
					glPixelStorei(GL_PACK_INVERT_MESA, GL_FALSE);
				else
#endif
				{
					// opengl images are horizontally flipped, so we have to fix that here.
					const s32 pitch=image->getPitch();
					u8* p2 = pixels + (image->getDimension().Height - 1) * pitch;
					u8* tmpBuffer = new u8[pitch];
					for (u32 i=0; i < image->getDimension().Height; i += 2)
					{
						memcpy(tmpBuffer, pixels, pitch);
						memcpy(pixels, p2, pitch);
						memcpy(p2, tmpBuffer, pitch);
						pixels += pitch;
						p2 -= pitch;
					}
					delete [] tmpBuffer;
				}
			}
			image->unlock();

			//reset old bound texture
			glBindTexture(tmpTextureType, tmpTextureName);
		}
	}
	return image->lock();
}


//! unlock function
void COpenGLTexture::unlock()
{
	if (IsCompressed || Type != ETT_2D) // TO-DO
		return;

	// test if miplevel or main texture was locked
	IImage* image = 0;

	if (!MipImage)
	{
		if (Image.size() > 0)
			image = Image[0];
	}
	else
	{
		image = MipImage;
	}

	if (!image)
		return;
	// unlock image to see changes
	image->unlock();
	// copy texture data to GPU
	if (!ReadOnlyLock)
		uploadTexture(false, 0, true, 0, MipLevelStored);
	ReadOnlyLock = false;
	// cleanup local image
	if (MipImage)
	{
		MipImage->drop();
		MipImage=0;
	}
	else if (!KeepImage)
	{
		Image[0]->drop();
		Image.clear();
	}
	// update information
	if (Image.size() > 0)
		ColorFormat=Image[0]->getColorFormat();
	else
		ColorFormat=ECF_A8R8G8B8;
}


//! Returns size of the original image.
const core::dimension2d<u32>& COpenGLTexture::getOriginalSize() const
{
	return ImageSize;
}


//! Returns size of the texture.
const core::dimension2d<u32>& COpenGLTexture::getSize() const
{
	return TextureSize;
}


//! returns driver type of texture, i.e. the driver, which created the texture
E_DRIVER_TYPE COpenGLTexture::getDriverType() const
{
	return EDT_OPENGL;
}


//! returns color format of texture
ECOLOR_FORMAT COpenGLTexture::getColorFormat() const
{
	return ColorFormat;
}


//! returns pitch of texture (in bytes)
u32 COpenGLTexture::getPitch() const
{
	if (Image.size() > 0)
		return Image[0]->getPitch();
	else
		return 0;
}


//! return open gl texture name
GLuint COpenGLTexture::getOpenGLTextureName() const
{
	return TextureName;
}


//! return open gl texture type
GLenum COpenGLTexture::getOpenGLTextureType() const
{
	return TextureType;
}


//! Returns whether this texture has mipmaps
bool COpenGLTexture::hasMipMaps() const
{
	return HasMipMaps;
}


//! Regenerates the mip map levels of the texture. Useful after locking and
//! modifying the texture
void COpenGLTexture::regenerateMipMapLevels(void* mipmapData)
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

		// texture use legacy method for generate mipmaps?
		if (AutomaticMipmapUpdate && MipmapLegacyMode)
			return;

		// hardware doesn't support generate mipmaps for certain texture but image data doesn't exist or is wrong.
		if (!AutomaticMipmapUpdate && (Image.size() == 0 || (Image.size() > 0 && ((Image[0]->getDimension().Width==1) && (Image[0]->getDimension().Height==1)))))
			return;
	}

	// hardware moethods for generate mipmaps.
	if (!mipmapData && AutomaticMipmapUpdate && !MipmapLegacyMode)
	{
		glEnable(TextureType);

		const COpenGLTexture* tmpTexture = static_cast<const COpenGLTexture*>(Driver->CurrentTexture[0]);
		GLuint tmpTextureType = GL_TEXTURE_2D;
		GLint tmpTextureName = (tmpTexture) ? tmpTexture->getOpenGLTextureName() : 0;
		bool tmpFixedPipeline = false;
		Driver->getBridgeCalls()->getTexture(0, tmpTextureType, tmpFixedPipeline);
		glBindTexture(TextureType, TextureName);
		Driver->extGlGenerateMipmap(TextureType);
		glBindTexture(tmpTextureType, tmpTextureName);

		return;
	}

	// only 2D textures are supported in manual creation mipmaps process.
	if (Type != ETT_2D)
		return;

	const COpenGLTexture* tmpTexture = static_cast<const COpenGLTexture*>(Driver->CurrentTexture[0]);
	GLuint tmpTextureType = GL_TEXTURE_2D;
	GLint tmpTextureName = (tmpTexture) ? tmpTexture->getOpenGLTextureName() : 0;
	bool tmpFixedPipeline = false;
	Driver->getBridgeCalls()->getTexture(0, tmpTextureType, tmpFixedPipeline);
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

			Driver->extGlCompressedTexImage2D(TextureType, i, InternalFormat, width,
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

	glBindTexture(tmpTextureType, tmpTextureName);
}


bool COpenGLTexture::isRenderTarget() const
{
	return IsRenderTarget;
}


void COpenGLTexture::setIsRenderTarget(bool isTarget)
{
	IsRenderTarget = isTarget;
}


bool COpenGLTexture::isFrameBufferObject() const
{
	return false;
}


//! Bind Render Target Texture
void COpenGLTexture::bindRTT()
{
}


//! Unbind Render Target Texture
void COpenGLTexture::unbindRTT()
{
	Driver->setActiveTexture(0, this);
	Driver->getBridgeCalls()->setTexture(0, TextureType, true);

	// Copy Our ViewPort To The Texture
	glCopyTexSubImage2D(TextureType, 0, 0, 0, 0, 0, getSize().Width, getSize().Height);

	Driver->setActiveTexture(0, 0);
	Driver->getBridgeCalls()->setTexture(0, TextureType, true);
}


//! Get an access to texture states cache.
COpenGLTexture::SStatesCache& COpenGLTexture::getStatesCache() const
{
	return StatesCache;
}


/* FBO Textures */

// helper function for render to texture
static bool checkFBOStatus(COpenGLDriver* Driver);

//! RTT ColorFrameBuffer constructor
COpenGLFBOTexture::COpenGLFBOTexture(const core::dimension2d<u32>& size,
					const io::path& name, COpenGLDriver* driver,
					ECOLOR_FORMAT format)
	: COpenGLTexture(name, driver), DepthTexture(0), ColorFrameBuffer(0)
{
	#ifdef _DEBUG
	setDebugName("COpenGLTexture_FBO");
	#endif

	ImageSize = size;
	TextureSize = size;

	if (ECF_UNKNOWN == format)
		format = getBestColorFormat(driver->getColorFormat());

	ColorFormat = format;

	GLint filtering = GL_LINEAR;
	getFormatParameters(format, InternalFormat, filtering, PixelFormat, PixelType);

	HasMipMaps = false;
	IsRenderTarget = true;

#ifdef GL_EXT_framebuffer_object
	// generate frame buffer
	Driver->extGlGenFramebuffers(1, &ColorFrameBuffer);
	bindRTT();

	// generate color texture
	glGenTextures(1, &TextureName);

    Driver->setActiveTexture(0, this);
	Driver->getBridgeCalls()->setTexture(0, TextureType, true);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filtering);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    if(filtering == GL_NEAREST)
        StatesCache.BilinearFilter = false;
    else
        StatesCache.BilinearFilter = true;

    StatesCache.WrapU = ETC_CLAMP_TO_EDGE;
    StatesCache.WrapV = ETC_CLAMP_TO_EDGE;

	glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, ImageSize.Width,
		ImageSize.Height, 0, PixelFormat, PixelType, 0);
#ifdef _DEBUG
	driver->testGLError();
#endif

	// attach color texture to frame buffer
	Driver->extGlFramebufferTexture2D(GL_FRAMEBUFFER_EXT,
						GL_COLOR_ATTACHMENT0_EXT,
						GL_TEXTURE_2D,
						TextureName,
						0);
#ifdef _DEBUG
	checkFBOStatus(Driver);
#endif

#endif
	unbindRTT();

    Driver->setActiveTexture(0, 0);
	Driver->getBridgeCalls()->setTexture(0, TextureType, true);
}


//! destructor
COpenGLFBOTexture::~COpenGLFBOTexture()
{
	if (DepthTexture)
		if (DepthTexture->drop())
			Driver->removeDepthTexture(DepthTexture);
	if (ColorFrameBuffer)
		Driver->extGlDeleteFramebuffers(1, &ColorFrameBuffer);
}


bool COpenGLFBOTexture::isFrameBufferObject() const
{
	return true;
}


//! Bind Render Target Texture
void COpenGLFBOTexture::bindRTT()
{
#ifdef GL_EXT_framebuffer_object
	if (ColorFrameBuffer != 0)
		Driver->extGlBindFramebuffer(GL_FRAMEBUFFER_EXT, ColorFrameBuffer);
	glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
#endif
}


//! Unbind Render Target Texture
void COpenGLFBOTexture::unbindRTT()
{
#ifdef GL_EXT_framebuffer_object
	if (ColorFrameBuffer != 0)
		Driver->extGlBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);
#endif
}


/* FBO Depth Textures */

//! RTT DepthBuffer constructor
COpenGLFBODepthTexture::COpenGLFBODepthTexture(
		const core::dimension2d<u32>& size,
		const io::path& name,
		COpenGLDriver* driver,
		bool useStencil)
	: COpenGLTexture(name, driver), DepthRenderBuffer(0),
	StencilRenderBuffer(0), UseStencil(useStencil)
{
#ifdef _DEBUG
	setDebugName("COpenGLTextureFBO_Depth");
#endif

	ImageSize = size;
	TextureSize = size;
	InternalFormat = GL_RGBA;
	PixelFormat = GL_RGBA;
	PixelType = GL_UNSIGNED_BYTE;
	HasMipMaps = false;

	if (useStencil)
	{
		glGenTextures(1, &DepthRenderBuffer);
		glBindTexture(GL_TEXTURE_2D, DepthRenderBuffer);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
#ifdef GL_EXT_packed_depth_stencil
		if (Driver->queryOpenGLFeature(COpenGLExtensionHandler::IRR_EXT_packed_depth_stencil))
		{
			// generate packed depth stencil texture
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_STENCIL_EXT, ImageSize.Width,
				ImageSize.Height, 0, GL_DEPTH_STENCIL_EXT, GL_UNSIGNED_INT_24_8_EXT, 0);
			StencilRenderBuffer = DepthRenderBuffer; // stencil is packed with depth
		}
		else // generate separate stencil and depth textures
#endif
		{
			// generate depth texture
			glTexImage2D(GL_TEXTURE_2D, 0, Driver->getZBufferBits(), ImageSize.Width,
				ImageSize.Height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);

			// generate stencil texture
			glGenTextures(1, &StencilRenderBuffer);
			glBindTexture(GL_TEXTURE_2D, StencilRenderBuffer);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_STENCIL_INDEX, ImageSize.Width,
				ImageSize.Height, 0, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE, 0);
		}
	}
#ifdef GL_EXT_framebuffer_object
	else
	{
		// generate depth buffer
		Driver->extGlGenRenderbuffers(1, &DepthRenderBuffer);
		Driver->extGlBindRenderbuffer(GL_RENDERBUFFER_EXT, DepthRenderBuffer);
		Driver->extGlRenderbufferStorage(GL_RENDERBUFFER_EXT,
				Driver->getZBufferBits(), ImageSize.Width,
				ImageSize.Height);
	}
#endif
}


//! destructor
COpenGLFBODepthTexture::~COpenGLFBODepthTexture()
{
	if (DepthRenderBuffer && UseStencil)
		glDeleteTextures(1, &DepthRenderBuffer);
	else
		Driver->extGlDeleteRenderbuffers(1, &DepthRenderBuffer);
	if (StencilRenderBuffer && StencilRenderBuffer != DepthRenderBuffer)
		glDeleteTextures(1, &StencilRenderBuffer);
}


//combine depth texture and rtt
bool COpenGLFBODepthTexture::attach(ITexture* renderTex)
{
	if (!renderTex)
		return false;
	video::COpenGLFBOTexture* rtt = static_cast<video::COpenGLFBOTexture*>(renderTex);
	rtt->bindRTT();
#ifdef GL_EXT_framebuffer_object
	if (UseStencil)
	{
		// attach stencil texture to stencil buffer
		Driver->extGlFramebufferTexture2D(GL_FRAMEBUFFER_EXT,
						GL_STENCIL_ATTACHMENT_EXT,
						GL_TEXTURE_2D,
						StencilRenderBuffer,
						0);

		// attach depth texture to depth buffer
		Driver->extGlFramebufferTexture2D(GL_FRAMEBUFFER_EXT,
						GL_DEPTH_ATTACHMENT_EXT,
						GL_TEXTURE_2D,
						DepthRenderBuffer,
						0);
	}
	else
	{
		// attach depth renderbuffer to depth buffer
		Driver->extGlFramebufferRenderbuffer(GL_FRAMEBUFFER_EXT,
						GL_DEPTH_ATTACHMENT_EXT,
						GL_RENDERBUFFER_EXT,
						DepthRenderBuffer);
	}
#endif
	// check the status
	if (!checkFBOStatus(Driver))
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
void COpenGLFBODepthTexture::bindRTT()
{
}


//! Unbind Render Target Texture
void COpenGLFBODepthTexture::unbindRTT()
{
}


bool checkFBOStatus(COpenGLDriver* Driver)
{
#ifdef GL_EXT_framebuffer_object
	GLenum status = Driver->extGlCheckFramebufferStatus(GL_FRAMEBUFFER_EXT);

	switch (status)
	{
		//Our FBO is perfect, return true
		case GL_FRAMEBUFFER_COMPLETE_EXT:
			return true;

		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
			os::Printer::log("FBO has invalid read buffer", ELL_ERROR);
			break;

		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
			os::Printer::log("FBO has invalid draw buffer", ELL_ERROR);
			break;

		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
			os::Printer::log("FBO has one or several incomplete image attachments", ELL_ERROR);
			break;

		case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
			os::Printer::log("FBO has one or several image attachments with different internal formats", ELL_ERROR);
			break;

		case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
			os::Printer::log("FBO has one or several image attachments with different dimensions", ELL_ERROR);
			break;

// not part of fbo_object anymore, but won't harm as it is just a return value
#ifdef GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT
		case GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT:
			os::Printer::log("FBO has a duplicate image attachment", ELL_ERROR);
			break;
#endif

		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
			os::Printer::log("FBO missing an image attachment", ELL_ERROR);
			break;

#ifdef GL_EXT_framebuffer_multisample
		case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE_EXT:
			os::Printer::log("FBO wrong multisample setup", ELL_ERROR);
			break;
#endif

		case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
			os::Printer::log("FBO format unsupported", ELL_ERROR);
			break;

		default:
			break;
	}
#endif
	os::Printer::log("FBO error", ELL_ERROR);
//	_IRR_DEBUG_BREAK_IF(true);
	return false;
}


} // end namespace video
} // end namespace irr

#endif // _IRR_COMPILE_WITH_OPENGL_

