// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#include "ITexture.h"
#include "Color.h"

namespace Irrlicht
{
namespace Video
{
	/// <summary>
	/// Abstracted and easy to use standard material modes.
	/// </summary>
	public __value enum MaterialType
	{
		/// Standard solid material. Only first texture is used, which is
		/// supposed to be the diffuse material.
		SOLID = 0,			

		/// Solid material with 2 texture layers. The second is blended onto the
		/// first using the alpha value of the vertex colors.
		/// This material is currently not implemented in OpenGL, but it
		/// works with DirectX.
		SOLID_2_LAYER,

		/// Material type with standard lightmap technique: 
		/// There should be 2 textures: The first texture layer is a diffuse map,
		/// the second is a light map. Vertex light is ignored.
		LIGHTMAP,

		/// Material type with lightmap technique like EMT_LIGHTMAP, but
		/// lightmap and diffuse texture are not modulated, but added instead.
		LIGHTMAP_ADD,

		/// Material type with standard lightmap technique: 
		/// There should be 2 textures: The first texture layer is a diffuse map,
		/// the second is a light map. Vertex light is ignored.
		/// The texture colors are effectively multiplyied by 2 for brightening.
		/// like known in DirectX as D3DTOP_MODULATE2X.
		LIGHTMAP_M2,

		/// Material type with standard lightmap technique: 
		/// There should be 2 textures: The first texture layer is a diffuse map,
		/// the second is a light map. Vertex light is ignored.
		/// The texture colors are effectively multiplyied by 4 for brightening.
		/// like known in DirectX as D3DTOP_MODULATE4X.
		LIGHTMAP_M4,

		/// Like EMT_LIGHTMAP, but also supports dynamic lighting.
		LIGHTMAP_LIGHTING,

		/// Like EMT_LIGHTMAP_M2, but also supports dynamic lighting.
		LIGHTMAP_LIGHTING_M2,

		/// Like EMT_LIGHTMAP_4, but also supports dynamic lighting.
		LIGHTMAP_LIGHTING_M4,

		/// Detail mapped material. The first texture is diffuse color map, the
		/// second is added to this and usually displayed with a bigger scale value
		/// so that it adds more detail. The detail map is added to the diffuse map using
		/// ADD_SIGNED, so that it is possible to add and substract color from the diffuse 
		/// map. For example a value of (127,127,127) will not change the appearance of 
		/// the diffuse map at all.
		/// Often used for terrain rendering. 
		DETAIL_MAP,

		/// Makes the material look like it was reflection the environment
		/// around it. To make this possible, a texture called 'sphere map'
		/// is used, which must be set as Texture1.
		SPHERE_MAP,

		/// A reflecting material with an 
		/// optional additional non reflecting texture layer. The reflection
		/// map should be set as Texture 1.
		/// Please note that this material type is currently not 100% implemented
		/// in OpenGL. It works in DirectX8.
		REFLECTION_2_LAYER,

		/// A transparent material. Only the first texture is used.
		/// The new color is calculated by simply adding the source color and
		/// the dest color. This means if for example a billboard using a texture with 
		/// black background and a red circle on it is drawed with this material, the
		/// result is that only the red circle will be drawn a little bit transparent,
		/// and everything which was black is 100% transparent and not visible.
		/// This material type is useful for e.g. particle effects.
		TRANSPARENT_ADD_COLOR,

		/// Makes the material transparent based on the texture alpha channel.
		/// The final color is blended together from the destination color and the
		/// texture color, using the alpha channel value as blend factor.
		/// Only first texture is used. If you are using this material with small
		/// textures and 3d object, it is a good idea to load the texture in 32 bit
		/// mode (video::IVideoDriver::setTextureCreationFlag()).
		TRANSPARENT_ALPHA_CHANNEL,	

		/// Makes the material transparent based on the texture alpha channel.
		/// If the alpha channel value is greater than 127, a pixel is written to the
		/// target, otherwise not. This material does not use alpha blending
		/// and is a lot faster than EMT_TRANSPARENT_ALPHA_CHANNEL. It 
		/// is ideal for drawing stuff like leafes of plants, because the borders
		/// are not blurry but sharp.
		/// Only first texture is used. If you are using this material with small
		/// textures and 3d object, it is a good idea to load the texture in 32 bit
		/// mode (video::IVideoDriver::setTextureCreationFlag()).
		EMT_TRANSPARENT_ALPHA_CHANNEL_REF,

		/// Makes the material transparent based on the vertex alpha value.
		TRANSPARENT_VERTEX_ALPHA,

		/// A transparent reflecting material with an 
		/// optional additional non reflecting texture layer. The reflection
		/// map should be set as Texture 1. The transparency depends on the
		/// alpha value in the vertex colors. A texture which will not reflect
		/// can be set als Texture 2.
		/// Please note that this material type is currently not 100% implemented
		/// in OpenGL. It works in DirectX8.
		TRANSPARENT_REFLECTION_2_LAYER,

		/// A solid normal map renderer. First texture is the color map, the 
		/// second should be the normal map. Note that you should use this material
		/// only when drawing geometry consisting of vertices of type S3DVertexTangents 
		/// (TANGENTS). You can convert any mesh into this format using
		/// IMeshManipulator::createMeshWithTangents() (See SpecialFX2 Tutorial).
		/// This shader runs on vertex shader 1.1 and pixel shader 1.1 capable hardware and 
		/// falls back on a fixed function lighted material if this hardware is not available.
		/// Only two lights are supported by this shader, if there are more, the nearest two
		/// are chosen. Currently, this shader is only implemented for the D3D8 and D3D9 renderers.
		NORMAL_MAP_SOLID,

		/// A transparent normal map renderer. First texture is the color map, the 
		/// second should be the normal map. Note that you should use this material
		/// only when drawing geometry consisting of vertices of type S3DVertexTangents 
		/// (TANGENTS). You can convert any mesh into this format using
		/// IMeshManipulator::createMeshWithTangents() (See SpecialFX2 Tutorial).
		/// This shader runs on vertex shader 1.1 and pixel shader 1.1 capable hardware and 
		/// falls back on a fixed function lighted material if this hardware is not available.
		/// Only two lights are supported by this shader, if there are more, the nearest two
		/// are chosen. Currently, this shader is only implemented for the D3D8 and D3D9 renderers.
		NORMAL_MAP_TRANSPARENT_ADD_COLOR,

		/// A transparent (based on the vertex alpha value) normal map renderer.
		/// First texture is the color map, the 
		/// second should be the normal map. Note that you should use this material
		/// only when drawing geometry consisting of vertices of type S3DVertexTangents 
		/// (EVT_TANGENTS). You can convert any mesh into this format using
		/// IMeshManipulator::createMeshWithTangents() (See SpecialFX2 Tutorial).
		/// This shader runs on vertex shader 1.1 and pixel shader 1.1 capable hardware and 
		/// falls back on a fixed function lighted material if this hardware is not available.
		/// Only two lights are supported by this shader, if there are more, the nearest two
		/// are chosen. Currently, this shader is only implemented for the D3D8 and D3D9 renderers.
		NORMAL_MAP_TRANSPARENT_VERTEX_ALPHA,

		/// Just like EMT_NORMAL_MAP_SOLID, but uses parallax mapping too, which 
		/// looks a lot more realistic. This only works when the hardware supports at 
		/// least vertex shader 1.1 and pixel shader 1.4. 
		/// First texture is the color map, the  second should be the normal map.
		/// The normal map texture should contain the height value in the 
		/// alpha component. The IVideoDriver::makeNormalMapTexture() method writes 
		/// this value automaticly when creating normal maps from a heightmap when using a 32 bit
		/// texture. 
		/// The height scale of the material (affecting the bumpiness) is being controlled 
		/// by the SMaterial::MaterialTypeParam member. 
		/// If set to zero, the default value (0.02f) will be applied. Otherwise
		/// the value set in SMaterial::MaterialTypeParam is taken. This value depends on with which
		/// scale the texture is mapped on the material. Too high or low values of MaterialTypeParam
		/// can result in strange artifacts. 
		PARALLAX_MAP_SOLID,

		/// A material just like EMT_PARALLAX_MAP_SOLID, but it is transparent, using
		/// EMT_TRANSPARENT_ADD_COLOR as base material.
		PARALLAX_MAP_TRANSPARENT_ADD_COLOR,

		/// A material just like EMT_PARALLAX_MAP_SOLID, but it is transparent, using
		/// EMT_TRANSPARENT_VERTEX_ALPHA as base material.
		PARALLAX_MAP_TRANSPARENT_VERTEX_ALPHA,

		/// This value is not used. It only forces this enumeration to compile in 32 bit. 
		FORCE_32BIT_DO_NOT_USE = 0x7fffffff	
	};

