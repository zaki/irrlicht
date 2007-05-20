// Copyright (C) 2002-2007 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_GUI_IMAGE_H_INCLUDED__
#define __I_GUI_IMAGE_H_INCLUDED__

#include "IGUIElement.h"

namespace irr
{
namespace video
{
	class ITexture;
}
namespace gui
{

	//! GUI element displaying an image.
	class IGUIImage : public IGUIElement
	{
	public:

		//! constructor
		IGUIImage(IGUIEnvironment* environment, IGUIElement* parent, s32 id, core::rect<s32> rectangle)
			: IGUIElement(EGUIET_IMAGE, environment, parent, id, rectangle) {}

		//! destructor
		~IGUIImage() {};

		//! sets an image
		virtual void setImage(video::ITexture* image) = 0;

		//! sets the colour of the image
		virtual void setColor(video::SColor color) = 0;

		//! sets if the image should scale to fit the element
		virtual void setScaleImage(bool scale) = 0;

		//! sets if the image should use its alpha channel to draw itself
		virtual void setUseAlphaChannel(bool use) = 0;
	};


} // end namespace gui
} // end namespace irr

#endif

