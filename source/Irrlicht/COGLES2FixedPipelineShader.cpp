// Copyright (C) 2009-2010 Amundis
// Heavily based on the OpenGL driver implemented by Nikolaus Gebhardt
// and OpenGL ES driver implemented by Christian Stehno
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h
#include "COGLES2FixedPipelineShader.h"

#ifdef _IRR_COMPILE_WITH_OGLES2_

#include "COGLES2SLMaterialRenderer.h"
#include "COGLES2Utils.h"

namespace irr
{
    namespace video
    {

        const char* const COGLES2FixedPipelineShader::sBuiltInShaderUniformNames[] =
        {
            "uRenderMode",
            "uMvpMatrix",
            "uWorldMatrix",
            "uNormalize",
            "uEyePos",
            "uUseLight",
            "uLightPosition",
            "uLightAmbient",
            "uLightDiffuse",
            "uLightSpecular",
            "uLightDirection",
            "uLightAttenuation",
            "uLightExponent",
            "uLightCutoff",
            "uAmbientColor",
            "uLighting",
            "uMaterialAmbient",
            "uMaterialEmission",
            "uMaterialDiffuse",
            "uMaterialSpecular",
            "uMaterialShininess",
            "uColorMaterial",
            "uUseTexture",
            "uTextureMatrix",
            "uUseTexMatrix",
            "uClip",
            "uClipPlane",
            "uAlphaTest",
            "uAlphaValue",
            "uFog",
            "uFogType",
            "uFogColor",
            "uFogStart",
            "uFogEnd",
            "uFogDensity",
            "uTextureUnit0",
            "uTextureUnit1",
            0
        };

        const c8 VertexShaderFile[] = "../../media/Shaders/COGLES2FixedPipeline.vsh";
        const c8 FragmentShaderFile[] = "../../media/Shaders/COGLES2FixedPipeline.fsh";

        COGLES2FixedPipelineShader::COGLES2FixedPipelineShader( video::COGLES2Driver *driver, io::IFileSystem* fs )
                : COGLES2SLMaterialRenderer( driver, fs, 0, 0, sBuiltInShaderUniformNames, UNIFORM_COUNT ), Normalize( 0 ), AlphaTest( 0 ), AlphaValue( 0.f ),
                AlphaFunction( ALPHA_GREATER ), Lighting( 0 ), Fog( 0 ), FogType( 0 ), FogStart( 0.f ), FogEnd( 0.f ), FogDensity( 0.f ),
                ColorMaterial( 0 ), MaterialShininess( 0.f ), RenderMode( EMT_SOLID )
        {
            s32 dummy;
            initFromFiles( dummy, VertexShaderFile, FragmentShaderFile, false );
            initData();
        };

        void COGLES2FixedPipelineShader::reload()
        {
            reloadFromFiles( VertexShaderFile, FragmentShaderFile );
            //initData();
        }

        void COGLES2FixedPipelineShader::initData()
        {
            for ( size_t i = 0; i < MAX_TEXTURE_UNITS; ++i )
                TextureUnits[i] = i;
            memset( UseTexture, 0, sizeof( UseTexture ) );
            memset( UseTexMatrix, 0, sizeof( UseTexMatrix ) );
            memset( UseLight, 0, sizeof( UseLight ) );
            memset( LightPosition, 0, sizeof( LightPosition ) );
            memset( LightAmbient, 0, sizeof( LightAmbient ) );
            memset( LightDiffuse, 0, sizeof( LightDiffuse ) );
            memset( LightSpecular, 0, sizeof( LightSpecular ) );
            memset( LightDirection, 0, sizeof( LightDirection ) );
            memset( LightAttenuation, 0, sizeof( LightAttenuation ) );
            memset( LightExponent, 0, sizeof( LightExponent ) );
            memset( LightCutoff, 0, sizeof( LightCutoff ) );
            memset( &AmbientColor, 0, sizeof( AmbientColor ) );
            memset( FogColor, 0, sizeof( FogColor ) );
            memset( &ClipPlane, 0, sizeof( ClipPlane ) );
            memset( &MaterialAmbient, 0, sizeof( MaterialAmbient ) );
            memset( &MaterialEmission, 0, sizeof( MaterialEmission ) );
            memset( &MaterialDiffuse, 0, sizeof( MaterialDiffuse ) );
            memset( &MaterialSpecular, 0, sizeof( MaterialSpecular ) );
        }

