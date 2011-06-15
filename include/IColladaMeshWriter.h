// Copyright (C) 2002-2011 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __IRR_I_COLLADA_MESH_WRITER_H_INCLUDED__
#define __IRR_I_COLLADA_MESH_WRITER_H_INCLUDED__

#include "IMeshWriter.h"
#include "SMaterial.h"

namespace irr
{
namespace io
{
	class IWriteFile;
} // end namespace io

namespace scene
{
	//! Lighting models - more or less the way Collada categorizes materials
	enum E_COLLADA_TECHNIQUE_FX
	{
		//! Blinn-phong which is default for opengl and dx fixed function pipelines.
		//! But several well-known renderers don't support it and prefer phong.
		ECTF_BLINN,	
		//! Phong shading, default in many external renderers.
		ECTF_PHONG,
		//! diffuse shaded surface that is independent of lighting.
		ECTF_LAMBERT,
		// constantly shaded surface that is independent of lighting. 
		ECTF_CONSTANT
	};

	//! How to export opacity to collada
	/** Note that there is also a check for transparent textures in IColladaMeshWriterProperties
	which will be evaluated additionally to the flags:
	No ECOF_TRANSPARENT_* flag and no texture => <transparent> will not be written at all.
	No ECOF_TRANSPARENT_* flag but a valid texture => texture will be written.
	ECOF_TRANSPARENT_* flag is set and no texture => A color will be written.
	Both, a ECOF_TRANSPARENT_* flag and a valid texture => only the texture will be written.

	You can only have either ECOF_A_ONE or ECOF_RGB_ZERO otherwise ECOF_RGB_ZERO is used.
	You can only have one of the ECOF_TRANSPARENT_* flags otherwise the first is used.
	*/
	enum E_COLLADA_TRANSPARENT_FX
	{
		//! default - only alpha channel of color or texture is used. Can't be used together with ECOF_RGB_ZERO.
		ECOF_A_ONE = 0,

		//! Alpha values for each RGB channel of color or texture are used. Can't be used together with ECOF_A_ONE.
		ECOF_RGB_ZERO = 1,

		//! Write diffuse material color as transparent
		ECOF_TRANSPARENT_DIFFUSE = 2,

		//! Write ambient material color as transparent
		ECOF_TRANSPARENT_AMBIENT = 4,

		//! Write emissive material color as transparent
		ECOF_TRANSPARENT_EMISSIVE = 8, 

		//! Write specular material color as transparent
		ECOF_TRANSPARENT_SPECULAR = 16,
	};

	//! Callback interface for properties which can be used to influence collada writing
	class IColladaMeshWriterProperties  : public virtual IReferenceCounted
	{
	public:
		virtual ~IColladaMeshWriterProperties ()	{}

		//! Which lighting model should be used in the technique (FX) section when exporting effects (materials)
		virtual E_COLLADA_TECHNIQUE_FX getTechniqueFx(const video::SMaterial& material) const = 0;

		//! Which texture index should be used when writing the emissive texture
		/** \return the index to the texture-layer or -1 if that texture should never be exported */
		virtual s32 getEmissiveTextureIdx(const video::SMaterial& material) const	= 0;

		//! Which texture index should be used when writing the ambient texture
		/** \return the index to the texture-layer or -1 if that texture should never be exported */
		virtual s32 getAmbientTextureIdx(const video::SMaterial& material) const 	= 0;

		//! Which texture index should be used when writing the diffuse texture
		/** \return the index to the texture-layer or -1 if that texture should never be exported */
		virtual s32 getDiffuseTextureIdx(const video::SMaterial& material) const 	= 0;

		//! Which texture index should be used when writing the specular texture
		/** \return the index to the texture-layer or -1 if that texture should never be exported */
		virtual s32 getSpecularTextureIdx(const video::SMaterial& material) const 	= 0;

		//! Which texture index should be used when writing the transparent texture
		/** Note: By default the alpha channel is used, if you want to use RGB you have to set
		the ECOF_RGB_ZERO flag in getTransparentFx.
		\return the index to the texture-layer or -1 if that texture should never be exported */
		virtual s32 getTransparentTextureIdx( const video::SMaterial& material) const 	= 0;

		//! Return the settings for transparence
		virtual E_COLLADA_TRANSPARENT_FX getTransparentFx(const video::SMaterial& material) const = 0;

		//! Transparency value for the material. 
		/** This value is additional to transparent settings, if both are set they will be multiplicated.
		\return 1.0 for fully transparent, 0.0 for not transparent and not written at all when < 0.f */
		virtual f32 getTransparency(const video::SMaterial& material) const = 0;
	};


	//! Interface for writing meshes
	class IColladaMeshWriter : public IMeshWriter
	{
	public:

		IColladaMeshWriter() : Properties(0), DefaultProperties(0), WriteTextures(true), WriteScene(false)

		{
		}

		//! Destructor
		virtual ~IColladaMeshWriter() 
		{
			if ( Properties )
				Properties->drop();
			if ( DefaultProperties )
				DefaultProperties->drop();
		}

		//! Set if texture information should be written
		virtual void setWriteTextures(bool write)
		{
			WriteTextures = write;
		}

		//! Get if texture information should be written
		virtual bool getWriteTextures() const 
		{
			return WriteTextures;
		}

		//! Set if a default scene should be written
		/** Many collada readers fail to read a mesh if the collada files doesn't contain a scene as well.
		The scene is doing an instantiation of the mesh.
		*/
		virtual void setWriteScene(bool write)
		{
			WriteScene = write;
		}

		//! Get if a default scene should be written
		virtual bool getWriteScene() const
		{
			return WriteScene;
		}

		//! Set properties to use by the meshwriter instead of it's default properties.
		/** Overloading properties with an own class allows modifying the writing process in certain ways. 
		By default properties are set to the DefaultProperties. */
		virtual void setProperties(IColladaMeshWriterProperties * p)
		{
			if ( p == Properties )
				return;
			if ( p ) 
				p->grab();
			if ( Properties )
				Properties->drop();
			Properties = p;
		}

		//! Get properties which are currently used.
		virtual IColladaMeshWriterProperties * getProperties()
		{
			return Properties;
		}

		//! Return the original default properties of the writer. 
		/** You can use this pointer in your own properties to access and return default values. */
		IColladaMeshWriterProperties * getDefaultProperties() const 
		{ 
			return DefaultProperties; 
		}

	protected:
		// NOTE: you should also call setProperties
		virtual void setDefaultProperties(IColladaMeshWriterProperties * p)
		{
			if ( p == DefaultProperties )
				return;
			if ( p ) 
				p->grab();
			if ( DefaultProperties )
				DefaultProperties->drop();
			DefaultProperties = p;
		}

	private:
		IColladaMeshWriterProperties * Properties;
		IColladaMeshWriterProperties * DefaultProperties;
		bool WriteTextures;
		bool WriteScene;
	};


} // end namespace
} // end namespace

#endif
