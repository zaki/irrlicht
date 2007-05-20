// Copyright (C) 2002-2007 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __S_MATERIAL_H_INCLUDED__
#define __S_MATERIAL_H_INCLUDED__

#include "SColor.h"
#include "matrix4.h"
#include "irrArray.h"

namespace irr
{
namespace video
{
	class ITexture;

	//! Abstracted and easy to use fixed function/programmable pipeline material modes.
	enum E_MATERIAL_TYPE
	{
		//! Standard solid material. Only first texture is used, which is
		//! supposed to be the diffuse material.
		EMT_SOLID = 0,

		//! Solid material with 2 texture layers. The second is blended onto the
		//! first using the alpha value of the vertex colors.
		//! This material is currently not implemented in OpenGL, but it
		//! works with DirectX.
		EMT_SOLID_2_LAYER,

		//! Material type with standard lightmap technique:
		//! There should be 2 textures: The first texture layer is a diffuse map,
		//! the second is a light map. Vertex light is ignored.
		EMT_LIGHTMAP,

		//! Material type with lightmap technique like EMT_LIGHTMAP, but
		//! lightmap and diffuse texture are not modulated, but added instead.
		EMT_LIGHTMAP_ADD,

		//! Material type with standard lightmap technique:
		//! There should be 2 textures: The first texture layer is a diffuse map,
		//! the second is a light map. Vertex light is ignored.
		//! The texture colors are effectively multiplyied by 2 for brightening.
		//! like known in DirectX as D3DTOP_MODULATE2X.
		EMT_LIGHTMAP_M2,

		//! Material type with standard lightmap technique:
		//! There should be 2 textures: The first texture layer is a diffuse map,
		//! the second is a light map. Vertex light is ignored.
		//! The texture colors are effectively multiplyied by 4 for brightening.
		//! like known in DirectX as D3DTOP_MODULATE4X.
		EMT_LIGHTMAP_M4,

		//! Like EMT_LIGHTMAP, but also supports dynamic lighting.
		EMT_LIGHTMAP_LIGHTING,

		//! Like EMT_LIGHTMAP_M2, but also supports dynamic lighting.
		EMT_LIGHTMAP_LIGHTING_M2,

		//! Like EMT_LIGHTMAP_4, but also supports dynamic lighting.
		EMT_LIGHTMAP_LIGHTING_M4,

		//! Detail mapped material. The first texture is diffuse color map, the
		//! second is added to this and usually displayed with a bigger scale value
		//! so that it adds more detail. The detail map is added to the diffuse map using
		//! ADD_SIGNED, so that it is possible to add and substract color from the diffuse
		//! map. For example a value of (127,127,127) will not change the appearance of
		//! the diffuse map at all.
		//! Often used for terrain rendering.
		EMT_DETAIL_MAP,

		//! Makes the material look like it was reflection the environment
		//! around it. To make this possible, a texture called 'sphere map'
		//! is used, which must be set as Textures[0].
		EMT_SPHERE_MAP,

		//! A reflecting material with an
		//! optional additional non reflecting texture layer. The reflection
		//! map should be set as Texture 1.
		EMT_REFLECTION_2_LAYER,

		//! A transparent material. Only the first texture is used.
		//! The new color is calculated by simply adding the source color and
		//! the dest color. This means if for example a billboard using a texture with
		//! black background and a red circle on it is drawn with this material, the
		//! result is that only the red circle will be drawn a little bit transparent,
		//! and everything which was black is 100% transparent and not visible.
		//! This material type is useful for e.g. particle effects.
		EMT_TRANSPARENT_ADD_COLOR,

		//! Makes the material transparent based on the texture alpha channel.
		//! The final color is blended together from the destination color and the
		//! texture color, using the alpha channel value as blend factor.
		//! Only first texture is used. If you are using this material with small
		//! textures, it is a good idea to load the texture in 32 bit
		//! mode (video::IVideoDriver::setTextureCreationFlag()).
		//! Also, an alpha ref is used, which can be manipulated using SMaterial::MaterialTypeParam.
		//! If set to 0, the alpha ref gets its default value which is 0.5f and which means
		//! that pixels with an alpha value >127 will be written, others not.  In other, simple
		//! words: this value controls how sharp the edges become when going from a
		//! transparent to a solid spot on the texture.
		EMT_TRANSPARENT_ALPHA_CHANNEL,

