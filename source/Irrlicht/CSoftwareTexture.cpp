// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_SOFTWARE_

#include "CSoftwareTexture.h"
#include "os.h"

namespace irr
{
namespace video
{

//! constructor
CSoftwareTexture::CSoftwareTexture(IImage* image, const io::path& name,
		bool renderTarget, void* mipmapData)
: ITexture(name), Texture(0)
{
	#ifdef _DEBUG
	setDebugName("CSoftwareTexture");
	#endif

	DriverType = EDT_SOFTWARE;
	ColorFormat = ECF_A1R5G5B5;
	HasMipMaps = false;
	HasAlpha = true;
	IsRenderTarget = renderTarget;

	if (image)
	{
		bool IsCompressed = false;

		if(IImage::isCompressedFormat(image->getColorFormat()))
		{
			os::Printer::log("Texture compression not available.", ELL_ERROR);
			IsCompressed = true;
		}

		OriginalSize = image->getDimension();
		core::dimension2d<u32> optSize = OriginalSize.getOptimalSize();

		Image = new CImage(ECF_A1R5G5B5, OriginalSize);

		if (!IsCompressed)
			image->copyTo(Image);

		if (optSize == OriginalSize)
		{
			Texture = Image;
			Texture->grab();
		}
		else
		{
			Texture = new CImage(ECF_A1R5G5B5, optSize);
			Image->copyToScaling(Texture);
		}

		Size = Texture->getDimension();
		Pitch = Texture->getDimension().Width * 2;
	}
}



//! destructor
CSoftwareTexture::~CSoftwareTexture()
{
	if (Image)
		Image->drop();

	if (Texture)
		Texture->drop();
}



//! lock function
void* CSoftwareTexture::lock(E_TEXTURE_LOCK_MODE mode, u32 mipmapLevel)
{
	return Image->lock();
}



//! unlock function
void CSoftwareTexture::unlock()
{
	if (Image != Texture)
	{
		os::Printer::log("Performance warning, slow unlock of non power of 2 texture.", ELL_WARNING);
		Image->copyToScaling(Texture);
	}

	Image->unlock();
}


//! returns unoptimized surface
CImage* CSoftwareTexture::getImage()
{
	return Image;
}


//! returns texture surface
CImage* CSoftwareTexture::getTexture()
{
	return Texture;
}


//! Regenerates the mip map levels of the texture. Useful after locking and
//! modifying the texture
void CSoftwareTexture::regenerateMipMapLevels(void* mipmapData)
{
	// our software textures don't have mip maps
}


} // end namespace video
} // end namespace irr

#endif // _IRR_COMPILE_WITH_SOFTWARE_

