// Copyright (C) 2014 Patryk Nadrowski
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "IrrCompileConfig.h"

#ifdef _IRR_COMPILE_WITH_OGLES2_

#include "COGLES2FixedPipelineRenderer.h"
#include "IVideoDriver.h"

namespace irr
{
namespace video
{

// EMT_SOLID + EMT_TRANSPARENT_ADD_COLOR + EMT_TRANSPARENT_ALPHA_CHANNEL + EMT_TRANSPARENT_VERTEX_ALPHA + EMT_ONETEXTURE_BLEND

COGLES2MaterialSolidCB::COGLES2MaterialSolidCB() :
	FirstUpdate(true), MVPMatrixID(-1), TMatrix0ID(-1), AlphaRefID(-1), TextureUsage0ID(-1), TextureUnit0ID(-1), AlphaRef(0.5f), TextureUsage0(0), TextureUnit0(0)
{
}

void COGLES2MaterialSolidCB::OnSetMaterial(const SMaterial& material)
{
	AlphaRef = material.MaterialTypeParam;
	TextureUsage0 = (material.TextureLayer[0].Texture) ? 1 : 0;
}

void COGLES2MaterialSolidCB::OnSetConstants(IMaterialRendererServices* services, s32 userData)
{
	IVideoDriver* driver = services->getVideoDriver();

	if (FirstUpdate)
	{
		MVPMatrixID = services->getVertexShaderConstantID("uMVPMatrix");
		TMatrix0ID = services->getVertexShaderConstantID("uTMatrix0");
		AlphaRefID = services->getVertexShaderConstantID("uAlphaRef");
		TextureUsage0ID = services->getVertexShaderConstantID("uTextureUsage0");
		TextureUnit0ID = services->getVertexShaderConstantID("uTextureUnit0");

		FirstUpdate = false;
	}

	core::matrix4 Matrix = driver->getTransform(ETS_PROJECTION);
	Matrix *= driver->getTransform(ETS_VIEW);
	Matrix *= driver->getTransform(ETS_WORLD);
	services->setPixelShaderConstant(MVPMatrixID, Matrix.pointer(), 16);

	Matrix = driver->getTransform(ETS_TEXTURE_0);
	services->setPixelShaderConstant(TMatrix0ID, Matrix.pointer(), 16);

	services->setPixelShaderConstant(AlphaRefID, &AlphaRef, 1);
	services->setPixelShaderConstant(TextureUsage0ID, &TextureUsage0, 1);
	services->setPixelShaderConstant(TextureUnit0ID, &TextureUnit0, 1);
}

// EMT_SOLID_2_LAYER + EMT_DETAIL_MAP

COGLES2MaterialSolid2CB::COGLES2MaterialSolid2CB() :
	FirstUpdate(true), MVPMatrixID(-1), TMatrix0ID(-1), TMatrix1ID(-1), TextureUsage0ID(-1), TextureUsage1ID(-1), TextureUnit0ID(-1), TextureUnit1ID(-1),
	TextureUsage0(0), TextureUsage1(0), TextureUnit0(0), TextureUnit1(1)
{
}

void COGLES2MaterialSolid2CB::OnSetMaterial(const SMaterial& material)
{
	TextureUsage0 = (material.TextureLayer[0].Texture) ? 1 : 0;
	TextureUsage1 = (material.TextureLayer[1].Texture) ? 1 : 0;
}

void COGLES2MaterialSolid2CB::OnSetConstants(IMaterialRendererServices* services, s32 userData)
{
	IVideoDriver* driver = services->getVideoDriver();

	if (FirstUpdate)
	{
		MVPMatrixID = services->getVertexShaderConstantID("uMVPMatrix");
		TMatrix0ID = services->getVertexShaderConstantID("uTMatrix0");
		TMatrix1ID = services->getVertexShaderConstantID("uTMatrix1");
		TextureUsage0ID = services->getVertexShaderConstantID("uTextureUsage0");
		TextureUsage1ID = services->getVertexShaderConstantID("uTextureUsage1");
		TextureUnit0ID = services->getVertexShaderConstantID("uTextureUnit0");
		TextureUnit1ID = services->getVertexShaderConstantID("uTextureUnit1");

		FirstUpdate = false;
	}

	core::matrix4 Matrix = driver->getTransform(ETS_PROJECTION);
	Matrix *= driver->getTransform(ETS_VIEW);
	Matrix *= driver->getTransform(ETS_WORLD);
	services->setPixelShaderConstant(MVPMatrixID, Matrix.pointer(), 16);

	Matrix = driver->getTransform(ETS_TEXTURE_0);
	services->setPixelShaderConstant(TMatrix0ID, Matrix.pointer(), 16);

	Matrix = driver->getTransform(ETS_TEXTURE_1);
	services->setPixelShaderConstant(TMatrix1ID, Matrix.pointer(), 16);

	services->setPixelShaderConstant(TextureUsage0ID, &TextureUsage0, 1);
	services->setPixelShaderConstant(TextureUsage1ID, &TextureUsage1, 1);
	services->setPixelShaderConstant(TextureUnit0ID, &TextureUnit0, 1);
	services->setPixelShaderConstant(TextureUnit1ID, &TextureUnit1, 1);
}

// EMT_LIGHTMAP + EMT_LIGHTMAP_ADD + EMT_LIGHTMAP_M2 + EMT_LIGHTMAP_M4

COGLES2MaterialLightmapCB::COGLES2MaterialLightmapCB(float modulate) :
	FirstUpdate(true), MVPMatrixID(-1), TMatrix0ID(-1), TMatrix1ID(-1), ModulateID(-1), TextureUsage0ID(-1), TextureUsage1ID(-1), TextureUnit0ID(-1), TextureUnit1ID(-1),
	Modulate(modulate), TextureUsage0(0), TextureUsage1(0), TextureUnit0(0), TextureUnit1(1)
{
}

void COGLES2MaterialLightmapCB::OnSetMaterial(const SMaterial& material)
{
	TextureUsage0 = (material.TextureLayer[0].Texture) ? 1 : 0;
	TextureUsage1 = (material.TextureLayer[1].Texture) ? 1 : 0;
}

void COGLES2MaterialLightmapCB::OnSetConstants(IMaterialRendererServices* services, s32 userData)
{
	IVideoDriver* driver = services->getVideoDriver();

	if (FirstUpdate)
	{
		MVPMatrixID = services->getVertexShaderConstantID("uMVPMatrix");
		TMatrix0ID = services->getVertexShaderConstantID("uTMatrix0");
		TMatrix1ID = services->getVertexShaderConstantID("uTMatrix1");
		ModulateID = services->getVertexShaderConstantID("uModulate");
		TextureUsage0ID = services->getVertexShaderConstantID("uTextureUsage0");
		TextureUsage1ID = services->getVertexShaderConstantID("uTextureUsage1");
		TextureUnit0ID = services->getVertexShaderConstantID("uTextureUnit0");
		TextureUnit1ID = services->getVertexShaderConstantID("uTextureUnit1");

		FirstUpdate = false;
	}

	core::matrix4 Matrix = driver->getTransform(ETS_PROJECTION);
	Matrix *= driver->getTransform(ETS_VIEW);
	Matrix *= driver->getTransform(ETS_WORLD);
	services->setPixelShaderConstant(MVPMatrixID, Matrix.pointer(), 16);

	Matrix = driver->getTransform(ETS_TEXTURE_0);
	services->setPixelShaderConstant(TMatrix0ID, Matrix.pointer(), 16);

	Matrix = driver->getTransform(ETS_TEXTURE_1);
	services->setPixelShaderConstant(TMatrix1ID, Matrix.pointer(), 16);

	services->setPixelShaderConstant(ModulateID, &Modulate, 1);
	services->setPixelShaderConstant(TextureUsage0ID, &TextureUsage0, 1);
	services->setPixelShaderConstant(TextureUsage1ID, &TextureUsage1, 1);
	services->setPixelShaderConstant(TextureUnit0ID, &TextureUnit0, 1);
	services->setPixelShaderConstant(TextureUnit1ID, &TextureUnit1, 1);
}

// EMT_SPHERE_MAP + EMT_REFLECTION_2_LAYER + EMT_TRANSPARENT_REFLECTION_2_LAYER

COGLES2MaterialReflectionCB::COGLES2MaterialReflectionCB() :
	FirstUpdate(true), MVPMatrixID(-1), MVMatrixID(-1), NMatrixID(-1), TMatrix0ID(-1), TextureUsage0ID(-1), TextureUsage1ID(-1), TextureUnit0ID(-1), TextureUnit1ID(-1),
	TextureUsage0(0), TextureUsage1(0), TextureUnit0(0), TextureUnit1(1)
{
}

void COGLES2MaterialReflectionCB::OnSetMaterial(const SMaterial& material)
{
	TextureUsage0 = (material.TextureLayer[0].Texture) ? 1 : 0;
	TextureUsage1 = (material.TextureLayer[1].Texture) ? 1 : 0;
}

void COGLES2MaterialReflectionCB::OnSetConstants(IMaterialRendererServices* services, s32 userData)
{
	IVideoDriver* driver = services->getVideoDriver();

	if (FirstUpdate)
	{
		MVPMatrixID = services->getVertexShaderConstantID("uMVPMatrix");
		MVMatrixID = services->getVertexShaderConstantID("uMVMatrix");
		NMatrixID = services->getVertexShaderConstantID("uNMatrix");
		TMatrix0ID = services->getVertexShaderConstantID("uTMatrix0");
		TextureUsage0ID = services->getVertexShaderConstantID("uTextureUsage0");
		TextureUsage1ID = services->getVertexShaderConstantID("uTextureUsage1");
		TextureUnit0ID = services->getVertexShaderConstantID("uTextureUnit0");
		TextureUnit1ID = services->getVertexShaderConstantID("uTextureUnit1");

		FirstUpdate = false;
	}

	core::matrix4 Matrix = driver->getTransform(ETS_PROJECTION);
	Matrix *= driver->getTransform(ETS_VIEW);
	Matrix *= driver->getTransform(ETS_WORLD);
	services->setPixelShaderConstant(MVPMatrixID, Matrix.pointer(), 16);

	Matrix = driver->getTransform(ETS_VIEW);
	Matrix *= driver->getTransform(ETS_WORLD);
	services->setPixelShaderConstant(MVMatrixID, Matrix.pointer(), 16);

	Matrix.makeInverse();
	Matrix = Matrix.getTransposed();
	services->setPixelShaderConstant(NMatrixID, Matrix.pointer(), 16);

	Matrix = driver->getTransform(ETS_TEXTURE_0);
	services->setPixelShaderConstant(TMatrix0ID, Matrix.pointer(), 16);

	services->setPixelShaderConstant(TextureUsage0ID, &TextureUsage0, 1);
	services->setPixelShaderConstant(TextureUsage1ID, &TextureUsage1, 1);
	services->setPixelShaderConstant(TextureUnit0ID, &TextureUnit0, 1);
	services->setPixelShaderConstant(TextureUnit1ID, &TextureUnit1, 1);
}

}
}

#endif