        bool COGLES2FixedPipelineShader::OnRender( IMaterialRendererServices* service, E_VERTEX_TYPE vtxtype )
        {
            Driver->testGLError();
            bool statusOk = true;

            /* Matrices Upload */
            core::matrix4 world = Driver->getTransform( ETS_WORLD );
            //statusOk &= setVertexShaderConstant( "uWorldMatrix", world.pointer(), 16 );
            //glUniformMatrix4fv(locWorldMatrix, 1, false, world.pointer() );
            setUniform( WORLD_MATRIX, world.pointer() );

            core::matrix4 worldViewProj = Driver->getTransform( video::ETS_PROJECTION );
            worldViewProj *= Driver->getTransform( video::ETS_VIEW );
            worldViewProj *= Driver->getTransform( ETS_WORLD );
            //statusOk &= setVertexShaderConstant( "uMvpMatrix", worldViewProj.pointer(), 16 );
            //glUniformMatrix4fv(locMvpMatrix, 1, false, worldViewProj.pointer());
            setUniform( MVP_MATRIX, worldViewProj.pointer() );

            /* Textures Upload */
            //statusOk &= setVertexShaderConstant("uTextureUnit", (f32*)TextureUnits, MAX_TEXTURE_UNITS);
            //statusOk &= setVertexShaderConstant( "uTextureUnit0", ( f32* ) & TextureUnits[0], 1 );
            setUniform( TEXTURE_UNIT0, &TextureUnits[0] );
            //statusOk &= setVertexShaderConstant( "uTextureUnit1", ( f32* ) & TextureUnits[1], 1 );
            setUniform( TEXTURE_UNIT1, &TextureUnits[1] );

            //statusOk &= setVertexShaderConstant( "uUseTexture", ( f32* )UseTexture, MAX_TEXTURE_UNITS );
            setUniform( USE_TEXTURE, UseTexture, MAX_TEXTURE_UNITS );
            //statusOk &= setVertexShaderConstant( "uUseTexMatrix", ( f32* )UseTexMatrix, MAX_TEXTURE_UNITS );
            setUniform( USE_TEXTURE_MATRIX, UseTexMatrix, MAX_TEXTURE_UNITS );
            //statusOk &= setVertexShaderConstant( "uTextureMatrix", ( f32* )TextureMatrix, MAX_TEXTURE_UNITS * 16 );
            setUniform( TEXTURE_MATRIX, TextureMatrix, MAX_TEXTURE_UNITS );
            core::matrix4 invWorld;

            /* Lights (in Object Space) Upload */
            if ( Lighting )
            {
                u32 cnt = Driver->getDynamicLightCount();
                Driver->getTransform( ETS_WORLD ).getInverse( invWorld );
                for ( size_t i = 0; i < MAX_LIGHTS; ++i )
                {

                    if ( i < cnt )
                    {
                        UseLight[i] = 1;
                        video::SLight light;
                        light = Driver->getDynamicLight( i );

                        switch ( light.Type )
                        {
                            case ELT_DIRECTIONAL:
                                invWorld.rotateVect(( f32* )&LightPosition[i], light.Direction );
                                LightPosition[i].data[4] = 0.0;
                                break;
                            case ELT_SPOT:
                                invWorld.rotateVect( LightDirection[i], light.Direction );
                                LightExponent[i] = light.Falloff;
                                LightCutoff[i]   = light.OuterCone;
                                //no break on purpose !
                            case ELT_POINT:
                                invWorld.transformVect(( f32* )&LightPosition[i], light.Position );
                                LightPosition[i].data[4] = 1.0;
                                LightAttenuation[i] = light.Attenuation;
                                break;

                            default:
                                UseLight[i] = 0;
                                break;
                        }

                        LightAmbient[i]  = light.AmbientColor;
                        LightDiffuse[i]  = light.DiffuseColor;
                        LightSpecular[i] = light.SpecularColor;
                        LightAttenuation[i] = light.Attenuation;
                    }
                    else
                    {
                        UseLight[i] = 0;
                    }
                }
                //statusOk &= setVertexShaderConstant( "uLighting", ( f32* ) & Lighting, 1 );
                //statusOk &= setVertexShaderConstant( "uUseLight", ( f32* )UseLight, MAX_LIGHTS );
                setUniform( USE_LIGHT, UseLight, MAX_LIGHTS );
                //statusOk &= setVertexShaderConstant( "uLightPosition", ( f32* )LightPosition,    MAX_LIGHTS * 4 );
                setUniform( LIGHT_POSITION, LightPosition, MAX_LIGHTS );
                //statusOk &= setVertexShaderConstant( "uLightDirection", ( f32* )LightDirection,   MAX_LIGHTS * 3 );
                setUniform( LIGHT_DIRECTION, LightDirection, MAX_LIGHTS );
                //statusOk &= setVertexShaderConstant( "uLightAmbient", ( f32* )LightAmbient,     MAX_LIGHTS * 4 );
                setUniform( LIGHT_AMBIENT, LightAmbient, MAX_LIGHTS );
                //statusOk &= setVertexShaderConstant( "uLightDiffuse", ( f32* )LightDiffuse,     MAX_LIGHTS * 4 );
                setUniform( LIGHT_DIFFUSE, LightDiffuse, MAX_LIGHTS );
                //statusOk &= setVertexShaderConstant( "uLightSpecular", ( f32* )LightSpecular,    MAX_LIGHTS * 4 );
                setUniform( LIGHT_SPECULAR, LightSpecular, MAX_LIGHTS );
                //statusOk &= setVertexShaderConstant( "uLightAttenuation", ( f32* )LightAttenuation, MAX_LIGHTS * 3 );
                setUniform( LIGHT_ATTENUATION, LightAttenuation, MAX_LIGHTS );
                //statusOk &= setVertexShaderConstant( "uLightExponent", LightExponent, MAX_LIGHTS );
                setUniform( LIGHT_EXPONENT, LightExponent, MAX_LIGHTS );
                //statusOk &= setVertexShaderConstant( "uLightCutoff",   LightCutoff,   MAX_LIGHTS );
                setUniform( LIGHT_CUTOFF, LightCutoff, MAX_LIGHTS );

                AmbientColor = Driver->getAmbientLight();
                setUniform( LIGHT_AMBIENT, &AmbientColor );
            }

            /* Fog */
            /* statusOk &= setVertexShaderConstant( "uFog", ( f32* ) & Fog, 1 );
             statusOk &= setVertexShaderConstant( "uFogType", ( f32* ) & FogType, 1 );
             statusOk &= setVertexShaderConstant( "uFogColor", FogColor, 4 );
             statusOk &= setVertexShaderConstant( "uFogStart", &FogStart, 1 );
             statusOk &= setVertexShaderConstant( "uFogEnd", &FogEnd, 1 );
             statusOk &= setVertexShaderConstant( "uFogDensity", &FogDensity, 1 );*/

            /* Clip Plane */
            u32 cnt = Driver->getClipPlaneCount();
            if ( cnt > 0 )
            {
                Clip = 1;
                ClipPlane = Driver->getClipPlane( 0 );
            }
            else
            {
                Clip = 0;
            }

            /* Eye/Camera Position in ObjectSpace */
            if ( Clip || RenderMode == 10 || RenderMode == 11 ) // Need clipping or reflection
            {
                if ( !Lighting )
                    Driver->getTransform( ETS_WORLD ).getInverse( invWorld );
                core::vector3df viewPos( 0.0f, 0.0f, 0.0f );
                core::matrix4 inverseView;
                Driver->getTransform( video::ETS_VIEW ).getInverse( inverseView );
                inverseView.transformVect( viewPos );
                invWorld.transformVect( viewPos );
                //setVertexShaderConstant( "uEyePos", &viewPos.X, 3 );
                setUniform( EYE_POSITION, &viewPos.X );
            }

            //statusOk &= setVertexShaderConstant( "uClip", ( f32* ) & Clip );
            setUniform( CLIP, &Clip );
            //statusOk &= setVertexShaderConstant( "uClipPlane", ( f32* ) & ClipPlane );
            setUniform( CLIP_PLANE, &ClipPlane );
            //statusOk &= setVertexShaderConstant( "uRenderMode", ( f32* ) & RenderMode );
            setUniform( RENDER_MODE, &RenderMode );
            //statusOk &= setVertexShaderConstant( "uNormalize", ( f32* ) & Normalize, 1 );

            return statusOk ;
        };

