// Copyright (C) 2009-2010 Amundis
// Heavily based on the OpenGL driver implemented by Nikolaus Gebhardt
// and OpenGL ES driver implemented by Christian Stehno
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#include "IrrCompileConfig.h"

#ifdef _IRR_COMPILE_WITH_OGLES2_

#include "irrTypes.h"
#include "COGLES2Texture.h"
#include "COGLES2Driver.h"
#include "os.h"
#include "CImage.h"
#include "CColorConverter.h"
#include "irrString.h"

#if !defined(_IRR_COMPILE_WITH_IPHONE_DEVICE_)
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <EGL/egl.h>
#endif

namespace
{
#ifndef GL_BGRA
// we need to do this for the IMG_BGRA8888 extension
int GL_BGRA = GL_RGBA;
#endif
}

namespace irr
{
namespace video
{

	//! constructor for usual textures
	COGLES2Texture::COGLES2Texture( IImage* origImage, const io::path& name, COGLES2Driver* driver )
			: ITexture( name ), Driver( driver ), Image( 0 ),
			TextureName( 0 ), InternalFormat( GL_RGBA ),
			PixelFormat(GL_BGRA), PixelType( GL_UNSIGNED_BYTE ),
			HasMipMaps( true ), IsRenderTarget( false ), AutomaticMipmapUpdate( false ),
			UseStencil( false ), ReadOnlyLock( false )
	{
#ifdef _DEBUG
		setDebugName( "COGLES2Texture" );
#endif

		HasMipMaps = Driver->getTextureCreationFlag( ETCF_CREATE_MIP_MAPS );
		getImageData( origImage );

		if ( Image )
		{
			glGenTextures( 1, &TextureName );
			copyTexture();
		}
	}


	//! constructor for basic setup (only for derived classes)
	COGLES2Texture::COGLES2Texture( const io::path& name, COGLES2Driver* driver )
			: ITexture( name ), Driver( driver ), Image( 0 ),
			TextureName( 0 ), InternalFormat( GL_RGBA ), PixelFormat( GL_RGBA ),
			PixelType( GL_UNSIGNED_BYTE ),
			HasMipMaps( true ), IsRenderTarget( false ), AutomaticMipmapUpdate( false ),
			ReadOnlyLock( false )
	{
#ifdef _DEBUG
		setDebugName( "COGLES2Texture" );
#endif
	}


	//! destructor
	COGLES2Texture::~COGLES2Texture()
	{
		glDeleteTextures( 1, &TextureName );
		if ( Image )
			Image->drop();
	}


	ECOLOR_FORMAT COGLES2Texture::getBestColorFormat( ECOLOR_FORMAT format )
	{
		ECOLOR_FORMAT destFormat = ECF_A8R8G8B8;
		switch ( format )
		{
			case ECF_A1R5G5B5:
				if ( !Driver->getTextureCreationFlag( ETCF_ALWAYS_32_BIT ) )
					destFormat = ECF_A1R5G5B5;
				break;
			case ECF_R5G6B5:
				if ( !Driver->getTextureCreationFlag( ETCF_ALWAYS_32_BIT ) )
					destFormat = ECF_A1R5G5B5;
				break;
			case ECF_A8R8G8B8:
				if ( Driver->getTextureCreationFlag( ETCF_ALWAYS_16_BIT ) ||
					 Driver->getTextureCreationFlag( ETCF_OPTIMIZED_FOR_SPEED ) )
					destFormat = ECF_A1R5G5B5;
				break;
			case ECF_R8G8B8:
				if ( Driver->getTextureCreationFlag( ETCF_ALWAYS_16_BIT ) ||
					 Driver->getTextureCreationFlag( ETCF_OPTIMIZED_FOR_SPEED ) )
					destFormat = ECF_A1R5G5B5;
				break;
			default:
				destFormat = ECF_A8R8G8B8;
				break;
		}
		if ( Driver->getTextureCreationFlag( ETCF_NO_ALPHA_CHANNEL ) )
		{
			switch ( destFormat )
			{
				case ECF_A1R5G5B5:
					destFormat = ECF_R5G6B5;
					break;
				case ECF_A8R8G8B8:
					destFormat = ECF_R8G8B8;
					break;
				default:
					break;
			}
		}
		return destFormat;
	}


