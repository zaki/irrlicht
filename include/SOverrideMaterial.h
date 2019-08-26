// Copyright (C) 2017 Michael Zeilfelder
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __S_OVERRIDE_MATERIAL_H_INCLUDED__
#define __S_OVERRIDE_MATERIAL_H_INCLUDED__

#include "SMaterial.h"

namespace irr
{
namespace video
{

	struct SOverrideMaterial
	{
		//! The Material values
		SMaterial Material;

		//! Which values are overridden
		/** OR'ed values from E_MATERIAL_FLAGS. */
		u32 EnableFlags;

		//! For those flags in EnableFlags which affect layers, set which of the layers are affected
		bool EnableLayerFlags[MATERIAL_MAX_TEXTURES];

		//! Which textures are overridden
		bool EnableTextures[MATERIAL_MAX_TEXTURES];

		//! Overwrite complete layers (settings of EnableLayerFlags and EnableTextures don't matter then for layer data)
		bool EnableLayers[MATERIAL_MAX_TEXTURES];

		//! Set in which render passes the material override is active.
		/** OR'ed values from E_SCENE_NODE_RENDER_PASS. */
		u16 EnablePasses;

		//! Global enable flag, overwritten by the SceneManager in each pass
		/** The Scenemanager uses the EnablePass array and sets Enabled to
		true if the Override material is enabled in the current pass. */
		bool Enabled;

		struct SMaterialTypeReplacement
		{
			SMaterialTypeReplacement(s32 original, u32 replacement) : Original(original), Replacement(replacement) {}
			SMaterialTypeReplacement(u32 replacement) : Original(-1), Replacement(replacement) {}

			//! SMaterial.MaterialType to replace. 
			//! -1 for all types or a specific value to only replace that one (which is either one of E_MATERIAL_TYPE or a shader material id)
			s32 Original;

			//! MaterialType to used to override Original (either one of E_MATERIAL_TYPE or a shader material id)
			u32 Replacement;
		};

		//! To overwrite SMaterial::MaterialType
		core::array<SMaterialTypeReplacement> MaterialTypes;

		//! Default constructor
		SOverrideMaterial() : EnableFlags(0), EnablePasses(0), Enabled(false)
		{
		}

		//! disable overrides and reset all flags
		void reset()
		{
			EnableFlags = 0;
			EnablePasses = 0;
			Enabled = false;
			for (u32 i = 0; i < MATERIAL_MAX_TEXTURES; ++i)
			{
				EnableLayerFlags[i] = true;	// doesn't do anything unless EnableFlags is set, just saying by default all texture layers are affected by flags
				EnableTextures[i] = false;
				EnableLayers[i] = false;
			}
			MaterialTypes.clear();
		}

		//! Apply the enabled overrides
		void apply(SMaterial& material)
		{
			if (Enabled)
			{
				for (u32 i = 0; i < MaterialTypes.size(); ++i)
				{
					const SMaterialTypeReplacement& mtr = MaterialTypes[i];
					if (mtr.Original < 0 || (s32)mtr.Original == material.MaterialType)
						material.MaterialType = (E_MATERIAL_TYPE)mtr.Replacement;
				}
				for (u32 f=0; f<32; ++f)
				{
					const u32 num=(1<<f);
					if (EnableFlags & num)
					{
						switch (num)
						{
						case EMF_WIREFRAME: material.Wireframe = Material.Wireframe; break;
						case EMF_POINTCLOUD: material.PointCloud = Material.PointCloud; break;
						case EMF_GOURAUD_SHADING: material.GouraudShading = Material.GouraudShading; break;
						case EMF_LIGHTING: material.Lighting = Material.Lighting; break;
						case EMF_ZBUFFER: material.ZBuffer = Material.ZBuffer; break;
						case EMF_ZWRITE_ENABLE: material.ZWriteEnable = Material.ZWriteEnable; break;
						case EMF_BACK_FACE_CULLING: material.BackfaceCulling = Material.BackfaceCulling; break;
						case EMF_FRONT_FACE_CULLING: material.FrontfaceCulling = Material.FrontfaceCulling; break;
						case EMF_BILINEAR_FILTER:
							for ( u32 i=0; i<MATERIAL_MAX_TEXTURES; ++i)
							{
								if ( EnableLayerFlags[i] )
								{
									material.TextureLayer[i].BilinearFilter = Material.TextureLayer[i].BilinearFilter;
								}
							}
							break;
						case EMF_TRILINEAR_FILTER:
							for ( u32 i=0; i<MATERIAL_MAX_TEXTURES; ++i)
							{
								if ( EnableLayerFlags[i] )
								{
									material.TextureLayer[i].TrilinearFilter = Material.TextureLayer[i].TrilinearFilter;
								}
							}
							break;
						case EMF_ANISOTROPIC_FILTER:
							for ( u32 i=0; i<MATERIAL_MAX_TEXTURES; ++i)
							{
								if ( EnableLayerFlags[i] )
								{
									material.TextureLayer[i].AnisotropicFilter = Material.TextureLayer[i].AnisotropicFilter;
								}
							}
							break;
						case EMF_FOG_ENABLE: material.FogEnable = Material.FogEnable; break;
						case EMF_NORMALIZE_NORMALS: material.NormalizeNormals = Material.NormalizeNormals; break;
						case EMF_TEXTURE_WRAP:
							for ( u32 i=0; i<MATERIAL_MAX_TEXTURES; ++i)
							{
								if ( EnableLayerFlags[i] )
								{
									material.TextureLayer[i].TextureWrapU = Material.TextureLayer[i].TextureWrapU;
									material.TextureLayer[i].TextureWrapV = Material.TextureLayer[i].TextureWrapV;
									material.TextureLayer[i].TextureWrapW = Material.TextureLayer[i].TextureWrapW;
								}
							}
							break;
						case EMF_ANTI_ALIASING: material.AntiAliasing = Material.AntiAliasing; break;
						case EMF_COLOR_MASK: material.ColorMask = Material.ColorMask; break;
						case EMF_COLOR_MATERIAL: material.ColorMaterial = Material.ColorMaterial; break;
						case EMF_USE_MIP_MAPS: material.UseMipMaps = Material.UseMipMaps; break;
						case EMF_BLEND_OPERATION: material.BlendOperation = Material.BlendOperation; break;
						case EMF_BLEND_FACTOR: material.BlendFactor = Material.BlendFactor; break;
						case EMF_POLYGON_OFFSET:
							material.PolygonOffsetDirection = Material.PolygonOffsetDirection;
							material.PolygonOffsetFactor = Material.PolygonOffsetFactor;
							material.PolygonOffsetDepthBias = Material.PolygonOffsetDepthBias;
							material.PolygonOffsetSlopeScale = Material.PolygonOffsetSlopeScale;
							break;
						}
					}
				}
				for(u32 i=0; i<MATERIAL_MAX_TEXTURES; ++i )
				{
					if ( EnableLayers[i] )
					{
						material.TextureLayer[i] = Material.TextureLayer[i];
					}
					else if ( EnableTextures[i] )
					{
						material.TextureLayer[i].Texture = Material.TextureLayer[i].Texture;
					}
				}
			}
		}

	};

} // end namespace video
} // end namespace irr

#endif // __S_OVERRIDE_MATERIAL_H_INCLUDED__