		//! Makes the material transparent based on the texture alpha channel.
		//! If the alpha channel value is greater than 127, a pixel is written to the
		//! target, otherwise not. This material does not use alpha blending
		//! and is a lot faster than EMT_TRANSPARENT_ALPHA_CHANNEL. It
		//! is ideal for drawing stuff like leafes of plants, because the borders
		//! are not blurry but sharp.
		//! Only first texture is used. If you are using this material with small
		//! textures and 3d object, it is a good idea to load the texture in 32 bit
		//! mode (video::IVideoDriver::setTextureCreationFlag()).
		EMT_TRANSPARENT_ALPHA_CHANNEL_REF,

		//! Makes the material transparent based on the vertex alpha value.
		EMT_TRANSPARENT_VERTEX_ALPHA,

		//! A transparent reflecting material with an
		//! optional additional non reflecting texture layer. The reflection
		//! map should be set as Texture 1. The transparency depends on the
		//! alpha value in the vertex colors. A texture which will not reflect
		//! can be set als Texture 2.
		//! Please note that this material type is currently not 100% implemented
		//! in OpenGL. It works in Direct3D.
		EMT_TRANSPARENT_REFLECTION_2_LAYER,

		//! A solid normal map renderer. First texture is the color map, the
		//! second should be the normal map. Note that you should use this material
		//! only when drawing geometry consisting of vertices of type S3DVertexTangents
		//! (EVT_TANGENTS). You can convert any mesh into this format using
		//! IMeshManipulator::createMeshWithTangents() (See SpecialFX2 Tutorial).
		//! This shader runs on vertex shader 1.1 and pixel shader 1.1 capable hardware and
		//! falls back on a fixed function lighted material if this hardware is not available.
		//! Only two lights are supported by this shader, if there are more, the nearest two
		//! are chosen. Currently, this shader is only implemented for the D3D8 and D3D9 renderers.
		EMT_NORMAL_MAP_SOLID,

		//! A transparent normal map renderer. First texture is the color map, the
		//! second should be the normal map. Note that you should use this material
		//! only when drawing geometry consisting of vertices of type S3DVertexTangents
		//! (EVT_TANGENTS). You can convert any mesh into this format using
		//! IMeshManipulator::createMeshWithTangents() (See SpecialFX2 Tutorial).
		//! This shader runs on vertex shader 1.1 and pixel shader 1.1 capable hardware and
		//! falls back on a fixed function lighted material if this hardware is not available.
		//! Only two lights are supported by this shader, if there are more, the nearest two
		//! are chosen. Currently, this shader is only implemented for the D3D8 and D3D9 renderers.
		EMT_NORMAL_MAP_TRANSPARENT_ADD_COLOR,

		//! A transparent (based on the vertex alpha value) normal map renderer.
		//! First texture is the color map, the
		//! second should be the normal map. Note that you should use this material
		//! only when drawing geometry consisting of vertices of type S3DVertexTangents
		//! (EVT_TANGENTS). You can convert any mesh into this format using
		//! IMeshManipulator::createMeshWithTangents() (See SpecialFX2 Tutorial).
		//! This shader runs on vertex shader 1.1 and pixel shader 1.1 capable hardware and
		//! falls back on a fixed function lighted material if this hardware is not available.
		//! Only two lights are supported by this shader, if there are more, the nearest two
		//! are chosen. Currently, this shader is only implemented for the D3D8 and D3D9 renderers.
		EMT_NORMAL_MAP_TRANSPARENT_VERTEX_ALPHA,

