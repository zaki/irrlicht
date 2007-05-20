// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "ITexture.h"

namespace Irrlicht
{
namespace Video
{
	ITexture::ITexture(irr::video::ITexture* texture)
		: Texture(texture)
	{
		if (!Texture)
			throw new System::Exception(new System::String("Null pointer set in ITexture."));

		Texture->grab();
	}

	//! destructor
	ITexture::~ITexture()
	{
		Texture->drop();
	}

	void* ITexture::Lock()
	{
		return Texture->lock();
	}

	void ITexture::Unlock()
	{
		Texture->unlock();
	}

	Core::Dimension2D ITexture::get_OriginalSize()
	{
		irr::core::dimension2d<irr::s32> dim = Texture->getOriginalSize();
		return Core::Dimension2D(dim.Width, dim.Height);
	}

	Core::Dimension2D ITexture::get_Size()
	{
		irr::core::dimension2d<irr::s32> dim = Texture->getSize();
		return Core::Dimension2D(dim.Width, dim.Height);
	}

	DriverType ITexture::get_DriverType()
	{
		return (Video::DriverType)Texture->getDriverType();
	}

	ColorFormat ITexture::get_ColorFormat()
	{
		return (Video::ColorFormat)Texture->getColorFormat();
	}

	System::Int32 ITexture::get_Pitch()
	{
		return Texture->getPitch();
	}

	irr::video::ITexture* ITexture::get_NativeTexture()
	{
		return Texture;
	}
}
}
