// Copyright (C) 2015 Patryk Nadrowski
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_OPEN_GL_RENDER_TARGET_H_INCLUDED__
#define __C_OPEN_GL_RENDER_TARGET_H_INCLUDED__

#include "IrrCompileConfig.h"

#ifdef _IRR_COMPILE_WITH_OPENGL_

#include "IRenderTarget.h"

#include "dimension2d.h"
#include "COpenGLExtensionHandler.h"

namespace irr
{
namespace video
{

class COpenGLDriver;
class COpenGLCallBridge;

class COpenGLRenderTarget : public IRenderTarget
{
public:
	COpenGLRenderTarget(COpenGLDriver* driver);
	virtual ~COpenGLRenderTarget();

	virtual void setTexture(const core::array<ITexture*>& texture, ITexture* depthStencil) _IRR_OVERRIDE_;

	void update(const core::array<u32>& id);

	GLuint getBufferID() const;

	const core::dimension2d<u32>& getSize() const;

	ITexture* getTexture() const;

protected:
	core::array<u32> AssignedActiveTextureID;
	u32 AssignedTextureCount;
	bool AssignedDepth;
	bool AssignedStencil;

	bool TextureUpdate;
	bool DepthStencilUpdate;

	GLuint BufferID;

	core::dimension2d<u32> Size;

	bool SupportForFBO;
	bool SupportForMRT;

	COpenGLCallBridge* BridgeCalls;

	COpenGLDriver* Driver;
};

}
}

#endif
#endif