		//! Just like EMT_NORMAL_MAP_SOLID, but uses parallax mapping too, which
		//! looks a lot more realistic. This only works when the hardware supports at
		//! least vertex shader 1.1 and pixel shader 1.4.
		//! First texture is the color map, the  second should be the normal map.
		//! The normal map texture should contain the height value in the
		//! alpha component. The IVideoDriver::makeNormalMapTexture() method writes
		//! this value automaticly when creating normal maps from a heightmap when using a 32 bit
		//! texture.
		//! The height scale of the material (affecting the bumpiness) is being controlled
		//! by the SMaterial::MaterialTypeParam member.
		//! If set to zero, the default value (0.02f) will be applied. Otherwise
		//! the value set in SMaterial::MaterialTypeParam is taken. This value depends on with which
		//! scale the texture is mapped on the material. Too high or low values of MaterialTypeParam
		//! can result in strange artifacts.
		EMT_PARALLAX_MAP_SOLID,

		//! A material just like EMT_PARALLAX_MAP_SOLID, but it is transparent, using
		//! EMT_TRANSPARENT_ADD_COLOR as base material.
		EMT_PARALLAX_MAP_TRANSPARENT_ADD_COLOR,

		//! A material just like EMT_PARALLAX_MAP_SOLID, but it is transparent, using
		//! EMT_TRANSPARENT_VERTEX_ALPHA as base material.
		EMT_PARALLAX_MAP_TRANSPARENT_VERTEX_ALPHA,

		//! BlendFunc = source * sourceFactor + dest * destFactor ( E_BLEND_FUNC )
		//! Using only Textures[0]. generic Blender
		EMT_ONETEXTURE_BLEND,

		//! This value is not used. It only forces this enumeration to compile in 32 bit.
		EMT_FORCE_32BIT = 0x7fffffff
	};

	//! Flag for EMT_ONETEXTURE_BLEND, ( BlendFactor )
	//! BlendFunc = source * sourceFactor + dest * destFactor
	enum E_BLEND_FACTOR
	{
		EBF_ZERO	= 0,		// src & dest	(0, 0, 0, 0)
		EBF_ONE,			// src & dest	(1, 1, 1, 1)
		EBF_DST_COLOR, 			// src		(destR, destG, destB, destA)
		EBF_ONE_MINUS_DST_COLOR, 	// src		(1-destR, 1-destG, 1-destB, 1-destA)
		EBF_SRC_COLOR,			// dest		(srcR, srcG, srcB, srcA)
		EBF_ONE_MINUS_SRC_COLOR, 	// dest		(1-srcR, 1-srcG, 1-srcB, 1-srcA)
		EBF_SRC_ALPHA,			// src & dest	(srcA, srcA, srcA, srcA)
		EBF_ONE_MINUS_SRC_ALPHA,	// src & dest	(1-srcA, 1-srcA, 1-srcA, 1-srcA)
		EBF_DST_ALPHA,			// src & dest	(destA, destA, destA, destA)
		EBF_ONE_MINUS_DST_ALPHA,	// src & dest	(1-destA, 1-destA, 1-destA, 1-destA)
		EBF_SRC_ALPHA_SATURATE		// src		(min(srcA, 1-destA), idem, ...)
	};

	//! Texture coord clamp mode outside [0.0, 1.0]
	enum E_TEXTURE_CLAMP
	{
		//! Texture repeats
		ETC_REPEAT = 0,
		//! Texture is clamped to the last pixel
		ETC_CLAMP,
		//! Texture is clamped to the edge pixel
		ETC_CLAMP_TO_EDGE,
		//! Texture is clamped to the border pixel (if exists)
		ETC_CLAMP_TO_BORDER,
		//! Texture is alternatingly mirrored (0..1..0..1..0..)
		ETC_MIRROR
	};
	static const char* const aTextureClampNames[] = {
			"texture_clamp_repeat",
			"texture_clamp_clamp",
			"texture_clamp_clamp_to_edge",
			"texture_clamp_clamp_to_border",
			"texture_clamp_mirror", 0};

	//! MaterialTypeParam: eg. DirectX: D3DTOP_MODULATE, D3DTOP_MODULATE2X, D3DTOP_MODULATE4X
	enum E_MODULATE_FUNC
	{
		EMFN_MODULATE_1X	= 1,
		EMFN_MODULATE_2X	= 2,
		EMFN_MODULATE_4X	= 4
	};

	//! EMT_ONETEXTURE_BLEND: pack srcFact & dstFact and Modulo to MaterialTypeParam
	inline f32 pack_texureBlendFunc ( const E_BLEND_FACTOR srcFact, const E_BLEND_FACTOR dstFact, const E_MODULATE_FUNC modulate )
	{
		return (f32)(modulate << 16 | srcFact << 8 | dstFact);
	}

