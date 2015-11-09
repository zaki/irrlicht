// Copyright (C) 2015 Patryk Nadrowski
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_OGLCORE_TEXTURE_H_INCLUDED__
#define __C_OGLCORE_TEXTURE_H_INCLUDED__

#include "IrrCompileConfig.h"

#if defined(_IRR_COMPILE_WITH_OPENGL_) || defined(_IRR_COMPILE_WITH_OGLES1_) || defined(_IRR_COMPILE_WITH_OGLES2_)

#include "irrArray.h"
#include "SMaterialLayer.h"
#include "ITexture.h"
#include "EDriverFeatures.h"
#include "os.h"

namespace irr
{
namespace video
{

template <class TOGLDriver>
class COGLCoreTexture : public ITexture
{
public:
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

	COGLCoreTexture(const io::path& name, const core::array<IImage*>& image, TOGLDriver* driver) : ITexture(name), Driver(driver), TextureType(GL_TEXTURE_2D),
		TextureName(0), InternalFormat(GL_RGBA), PixelFormat(GL_RGBA), PixelType(GL_UNSIGNED_BYTE), LockReadOnly(false), LockImage(0), LockLevel(0), KeepImage(true),
		AutoGenerateMipMaps(false)
	{
		_IRR_DEBUG_BREAK_IF(image.size() == 0)

		DriverType = Driver->getDriverType();
		HasMipMaps = Driver->getTextureCreationFlag(ETCF_CREATE_MIP_MAPS);
		AutoGenerateMipMaps = Driver->queryFeature(EVDF_MIP_MAP_AUTO_UPDATE);

		getImageValues(image[0]);

		const core::array<IImage*>* tmpImage = &image;

		if (KeepImage || OriginalSize != Size || OriginalColorFormat != ColorFormat)
		{
			Image.set_used(image.size());

			for (u32 i = 0; i < image.size(); ++i)
			{
				Image[i] = Driver->createImage(ColorFormat, Size);

				if (image[i]->getDimension() == Size)
					image[i]->copyTo(Image[i]);
				else
					image[i]->copyToScaling(Image[i]);
			}

			tmpImage = &Image;
		}

		Pitch = (*tmpImage)[0]->getPitch();

		glGenTextures(1, &TextureName);

		const COGLCoreTexture* prevTexture = Driver->getCacheHandler()->getTextureCache().get(0);
		Driver->getCacheHandler()->getTextureCache().set(0, this);

		if (HasMipMaps && AutoGenerateMipMaps)
		{
			if (Driver->getTextureCreationFlag(ETCF_OPTIMIZED_FOR_SPEED))
				glHint(GL_GENERATE_MIPMAP_HINT, GL_FASTEST);
			else if (Driver->getTextureCreationFlag(ETCF_OPTIMIZED_FOR_QUALITY))
				glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
			else
				glHint(GL_GENERATE_MIPMAP_HINT, GL_DONT_CARE);
		}

#if defined(IRR_OPENGL_VERSION) && IRR_OPENGL_VERSION < 20
		if (HasMipMaps)
			glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, (AutoGenerateMipMaps) ? GL_TRUE : GL_FALSE);
#endif

		uploadTexture(true, 0, (*tmpImage)[0]->getData());

		Driver->getCacheHandler()->getTextureCache().set(0, prevTexture);

		regenerateMipMapLevels((*tmpImage)[0]->getMipMapsData());

		if (!KeepImage)
		{
			for (u32 i = 0; i < Image.size(); ++i)
				Image[i]->drop();

			Image.clear();
		}
	}