	/// <summary>
	/// Material flags
	/// </summary>
	public __value enum MaterialFlag
	{
		/// Draw as wireframe or filled triangles? Default: false
		WIREFRAME = 0,

		/// Draw as point cloud or filled triangles? Default: false
		POINTCLOUD,

		/// Flat or Gouraud shading? Default: true
		GOURAUD_SHADING,

		/// Will this material be lighted? Default: true
		LIGHTING,

		/// Is the ZBuffer enabled? Default: true
		ZBUFFER,

		/// May be written to the zbuffer or is it readonly. Default: true
		/// This flag is ignored, if the material type is a transparent type.
		ZWRITE_ENABLE,

		/// Is backfaceculling enabled? Default: true
		BACK_FACE_CULLING,

		/// Is bilinear filtering enabled? Default: true
		BILINEAR_FILTER,

		/// Is trilinear filtering enabled? Default: false
		/// If the trilinear filter flag is enabled,
		/// the bilinear filtering flag is ignored.
		TRILINEAR_FILTER,

		//! Is anisotropic filtering? Default: false
		//! In Irrlicht you can use anisotropic texture filtering in conjunction with bilinear or trilinear 
		//! texture filtering to improve rendering results. Primitives will look less blurry with this
		//! flag switched on.
		ANISOTROPIC_FILTER,

		/// Is fog enabled? Default: false
		FOG_ENABLE,

		/// Normalizes normals.You can enable this if you need 
		/// to scale a dynamic lighted model. Usually, its normals will get scaled 
		/// too then and it will get darker. If you enable the EMF_NORMALIZE_NORMALS flag,
		/// the normals will be normalized again, and the model will look as bright as it should.
		NORMALIZE_NORMALS,

		/// This is not a flag, but a value indicating how much flags there are.
		MATERIAL_FLAG_COUNT 
	};