	//! EMT_ONETEXTURE_BLEND: unpack srcFact & dstFact and Modulo to MaterialTypeParam
	inline void unpack_texureBlendFunc ( E_BLEND_FACTOR &srcFact, E_BLEND_FACTOR &dstFact, E_MODULATE_FUNC &modulo, const f32 param )
	{
		const u32 state = (u32)param;
		modulo	= E_MODULATE_FUNC  ( ( state & 0x00FF0000 ) >> 16 );
		srcFact = E_BLEND_FACTOR   ( ( state & 0x0000FF00 ) >> 8  );
		dstFact = E_BLEND_FACTOR   ( ( state & 0x000000FF )       );
	}

	//! Material flags
	enum E_MATERIAL_FLAG
	{
		//! Draw as wireframe or filled triangles? Default: false
		EMF_WIREFRAME = 0,

		//! Draw as point cloud or filled triangles? Default: false
		EMF_POINTCLOUD,

		//! Flat or Gouraud shading? Default: true
		EMF_GOURAUD_SHADING,

		//! Will this material be lighted? Default: true
		EMF_LIGHTING,

		//! Is the ZBuffer enabled? Default: true
		EMF_ZBUFFER,

		//! May be written to the zbuffer or is it readonly. Default: true
		//! This flag is ignored, if the material type is a transparent type.
		EMF_ZWRITE_ENABLE,

		//! Is backfaceculling enabled? Default: true
		EMF_BACK_FACE_CULLING,

		//! Is bilinear filtering enabled? Default: true
		EMF_BILINEAR_FILTER,

		//! Is trilinear filtering enabled? Default: false
		//! If the trilinear filter flag is enabled,
		//! the bilinear filtering flag is ignored.
		EMF_TRILINEAR_FILTER,

		//! Is anisotropic filtering? Default: false
		//! In Irrlicht you can use anisotropic texture filtering in
		//! conjunction with bilinear or trilinear texture filtering
		//! to improve rendering results. Primitives will look less
		//! blurry with this flag switched on.
		EMF_ANISOTROPIC_FILTER,

		//! Is fog enabled? Default: false
		EMF_FOG_ENABLE,

		//! Normalizes normals.You can enable this if you need
		//! to scale a dynamic lighted model. Usually, its normals will get scaled
		//! too then and it will get darker. If you enable the EMF_NORMALIZE_NORMALS flag,
		//! the normals will be normalized again, and the model will look as bright as it should.
		EMF_NORMALIZE_NORMALS,

		//! Access to all layers texture wrap settings. Overwrites separate layer settings.
		EMF_TEXTURE_WRAP,

		//! This is not a flag, but a value indicating how much flags there are.
		EMF_MATERIAL_FLAG_COUNT
	};

	//! Maximum number of texture an SMaterial can have.
	const u32 MATERIAL_MAX_TEXTURES = 4;


	//! struct for holding parameters for a material renderer
	class SMaterial
	{
	public:
		//! default constructor, creates a solid material with standard colors
		SMaterial()
		: MaterialType(EMT_SOLID), AmbientColor(255,255,255,255), DiffuseColor(255,255,255,255),
			EmissiveColor(0,0,0,0), SpecularColor(255,255,255,255),
			Shininess(0.0f), MaterialTypeParam(0.0f), MaterialTypeParam2(0.0f), Thickness(1.0f),
			Wireframe(false), PointCloud(false), GouraudShading(true), Lighting(true),
			ZBuffer(true), ZWriteEnable(true), BackfaceCulling(true),
			BilinearFilter(true), TrilinearFilter(false), AnisotropicFilter(false),
			FogEnable(false), NormalizeNormals(false)
		{
			for (u32 i=0; i<MATERIAL_MAX_TEXTURES; ++i)
			{
				Textures[i] = 0;
				TextureMatrix[i] = 0;
				TextureWrap[i] = ETC_REPEAT;
			}
		}

