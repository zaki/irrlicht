// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_GUI_SPRITE_BANK_H_INCLUDED__
#define __C_GUI_SPRITE_BANK_H_INCLUDED__

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_GUI_

#include "IGUISpriteBank.h"

namespace irr
{

namespace video
{
	class IVideoDriver;
	class ITexture;
}

namespace gui
{

	class IGUIEnvironment;

//! Sprite bank interface.
class CGUISpriteBank : public IGUISpriteBank
{
public:

	CGUISpriteBank(IGUIEnvironment* env);
	virtual ~CGUISpriteBank();

	virtual core::array< core::rect<s32> >& getPositions() _IRR_OVERRIDE_;
	virtual core::array< SGUISprite >& getSprites() _IRR_OVERRIDE_;

	virtual u32 getTextureCount() const _IRR_OVERRIDE_;
	virtual video::ITexture* getTexture(u32 index) const _IRR_OVERRIDE_;
	virtual void addTexture(video::ITexture* texture) _IRR_OVERRIDE_;
	virtual void setTexture(u32 index, video::ITexture* texture) _IRR_OVERRIDE_;

	//! Add the texture and use it for a single non-animated sprite.
	virtual s32 addTextureAsSprite(video::ITexture* texture) _IRR_OVERRIDE_;

	//! clears sprites, rectangles and textures
	virtual void clear() _IRR_OVERRIDE_;

	//! Draws a sprite in 2d with position and color
	virtual void draw2DSprite(u32 index, const core::position2di& pos, const core::rect<s32>* clip=0,
				const video::SColor& color= video::SColor(255,255,255,255),
				u32 starttime=0, u32 currenttime=0, bool loop=true, bool center=false) _IRR_OVERRIDE_;

	//! Draws a sprite batch in 2d using an array of positions and a color
	virtual void draw2DSpriteBatch(const core::array<u32>& indices, const core::array<core::position2di>& pos,
			const core::rect<s32>* clip=0,
			const video::SColor& color= video::SColor(255,255,255,255),
			u32 starttime=0, u32 currenttime=0,
			bool loop=true, bool center=false) _IRR_OVERRIDE_;

protected:

	struct SDrawBatch
	{
		core::array<core::position2di> positions;
		core::array<core::recti> sourceRects;
		u32 textureNumber;
	};

	core::array<SGUISprite> Sprites;
	core::array< core::rect<s32> > Rectangles;
	core::array<video::ITexture*> Textures;
	IGUIEnvironment* Environment;
	video::IVideoDriver* Driver;

};

} // end namespace gui
} // end namespace irr

#endif // _IRR_COMPILE_WITH_GUI_

#endif // __C_GUI_SPRITE_BANK_H_INCLUDED__