	const System::Int32 MATERIAL_MAX_TEXTURES = 2;

	/// <summary>
	/// Class describing material parameters
	///</summary>
	public __value class Material
	{
	public:

		/// default constructor, creates a solid material with standard colors
		Material()
		: AmbientColor(255,255,255,255), DiffuseColor(255,255,255,255),
		EmissiveColor(0,0,0,0), SpecularColor(0,0,0,0), Texture1(0), Texture2(0),
			Type(SOLID), Shininess(0.0f)
		{
			Flags[MaterialFlag::WIREFRAME] = false;
			Flags[MaterialFlag::POINTCLOUD] = false;
			Flags[MaterialFlag::LIGHTING] = true;
			Flags[MaterialFlag::ZBUFFER] = true;
			Flags[MaterialFlag::ZWRITE_ENABLE] = true;
			Flags[MaterialFlag::BACK_FACE_CULLING] = true;
			Flags[MaterialFlag::GOURAUD_SHADING] = true;
			Flags[MaterialFlag::BILINEAR_FILTER] = true;
			Flags[MaterialFlag::TRILINEAR_FILTER] = false;
			Flags[MaterialFlag::ANISOTROPIC_FILTER] = false;
			Flags[MaterialFlag::FOG_ENABLE] = false;
			Flags[MaterialFlag::NORMALIZE_NORMALS] = false;
		}

		/// <summary>
		/// Type of the material. Specifies how everything is blended together
		/// </summary>
		MaterialType Type;	

		/// <summary>
		/// How much ambient light (a global light) is reflected by this material.
		/// The default is full white, meaning objects are completely globally illuminated.
		/// Reduce this if you want to see diffuse or specular light effects, or change
		/// the blend of colors to make the object have a base color other than white.
		/// </summary>
		Color AmbientColor;		

		/// <summary>
		/// How much diffuse light coming from a light source is reflected by this material.
		/// The default is full white.
		/// </summary>
		Color DiffuseColor;		

		/// <summary>
		/// Light emitted by this material. Default is to emitt no light.
		/// </summary>
		Color EmissiveColor;	

		/// <summary>
		/// How much specular light (highlights from a light) is reflected. 
		/// The default is to reflect no specular light. 
		///</summary>
		Color SpecularColor;	

		/// <summary>
		/// Value affecting the size of specular highlights.
		/// </summary>
		float Shininess;			

		/// <summary>
		/// Free parameter dependend on the material type. 
		/// Mostly ignored, used for example in PARALLAX_MAP_SOLID.
		/// </summary>
		float MaterialTypeParam;

		/// </summary>
		/// Primary texture layer.
		/// <summary>
		ITexture* Texture1;

		/// </summary>
		/// Secondary texture layer.
		/// <summary>
		ITexture* Texture2;

		/// </summary>
		/// 3rd texture layer.
		/// <summary>
		ITexture* Texture3;

		/// </summary>
		/// 4th texture layer.
		/// <summary>
		ITexture* Texture4;

		///<summary>
		/// Material flags, can also be set by all the properties of this class
		///</summary>
		bool Flags __nogc [MATERIAL_FLAG_COUNT];


		/// <summary>
		/// Draw as wireframe or filled triangles? Default: false
		/// </summary>
		__property bool get_Wireframe()	{	return Flags[WIREFRAME];	}


		/// <summary>
		/// Draw as pointcloud? Default: false
		/// </summary>
		__property bool get_PointClound()	{	return Flags[POINTCLOUD];	}

		/// <summary>
		/// Flat or Gouraud shading? Default: true
		/// </summary>
		__property bool get_GouraudShading() {	return Flags[GOURAUD_SHADING];	}

		/// <summary>
		/// Will this material be lighted? Default: true
		/// </summary>
		__property bool get_Lighting()	{	return Flags[LIGHTING];	}

		/// <summary>
		/// Is the ZBuffer enabled? Default: true
		/// </summary>
		__property bool get_ZBuffer() {	return Flags[ZBUFFER];	}

		/// <summary>
		/// May be written to the zbuffer or is it readonly. Default: true
		/// This flag is ignored, if the MaterialType is a transparent type.
		/// </summary>
		__property bool get_ZWriteEnable()	{	return Flags[ZWRITE_ENABLE];	}

		/// <summary>
		/// Is backfaceculling enabled? Default: true
		/// </summary>
		__property bool get_BackfaceCulling() {	return Flags[BACK_FACE_CULLING]; }

		/// <summary>
		/// Is bilinear filtering enabled? Default: true
		/// </summary>
		__property bool get_BilinearFilter() {	return Flags[BILINEAR_FILTER];	}

		/// <summary>
		/// Is trilinear filtering enabled? Default: false
		/// If the trilinear filter flag is enabled,
		/// the bilinear filtering flag is ignored.
		/// </summary>
		__property bool get_TrilinearFilter() {	return Flags[TRILINEAR_FILTER];	}

		/// <summary>
		/// Is fog enabled? Default: false
		/// </summary>
		__property bool get_FogEnable() {	return Flags[FOG_ENABLE];	}

		/// <summary>
		/// Normalizes normals? Default: false
		/// </summary>
		__property bool get_NormalizeNormals() { return Flags[NORMALIZE_NORMALS]; }

		/// <summary>
		/// Draw as wireframe or filled triangles? Default: false
		/// </summary>
		__property void set_Wireframe(bool f)	{	Flags[WIREFRAME] = f;	}

		/// <summary>
		/// Draw as point cloud? Default: false
		/// </summary>
		__property void set_PointCloud(bool f)	{	Flags[POINTCLOUD] = f;	}

		/// <summary>
		/// Flat or Gouraud shading? Default: true
		/// </summary>
		__property void set_GouraudShading(bool f) {	Flags[GOURAUD_SHADING] = f;	}

		/// <summary>
		/// Will this material be lighted? Default: true
		/// </summary>
		__property void set_Lighting(bool f)	{	Flags[LIGHTING] = f;	}

		/// <summary>
		/// Is the ZBuffer enabled? Default: true
		/// </summary>
		__property void set_ZBuffer(bool f) {	Flags[ZBUFFER] = f;	}

		/// <summary>
		/// May be written to the zbuffer or is it readonly. Default: true
		/// This flag is ignored, if the MaterialType is a transparent type.
		/// </summary>
		__property void set_ZWriteEnable(bool f)	{	Flags[ZWRITE_ENABLE] = f;	}

		/// <summary>
		/// Is backfaceculling enabled? Default: true
		/// </summary>
		__property void set_BackfaceCulling(bool f) {	Flags[BACK_FACE_CULLING] = f; }

		/// <summary>
		/// Is bilinear filtering enabled? Default: true
		/// </summary>
		__property void set_BilinearFilter(bool f) {	Flags[BILINEAR_FILTER] = f;	}

		/// <summary>
		/// Is trilinear filtering enabled? Default: false
		/// If the trilinear filter flag is enabled,
		/// the bilinear filtering flag is ignored.
		/// </summary>
		__property void set_TrilinearFilter(bool f) {	Flags[TRILINEAR_FILTER] = f;	}

		/// <summary>
		/// Is anisotropic filtering enabled? Default: false
		/// </summary>
		__property void set_AnisotropicFilter(bool f) {	Flags[ANISOTROPIC_FILTER] = f;	}

		/// <summary>
		/// Is fog enabled? Default: false
		/// </summary>
		__property void set_FogEnable(bool f) {	Flags[FOG_ENABLE] = f;	}

		/// <summary>
		/// Normalizes normals? Default: false
		/// </summary>
		__property void set_NormalizeNormals(bool f) {	Flags[NORMALIZE_NORMALS] = f;	}


	};

} // end namespace video
} // end namespace irr
