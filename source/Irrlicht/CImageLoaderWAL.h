// Copyright (C) 2004 Murphy McCauley
// Copyright (C) 2007-2008 Christian Stehno
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h
/*
 Thanks to:
 Max McGuire for his Flipcode article about WAL textures
 Nikolaus Gebhardt for the Irrlicht 3D engine
*/

#ifndef __C_IMAGE_LOADER_WAL_H_INCLUDED__
#define __C_IMAGE_LOADER_WAL_H_INCLUDED__

#include "IImageLoader.h"

namespace irr 
{
namespace video
{

#ifdef _MSC_VER
#	pragma pack( push, packing )
#	pragma pack( 1 )
#	define PACK_STRUCT
#elif defined( __GNUC__ )
#	define PACK_STRUCT	__attribute__((packed))
#else
#	error compiler not supported
#endif

	 struct SWALHeader {
		char	FrameName[32];
		
		u32	ImageWidth;
		u32	ImageHeight;

		s32	MipmapOffset[4];

		char	NextFrameName[32];

		u32	Flags;		// surface properties, i.e. slick, sky, nodraw
		u32	Contents;	// i.e. solid, clip, area portal
		u32	Value;		// light
    } PACK_STRUCT;

#ifdef _MSC_VER
#	pragma pack( pop, packing )
#endif
#undef PACK_STRUCT

//! An Irrlicht image loader for Quake engine WAL textures
class CImageLoaderWAL : public irr::video::IImageLoader
{
public:
	virtual bool isALoadableFileExtension(const c8* fileName) const;

	virtual bool isALoadableFileFormat(irr::io::IReadFile* file) const;

	virtual irr::video::IImage* loadImage(irr::io::IReadFile* file) const;

private:
	static s32 DefaultPaletteQ2[256];
};

}
}

#endif
