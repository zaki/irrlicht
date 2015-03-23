// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_DIRECTX9_TEXTURE_H_INCLUDED__
#define __C_DIRECTX9_TEXTURE_H_INCLUDED__

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_DIRECT3D_9_

#include "ITexture.h"
#include "IImage.h"
#if defined(__BORLANDC__) || defined (__BCPLUSPLUS__)
#include "irrMath.h"    // needed by borland for sqrtf define
#endif
#include <d3d9.h>

namespace irr
{
namespace video
{

class CD3D9Driver;
// forward declaration for RTT depth buffer handling
struct SDepthSurface;
/*!
	interface for a Video Driver dependent Texture.
*/
class CD3D9Texture : public ITexture
{
public:

	//! constructor
	CD3D9Texture(IImage* image, CD3D9Driver* driver,
			u32 flags, const io::path& name, void* mipmapData=0);

	//! rendertarget constructor
	CD3D9Texture(CD3D9Driver* driver, const core::dimension2d<u32>& size, const io::path& name,
		const ECOLOR_FORMAT format = ECF_UNKNOWN);

	//! destructor
	virtual ~CD3D9Texture();

	//! lock function
	virtual void* lock(E_TEXTURE_LOCK_MODE mode=ETLM_READ_WRITE, u32 mipmapLevel=0) _IRR_OVERRIDE_;

	//! unlock function
	virtual void unlock() _IRR_OVERRIDE_;

	//! Regenerates the mip map levels of the texture. Useful after locking and
	//! modifying the texture
	virtual void regenerateMipMapLevels(void* mipmapData = 0) _IRR_OVERRIDE_;

	//! returns the DIRECT3D9 Texture
	IDirect3DTexture9* getDX9Texture() const;

private:
	friend class CD3D9Driver;

	void createRenderTarget(const ECOLOR_FORMAT format = ECF_UNKNOWN);

	//! creates the hardware texture
	bool createTexture(u32 flags, IImage * image);

	//! copies the image to the texture
	bool copyTexture(IImage * image);

	//! Helper function for mipmap generation.
	bool createMipMaps(u32 level=1);

	//! Helper function for mipmap generation.
	void copy16BitMipMap(char* src, char* tgt,
			const s32 srcWidth, const s32 srcHeight,
			const s32 width, const s32 height,
			const s32 pitchsrc, const s32 pitchtgt) const;

	//! Helper function for mipmap generation.
	void copy32BitMipMap(char* src, char* tgt,
			const s32 srcWidth, const s32 srcHeight,
			const s32 width, const s32 height,
			const s32 pitchsrc, const s32 pitchtgt) const;

	//! set Pitch based on the d3d format
	void setPitch(D3DFORMAT d3dformat);

	IDirect3DDevice9* Device;
	IDirect3DTexture9* Texture;
	IDirect3DSurface9* RTTSurface;
	CD3D9Driver* Driver;
	u32 MipLevelLocked;

	bool HardwareMipMaps;
	bool IsCompressed;
};


} // end namespace video
} // end namespace irr

#endif // _IRR_COMPILE_WITH_DIRECT3D_9_

#endif // __C_DIRECTX9_TEXTURE_H_INCLUDED__


