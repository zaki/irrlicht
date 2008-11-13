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

namespace irr
{
namespace video
{

//! constructor for usual textures
COGLES1Texture::COGLES1Texture(IImage* origImage, const char* name, COGLES1Driver* driver)
	: ITexture(name), Driver(driver), Image(0),
	TextureName(0), InternalFormat(GL_RGBA), PixelFormat(GL_RGBA),
	// TODO ogl-es
	// PixelFormat(GL_BGRA),
	PixelType(GL_UNSIGNED_BYTE),
	HasMipMaps(true), IsRenderTarget(false), AutomaticMipmapUpdate(false),
	UseStencil(false), ReadOnlyLock(false)
{
	#ifdef _DEBUG
	setDebugName("COGLES1Texture");
	#endif

	getImageData(origImage);

	HasMipMaps = Driver->getTextureCreationFlag(ETCF_CREATE_MIP_MAPS);
	if (Image)
	{
		glGenTextures(1, &TextureName);
		copyTexture();
	}
}


//! destructor
COGLES1Texture::~COGLES1Texture()
{
	glDeleteTextures(1, &TextureName);
	if (Image)
		Image->drop();
}


ECOLOR_FORMAT COGLES1Texture::getBestColorFormat(ECOLOR_FORMAT format)
{
	ECOLOR_FORMAT destFormat = ECF_A8R8G8B8;
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
	}
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


void COGLES1Texture::getImageData(IImage* image)
{
	if (!image)
	{
		os::Printer::log("No image for OGLES1 texture.", ELL_ERROR);
		return;
	}

	ImageSize = image->getDimension();

	if ( !ImageSize.Width || !ImageSize.Height)
	{
		os::Printer::log("Invalid size of image for OGLES1 Texture.", ELL_ERROR);
		return;
	}

	core::dimension2d<s32> nImageSize;
	if (Driver->queryFeature(EVDF_TEXTURE_NPOT))
		nImageSize=ImageSize;
	else
	{
		nImageSize.Width = getTextureSizeFromSurfaceSize(ImageSize.Width);
		nImageSize.Height = getTextureSizeFromSurfaceSize(ImageSize.Height);
	}

	ECOLOR_FORMAT destFormat = getBestColorFormat(image->getColorFormat());
	if (ImageSize==nImageSize)
		Image = new CImage(destFormat, image);
	else
	{
		Image = new CImage(destFormat, nImageSize);
		// scale texture
		image->copyToScaling(Image);
	}
}


//! copies the the texture into an open gl texture.
void COGLES1Texture::copyTexture(bool newTexture)
{
	glBindTexture(GL_TEXTURE_2D, TextureName);
	if (Driver->testGLError())
		os::Printer::log("Could not bind Texture", ELL_ERROR);

	if (!Image)
	{
		os::Printer::log("No image for OGLES1 texture to upload", ELL_ERROR);
		return;
	}

	switch (Image->getColorFormat())
	{
		case ECF_A1R5G5B5:
			InternalFormat=GL_RGBA;
	// TODO ogl-es
	//		PixelFormat=GL_BGRA;
	//		PixelType=GL_UNSIGNED_SHORT_1_5_5_5_REV;
			PixelFormat=GL_RGBA;
			PixelType=GL_UNSIGNED_BYTE;
			break;
		case ECF_R5G6B5:
			InternalFormat=GL_RGB;
	// TODO ogl-es
	//		PixelFormat=GL_BGR;
	//		PixelType=GL_UNSIGNED_SHORT_5_6_5_REV;
			PixelFormat=GL_RGB;
			PixelType=GL_UNSIGNED_BYTE;
			break;
		case ECF_R8G8B8:
			InternalFormat=GL_RGB;
	// TODO ogl-es
	//		PixelFormat=GL_BGR;
			PixelFormat=GL_RGB;
			PixelType=GL_UNSIGNED_BYTE;
			break;
		case ECF_A8R8G8B8:
			InternalFormat=GL_RGBA;
	// TODO ogl-es
	//		PixelFormat=GL_BGRA;
			PixelFormat=GL_RGBA;
			PixelType=GL_UNSIGNED_BYTE;
			break;
		default:
			os::Printer::log("Unsupported texture format", ELL_ERROR);
			break;
	}

	if (newTexture)
	{
		#ifndef DISABLE_MIPMAPPING
		if (HasMipMaps && Driver->queryFeature(EVDF_MIP_MAP_AUTO_UPDATE))
		{
			// automatically generate and update mipmaps
			glTexParameteri( GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE );
			AutomaticMipmapUpdate=true;
		}
		else
		{
			AutomaticMipmapUpdate=false;
			regenerateMipMapLevels();
		}
		if (HasMipMaps) // might have changed in regenerateMipMapLevels
		{
			// enable bilinear mipmap filter
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		else
		#else
			HasMipMaps=false;
			os::Printer::log("Did not create OGLES1 texture mip maps.", ELL_ERROR);
		#endif
		{
			// enable bilinear filter without mipmaps
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
	}

	void* source = Image->lock();
	if (newTexture)
		glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, Image->getDimension().Width,
			Image->getDimension().Height, 0, PixelFormat, PixelType, source);
	else
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, Image->getDimension().Width,
			Image->getDimension().Height, PixelFormat, PixelType, source);
	Image->unlock();

	if (Driver->testGLError())
		os::Printer::log("Could not glTexImage2D", ELL_ERROR);
}


//! returns the size of a texture which would be the optimal size for rendering it
inline s32 COGLES1Texture::getTextureSizeFromSurfaceSize(s32 size) const
{
	s32 ts = 0x01;
	while(ts < size)
		ts <<= 1;

	return ts;
}


//! lock function
void* COGLES1Texture::lock(bool readOnly)
{
	ReadOnlyLock |= readOnly;

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
		const s32 pitch=Image->getPitch();
		u8* p2 = pPixels + (ImageSize.Height - 1) * pitch;
		u8* tmpBuffer = new u8[pitch];
		for (s32 i=0; i < ImageSize.Height; i += 2)
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
		copyTexture(false);
	ReadOnlyLock = false;
}


//! Returns size of the original image.
const core::dimension2d<s32>& COGLES1Texture::getOriginalSize() const
{
	return ImageSize;
}


//! Returns size of the texture.
const core::dimension2d<s32>& COGLES1Texture::getSize() const
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
void COGLES1Texture::regenerateMipMapLevels()
{
	if (AutomaticMipmapUpdate || !HasMipMaps)
		return;
	if ((Image->getDimension().Width==1) && (Image->getDimension().Height==1))
		return;

	// Manually create mipmaps
	u32 width=Image->getDimension().Width;
	u32 height=Image->getDimension().Height;
	u32 i=0;
	u8* target = new u8[Image->getImageDataSizeInBytes()];
	do
	{
		if (width>1)
			width>>=1;
		if (height>1)
			height>>=1;
		++i;
		Image->copyToScaling(target, width, height, Image->getColorFormat());
		glTexImage2D(GL_TEXTURE_2D, i, InternalFormat, width, height,
				0, PixelFormat, PixelType, target);
	}
	while (width!=1 || height!=1);
	delete [] target;
	Image->unlock();
}


bool COGLES1Texture::isRenderTarget() const
{
    return IsRenderTarget;
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

} // end namespace video
} // end namespace irr

#endif // _IRR_COMPILE_WITH_OPENGL_