		//! copy constructor
		SMaterial(const SMaterial& other)
		{
			// These pointers are checked during assignment
			for (u32 i=0; i<MATERIAL_MAX_TEXTURES; ++i)
			{
				TextureMatrix[i] = 0;
			}
			*this = other;
		}

		//! destructor
		~SMaterial()
		{
			for (u32 i=0; i<MATERIAL_MAX_TEXTURES; ++i)
				if (TextureMatrix[i])
					delete TextureMatrix[i];
		}

		//! Assignment operator
		SMaterial& operator=(const SMaterial& other)
		{
			MaterialType = other.MaterialType;

			AmbientColor = other.AmbientColor;
			DiffuseColor = other.DiffuseColor;
			EmissiveColor = other.EmissiveColor;
			SpecularColor = other.SpecularColor;
			Shininess = other.Shininess;
			MaterialTypeParam = other.MaterialTypeParam;
			MaterialTypeParam2 = other.MaterialTypeParam2;
			Thickness = other.Thickness;
			for (u32 i=0; i<MATERIAL_MAX_TEXTURES; ++i)
			{
				Textures[i] = other.Textures[i];
				if (TextureMatrix[i])
				{
					if (other.TextureMatrix[i])
						*TextureMatrix[i] = *other.TextureMatrix[i];
					else
					{
						delete TextureMatrix[i];
						TextureMatrix[i] = 0;
					}
				}
				else
				{
					if (other.TextureMatrix[i])
						TextureMatrix[i] = new core::matrix4(*other.TextureMatrix[i]);
					else
						TextureMatrix[i] = 0;
				}
				TextureWrap[i] = other.TextureWrap[i];
			}

			Wireframe = other.Wireframe;
			PointCloud = other.PointCloud;
			GouraudShading = other.GouraudShading;
			Lighting = other.Lighting;
			ZBuffer = other.ZBuffer;
			ZWriteEnable = other.ZWriteEnable;
			BackfaceCulling = other.BackfaceCulling;
			BilinearFilter = other.BilinearFilter;
			TrilinearFilter = other.TrilinearFilter;
			AnisotropicFilter = other.AnisotropicFilter;
			FogEnable = other.FogEnable;
			NormalizeNormals = other.NormalizeNormals;

			return *this;
		}

		//! Type of the material. Specifies how everything is blended together
		E_MATERIAL_TYPE MaterialType;

		//! How much ambient light (a global light) is reflected by this material.
		/** The default is full white, meaning objects are completely globally illuminated.
		 Reduce this if you want to see diffuse or specular light effects. */
		SColor AmbientColor;

		//! How much diffuse light coming from a light source is reflected by this material.
		/** The default is full white. */
		SColor DiffuseColor;

		//! Light emitted by this material. Default is to emitt no light.
		SColor EmissiveColor;

		//! How much specular light (highlights from a light) is reflected.
		/** The default is to reflect white specular light.  See SMaterial::Shininess how to
		enable specular lights. */
		SColor SpecularColor;

		//! Value affecting the size of specular highlights. A value of 20 is common.
		/** If set to 0, no specular highlights are being used.
		To activate, simply set the shininess of a material to a value other than 0:
		Using scene nodes:
		\code
		sceneNode->getMaterial(0).Shininess = 20.0f;
		\endcode

		You can also change the color of the highlights using
		\code
		sceneNode->getMaterial(0).SpecularColor.set(255,255,255,255);
		\endcode

		The specular color of the dynamic lights (SLight::SpecularColor) will influence
		the the highlight color too, but they are set to a useful value by default when
		creating the light scene node. Here is a simple example on how
		to use specular highlights:
		\code
		// load and display mesh
		scene::IAnimatedMeshSceneNode* node = smgr->addAnimatedMeshSceneNode(
		smgr->getMesh("data/faerie.md2"));
		node->setMaterialTexture(0, driver->getTexture("data/Faerie2.pcx")); // set diffuse texture
		node->setMaterialFlag(video::EMF_LIGHTING, true); // enable dynamic lighting
		node->getMaterial(0).Shininess = 20.0f; // set size of specular highlights

		// add white light
		scene::ILightSceneNode* light = smgr->addLightSceneNode(0,
		    core::vector3df(5,5,5), video::SColorf(1.0f, 1.0f, 1.0f));
		\endcode */
		f32 Shininess;

