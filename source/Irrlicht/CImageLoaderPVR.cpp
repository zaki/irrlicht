// Copyright (C) 2013 Patryk Nadrowski
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CImageLoaderPVR.h"

#ifdef _IRR_COMPILE_WITH_PVR_LOADER_

#include "IReadFile.h"
#include "os.h"
#include "CImage.h"
#include "irrString.h"

namespace irr
{

namespace video
{

//! returns true if the file maybe is able to be loaded by this class
//! based on the file extension (e.g. ".tga")
bool CImageLoaderPVR::isALoadableFileExtension(const io::path& filename) const
{
	return core::hasFileExtension(filename, "pvr");
}


//! returns true if the file maybe is able to be loaded by this class
bool CImageLoaderPVR::isALoadableFileFormat(io::IReadFile* file) const
{
	if (!file)
		return false;

	c8 fourCC[4];
	file->seek(0);
	file->read(&fourCC, 4);

	/*if (header.Version == 0x03525650) // TO-DO - fix endiannes
	{
		printf("Bad endian2\n");
		fourCC[0] = os::Byteswap::byteswap(fourCC[0]);
		fourCC[1] = os::Byteswap::byteswap(fourCC[1]);
		fourCC[2] = os::Byteswap::byteswap(fourCC[2]);
		fourCC[3] = os::Byteswap::byteswap(fourCC[3]);
	}*/

	return (fourCC[0] == 'P' && fourCC[1] == 'V' && fourCC[2] == 'R');
}


//! creates a surface from the file
IImage* CImageLoaderPVR::loadImage(io::IReadFile* file) const
{
	SPVRHeader header;
	IImage* image = 0;
	ECOLOR_FORMAT format = ECF_UNKNOWN;
	u32 dataSize = 0;

	file->seek(0);
	file->read(&header, sizeof(SPVRHeader));

	/*if (header.Version == 0x03525650) // TO-DO - fix endiannes
	{
		header.Flags = os::Byteswap::byteswap(header.Flags);
		header.PixelFormat = os::Byteswap::byteswap(header.PixelFormat);
		header.ColourSpace = os::Byteswap::byteswap(header.ColourSpace);
		header.ChannelType = os::Byteswap::byteswap(header.ChannelType);
		header.Height = os::Byteswap::byteswap(header.Height);
		header.Width = os::Byteswap::byteswap(header.Width);
		header.Depth = os::Byteswap::byteswap(header.Depth);
		header.NumSurfaces = os::Byteswap::byteswap(header.NumSurfaces);
		header.NumFaces = os::Byteswap::byteswap(header.NumFaces);
		header.MipMapCount = os::Byteswap::byteswap(header.MipMapCount);
		header.MetDataSize = os::Byteswap::byteswap(header.MetDataSize);
	}*/

	c8 fourCC[4];
	u32 key;
	u32 helperDataSize;

	if (header.MetDataSize > 0)
	{
		file->read(&fourCC, 4);
		file->read(&key, sizeof(u32));
		file->read(&helperDataSize, sizeof(u32));
		file->seek(helperDataSize, true);
	}

	if (header.PixelFormat & 0xFFFFFFFF00000000) // Uncompressed texture formats
	{
		// TO-DO add support for uncompressed images.
	}
	else // Compressed texture formats
	{
		switch(header.PixelFormat)
		{
			case 0: // PVRTC 2bpp RGB
				format = ECF_PVRTC_RGB2;
				break;
			case 1: // PVRTC 2bpp RGBA
				format = ECF_PVRTC_ARGB2;
				break;
			case 2: // PVRTC 4bpp RGB
				format = ECF_PVRTC_RGB4;
				break;
			case 3: // PVRTC 4bpp RGBA
				format = ECF_PVRTC_ARGB4;
				break;
			case 4: // PVRTC-II 2bpp
				format = ECF_PVRTC2_ARGB2;
				break;
			case 5: // PVRTC-II 4bpp
				format = ECF_PVRTC2_ARGB4;
				break;
			case 6: // ETC1
				format = ECF_ETC1;
				break;
			case 7: // DXT1 / BC1
				format = ECF_DXT1;
				break;
			case 8: // DXT2
			case 9: // DXT3 / BC2
				format = ECF_DXT3;
				break;
			case 10: // DXT4
			case 11: // DXT5 / BC3
				format = ECF_DXT5;
				break;
			case 22: // ETC2 RGB
				format = ECF_ETC2_RGB;
				break;
			case 23: // ETC2 RGBA
				format = ECF_ETC2_ARGB;
				break;
			default:
				format = ECF_UNKNOWN;
				break;
		}

		if (format != ECF_UNKNOWN)
		{
			// Calculate image data size.
			u32 curWidth = header.Width;
			u32 curHeight = header.Height;

			dataSize = IImage::getCompressedImageSize(format, curWidth, curHeight);

			do
			{
				if (curWidth > 1)
					curWidth >>= 1;

				if (curHeight > 1)
					curHeight >>= 1;

				dataSize += IImage::getCompressedImageSize(format, curWidth, curHeight);
			}
			while (curWidth != 1 || curWidth != 1);

			// 3D textures, texture arrays, cube maps textures aren't currently supported
			if (header.Depth < 2 && header.NumSurfaces < 2 && header.NumFaces < 2)
			{
				u8* data = new u8[dataSize];
				file->read(data, dataSize);

				bool hasMipMap = (header.MipMapCount > 0) ? true : false;

				image = new CImage(format, core::dimension2d<u32>(header.Width, header.Height), data, true, true, true, hasMipMap);
			}
		}
	}

	return image;
}


//! creates a loader which is able to load pvr images
IImageLoader* createImageLoaderPVR()
{
	return new CImageLoaderPVR();
}


} // end namespace video
} // end namespace irr

#endif