        void COGLES2FixedPipelineShader::setMaterial( const SMaterial &material )
        {
            if (Fog != static_cast<int>(material.FogEnable))
            {
                Fog = material.FogEnable;
                setUniform( FOG, &Fog );
            }
            if (Lighting != static_cast<int>(material.Lighting))
            {
                Lighting  = material.Lighting;
                setUniform( LIGHTING, &Lighting );
            }

            if (Normalize != static_cast<float>(material.NormalizeNormals))
            {
                Normalize = material.NormalizeNormals;
                setUniform( NORMALIZE, &Normalize );
            }

            for ( size_t i = 0; i < MAX_TEXTURE_UNITS; ++i )
            {
                UseTexture[i] = material.getTexture( i ) != 0;
                if ( UseTexture[i] )
                {
                    UseTexMatrix[i] = false;
                    core::matrix4 texMat = material.getTextureMatrix( i );
                    if ( !texMat.isIdentity() )
                    {
                        UseTexMatrix[i] = true;
                        memcpy( &TextureMatrix[i], texMat.pointer(), sizeof( mat4 ) );
                    }
                }
            }


            if ( ColorMaterial != material.ColorMaterial )
            {
                ColorMaterial = material.ColorMaterial;
                setUniform( COLOR_MATERIAL, &ColorMaterial );

            }
            if ( MaterialAmbient != material.AmbientColor )
            {
                MaterialAmbient   = material.AmbientColor;
                setUniform( MATERIAL_AMBIENT, &MaterialAmbient );
            }
            if ( MaterialEmission != material.EmissiveColor )
            {
                MaterialEmission  = material.EmissiveColor;
                setUniform( MATERIAL_EMISSION, &MaterialEmission );
            }
            if ( MaterialDiffuse != material.DiffuseColor )
            {
                MaterialDiffuse   = material.DiffuseColor;
                setUniform( MATERIAL_DIFFUSE, &MaterialDiffuse );
            }
            if ( MaterialSpecular != material.SpecularColor )
            {
                MaterialSpecular  = material.SpecularColor;
                setUniform( MATERIAL_SPECULAR, &MaterialSpecular );
            }
            if ( MaterialShininess != material.Shininess )
            {
                MaterialShininess = material.Shininess;
                setUniform( MATERIAL_SHININESS, &MaterialShininess );
            }
        }

    }
}

#endif //_IRR_COMPILE_WITH_OGLES2_