		//! Free parameter dependend on the material type.
		/** Mostly ignored, used for example in EMT_PARALLAX_MAP_SOLID
		and EMT_TRANSPARENT_ALPHA_CHANNEL. */
		f32 MaterialTypeParam;

		//! Second free parameter dependend on the material type.
		/** Mostly ignored. */
		f32 MaterialTypeParam2;

		//! Thickness of non-3dimensional elements such as lines and points.
		f32 Thickness;

		//! Texture layer array.
		ITexture* Textures[MATERIAL_MAX_TEXTURES];

		//! Texture Matrix array
		//! Do not acces the elements directly as the internal
		//! ressource management has to cope with Null pointers etc.
		core::matrix4* TextureMatrix[MATERIAL_MAX_TEXTURES];

		//! Texture Clamp Mode
		E_TEXTURE_CLAMP TextureWrap[MATERIAL_MAX_TEXTURES];

		//! material flags
		/** The user can access the material flag using 
		material.Wireframe = true or material.setFlag(EMF_WIREFRAME, true); */
		//! Draw as wireframe or filled triangles? Default: false
		bool Wireframe;

		//! Draw as point cloud or filled triangles? Default: false
		bool PointCloud;

		//! Flat or Gouraud shading? Default: true
		bool GouraudShading;

		//! Will this material be lighted? Default: true
		bool Lighting;

		//! Is the ZBuffer enabled? Default: true
		//! Changed from Bool to Integer
		// ( 0 == ZBuffer Off, 1 == ZBuffer LessEqual, 2 == ZBuffer Equal )
		u32 ZBuffer;

		//! May be written to the zbuffer or is it readonly.
		/** Default: 1 This flag is ignored, if the MaterialType
		is a transparent type. */
		bool ZWriteEnable;

		//! Is backfaceculling enabled? Default: true
		bool BackfaceCulling;

		//! Is bilinear filtering enabled? Default: true
		bool BilinearFilter;

		//! Is trilinear filtering enabled? Default: false
		/** If the trilinear filter flag is enabled,
		the bilinear filtering flag is ignored. */
		bool TrilinearFilter;

		//! Is anisotropic filtering enabled? Default: false
		/** In Irrlicht you can use anisotropic texture filtering
		    in conjunction with bilinear or trilinear texture
		    filtering to improve rendering results. Primitives
		    will look less blurry with this flag switched on. */
		bool AnisotropicFilter;

		//! Is fog enabled? Default: false
		bool FogEnable;

		//! Should normals be normalized? Default: false
		bool NormalizeNormals;

		//! Gets the texture transformation matrix for level i
		core::matrix4& getTextureMatrix(u32 i)
		{
			if (i<MATERIAL_MAX_TEXTURES && !TextureMatrix[i])
				TextureMatrix[i] = new core::matrix4(core::matrix4::EM4CONST_IDENTITY);
			return *TextureMatrix[i];
		}

		//! Gets the immutable texture transformation matrix for level i
		const core::matrix4& getTextureMatrix(u32 i) const
		{
			if (i<MATERIAL_MAX_TEXTURES && TextureMatrix[i])
				return *TextureMatrix[i];
			else
				return core::IdentityMatrix;
		}

		//! Sets the i-th texture transformation matrix to mat
		void setTextureMatrix(u32 i, const core::matrix4& mat)
		{
			if (i>=MATERIAL_MAX_TEXTURES)
				return;
			if (!TextureMatrix[i])
				TextureMatrix[i] = new core::matrix4(mat);
			else
				*TextureMatrix[i] = mat;
		}