	void COGLES2Texture::getImageData( IImage* image )
	{
		if ( !image )
		{
			os::Printer::log( "No image for OGLES2 texture.", ELL_ERROR );
			return;
		}

		ImageSize = image->getDimension();

		if ( !ImageSize.Width || !ImageSize.Height )
		{
			os::Printer::log( "Invalid size of image for OGLES2 Texture.", ELL_ERROR );
			return;
		}

		const core::dimension2d<u32> nImageSize = ImageSize.getOptimalSize( !Driver->queryFeature( EVDF_TEXTURE_NPOT ) );
		const ECOLOR_FORMAT destFormat = getBestColorFormat( image->getColorFormat() );

		Image = new CImage( destFormat, nImageSize );
		// copy texture
		image->copyToScaling( Image );
	}


	//! copies the texture into an opengl-es2 texture.
	void COGLES2Texture::copyTexture( bool newTexture )
	{
#ifndef GL_BGRA
		// whoa, pretty badly implemented extension...
		if (Driver->queryOpenGLFeature(COGLES2ExtensionHandler::IRR_IMG_texture_format_BGRA8888) ||
			Driver->queryOpenGLFeature(COGLES2ExtensionHandler::IRR_EXT_texture_format_BGRA8888))
			GL_BGRA = 0x80E1;
		else
			GL_BGRA = GL_RGBA;
#endif
		if ( !Image )
		{
			os::Printer::log( "No image for OGLES2 texture to upload", ELL_ERROR );
			return;
		}

		void( *convert )( const void*, s32, void* ) = 0;
		switch ( Image->getColorFormat() )
		{
			case ECF_A1R5G5B5:
				InternalFormat = GL_RGBA;
				PixelFormat = GL_RGBA;
				PixelType = GL_UNSIGNED_SHORT_5_5_5_1;
				convert = CColorConverter::convert_A1R5G5B5toR5G5B5A1;
				break;
			case ECF_R5G6B5:
				InternalFormat = GL_RGB;
				PixelFormat = GL_RGB;
				PixelType = GL_UNSIGNED_SHORT_5_6_5;
				break;
			case ECF_R8G8B8:
				InternalFormat = GL_RGB;
				PixelFormat = GL_RGB;
				PixelType = GL_UNSIGNED_BYTE;
				convert = CColorConverter::convert_R8G8B8toB8G8R8;
				break;
			case ECF_A8R8G8B8:
				PixelType = GL_UNSIGNED_BYTE;
				if (Driver->queryOpenGLFeature(COGLES2ExtensionHandler::IRR_IMG_texture_format_BGRA8888) ||
					Driver->queryOpenGLFeature( COGLES2ExtensionHandler::IRR_EXT_texture_format_BGRA8888))
				{
					InternalFormat = GL_BGRA;
					PixelFormat = GL_BGRA;
				}
				else
				{
					convert = CColorConverter::convert_A8R8G8B8toA8B8G8R8;
					InternalFormat = GL_RGBA;
					PixelFormat = GL_RGBA;
				}
				break;
			default:
				os::Printer::log( "Unsupported texture format", ELL_ERROR );
				break;
		}

		glBindTexture( GL_TEXTURE_2D, TextureName );
		if ( Driver->testGLError() )
			os::Printer::log( "Could not bind Texture", ELL_ERROR );

		if ( newTexture )
		{
			// enable bilinear filter without mipmaps
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		}

		void* source = 0;
		IImage* tmpImage = 0;
		source = Image->lock();
		if ( convert )
		{
			tmpImage = new CImage( Image->getColorFormat(), Image->getDimension() );
			void* dest = tmpImage->lock();
			convert( source, Image->getDimension().getArea(), dest );
			Image->unlock();
			source = dest;
		}
		if ( newTexture )
			glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, Image->getDimension().Width,
						Image->getDimension().Height, 0, PixelFormat, PixelType, source);
		else
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, Image->getDimension().Width,
							 Image->getDimension().Height, PixelFormat, PixelType, source);
		if ( convert )
		{
			tmpImage->unlock();
			tmpImage->drop();
		}
		else
			Image->unlock();