	COGLCoreTexture(const io::path& name, const core::dimension2d<u32>& size, ECOLOR_FORMAT format, TOGLDriver* driver) : ITexture(name), Driver(driver), TextureType(GL_TEXTURE_2D),
		TextureName(0), InternalFormat(GL_RGBA), PixelFormat(GL_RGBA), PixelType(GL_UNSIGNED_BYTE), LockReadOnly(false), LockImage(0), LockLevel(0), KeepImage(false),
		AutoGenerateMipMaps(false)
	{
		DriverType = Driver->getDriverType();
		HasMipMaps = false;
		IsRenderTarget = true;

		OriginalColorFormat = format;

		if (ECF_UNKNOWN == OriginalColorFormat)
			ColorFormat = getBestColorFormat(Driver->getColorFormat());
		else
			ColorFormat = OriginalColorFormat;

		OriginalSize = size;
		Size = OriginalSize;

		void(*converter)(const void*, s32, void*) = 0;

		Driver->getColorFormatParameters(ColorFormat, InternalFormat, PixelFormat, PixelType, &converter);

		glGenTextures(1, &TextureName);

		const COGLCoreTexture* prevTexture = Driver->getCacheHandler()->getTextureCache().get(0);
		Driver->getCacheHandler()->getTextureCache().set(0, this);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		StatesCache.WrapU = ETC_CLAMP_TO_EDGE;
		StatesCache.WrapV = ETC_CLAMP_TO_EDGE;

		glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, Size.Width, Size.Height, 0, PixelFormat, PixelType, 0);

		Driver->getCacheHandler()->getTextureCache().set(0, prevTexture);
	}

	virtual ~COGLCoreTexture()
	{
		Driver->getCacheHandler()->getTextureCache().remove(this);

		if (TextureName)
			glDeleteTextures(1, &TextureName);

		for (u32 i = 0; i < Image.size(); ++i)
			Image[i]->drop();

		if (LockImage)
			LockImage->drop();
	}

	virtual void* lock(E_TEXTURE_LOCK_MODE mode = ETLM_READ_WRITE, u32 mipmapLevel = 0) _IRR_OVERRIDE_
	{
		// TO-DO - this method will be improved.

		if (IImage::isCompressedFormat(ColorFormat) || IsRenderTarget || mipmapLevel > 0) // TO-DO
			return 0;

		LockReadOnly |= (mode == ETLM_READ_ONLY);
		LockLevel = mipmapLevel;

		LockImage = Image[0];

		return LockImage->getData();
	}

	virtual void unlock() _IRR_OVERRIDE_
	{
		if (!LockImage)
			return;

		if (!LockReadOnly)
		{
			const COGLCoreTexture* prevTexture = Driver->getCacheHandler()->getTextureCache().get(0);
			Driver->getCacheHandler()->getTextureCache().set(0, this);

			uploadTexture(false, LockLevel, LockImage->getData());

			Driver->getCacheHandler()->getTextureCache().set(0, prevTexture);

			if (LockLevel == 0)
				regenerateMipMapLevels(LockImage->getMipMapsData());
		}

		if (!KeepImage || LockLevel != 0)
			LockImage->drop();

		LockReadOnly = false;
		LockImage = 0;
		LockLevel = 0;
	}

	virtual void regenerateMipMapLevels(void* mipMapsData = 0) _IRR_OVERRIDE_
	{
		if (!HasMipMaps || (!mipMapsData && !AutoGenerateMipMaps) || (Size.Width <= 1 && Size.Height <= 1))
			return;

		const COGLCoreTexture* prevTexture = Driver->getCacheHandler()->getTextureCache().get(0);
		Driver->getCacheHandler()->getTextureCache().set(0, this);

		if (mipMapsData)
		{
			u32 width = Size.Width;
			u32 height = Size.Height;
			u8* data = static_cast<u8*>(mipMapsData);
			u32 dataSize = 0;
			u32 level = 0;

			do
			{
				if (width > 1)
					width >>= 1;

				if (height > 1)
					height >>= 1;

				dataSize = IImage::getDataSizeFromFormat(ColorFormat, width, height);
				++level;

				if (!IImage::isCompressedFormat(ColorFormat))
					glTexImage2D(GL_TEXTURE_2D, level, InternalFormat, width, height, 0, PixelFormat, PixelType, data);
				else
					Driver->irrGlCompressedTexImage2D(GL_TEXTURE_2D, level, InternalFormat, width, height, 0, dataSize, data);

				data += dataSize;
			}
			while (width != 1 || height != 1);
		}
		else
		{
#if defined(IRR_OPENGL_VERSION) && IRR_OPENGL_VERSION >= 20
			Driver->irrGlGenerateMipmap(GL_TEXTURE_2D);
#endif
		}

		Driver->getCacheHandler()->getTextureCache().set(0, prevTexture);
	}

