// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#pragma unmanaged
#include "..\\..\\include\\irrlicht.h"
#pragma managed

#include "Dimension2D.h"
#include "edrivertypes.h"
#include "ecolorformat.h"

namespace Irrlicht
{
namespace Video
{

/// <summary> Enumeration flags telling the video driver in which format textures
/// should be created. </summary>
__value public enum TextureCreationFlag
{
	/// <summary> Forces the driver to create 16 bit textures always, indepenent of
	/// which format the file on disk has. When choosing this you may loose
	/// some color detail, but gain much speed and memory. 16 bit textures
	/// can be transferred twice as fast as 32 bit textures and only use 
	/// half of the space in memory.
	/// When using this flag, it does not make sence to use the flags
	/// ETCF_ALWAYS_32_BIT, ETCF_OPTIMIZED_FOR_QUALITY,
	/// or ETCF_OPTIMIZED_FOR_SPEED at the same time. </summary> 
	ALWAYS_16_BIT = 0x00000001,

	/// <summary> Forces the driver to create 32 bit textures always, indepenent of
	/// which format the file on disk has. Please note that some drivers 
	/// (like the software device) will ignore this, because they only are
	/// able to create and use 16 bit textures.
	/// When using this flag, it does not make sence to use the flags
	/// ETCF_ALWAYS_16_BIT, ETCF_OPTIMIZED_FOR_QUALITY,
	/// or ETCF_OPTIMIZED_FOR_SPEED at the same time. </summary> 
	ALWAYS_32_BIT = 0x00000002,

	/// <summary> Lets the driver decide in which format the texutures are created and
	/// tries to make the textures look as good as possible.
	/// Usually it simply chooses the format in which the texture was stored on disk.
	/// When using this flag, it does not make sence to use the flags
	/// ETCF_ALWAYS_16_BIT, ETCF_ALWAYS_32_BIT, 
	/// or ETCF_OPTIMIZED_FOR_SPEED at the same time. </summary> 
	OPTIMIZED_FOR_QUALITY  = 0x00000004,

	/// <summary>  Lets the driver decide in which format the texutures are created and
	/// tries to create them maximizing render speed.
	/// When using this flag, it does not make sence to use the flags
	/// ETCF_ALWAYS_16_BIT, ETCF_ALWAYS_32_BIT, or ETCF_OPTIMIZED_FOR_QUALITY,
	/// at the same time. </summary> 
	OPTIMIZED_FOR_SPEED = 0x00000008,

	/// <summary>  Automaticly creates mip map levels for the textures. </summary> 
	CREATE_MIP_MAPS = 0x00000010,

	/// <summary> This flag is never used, it only forces the compiler to 
    /// compile these enumeration values to 32 bit. </summary>
	FORCE_32_BIT_DO_NOT_USE = 0x7fffffff,
};


/// <summary> 
/// Interface for a Video Driver dependent Texture.
/// An ITexture is created by an IVideoDriver by using IVideoDriver::addTexture or
/// IVideoDriver::getTexture. After this, this texture may only be used by this VideoDriver.
/// As you can imagine, textures of the DirectX and the OpenGL device will not be compatible,
/// for example.
/// An exception is the Software device and the NULL device, their textures are compatible.
/// If you try to use a texture created by one device with an other device, the device
/// will refuse to do that and write a warning or an error message to the output buffer. 
///
/// This class has been ported directly from the native C++ Irrlicht Engine, so it may not 
/// be 100% complete yet and the design may not be 100% .NET like.
/// </summary>
public __gc class ITexture
{
public:

	/// <summary>
	/// You should access ITexture
	/// through the Irrlicht::IrrlichtDevice::VideoDriver::getTexture method. Simply don't use
	/// this constructor.
	///</summary>
	///<param name="driver">The real, unmanaged C++ ITexture</param>
	ITexture(irr::video::ITexture* realTexture);

	//! destructor
	~ITexture();

	/// <summary>
	/// Lock function. Locks the Texture and returns a pointer to access the 
	/// pixels. After lock() has been called and all operations on the pixels
	/// are done, you must call unlock().
	///</summary>
	/// \return Returns a pointer to the pixel data. The format of the pixel can
	/// be determinated by using getColorFormat(). NULL is returned, if
	/// the texture cannot be locked.
	void* Lock();

	/// <summary>
	/// Unlock function. Must be called after a lock() to the texture.
	///</summary>
	void Unlock();

	/// <summary>
	/// Returns original size of the texture. The texture is usually
	/// scaled, if it was created with an unoptimal size. For example if the size
	/// of the texture file it was loaded from was not a power of two. This returns
	/// the size of the texture, it had before it was scaled. Can be useful
	/// when drawing 2d images on the screen, which should have the exact size
	/// of the original texture. Use ITexture::getSize() if you want to know 
	/// the real size it has now stored in the system.
	///</summary>
	/// \return Returns the original size of the texture.
	__property Core::Dimension2D get_OriginalSize();

	/// <summary>
	/// Returns (=size) of the texture.
	///</summary>
	/// \return Returns the size of the texture.
	__property Core::Dimension2D get_Size();

	/// <summary>
	/// Returns driver type of texture. This is the driver, which created the texture.
	/// This method is used internally by the video devices, to check, if they may
	/// use a texture because textures may be incompatible between different
	/// devices.
	///</summary>
	/// \return Returns driver type of texture.
	__property DriverType get_DriverType();

	/// <summary>
	/// Returns the color format of texture. This format is in most cases
	///</summary>
	/// ECF_A1R5G5B5 or ECF_A8R8G8B8.
	/// \return Returns the color format of texture.
	__property ColorFormat get_ColorFormat();

	/// <summary>
	/// Returns pitch of texture (in bytes). The pitch is the amount of bytes
	/// used for a row of pixels in a texture.
	///</summary>
	/// <returns> Returns pitch of texture in bytes.<returns>
	__property System::Int32 get_Pitch();

	/// <summary>
	/// Returns the internal pointer to the native C++ irrlicht texture.
	/// Do not use this, only needed by the internal .NET wrapper.
	///</summary>
	__property irr::video::ITexture* get_NativeTexture();

private:

	irr::video::ITexture* Texture;
};


}
}