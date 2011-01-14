// Copyright (C) 2004 Murphy McCauley
// Copyright (C) 2007-2011 Christian Stehno
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
#if 0

#if defined(_MSC_VER) || defined(__BORLANDC__) || defined (__BCPLUSPLUS__)
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

#if defined(_MSC_VER) || defined(__BORLANDC__) || defined (__BCPLUSPLUS__)
#	pragma pack( pop, packing )
#endif
#undef PACK_STRUCT

//! An Irrlicht image loader for Quake engine WAL textures
class CImageLoaderWAL : public irr::video::IImageLoader
{
public:
	virtual bool isALoadableFileExtension(const io::path& filename) const;

	virtual bool isALoadableFileFormat(irr::io::IReadFile* file) const;

	virtual irr::video::IImage* loadImage(irr::io::IReadFile* file) const;

private:
	static s32 DefaultPaletteQ2[256];
};

#endif

//! An Irrlicht image loader for Quake1,2 engine lmp textures/palette
class CImageLoaderLMP : public irr::video::IImageLoader
{
public:
	virtual bool isALoadableFileExtension(const io::path& filename) const;
	virtual bool isALoadableFileFormat(irr::io::IReadFile* file) const;
	virtual irr::video::IImage* loadImage(irr::io::IReadFile* file) const;
};

//! An Irrlicht image loader for quake2 wal engine textures
class CImageLoaderWAL : public irr::video::IImageLoader
{
public:
	virtual bool isALoadableFileExtension(const io::path& filename) const;
	virtual bool isALoadableFileFormat(irr::io::IReadFile* file) const;
	virtual irr::video::IImage* loadImage(irr::io::IReadFile* file) const;
};

//! An Irrlicht image loader for Halife 1 engine textures
class CImageLoaderWAL2 : public irr::video::IImageLoader
{
public:
	virtual bool isALoadableFileExtension(const io::path& filename) const;
	virtual bool isALoadableFileFormat(irr::io::IReadFile* file) const;
	virtual irr::video::IImage* loadImage(irr::io::IReadFile* file) const;
};



#if defined(_MSC_VER) || defined(__BORLANDC__) || defined (__BCPLUSPLUS__) 
#	pragma pack( push, packing )
#	pragma pack( 1 )
#	define PACK_STRUCT
#elif defined( __GNUC__ )
#	define PACK_STRUCT	__attribute__((packed))
#else
#	error compiler not supported
#endif

	 struct SLMPHeader {
		u32	width;				// width
		u32	height;				// height
								// variably sized
    } PACK_STRUCT;

	// Halfelife wad3 type 67 file
	struct miptex_halflife
	{
		c8  name[16];
		u32 width, height;
		u32 mipmap[4];		// four mip maps stored
	} PACK_STRUCT;

	//quake2 texture
	struct miptex_quake2
	{
		c8 name[32];
		u32 width;
		u32 height;
		u32 mipmap[4];		// four mip maps stored
		c8  animname[32];	// next frame in animation chain
		s32 flags;
		s32 contents;
		s32 value;
	};


#if defined(_MSC_VER) || defined(__BORLANDC__) || defined (__BCPLUSPLUS__) 
#	pragma pack( pop, packing )
#endif
#undef PACK_STRUCT

}
}

#endif