		//! Sets the Material flag to the given value
		void setFlag(E_MATERIAL_FLAG flag, bool value)
		{
			switch (flag)
			{
				case EMF_WIREFRAME:
					Wireframe = value; break;
				case EMF_POINTCLOUD:
					PointCloud = value; break;
				case EMF_GOURAUD_SHADING:
					GouraudShading = value; break;
				case EMF_LIGHTING:
					Lighting = value; break;
				case EMF_ZBUFFER:
					ZBuffer = value; break;
				case EMF_ZWRITE_ENABLE:
					ZWriteEnable = value; break;
				case EMF_BACK_FACE_CULLING:
					BackfaceCulling = value; break;
				case EMF_BILINEAR_FILTER:
					BilinearFilter = value; break;
				case EMF_TRILINEAR_FILTER:
					TrilinearFilter = value; break;
				case EMF_ANISOTROPIC_FILTER:
					AnisotropicFilter = value; break;
				case EMF_FOG_ENABLE:
					FogEnable = value; break;
				case EMF_NORMALIZE_NORMALS:
					NormalizeNormals = value; break;
				case EMF_TEXTURE_WRAP:
					TextureWrap[0] = TextureWrap[1] = TextureWrap[2] = TextureWrap[3] = (E_TEXTURE_CLAMP)value;
					break;
				default:
					break;
			}
		}

		//! Gets the Material flag
		bool getFlag(E_MATERIAL_FLAG flag) const
		{
			switch (flag)
			{
				case EMF_WIREFRAME:
					return Wireframe;
				case EMF_POINTCLOUD:
					return PointCloud;
				case EMF_GOURAUD_SHADING:
					return GouraudShading;
				case EMF_LIGHTING:
					return Lighting;
				case EMF_ZBUFFER:
					return ZBuffer!=0;
				case EMF_ZWRITE_ENABLE:
					return ZWriteEnable;
				case EMF_BACK_FACE_CULLING:
					return BackfaceCulling;
				case EMF_BILINEAR_FILTER:
					return BilinearFilter;
				case EMF_TRILINEAR_FILTER:
					return TrilinearFilter;
				case EMF_ANISOTROPIC_FILTER:
					return AnisotropicFilter;
				case EMF_FOG_ENABLE:
					return FogEnable;
				case EMF_NORMALIZE_NORMALS:
					return NormalizeNormals;
				case EMF_TEXTURE_WRAP:
					return !(TextureWrap[0] || TextureWrap[1] || TextureWrap[2] || TextureWrap[3]);
				case EMF_MATERIAL_FLAG_COUNT:
					break;
			}

			return false;
		}

		//! Inequality operator
		inline bool operator!=(const SMaterial& b) const
		{
			return 
				Textures[0] != b.Textures[0] ||
				Textures[1] != b.Textures[1] ||
				Textures[2] != b.Textures[2] ||
				Textures[3] != b.Textures[3] ||
				MaterialType != b.MaterialType ||
				AmbientColor != b.AmbientColor ||
				DiffuseColor != b.DiffuseColor ||
				EmissiveColor != b.EmissiveColor ||
				SpecularColor != b.SpecularColor ||
				Shininess != b.Shininess ||
				MaterialTypeParam != b.MaterialTypeParam ||
				MaterialTypeParam2 != b.MaterialTypeParam2 ||
				Thickness != b.Thickness ||
				Wireframe != b.Wireframe ||
				PointCloud != b.PointCloud ||
				GouraudShading != b.GouraudShading ||
				Lighting != b.Lighting ||
				ZBuffer != b.ZBuffer ||
				ZWriteEnable != b.ZWriteEnable ||
				BackfaceCulling != b.BackfaceCulling ||
				BilinearFilter != b.BilinearFilter ||
				TrilinearFilter != b.TrilinearFilter ||
				AnisotropicFilter != b.AnisotropicFilter ||
				FogEnable != b.FogEnable ||
				NormalizeNormals != b.NormalizeNormals ||
				TextureWrap[0] != b.TextureWrap[0] ||
				TextureWrap[1] != b.TextureWrap[1] ||
				TextureWrap[2] != b.TextureWrap[2] ||
				TextureWrap[3] != b.TextureWrap[3] ||
				TextureMatrix[0] != b.TextureMatrix[0] ||
				TextureMatrix[1] != b.TextureMatrix[1] ||
				TextureMatrix[2] != b.TextureMatrix[2] ||
				TextureMatrix[3] != b.TextureMatrix[3];
		}

		//! Equality operator
		inline bool operator==(const SMaterial& b) const
		{ return !(b!=*this); }
	};

} // end namespace video
} // end namespace irr

#endif