	GLenum getOpenGLTextureType() const
	{
		return TextureType;
	}

	GLuint getOpenGLTextureName() const
	{
		return TextureName;
	}

	SStatesCache& getStatesCache() const
	{
		return StatesCache;
	}

protected:
	ECOLOR_FORMAT getBestColorFormat(ECOLOR_FORMAT format)
	{
		ECOLOR_FORMAT destFormat = (!IImage::isCompressedFormat(format)) ? ECF_A8R8G8B8 : format;

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
			if (Driver->getTextureCreationFlag(ETCF_ALWAYS_16_BIT) || Driver->getTextureCreationFlag(ETCF_OPTIMIZED_FOR_SPEED))
				destFormat = ECF_A1R5G5B5;
		default:
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

	void getImageValues(const IImage* image)
	{
		OriginalColorFormat = image->getColorFormat();
		ColorFormat = getBestColorFormat(OriginalColorFormat);

		void(*converter)(const void*, s32, void*) = 0;

		Driver->getColorFormatParameters(ColorFormat, InternalFormat, PixelFormat, PixelType, &converter);

		if (IImage::isCompressedFormat(image->getColorFormat()))
		{
			KeepImage = false;
			AutoGenerateMipMaps = false;
		}

		OriginalSize = image->getDimension();
		Size = OriginalSize;

		if (Size.Width == 0 || Size.Height == 0)
		{
			os::Printer::log("Invalid size of image for texture.", ELL_ERROR);
			return;
		}

		const f32 ratio = (f32)Size.Width / (f32)Size.Height;

		if ((Size.Width > Driver->MaxTextureSize) && (ratio >= 1.f))
		{
			Size.Width = Driver->MaxTextureSize;
			Size.Height = (u32)(Driver->MaxTextureSize / ratio);
		}
		else if (Size.Height > Driver->MaxTextureSize)
		{
			Size.Height = Driver->MaxTextureSize;
			Size.Width = (u32)(Driver->MaxTextureSize * ratio);
		}

		Size = Size.getOptimalSize(!Driver->queryFeature(EVDF_TEXTURE_NPOT));
	}

	void uploadTexture(bool initTexture, u32 level, const void* data)
	{
		if (!data)
			return;

		if (initTexture)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}

		u32 width = Size.Width >> level;
		u32 height = Size.Height >> level;

		if (!IImage::isCompressedFormat(ColorFormat))
		{
			if (initTexture)
			{
				glTexImage2D(GL_TEXTURE_2D, level, InternalFormat, width, height, 0, PixelFormat, PixelType, data);
			}
			else
			{
				glTexSubImage2D(GL_TEXTURE_2D, level, 0, 0, width, height, PixelFormat, PixelType, data);
			}
		}
		else
		{
			u32 dataSize = IImage::getDataSizeFromFormat(ColorFormat, Size.Width, height);

			if (initTexture)
			{
				Driver->irrGlCompressedTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, width, height, 0, dataSize, data);
			}
			else
			{
				Driver->irrGlCompressedTexSubImage2D(GL_TEXTURE_2D, level, 0, 0, width, height, PixelFormat, dataSize, data);
			}
		}
	}

	TOGLDriver* Driver;

	GLenum TextureType;
	GLuint TextureName;
	GLint InternalFormat;
	GLenum PixelFormat;
	GLenum PixelType;

	bool LockReadOnly;
	IImage* LockImage;
	u32 LockLevel;

	bool KeepImage;
	core::array<IImage*> Image;

	bool AutoGenerateMipMaps;

	mutable SStatesCache StatesCache;
};

}
}

#endif
#endif
