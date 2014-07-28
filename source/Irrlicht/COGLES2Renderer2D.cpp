// Copyright (C) 2014 Patryk Nadrowski
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#include "IrrCompileConfig.h"

#ifdef _IRR_COMPILE_WITH_OGLES2_

#include "COGLES2Renderer2D.h"
#include "IGPUProgrammingServices.h"
#include "os.h"
#include "COGLES2Driver.h"

namespace irr
{
namespace video
{

COGLES2Renderer2D::COGLES2Renderer2D(const c8* vertexShaderProgram, const c8* pixelShaderProgram, COGLES2Driver* driver) :
	COGLES2MaterialRenderer(driver, 0, EMT_SOLID), Texture(0)
{
#ifdef _DEBUG
	setDebugName("COGLES2Renderer2D");
#endif

	int Temp = 0;

	init(Temp, vertexShaderProgram, pixelShaderProgram, false);

	Driver->getBridgeCalls()->setProgram(Program);

	// These states don't change later.

	ThicknessID = getPixelShaderConstantID("uThickness");
	TextureUsageID = getPixelShaderConstantID("uTextureUsage");
	s32 TextureUnitID = getPixelShaderConstantID("uTextureUnit");	

	s32 TextureUnit = 0;
	setPixelShaderConstant(TextureUnitID, &TextureUnit, 1);

	Driver->getBridgeCalls()->setProgram(0);
}

COGLES2Renderer2D::~COGLES2Renderer2D()
{
}

void COGLES2Renderer2D::OnSetMaterial(const video::SMaterial& material,
				const video::SMaterial& lastMaterial,
				bool resetAllRenderstates,
				video::IMaterialRendererServices* services)
{
	Driver->getBridgeCalls()->setProgram(Program);
	Driver->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);
	f32 Thickness = (material.Thickness > 0.f) ? material.Thickness : 1.f;
	setPixelShaderConstant(ThicknessID, &Thickness, 1);
}

bool COGLES2Renderer2D::OnRender(IMaterialRendererServices* service, E_VERTEX_TYPE vtxtype)
{
	Driver->setTextureRenderStates(Driver->getCurrentMaterial(), false);

	s32 TextureUsage = Texture ? 1 : 0;
	setPixelShaderConstant(TextureUsageID, &TextureUsage, 1);

	return true;
}

void COGLES2Renderer2D::setTexture(const ITexture* texture)
{
	Texture = texture;
}

}
}

#endif

