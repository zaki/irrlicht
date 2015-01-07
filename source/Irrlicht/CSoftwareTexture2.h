// Copyright (C) 2002-2012 Nikolaus Gebhardt / Thomas Alten
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_SOFTWARE_2_TEXTURE_H_INCLUDED__
#define __C_SOFTWARE_2_TEXTURE_H_INCLUDED__

#include "SoftwareDriver2_compile_config.h"

#include "ITexture.h"
#include "CImage.h"

namespace irr
{
namespace video
{

/*!
	interface for a Video Driver dependent Texture.
*/
class CSoftwareTexture2 : public ITexture
{
public:

	//! constructor
	enum eTex2Flags
	{
		GEN_MIPMAP	= 1,
		IS_RENDERTARGET	= 2,
		NP2_SIZE	= 4,
		HAS_ALPHA	= 8
	};
	CSoftwareTexture2(IImage* surface, const io::path& name, u32 flags, void* mipmapData=0);

	//! destructor
	virtual ~CSoftwareTexture2();

	//! lock function
	virtual void* lock(E_TEXTURE_LOCK_MODE mode=ETLM_READ_WRITE, u32 mipmapLevel=0) _IRR_OVERRIDE_
	{
		if (Flags & GEN_MIPMAP)
		{
			MipMapLOD = mipmapLevel;
			Size = MipMap[MipMapLOD]->getDimension();
			Pitch = MipMap[MipMapLOD]->getPitch();
		}

		return MipMap[MipMapLOD]->lock();
	}

	//! unlock function
	virtual void unlock() _IRR_OVERRIDE_
	{
		MipMap[MipMapLOD]->unlock();
	}

	//! Returns the size of the largest mipmap.
	f32 getLODFactor( const f32 texArea ) const
	{
		return OrigImageDataSizeInPixels * texArea;
		//return MipMap[0]->getImageDataSizeInPixels () * texArea;
	}

	//! returns unoptimized surface
	virtual CImage* getImage() const
	{
		return MipMap[0];
	}

	//! returns texture surface
	virtual CImage* getTexture() const
	{
		return MipMap[MipMapLOD];
	}

	//! Regenerates the mip map levels of the texture. Useful after locking and
	//! modifying the texture
	virtual void regenerateMipMapLevels(void* mipmapData=0) _IRR_OVERRIDE_;

private:
	f32 OrigImageDataSizeInPixels;

	CImage * MipMap[SOFTWARE_DRIVER_2_MIPMAPPING_MAX];

	u32 MipMapLOD;
	u32 Flags;
	ECOLOR_FORMAT OriginalFormat;
};


} // end namespace video
} // end namespace irr

#endif

