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

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <EGL/egl.h>

namespace irr
{
    namespace video
    {

//! constructor for usual textures
        COGLES2Texture::COGLES2Texture( IImage* origImage, const io::path& name, COGLES2Driver* driver )
                : ITexture( name ), Driver( driver ), Image( 0 ),
                TextureName( 0 ), InternalFormat( GL_RGBA ), PixelFormat( GL_RGBA ),
                // TODO ogl-es
                // PixelFormat(GL_BGRA),
                PixelType( GL_UNSIGNED_BYTE ),
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


//! copies the the texture into an open gl texture.
        void COGLES2Texture::copyTexture( bool newTexture )
        {
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
                    if ( !Driver->queryOpenGLFeature( COGLES2ExtensionHandler::IRR_IMG_texture_format_BGRA8888 ) && !Driver->queryOpenGLFeature( COGLES2ExtensionHandler::IRR_EXT_texture_format_BGRA8888 ) )
                    {
                        convert = CColorConverter::convert_A8R8G8B8toA8B8G8R8;
                        InternalFormat = GL_RGBA;
                        PixelFormat = GL_RGBA;
                    }
                    else
                    {
                        InternalFormat = GL_BGRA;
                        PixelFormat = GL_BGRA;
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
#ifndef DISABLE_MIPMAPPING
                {
                    AutomaticMipmapUpdate = false;
                    regenerateMipMapLevels();
                }
#else
                HasMipMaps = false;
                os::Printer::log( "Did not create OGLES2 texture mip maps.", ELL_ERROR );
#endif
                {
                    // enable bilinear filter without mipmaps
                    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
                    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
                }
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
                glTexImage2D( GL_TEXTURE_2D, 0, InternalFormat, Image->getDimension().Width,
                              Image->getDimension().Height, 0, PixelFormat, PixelType, source );
            else
                glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, Image->getDimension().Width,
                                 Image->getDimension().Height, PixelFormat, PixelType, source );
            if ( convert )
            {
                tmpImage->unlock();
                tmpImage->drop();
            }
            else
                Image->unlock();

            if ( Driver->testGLError() )
                os::Printer::log( "Could not glTexImage2D", ELL_ERROR );
        }


