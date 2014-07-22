// Copyright (C) 2014 Patryk Nadrowski
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_OGLES2_RENDERER_2D_H_INCLUDED__
#define __C_OGLES2_RENDERER_2D_H_INCLUDED__

#include "IrrCompileConfig.h"

#ifdef _IRR_COMPILE_WITH_OGLES2_

#include "COGLES2MaterialRenderer.h"

namespace irr
{
namespace video
{

class COGLES2Renderer2D : public COGLES2MaterialRenderer
{
public:
	COGLES2Renderer2D(const c8* vertexShaderProgram, const c8* pixelShaderProgram, COGLES2Driver* driver);
	~COGLES2Renderer2D();

	virtual void OnSetMaterial(const SMaterial& material, const SMaterial& lastMaterial,
		bool resetAllRenderstates, IMaterialRendererServices* services);

	virtual bool OnRender(IMaterialRendererServices* service, E_VERTEX_TYPE vtxtype);

	void setTexture(const ITexture* texture);

protected:
	s32 TextureUsageID;

	const ITexture* Texture;
};


}
}

#endif
#endif

