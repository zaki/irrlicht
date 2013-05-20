// Copyright (C) 2013 Patryk Nadrowski
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_IMAGE_LOADER_PVR_H_INCLUDED__
#define __C_IMAGE_LOADER_PVR_H_INCLUDED__

#include "IrrCompileConfig.h"

#ifdef _IRR_COMPILE_WITH_PVR_LOADER_

#include "IImageLoader.h"

namespace irr
{
namespace video
{

// byte-align structures
#include "irrpack.h"

/* structures */
struct SPVRHeader
{
    u32 Version;
    u32 Flags;
    u64 PixelFormat;
    u32 ColourSpace;
    u32 ChannelType;
    u32 Height;
    u32 Width;
    u32 Depth;
    u32 NumSurfaces;
    u32 NumFaces;
    u32 MipMapCount;
    u32 MetDataSize;
} PACK_STRUCT;

// Default alignment
#include "irrunpack.h"

/*!
	Surface Loader for PVR images
*/
class CImageLoaderPVR : public IImageLoader
{
public:

	//! returns true if the file maybe is able to be loaded by this class
	//! based on the file extension (e.g. ".tga")
	virtual bool isALoadableFileExtension(const io::path& filename) const;

	//! returns true if the file maybe is able to be loaded by this class
	virtual bool isALoadableFileFormat(io::IReadFile* file) const;

	//! creates a surface from the file
	virtual IImage* loadImage(io::IReadFile* file) const;
};

} // end namespace video
} // end namespace irr

#endif // compiled with PVR loader
#endif
