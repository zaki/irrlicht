// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CGUIImage.h"
#ifdef _IRR_COMPILE_WITH_GUI_

#include "IGUISkin.h"
#include "IGUIEnvironment.h"
#include "IVideoDriver.h"

namespace irr
{
namespace gui
{


//! constructor
CGUIImage::CGUIImage(IGUIEnvironment* environment, IGUIElement* parent, s32 id, core::rect<s32> rectangle)
: IGUIImage(environment, parent, id, rectangle), Texture(0), Color(255,255,255,255),
	UseAlphaChannel(false), ScaleImage(false)
{
	#ifdef _DEBUG
	setDebugName("CGUIImage");
	#endif
}


//! destructor
CGUIImage::~CGUIImage()
{
	if (Texture)
		Texture->drop();
}


//! sets an image
void CGUIImage::setImage(video::ITexture* image)
{
	if (image == Texture)
		return;

	if (Texture)
		Texture->drop();

	Texture = image;

	if (Texture)
		Texture->grab();
}

//! Gets the image texture
video::ITexture* CGUIImage::getImage() const
{
	return Texture;
}

//! sets the color of the image
void CGUIImage::setColor(video::SColor color)
{
	Color = color;
}

//! Gets the color of the image
video::SColor CGUIImage::getColor() const
{
	return Color;
}

//! draws the element and its children
void CGUIImage::draw()
{
	if (!IsVisible)
		return;

	IGUISkin* skin = Environment->getSkin();
	video::IVideoDriver* driver = Environment->getVideoDriver();

	if (Texture)
	{
		core::rect<s32> sourceRect(SourceRect);
		if (sourceRect.getWidth() == 0 || sourceRect.getHeight() == 0)
		{
			sourceRect = core::rect<s32>(core::position2d<s32>(0,0), core::dimension2di(Texture->getOriginalSize()));
		}

		if (ScaleImage)
		{
			const video::SColor Colors[] = {Color,Color,Color,Color};

			driver->draw2DImage(Texture, AbsoluteRect, sourceRect,
				&AbsoluteClippingRect, Colors, UseAlphaChannel);
		}
		else
		{
			driver->draw2DImage(Texture, AbsoluteRect.UpperLeftCorner, sourceRect,
				&AbsoluteClippingRect, Color, UseAlphaChannel);
		}
	}
	else
	{
		skin->draw2DRectangle(this, skin->getColor(EGDC_3D_DARK_SHADOW), AbsoluteRect, &AbsoluteClippingRect);
	}

	IGUIElement::draw();
}


//! sets if the image should use its alpha channel to draw itself
void CGUIImage::setUseAlphaChannel(bool use)
{
	UseAlphaChannel = use;
}


//! sets if the image should use its alpha channel to draw itself
void CGUIImage::setScaleImage(bool scale)
{
	ScaleImage = scale;
}


//! Returns true if the image is scaled to fit, false if not
bool CGUIImage::isImageScaled() const
{
	_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
	return ScaleImage;
}

//! Returns true if the image is using the alpha channel, false if not
bool CGUIImage::isAlphaChannelUsed() const
{
	_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
	return UseAlphaChannel;
}

//! Sets the source rectangle of the image. By default the full image is used.
void CGUIImage::setSourceRect(const core::rect<s32>& sourceRect)
{
	SourceRect = sourceRect;
}

//! Returns the customized source rectangle of the image to be used.
core::rect<s32> CGUIImage::getSourceRect() const
{
	return SourceRect;
}


//! Writes attributes of the element.
void CGUIImage::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options=0) const
{
	IGUIImage::serializeAttributes(out,options);

	out->addTexture	("Texture", Texture);
	out->addBool	("UseAlphaChannel", UseAlphaChannel);
	out->addColor	("Color", Color);
	out->addBool	("ScaleImage", ScaleImage);
	out->addRect 	("SourceRect", SourceRect);
}


//! Reads attributes of the element
void CGUIImage::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options=0)
{
	IGUIImage::deserializeAttributes(in,options);

	setImage(in->getAttributeAsTexture("Texture", Texture));
	setUseAlphaChannel(in->getAttributeAsBool("UseAlphaChannel", UseAlphaChannel));
	setColor(in->getAttributeAsColor("Color", Color));
	setScaleImage(in->getAttributeAsBool("ScaleImage", UseAlphaChannel));
	setSourceRect(in->getAttributeAsRect("SourceRect", SourceRect));
}


} // end namespace gui
} // end namespace irr


#endif // _IRR_COMPILE_WITH_GUI_

