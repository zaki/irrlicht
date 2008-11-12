// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_OGLES1_TEXTURE_H_INCLUDED__
#define __C_OGLES1_TEXTURE_H_INCLUDED__

#include "ITexture.h"
#include "IImage.h"

#include "IrrCompileConfig.h"
#if defined(_IRR_COMPILE_WITH_OGLES1_)

#include <GLES/egl.h>

namespace irr
{
namespace video
{

class COGLES1Driver;
//! OGLES1 texture.
class COGLES1Texture : public ITexture
{
public:

	//! constructor
	COGLES1Texture(IImage* surface, const char* name, COGLES1Driver* driver=0);

	//! destructor
	virtual ~COGLES1Texture();

	//! lock function
	virtual void* lock(bool readOnly = false);

	//! unlock function
	virtual void unlock();

	//! Returns original size of the texture (image).
	virtual const core::dimension2d<s32>& getOriginalSize() const;

	//! Returns size of the texture.
	virtual const core::dimension2d<s32>& getSize() const;

	//! returns driver type of texture (=the driver, that created it)
	virtual E_DRIVER_TYPE getDriverType() const;

	//! returns color format of texture
	virtual ECOLOR_FORMAT getColorFormat() const;

	//! returns pitch of texture (in bytes)
	virtual u32 getPitch() const;

	//! return open gl texture name
	GLuint getOGLES1TextureName() const;

	//! return whether this texture has mipmaps
	virtual bool hasMipMaps() const;

	//! Regenerates the mip map levels of the texture.
	virtual void regenerateMipMapLevels();

	//! Is it a render target?
	virtual bool isRenderTarget() const;

	//! Bind RenderTargetTexture
	void bindRTT();

	//! Unbind RenderTargetTexture
	void unbindRTT();

	//! sets whether this texture is intended to be used as a render target.
	void setIsRenderTarget(bool isTarget);

private:

	//! get the desired color format based on texture creation flags and the input format.
	ECOLOR_FORMAT getBestColorFormat(ECOLOR_FORMAT format);

	//! convert the image into an internal image with better properties for this driver.
	void getImageData(IImage* image);

	//! copies the the texture into an open gl texture.
	void copyTexture(bool newTexture=true);

	//! returns the size of a texture which would be optimal for rendering
	inline s32 getTextureSizeFromSurfaceSize(s32 size) const;

	core::dimension2d<s32> ImageSize;
	COGLES1Driver* Driver;
	IImage* Image;

	GLuint TextureName;
	GLint InternalFormat;
	GLenum PixelFormat;
	GLenum PixelType;

	bool HasMipMaps;
	bool IsRenderTarget;
	bool AutomaticMipmapUpdate;
	bool UseStencil;
	bool ReadOnlyLock;
};


} // end namespace video
} // end namespace irr

#endif
#endif // _IRR_COMPILE_WITH_OGLES1_