		if ( Driver->testGLError() )
			os::Printer::log( "Could not glTexImage2D", ELL_ERROR );

		if ( newTexture )
		{
#ifndef DISABLE_MIPMAPPING
			{
				AutomaticMipmapUpdate = false;
				regenerateMipMapLevels();
			}
#else
			HasMipMaps = false;
			os::Printer::log( "Did not create OGLES2 texture mip maps.", ELL_ERROR );
#endif
		}
	}


	//! lock function
	/** TODO: support miplevel */
	void* COGLES2Texture::lock(E_TEXTURE_LOCK_MODE mode, u32 mipmapLevel)
	{
		ReadOnlyLock |= (mode==ETLM_READ_ONLY);

		if ( !Image )
			Image = new CImage( ECF_A8R8G8B8, ImageSize );
		if (mode != ETLM_WRITE_ONLY)
		{
			u8* pPixels = static_cast<u8*>( Image->lock() );
			if ( !pPixels )
			{
				return 0;
			}
			// we need to keep the correct texture bound...
			GLint tmpTexture;
			glGetIntegerv( GL_TEXTURE_BINDING_2D, &tmpTexture );
			glBindTexture( GL_TEXTURE_2D, TextureName );

			// TODO ogl-es
			// glGetTexImage(GL_TEXTURE_2D, 0, GL_BGRA, GL_UNSIGNED_BYTE, pPixels);

			// opengl images are horizontally flipped, so we have to fix that here.
			const u32 pitch = Image->getPitch();
			u8* p2 = pPixels + ( ImageSize.Height - 1 ) * pitch;
			u8* tmpBuffer = new u8[pitch];
			for ( u32 i = 0; i < ImageSize.Height; i += 2 )
			{
				memcpy( tmpBuffer, pPixels, pitch );
				memcpy( pPixels, p2, pitch );
				memcpy( p2, tmpBuffer, pitch );
				pPixels += pitch;
				p2 -= pitch;
			}
			delete [] tmpBuffer;
			Image->unlock();

			//reset old bound texture
			glBindTexture( GL_TEXTURE_2D, tmpTexture );
		}
		return Image->lock();
	}


	//! unlock function
	void COGLES2Texture::unlock()
	{
		Image->unlock();
		if ( !ReadOnlyLock )
			copyTexture( false );
		ReadOnlyLock = false;
	}


	//! Returns size of the original image.
	const core::dimension2d<u32>& COGLES2Texture::getOriginalSize() const
	{
		return ImageSize;
	}


	//! Returns size of the texture.
	const core::dimension2d<u32>& COGLES2Texture::getSize() const
	{
		if ( Image )
			return Image->getDimension();
		else
			return ImageSize;
	}


	//! returns driver type of texture, i.e. the driver, which created the texture
	E_DRIVER_TYPE COGLES2Texture::getDriverType() const
	{
		return EDT_OGLES2;
	}


	//! returns color format of texture
	ECOLOR_FORMAT COGLES2Texture::getColorFormat() const
	{
		if ( Image )
			return Image->getColorFormat();
		else
			return ECF_A8R8G8B8;
	}


	//! returns pitch of texture (in bytes)
	u32 COGLES2Texture::getPitch() const
	{
		if ( Image )
			return Image->getPitch();
		else
			return 0;
	}


	//! return open gl texture name
	GLuint COGLES2Texture::getOGLES2TextureName() const
	{
		return TextureName;
	}


	//! Returns whether this texture has mipmaps
	bool COGLES2Texture::hasMipMaps() const
	{
		return HasMipMaps;
	}


	//! Regenerates the mip map levels of the texture.
	void COGLES2Texture::regenerateMipMapLevels(void* mipmapData)
	{
		if ( AutomaticMipmapUpdate || !HasMipMaps )
			return;
		if (( Image->getDimension().Width == 1 ) && ( Image->getDimension().Height == 1 ) )
			return;
		if (!mipmapData)
		{
			glGenerateMipmap(GL_TEXTURE_2D);
			return;
		}

		// Manually create mipmaps
		u32 width = Image->getDimension().Width;
		u32 height = Image->getDimension().Height;
		u32 i = 0;
		u8* target = static_cast<u8*>(mipmapData);
		do
		{
			if ( width > 1 )
				width >>= 1;
			if ( height > 1 )
				height >>= 1;
			++i;
			glTexImage2D(GL_TEXTURE_2D, i, InternalFormat, width, height,
					0, PixelFormat, PixelType, mipmapData);
			// get next prepared mipmap data if available
			if (mipmapData)
			{
				mipmapData = static_cast<u8*>(mipmapData)+width*height*Image->getBytesPerPixel();
				target = static_cast<u8*>(mipmapData);
			}
		}
		while ( width != 1 || height != 1 );
	}


	bool COGLES2Texture::isRenderTarget() const
	{
		return IsRenderTarget;
	}


	bool COGLES2Texture::isFrameBufferObject() const
	{
		return false;
	}


	void COGLES2Texture::setIsRenderTarget( bool isTarget )
	{
		IsRenderTarget = isTarget;
	}


	//! Bind Render Target Texture
	void COGLES2Texture::bindRTT()
	{
		glViewport( 0, 0, getSize().Width, getSize().Height );
	}


	//! Unbind Render Target Texture
	void COGLES2Texture::unbindRTT()
	{
		glBindTexture( GL_TEXTURE_2D, getOGLES2TextureName() );

		// Copy Our ViewPort To The Texture
		glCopyTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, 0, 0, getSize().Width, getSize().Height );
	}

	/* FBO Textures */

	// helper function for render to texture
	static bool checkFBOStatus( COGLES2Driver* Driver );

	//! RTT ColorFrameBuffer constructor
	COGLES2FBOTexture::COGLES2FBOTexture( const core::dimension2d<u32>& size,
			const io::path& name, COGLES2Driver* driver, ECOLOR_FORMAT format )
		: COGLES2Texture( name, driver ), DepthTexture( 0 ), ColorFrameBuffer( 0 )
	{
#ifdef _DEBUG
		setDebugName( "COGLES2Texture_FBO" );
#endif

		ECOLOR_FORMAT col = getBestColorFormat( format );
		switch ( col )
		{
			case ECF_A8R8G8B8:
				InternalFormat = GL_RGBA;
				PixelFormat = GL_RGBA;
				PixelType = GL_UNSIGNED_BYTE;
				break;
			case ECF_R8G8B8:
				InternalFormat = GL_RGB;
				PixelFormat = GL_RGB;
				PixelType = GL_UNSIGNED_BYTE;
				break;
			case ECF_A1R5G5B5:
				InternalFormat = GL_RGBA;
				PixelFormat = GL_RGBA;
				PixelType = GL_UNSIGNED_SHORT_5_5_5_1;
				break;
			case ECF_R5G6B5:
				InternalFormat = GL_RGB;
				PixelFormat = GL_RGB;
				PixelType = GL_UNSIGNED_SHORT_5_6_5;
				break;
			default:
				os::Printer::log( "color format not handled", ELL_WARNING );
				break;
		}
		ImageSize = size;
		HasMipMaps = false;
		IsRenderTarget = true;

		// generate frame buffer
		glGenFramebuffers( 1, &ColorFrameBuffer );
		glBindFramebuffer( GL_FRAMEBUFFER, ColorFrameBuffer );

		// generate color texture
		glGenTextures(1, &TextureName);
		glBindTexture(GL_TEXTURE_2D, TextureName);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, ImageSize.Width,
				ImageSize.Height, 0, PixelFormat, PixelType, 0);

		// attach color texture to frame buffer
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
				GL_TEXTURE_2D, TextureName, 0);
		// check the status
		if ( !checkFBOStatus( Driver ) )
		{
			os::Printer::log( "FBO incomplete" );
		}
		unbindRTT();
	}


	//! destructor
	COGLES2FBOTexture::~COGLES2FBOTexture()
	{
		if ( DepthTexture )
			if ( DepthTexture->drop() )
				Driver->removeDepthTexture( DepthTexture );
		if ( ColorFrameBuffer )
			Driver->deleteFramebuffers( 1, &ColorFrameBuffer );
	}


	bool COGLES2FBOTexture::isFrameBufferObject() const
	{
		return true;
	}


	//! Bind Render Target Texture
	void COGLES2FBOTexture::bindRTT()
	{
		if ( ColorFrameBuffer != 0 )
			glBindFramebuffer( GL_FRAMEBUFFER, ColorFrameBuffer );
	}


	//! Unbind Render Target Texture
	void COGLES2FBOTexture::unbindRTT()
	{
		if ( ColorFrameBuffer != 0 )
			glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	}


	/* FBO Depth Textures */

	//! RTT DepthBuffer constructor
	COGLES2FBODepthTexture::COGLES2FBODepthTexture(
			const core::dimension2d<u32>& size, const io::path& name,
			COGLES2Driver* driver, bool useStencil)
		: COGLES2FBOTexture(size, name, driver), DepthRenderBuffer(0),
		StencilRenderBuffer(0), UseStencil(useStencil)
	{
#ifdef _DEBUG
		setDebugName( "COGLES2TextureFBO_Depth" );
#endif

#if defined(GL_OES_depth24)
		if (Driver->queryOpenGLFeature(COGLES2ExtensionHandler::IRR_OES_depth24))
			InternalFormat = GL_DEPTH_COMPONENT24_OES;
		else
#endif
#if defined(GL_OES_depth32)
		if (Driver->queryOpenGLFeature(COGLES2ExtensionHandler::IRR_OES_depth32))
			InternalFormat = GL_DEPTH_COMPONENT32_OES;
		else
#endif
		InternalFormat = GL_DEPTH_COMPONENT16;
		PixelFormat = GL_RGB;
		PixelType = GL_UNSIGNED_BYTE;
		HasMipMaps = false;

		if ( UseStencil )
		{
			glGenRenderbuffers( 1, &StencilRenderBuffer );
			glBindRenderbuffer( GL_RENDERBUFFER, StencilRenderBuffer );
#ifdef GL_OES_packed_depth_stencil
			if (Driver->queryOpenGLFeature( COGLES2ExtensionHandler::IRR_OES_packed_depth_stencil))
			{
				// generate packed depth stencil texture
				glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8_OES,
					ImageSize.Width, ImageSize.Height);
				StencilRenderBuffer = DepthRenderBuffer; // stencil is packed with depth
				return;
			}
#endif
			// generate stencil buffer
			GLenum internalf = GL_STENCIL_INDEX8;
#if 0 // only of use if we can reduce the stencil precision by parameters
#if defined(GL_OES_stencil4)
			if (Driver->queryOpenGLFeature(COGLES2ExtensionHandler::IRR_OES_stencil4))
				internalf=GL_STENCIL_INDEX4_OES;
#endif
#if defined(GL_OES_stencil1)
			if (Driver->queryOpenGLFeature(COGLES2ExtensionHandler::IRR_OES_stencil1))
				internalf=GL_STENCIL_INDEX1_OES;
#endif
#endif
			glRenderbufferStorage(GL_RENDERBUFFER, internalf,
				ImageSize.Width, ImageSize.Height);
		}
		// generate depth buffer
		glGenRenderbuffers(1, &DepthRenderBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, DepthRenderBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER,
			InternalFormat, ImageSize.Width, ImageSize.Height);
	}


	//! destructor
	COGLES2FBODepthTexture::~COGLES2FBODepthTexture()
	{
		Driver->deleteRenderbuffers(1, &DepthRenderBuffer);
		if ( StencilRenderBuffer && (StencilRenderBuffer != DepthRenderBuffer))
			Driver->deleteRenderbuffers(1, &StencilRenderBuffer);
	}


	//! combine depth texture and rtt
	bool COGLES2FBODepthTexture::attach(ITexture* renderTex)
	{
		if ( !renderTex )
			return false;
		video::COGLES2FBOTexture* rtt = static_cast<video::COGLES2FBOTexture*>( renderTex );
		rtt->bindRTT();
		if ( UseStencil )
		{
			// attach stencil texture to stencil buffer
			glFramebufferRenderbuffer(GL_FRAMEBUFFER,
					GL_STENCIL_ATTACHMENT,
					GL_RENDERBUFFER, StencilRenderBuffer);

		}
		// attach depth renderbuffer to depth buffer
		glFramebufferRenderbuffer(GL_FRAMEBUFFER,
				GL_DEPTH_ATTACHMENT,
				GL_RENDERBUFFER, DepthRenderBuffer);
		// check the status
		if ( !checkFBOStatus( Driver ) )
		{
			os::Printer::log( "FBO incomplete" );
			return false;
		}
		rtt->DepthTexture = this;
		grab(); // grab the depth buffer, not the RTT
		rtt->unbindRTT();
		return true;
	}


	//! Bind Render Target Texture
	void COGLES2FBODepthTexture::bindRTT()
	{
	}


	//! Unbind Render Target Texture
	void COGLES2FBODepthTexture::unbindRTT()
	{
	}


	bool checkFBOStatus( COGLES2Driver* Driver )
	{
		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

		switch ( status )
		{
			//Our FBO is perfect, return true
			case GL_FRAMEBUFFER_COMPLETE:
				return true;

			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
				os::Printer::log( "FBO has one or several incomplete image attachments", ELL_ERROR );
				break;

			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
				os::Printer::log( "FBO missing an image attachment", ELL_ERROR );
				break;

			case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
				os::Printer::log( "FBO has one or several image attachments with different dimensions", ELL_ERROR );
				break;

#ifdef GL_FRAMEBUFFER_INCOMPLETE_FORMATS
			case GL_FRAMEBUFFER_INCOMPLETE_FORMATS:
				os::Printer::log( "FBO has one or several image attachments with different internal formats", ELL_ERROR );
				break;
#endif

#ifdef GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER
			// not part of all implementations
			case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
				os::Printer::log( "FBO has invalid draw buffer", ELL_ERROR );
				break;
#endif

#ifdef GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER
			// not part of all implementations
			case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
				os::Printer::log( "FBO has invalid read buffer", ELL_ERROR );
				break;
#endif

#ifdef GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT
			// not part of fbo_object anymore, but won't harm as it is just a return value
			case GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT:
				os::Printer::log( "FBO has a duplicate image attachment", ELL_ERROR );
				break;
#endif

			case GL_FRAMEBUFFER_UNSUPPORTED:
				os::Printer::log( "FBO format unsupported", ELL_ERROR );
				break;

			default:
				break;
		}
		os::Printer::log( "FBO error", ELL_ERROR );
		return false;
	}

} // end namespace video
} // end namespace irr

#endif // _IRR_COMPILE_WITH_OGLES2_

