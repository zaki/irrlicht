// Copyright (C) 2009-2010 Amundis
// Heavily based on the OpenGL driver implemented by Nikolaus Gebhardt
// and OpenGL ES driver implemented by Christian Stehno
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_OGLES2_

#include "COGLES2ParallaxMapRenderer.h"
#include "COGLES2Driver.h"
#include "IGPUProgrammingServices.h"
#include "IShaderConstantSetCallBack.h"
#include "IVideoDriver.h"
#include "os.h"

#define MAX_LIGHTS 2

namespace irr
{
namespace video
{

	const char * const COGLES2ParallaxMapRenderer::sBuiltInShaderUniformNames[] =
	{
		"uMvpMatrix",
		"uLightPos",
		"uLightColor",
		"uEyePos",
		"texture0",
		"texture1",
		"uLightDiffuse",
		"uHeightScale",
		0
	};

	// Irrlicht Engine OGLES2 render path normal map vertex shader
	const c8 VertexShaderFile[] = IRR_OGLES2_SHADER_PATH "COGLES2ParallaxMap.vsh";
	const c8 FragmentShaderFile[] = IRR_OGLES2_SHADER_PATH "COGLES2ParallaxMap.fsh";

	//! Constructor
	COGLES2ParallaxMapRenderer::COGLES2ParallaxMapRenderer( video::COGLES2Driver* driver,
															io::IFileSystem* fs, s32& outMaterialTypeNr, IMaterialRenderer* baseMaterial )
			: COGLES2SLMaterialRenderer( driver, fs, 0, baseMaterial, sBuiltInShaderUniformNames, UNIFORM_COUNT ), CompiledShaders( true )
	{

#ifdef _DEBUG
		setDebugName( "COGLES2ParallaxMapRenderer" );
#endif

		// set this as callback. We could have done this in
		// the initialization list, but some compilers don't like it.

		CallBack = this;

		// basically, this simply compiles the hard coded shaders if the
		// hardware is able to do them, otherwise it maps to the base material

		// check if already compiled normal map shaders are there.

		video::IMaterialRenderer* renderer = driver->getMaterialRenderer( EMT_PARALLAX_MAP_SOLID );

		if ( renderer )
		{
			// use the already compiled shaders
			video::COGLES2ParallaxMapRenderer* pmr = reinterpret_cast<video::COGLES2ParallaxMapRenderer*>( renderer );
			CompiledShaders = false;

			Program = pmr->Program;

			UniformInfo = pmr->UniformInfo;
			AttributeInfo = pmr->AttributeInfo;

			outMaterialTypeNr = driver->addMaterialRenderer( this );
		}
		else
		{
			// compile shaders on our own
			if (initFromFiles( outMaterialTypeNr, VertexShaderFile, FragmentShaderFile))
			{
				useProgram();
				int dummy = 0;
				setUniform( TEXTURE_UNIT0, &dummy );
				dummy = 1;
				setUniform( TEXTURE_UNIT1, &dummy );
			}
		}

		// fallback if compilation has failed
		if ( -1 == outMaterialTypeNr )
			outMaterialTypeNr = driver->addMaterialRenderer( this );
	}


	//! Destructor
	COGLES2ParallaxMapRenderer::~COGLES2ParallaxMapRenderer()
	{
		if ( CallBack == this )
			CallBack = 0;

		if ( !CompiledShaders )
		{
			// prevent this from deleting shaders we did not create
			Program = 0;
		}
	}


	void COGLES2ParallaxMapRenderer::OnSetMaterial(
			const video::SMaterial& material, const video::SMaterial& lastMaterial,
			bool resetAllRenderstates, video::IMaterialRendererServices* services )
	{
		COGLES2SLMaterialRenderer::OnSetMaterial(material, lastMaterial,
				resetAllRenderstates, services);

		CurrentScale = material.MaterialTypeParam;
	}

	//! Called by the engine when the vertex and/or pixel shader constants for an
	//! material renderer should be set.
	void COGLES2ParallaxMapRenderer::OnSetConstants( IMaterialRendererServices* services, s32 userData )
	{
		video::IVideoDriver* driver = services->getVideoDriver();

		// set transposed worldViewProj matrix
		core::matrix4 worldViewProj( driver->getTransform( video::ETS_PROJECTION ) );
		worldViewProj *= driver->getTransform( video::ETS_VIEW );
		worldViewProj *= driver->getTransform( video::ETS_WORLD );

		setUniform( MVP_MATRIX, worldViewProj.pointer() );


		// here we fetch the fixed function lights from the driver
		// and set them as constants

		u32 cnt = driver->getDynamicLightCount();

		// Load the inverse world matrix.
		core::matrix4 invWorldMat;
		driver->getTransform( video::ETS_WORLD ).getInverse( invWorldMat );

		float lightPosition[4*MAX_LIGHTS];
		float lightColor[4*MAX_LIGHTS];

		for ( u32 i = 0; i < 2; ++i )
		{
			video::SLight light;

			if ( i < cnt )
				light = driver->getDynamicLight( i );
			else
			{
				light.DiffuseColor.set( 0, 0, 0 ); // make light dark
				light.Radius = 1.0f;
			}

			light.DiffuseColor.a = 1.0f / ( light.Radius * light.Radius ); // set attenuation

			// Transform the light by the inverse world matrix to get it into object space.
			invWorldMat.transformVect( light.Position );

			memcpy( lightPosition + i*4, &light.Position, sizeof( float )*4 );
			memcpy( lightColor + i*4, &light.DiffuseColor, sizeof( float )*4 );
		}

		setUniform( LIGHT_POSITION, lightPosition, MAX_LIGHTS );
		setUniform( LIGHT_COLOR, lightColor, MAX_LIGHTS );

		// Obtain the view position by transforming 0,0,0 by the inverse view matrix
		// and then multiply this by the inverse world matrix.
		core::vector3df viewPos( 0.0f, 0.0f, 0.0f );
		core::matrix4 inverseView;
		driver->getTransform( video::ETS_VIEW ).getInverse( inverseView );
		inverseView.transformVect( viewPos );
		invWorldMat.transformVect( viewPos );
		setUniform( EYE_POSITION, &viewPos.X );

		// set scale factor
		f32 factor = 0.02f; // default value
		if ( CurrentScale != 0.0f )
			factor = CurrentScale;

		setUniform( HEIGHT_SCALE, &factor );
	}


} // end namespace video
} // end namespace irr


#endif

