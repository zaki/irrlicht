// Copyright (C) 2002-2009 Nikolaus Gebhardt / Thomas Alten
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_BURNINGSVIDEO_

#include "SoftwareDriver2_compile_config.h"
#include "SoftwareDriver2_helper.h"
#include "CSoftwareTexture2.h"
#include "os.h"

namespace irr
{
namespace video
{

//! constructor
CSoftwareTexture2::CSoftwareTexture2(IImage* image, const core::string<c16>& name, u32 flags )
: ITexture(name), MipMapLOD(0), Flags ( flags )
{
	#ifdef _DEBUG
	setDebugName("CSoftwareTexture2");
	#endif

	#ifndef SOFTWARE_DRIVER_2_MIPMAPPING
		Flags &= ~GEN_MIPMAP;
	#endif

	memset32 ( MipMap, 0, sizeof ( MipMap ) );

	if (image)
	{
		OrigSize = image->getDimension();

		core::setbit_cond ( Flags,
							image->getColorFormat () == video::ECF_A8R8G8B8 || 
							image->getColorFormat () == video::ECF_A1R5G5B5,
							HAS_ALPHA
						);
	
		core::dimension2d<u32> optSize(
				OrigSize.getOptimalSize( 0 != ( Flags & NP2_SIZE ),
				false, false,
				( Flags & NP2_SIZE ) ? SOFTWARE_DRIVER_2_TEXTURE_MAXSIZE : 0)
			);

		if ( OrigSize == optSize )
		{
			MipMap[0] = new CImage(BURNINGSHADER_COLOR_FORMAT, image);
		}
		else
		{
			char buf[256];
			core::stringw showName ( name );
			snprintf ( buf, 256, "Burningvideo: Warning Texture %ls reformat %dx%d -> %dx%d,%d",
							showName.c_str(),
							OrigSize.Width, OrigSize.Height, optSize.Width, optSize.Height, 
							BURNINGSHADER_COLOR_FORMAT
						);

			OrigSize = optSize;
			os::Printer::log ( buf, ELL_WARNING );
			MipMap[0] = new CImage(BURNINGSHADER_COLOR_FORMAT, optSize);
			MipMap[0]->fill ( 0 );


			// temporary CImage needed
			CImage * temp = new CImage ( BURNINGSHADER_COLOR_FORMAT, image );
			temp->copyToScalingBoxFilter ( MipMap[0],0, false );
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


//! Regenerates the mip map levels of the texture. Useful after locking and
//! modifying the texture
void CSoftwareTexture2::regenerateMipMapLevels()
{
	if ( !hasMipMaps () )
		return;

	s32 i;

	// release
	for ( i = 1; i < SOFTWARE_DRIVER_2_MIPMAPPING_MAX; ++i )
	{
		if ( MipMap[i] )
			MipMap[i]->drop();
	}

	core::dimension2d<u32> newSize;
	core::dimension2d<u32> currentSize;

	i = 1;
	CImage * c = MipMap[0];
	while ( i < SOFTWARE_DRIVER_2_MIPMAPPING_MAX )
	{
		currentSize = c->getDimension();
		newSize.Width = core::s32_max ( 1, currentSize.Width >> SOFTWARE_DRIVER_2_MIPMAPPING_SCALE );
		newSize.Height = core::s32_max ( 1, currentSize.Height >> SOFTWARE_DRIVER_2_MIPMAPPING_SCALE );

		MipMap[i] = new CImage(BURNINGSHADER_COLOR_FORMAT, newSize);
		MipMap[i]->fill ( 0 );
		MipMap[0]->copyToScalingBoxFilter( MipMap[i], 0, false );
		c = MipMap[i];
		++i;
	}
}


} // end namespace video
} // end namespace irr

#endif // _IRR_COMPILE_WITH_BURNINGSVIDEO_