		//! lock function
		/** TODO: support miplevel */
        void* COGLES2Texture::lock( bool readOnly, u32 mipmapLevel )
        {
            ReadOnlyLock |= readOnly;

            if ( !Image )
                Image = new CImage( ECF_A8R8G8B8, ImageSize );
            if ( IsRenderTarget )
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
                //  glGetTexImage(GL_TEXTURE_2D, 0, GL_BGRA, GL_UNSIGNED_BYTE, pPixels);

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

            // Manually create mipmaps
            u32 width = Image->getDimension().Width;
            u32 height = Image->getDimension().Height;
            u32 i = 0;
            u8* target = new u8[Image->getImageDataSizeInBytes()];
            do
            {
                if ( width > 1 )
                    width >>= 1;
                if ( height > 1 )
                    height >>= 1;
                ++i;
                Image->copyToScaling( target, width, height, Image->getColorFormat() );
                glTexImage2D( GL_TEXTURE_2D, i, InternalFormat, width, height,
                              0, PixelFormat, PixelType, target );
            }
            while ( width != 1 || height != 1 );
            delete [] target;
            Image->unlock();
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

#ifdef GL_OES_framebuffer_object
// helper function for render to texture
        static bool checkFBOStatus( COGLES2Driver* Driver );
#endif


//! RTT ColorFrameBuffer constructor
        COGLES2FBOTexture::COGLES2FBOTexture( const core::dimension2d<u32>& size,
                                              const io::path& name,
                                              COGLES2Driver* driver, ECOLOR_FORMAT format )
                : COGLES2Texture( name, driver ), DepthTexture( 0 ), ColorFrameBuffer( 0 )
        {
#ifdef _DEBUG
            setDebugName( "COGLES2Texture_FBO" );
#endif

            ECOLOR_FORMAT col = getBestColorFormat( format );
            switch ( col )
            {
                case ECF_A8R8G8B8:
#ifdef GL_OES_rgb8_rgba8
                    if ( driver->queryOpenGLFeature( video::COGLES2ExtensionHandler::IRR_OES_rgb8_rgba8 ) )
                        InternalFormat = GL_RGBA8_OES;
                    else
#endif
                        InternalFormat = GL_RGB5_A1;
                    break;
                case ECF_R8G8B8:
#ifdef GL_OES_rgb8_rgba8
                    if ( driver->queryOpenGLFeature( video::COGLES2ExtensionHandler::IRR_OES_rgb8_rgba8 ) )
                        InternalFormat = GL_RGB8_OES;
                    else
#endif
                        InternalFormat = GL_RGB565;
                    break;
                case ECF_A1R5G5B5:
                    InternalFormat = GL_RGB5_A1;
                    break;
                case ECF_R5G6B5:
                    InternalFormat = GL_RGB565;
                    break;
                default:
                    os::Printer::log( "color format not handled", ELL_WARNING );
                    break;
            }
            PixelFormat = GL_RGBA;
            PixelType = GL_UNSIGNED_BYTE;
            ImageSize = size;
            HasMipMaps = false;
            IsRenderTarget = true;

#ifdef GL_OES_framebuffer_object
            // generate frame buffer
            Driver->extGlGenFramebuffers( 1, &ColorFrameBuffer );
            Driver->extGlBindFramebuffer( GL_FRAMEBUFFER_OES, ColorFrameBuffer );

            // generate color texture
            glGenTextures( 1, &TextureName );
            glBindTexture( GL_TEXTURE_2D, TextureName );
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
            glTexImage2D( GL_TEXTURE_2D, 0, InternalFormat, ImageSize.Width,
                          ImageSize.Height, 0, PixelFormat, PixelType, 0 );

            // attach color texture to frame buffer
            Driver->extGlFramebufferTexture2D( GL_FRAMEBUFFER_OES,
                                               GL_COLOR_ATTACHMENT0_OES,
                                               GL_TEXTURE_2D,
                                               TextureName,
                                               0 );
#endif
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
#ifdef GL_OES_framebuffer_object
            if ( ColorFrameBuffer != 0 )
                Driver->extGlBindFramebuffer( GL_FRAMEBUFFER_OES, ColorFrameBuffer );
#endif
        }


//! Unbind Render Target Texture
        void COGLES2FBOTexture::unbindRTT()
        {
#ifdef GL_OES_framebuffer_object
            if ( ColorFrameBuffer != 0 )
                Driver->extGlBindFramebuffer( GL_FRAMEBUFFER_OES, 0 );
#endif
        }


        /* FBO Depth Textures */

//! RTT DepthBuffer constructor
        COGLES2FBODepthTexture::COGLES2FBODepthTexture(
            const core::dimension2d<u32>& size,
            const io::path& name,
            COGLES2Driver* driver,
            bool useStencil )
                : COGLES2FBOTexture( size, name, driver ), DepthRenderBuffer( 0 ),
                StencilRenderBuffer( 0 ), UseStencil( useStencil )
        {
#ifdef _DEBUG
            setDebugName( "COGLES2TextureFBO_Depth" );
#endif

            ImageSize = size;
#ifdef GL_OES_depth24
            InternalFormat = GL_DEPTH_COMPONENT24_OES;
#elif defined(GL_OES_depth32)
            InternalFormat = GL_DEPTH_COMPONENT32_OES;
#else
            InternalFormat = GL_DEPTH_COMPONENT16_OES;
#endif

            PixelFormat = GL_RGBA;
            PixelType = GL_UNSIGNED_BYTE;
            HasMipMaps = false;

            if ( useStencil )
            {
#ifdef GL_OES_packed_depth_stencil
                glGenTextures( 1, &DepthRenderBuffer );
                glBindTexture( GL_TEXTURE_2D, DepthRenderBuffer );
                glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
                glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
                glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
                if ( Driver->queryOpenGLFeature( COGLES2ExtensionHandler::IRR_OES_packed_depth_stencil ) )
                {
                    // generate packed depth stencil texture
                    glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_STENCIL_OES, ImageSize.Width,
                                  ImageSize.Height, 0, GL_DEPTH_STENCIL_OES, GL_UNSIGNED_INT_24_8_OES, 0 );
                    StencilRenderBuffer = DepthRenderBuffer; // stencil is packed with depth
                    return;
                }
#endif
#if defined(GL_OES_framebuffer_object) && (defined(GL_OES_stencil1) || defined(GL_OES_stencil4) || defined(GL_OES_stencil8))
                // generate stencil buffer
                Driver->extGlGenRenderbuffers( 1, &StencilRenderBuffer );
                Driver->extGlBindRenderbuffer( GL_RENDERBUFFER_OES, StencilRenderBuffer );
                Driver->extGlRenderbufferStorage( GL_RENDERBUFFER_OES,
#if defined(GL_OES_stencil8)
                                                  GL_STENCIL_INDEX8_OES,
#elif defined(GL_OES_stencil4)
                                                  GL_STENCIL_INDEX4_OES,
#elif defined(GL_OES_stencil1)
                                                  GL_STENCIL_INDEX1_OES,
#endif
                                                  ImageSize.Width, ImageSize.Height );
#endif
            }
#ifdef GL_OES_framebuffer_object
            // generate depth buffer
            Driver->extGlGenRenderbuffers( 1, &DepthRenderBuffer );
            Driver->extGlBindRenderbuffer( GL_RENDERBUFFER_OES, DepthRenderBuffer );
            Driver->extGlRenderbufferStorage( GL_RENDERBUFFER_OES,
                                              InternalFormat, ImageSize.Width, ImageSize.Height );
#endif
        }


//! destructor
        COGLES2FBODepthTexture::~COGLES2FBODepthTexture()
        {
            if ( DepthRenderBuffer && UseStencil )
                glDeleteTextures( 1, &DepthRenderBuffer );
            else
                Driver->deleteRenderbuffers( 1, &DepthRenderBuffer );
            if ( StencilRenderBuffer && StencilRenderBuffer != DepthRenderBuffer )
                glDeleteTextures( 1, &StencilRenderBuffer );
        }


//combine depth texture and rtt
        void COGLES2FBODepthTexture::attach( ITexture* renderTex )
        {
            if ( !renderTex )
                return;
            video::COGLES2FBOTexture* rtt = static_cast<video::COGLES2FBOTexture*>( renderTex );
            rtt->bindRTT();
#ifdef GL_OES_framebuffer_object
            if ( UseStencil )
            {
                // attach stencil texture to stencil buffer
                Driver->extGlFramebufferTexture2D( GL_FRAMEBUFFER_OES,
                                                   GL_STENCIL_ATTACHMENT_OES,
                                                   GL_TEXTURE_2D,
                                                   StencilRenderBuffer,
                                                   0 );

                // attach depth texture to depth buffer
                Driver->extGlFramebufferTexture2D( GL_FRAMEBUFFER_OES,
                                                   GL_DEPTH_ATTACHMENT_OES,
                                                   GL_TEXTURE_2D,
                                                   DepthRenderBuffer,
                                                   0 );
            }
            else
            {
                // attach depth renderbuffer to depth buffer
                Driver->extGlFramebufferRenderbuffer( GL_FRAMEBUFFER_OES,
                                                      GL_DEPTH_ATTACHMENT_OES,
                                                      GL_RENDERBUFFER_OES,
                                                      DepthRenderBuffer );
            }
            // check the status
            if ( !checkFBOStatus( Driver ) )
                os::Printer::log( "FBO incomplete" );
#endif
            rtt->DepthTexture = this;
            grab(); // grab the depth buffer, not the RTT
            rtt->unbindRTT();
        }


//! Bind Render Target Texture
        void COGLES2FBODepthTexture::bindRTT()
        {
        }


//! Unbind Render Target Texture
        void COGLES2FBODepthTexture::unbindRTT()
        {
        }


#ifdef GL_OES_framebuffer_object
        bool checkFBOStatus( COGLES2Driver* Driver )
        {
            GLenum status = Driver->extGlCheckFramebufferStatus( GL_FRAMEBUFFER_OES );

            switch ( status )
            {
                    //Our FBO is perfect, return true
                case GL_FRAMEBUFFER_COMPLETE_OES:
                    return true;

                case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_OES:
                    os::Printer::log( "FBO has one or several incomplete image attachments", ELL_ERROR );
                    break;

                case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_OES:
                    os::Printer::log( "FBO missing an image attachment", ELL_ERROR );
                    break;

                case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_OES:
                    os::Printer::log( "FBO has one or several image attachments with different dimensions", ELL_ERROR );
                    break;

                case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_OES:
                    os::Printer::log( "FBO has one or several image attachments with different internal formats", ELL_ERROR );
                    break;

// not part of all implementations
#ifdef GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_OES
                case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_OES:
                    os::Printer::log( "FBO has invalid draw buffer", ELL_ERROR );
                    break;
#endif

// not part of all implementations
#ifdef GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_OES
                case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_OES:
                    os::Printer::log( "FBO has invalid read buffer", ELL_ERROR );
                    break;
#endif

// not part of fbo_object anymore, but won't harm as it is just a return value
#ifdef GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_OES
                case GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_OES:
                    os::Printer::log( "FBO has a duplicate image attachment", ELL_ERROR );
                    break;
#endif

                case GL_FRAMEBUFFER_UNSUPPORTED_OES:
                    os::Printer::log( "FBO format unsupported", ELL_ERROR );
                    break;

                default:
                    break;
            }
            os::Printer::log( "FBO error", ELL_ERROR );
            return false;
        }
#endif

    } // end namespace video
} // end namespace irr

#endif // _IRR_COMPILE_WITH_OGLES21_

