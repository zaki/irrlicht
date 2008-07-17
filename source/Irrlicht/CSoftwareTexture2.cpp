// Copyright (C) 2002-2008 Nikolaus Gebhardt / Thomas Alten
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_BURNINGSVIDEO_

#include "SoftwareDriver2_compile_config.h"
#include "SoftwareDriver2_helper.h"
#include "CSoftwareTexture2.h"
#include "os.h"
#include "CImageWriterBMP.h"

namespace irr
{
namespace video  
{

IImageWriter* createImageWriterBMP();

//! constructor
CSoftwareTexture2::CSoftwareTexture2(IImage* image, const char* name, bool generateMipLevels, bool isRenderTarget)
: ITexture(name), MipMapLOD(0), HasMipMaps(generateMipLevels), IsRenderTarget(isRenderTarget)
{
	#ifdef _DEBUG
	setDebugName("CSoftwareTexture2");
	#endif

	#ifndef SOFTWARE_DRIVER_2_MIPMAPPING
		HasMipMaps = false;
	#endif

	memset32 ( MipMap, 0, sizeof ( MipMap ) );

	if (image)
	{
		
		core::dimension2d<s32> optSize;
		OrigSize = image->getDimension();

		optSize.Width = getTextureSizeFromSurfaceSize(OrigSize.Width);
		optSize.Height = getTextureSizeFromSurfaceSize(OrigSize.Height);
		
		if ( OrigSize == optSize )
		{
			MipMap[0] = new CImage(BURNINGSHADER_COLOR_FORMAT, image);
		}
		else
		{
			//os::Printer::log ( "Burningvideo: Warning Texture reformat", ELL_WARNING );
			MipMap[0] = new CImage(BURNINGSHADER_COLOR_FORMAT, optSize);

			// temporary CImage needed
			CImage * temp = new CImage ( BURNINGSHADER_COLOR_FORMAT, image );
			temp->copyToScalingBoxFilter ( MipMap[0], 0 );
			//temp->copyToScaling(MipMap[0]);
			temp->drop ();
		}
	}

	regenerateMipMapLevels();
	setCurrentMipMapLOD(0);
}


//! destructor
CSoftwareTexture2::~CSoftwareTexture2()
{
	for ( s32 i = 0; i!= SOFTWARE_DRIVER_2_MIPMAPPING_MAX; ++i )
	{
		if ( MipMap[i] )
			MipMap[i]->drop();
	}
}


//! returns the size of a texture which would be the optimize size for rendering it
s32 CSoftwareTexture2::getTextureSizeFromSurfaceSize(s32 size) const
{
	s32 ts = 0x01;

	while(ts < size)
		ts <<= 1;

	if ( ts > size )
		ts >>= 1;

	//ts = core::s32_min ( ts, 256 );
	return ts;
}


//! Regenerates the mip map levels of the texture. Useful after locking and 
//! modifying the texture
void CSoftwareTexture2::regenerateMipMapLevels()
{
	if ( !HasMipMaps )
		return;

	s32 i;

	// release
	for ( i = 1; i!= SOFTWARE_DRIVER_2_MIPMAPPING_MAX; ++i )
	{
		if ( MipMap[i] )
			MipMap[i]->drop();
	}

	core::dimension2d<s32> newSize;
	core::dimension2d<s32> currentSize;

	i = 1;
	CImage * c = MipMap[0];
	while ( i < SOFTWARE_DRIVER_2_MIPMAPPING_MAX )
	{
		currentSize = c->getDimension();
		newSize.Width = core::s32_max ( 1, currentSize.Width >> SOFTWARE_DRIVER_2_MIPMAPPING_SCALE );
		newSize.Height = core::s32_max ( 1, currentSize.Height >> SOFTWARE_DRIVER_2_MIPMAPPING_SCALE );

		MipMap[i] = new CImage(BURNINGSHADER_COLOR_FORMAT, newSize);
		MipMap[0]->copyToScalingBoxFilter( MipMap[i], 0 );
		c = MipMap[i];
		++i;
	}
}


} // end namespace video
} // end namespace irr

#endif // _IRR_COMPILE_WITH_BURNINGSVIDEO_

