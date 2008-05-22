// Copyright (C) 2004 Murphy McCauley
// Copyright (C) 2007-2008 Christian Stehno
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CImageLoaderWAL.h"
#include "CColorConverter.h"
#include "CImage.h"
#include "os.h"
#include "dimension2d.h"
#include "IVideoDriver.h"
#include "IFileSystem.h"
#include "IReadFile.h"
#include "irrString.h"

namespace irr
{
namespace video
{

// May or may not be fully implemented
#define TRY_LOADING_PALETTE_FROM_FILE 0

// Default palette for Q2 WALs.

s32 CImageLoaderWAL::DefaultPaletteQ2[256] = {
0x000000L,0x0F0F0FL,0x1F1F1FL,0x2F2F2FL,0x3F3F3FL,0x4B4B4BL,0x5B5B5BL,0x6B6B6BL,0x7B7B7BL,0x8B8B8BL,0x9B9B9BL,
0xABABABL,0xBBBBBBL,0xCBCBCBL,0xDBDBDBL,0xEBEBEBL,0x634B23L,0x5B431FL,0x533F1FL,0x4F3B1BL,0x47371BL,0x3F2F17L,
0x3B2B17L,0x332713L,0x2F2313L,0x2B1F13L,0x271B0FL,0x23170FL,0x1B130BL,0x170F0BL,0x130F07L,0x0F0B07L,0x5F5F6FL,
0x5B5B67L,0x5B535FL,0x574F5BL,0x534B53L,0x4F474BL,0x473F43L,0x3F3B3BL,0x3B3737L,0x332F2FL,0x2F2B2BL,0x272727L,
0x232323L,0x1B1B1BL,0x171717L,0x131313L,0x8F7753L,0x7B6343L,0x735B3BL,0x674F2FL,0xCF974BL,0xA77B3BL,0x8B672FL,
0x6F5327L,0xEB9F27L,0xCB8B23L,0xAF771FL,0x93631BL,0x774F17L,0x5B3B0FL,0x3F270BL,0x231707L,0xA73B2BL,0x9F2F23L,
0x972B1BL,0x8B2713L,0x7F1F0FL,0x73170BL,0x671707L,0x571300L,0x4B0F00L,0x430F00L,0x3B0F00L,0x330B00L,0x2B0B00L,
0x230B00L,0x1B0700L,0x130700L,0x7B5F4BL,0x735743L,0x6B533FL,0x674F3BL,0x5F4737L,0x574333L,0x533F2FL,0x4B372BL,
0x433327L,0x3F2F23L,0x37271BL,0x2F2317L,0x271B13L,0x1F170FL,0x170F0BL,0x0F0B07L,0x6F3B17L,0x5F3717L,0x532F17L,
0x432B17L,0x372313L,0x271B0FL,0x1B130BL,0x0F0B07L,0xB35B4FL,0xBF7B6FL,0xCB9B93L,0xD7BBB7L,0xCBD7DFL,0xB3C7D3L,
0x9FB7C3L,0x87A7B7L,0x7397A7L,0x5B879BL,0x47778BL,0x2F677FL,0x17536FL,0x134B67L,0x0F435BL,0x0B3F53L,0x07374BL,
0x072F3FL,0x072733L,0x001F2BL,0x00171FL,0x000F13L,0x00070BL,0x000000L,0x8B5757L,0x834F4FL,0x7B4747L,0x734343L,
0x6B3B3BL,0x633333L,0x5B2F2FL,0x572B2BL,0x4B2323L,0x3F1F1FL,0x331B1BL,0x2B1313L,0x1F0F0FL,0x130B0BL,0x0B0707L,
0x000000L,0x979F7BL,0x8F9773L,0x878B6BL,0x7F8363L,0x777B5FL,0x737357L,0x6B6B4FL,0x636347L,0x5B5B43L,0x4F4F3BL,
0x434333L,0x37372BL,0x2F2F23L,0x23231BL,0x171713L,0x0F0F0BL,0x9F4B3FL,0x934337L,0x8B3B2FL,0x7F3727L,0x772F23L,
0x6B2B1BL,0x632317L,0x571F13L,0x4F1B0FL,0x43170BL,0x37130BL,0x2B0F07L,0x1F0B07L,0x170700L,0x0B0000L,0x000000L,
0x777BCFL,0x6F73C3L,0x676BB7L,0x6363A7L,0x5B5B9BL,0x53578FL,0x4B4F7FL,0x474773L,0x3F3F67L,0x373757L,0x2F2F4BL,
0x27273FL,0x231F2FL,0x1B1723L,0x130F17L,0x0B0707L,0x9BAB7BL,0x8F9F6FL,0x879763L,0x7B8B57L,0x73834BL,0x677743L,
0x5F6F3BL,0x576733L,0x4B5B27L,0x3F4F1BL,0x374313L,0x2F3B0BL,0x232F07L,0x1B2300L,0x131700L,0x0B0F00L,0x00FF00L,
0x23E70FL,0x3FD31BL,0x53BB27L,0x5FA72FL,0x5F8F33L,0x5F7B33L,0xFFFFFFL,0xFFFFD3L,0xFFFFA7L,0xFFFF7FL,0xFFFF53L,
0xFFFF27L,0xFFEB1FL,0xFFD717L,0xFFBF0FL,0xFFAB07L,0xFF9300L,0xEF7F00L,0xE36B00L,0xD35700L,0xC74700L,0xB73B00L,
0xAB2B00L,0x9B1F00L,0x8F1700L,0x7F0F00L,0x730700L,0x5F0000L,0x470000L,0x2F0000L,0x1B0000L,0xEF0000L,0x3737FFL,
0xFF0000L,0x0000FFL,0x2B2B23L,0x1B1B17L,0x13130FL,0xEB977FL,0xC37353L,0x9F5733L,0x7B3F1BL,0xEBD3C7L,0xC7AB9BL,
0xA78B77L,0x876B57L,0x9F5B53L
};


bool CImageLoaderWAL::isALoadableFileExtension(const c8* fileName) const
{
	return strstr(fileName, ".wal") != 0;
}


bool CImageLoaderWAL::isALoadableFileFormat(irr::io::IReadFile* file) const
{
	return (false); // no recognition possible, use a proper file ending
}


IImage* CImageLoaderWAL::loadImage(irr::io::IReadFile* file) const
{
	// Try to get the color palette from elsewhere (usually in a pak along with the WAL).
	// If this fails we use the DefaultPaletteQ2.
	static s32 * palette = 0;
#if TRY_LOADING_PALETTE_FROM_FILE
	s32 loadedPalette[256];
#endif
	if (!palette)
	{
#if TRY_LOADING_PALETTE_FROM_FILE
		IImage * paletteImage;
		// Look in a couple different places...
		paletteImage = createImageFromFile("pics/colormap.pcx");
		if (!paletteImage)
			paletteImage = createImageFromFile("pics/colormap.tga");
		if (!paletteImage)
			paletteImage = createImageFromFile("colormap.pcx");
		if (!paletteImage)
			paletteImage = createImageFromFile("colormap.tga");
		if (paletteImage && (paletteImage->getDimension().Width == 256) ) {
			palette = &loadedPalette;
			for (u32 i = 0; i < 256; ++i) {
				palette[i] = paletteImage->getPixel(i, 0).color;
			}
		} else {
			//FIXME: try reading a simple palette from "wal.pal"
			palette = DefaultPaletteQ2;
		}
		if (paletteImage)
			paletteImage->drop();
#endif
	}
	else
	{
		palette = DefaultPaletteQ2;
	}

	SWALHeader header;

	file->seek(0);
	if (file->read(&header, sizeof(SWALHeader)) != sizeof(SWALHeader) )
		return 0;

	if (file->getSize() < header.MipmapOffset[0])
		return 0;
	file->seek(header.MipmapOffset[0]);

	// read image

	const u32 imageSize = header.ImageHeight * header.ImageWidth;
	if (file->getSize() < (long)(imageSize + header.MipmapOffset[0]))
		return 0;

	u8* data = new u8[imageSize];
	file->read(data, imageSize);

	IImage* image = 0;

	image = new CImage(ECF_A1R5G5B5,
		core::dimension2d<s32>(header.ImageWidth, header.ImageHeight));

	// I wrote an 8 to 32 converter, but this works with released Irrlicht code.
	CColorConverter::convert8BitTo16Bit(data,
		(s16*)image->lock(), header.ImageWidth, header.ImageHeight, palette);
	image->unlock();

	delete [] data;

	return image;
}


IImageLoader* createImageLoaderWAL()
{
	return new irr::video::CImageLoaderWAL();
}


}
}


