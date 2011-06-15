// Copyright (C) 2002-2011 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __IRR_C_COLLADA_MESH_WRITER_H_INCLUDED__
#define __IRR_C_COLLADA_MESH_WRITER_H_INCLUDED__

#include "IColladaMeshWriter.h"
#include "S3DVertex.h"
#include "IVideoDriver.h"

namespace irr
{
namespace io
{
	class IXMLWriter;
	class IFileSystem;
}
namespace scene
{

	//! Callback interface for properties which can be used to influence collada writing
	class CColladaMeshWriterProperties  : public virtual IColladaMeshWriterProperties
	{
	public:
		//! Which lighting model should be used in the technique (FX) section when exporting effects (materials)
		virtual E_COLLADA_TECHNIQUE_FX getTechniqueFx(const video::SMaterial& material) const;

		//! Which texture index should be used when setting the emissive texture
		/** \return the index to the texture-layer or -1 if that texture should never be exported */
		virtual s32 getEmissiveTextureIdx(const video::SMaterial& material) const;

		//! Which texture index should be used when setting the ambient texture
		/** \return the index to the texture-layer or -1 if that texture should never be exported */
		virtual s32 getAmbientTextureIdx(const video::SMaterial& material) const;

		//! Which texture index should be used when setting the diffuse texture
		/** \return the index to the texture-layer or -1 if that texture should never be exported */
		virtual s32 getDiffuseTextureIdx(const video::SMaterial& material) const;

		//! Which texture index should be used when setting the specular texture
		/** \return the index to the texture-layer or -1 if that texture should never be exported */
		virtual s32 getSpecularTextureIdx(const video::SMaterial& material) const;
	};


//! class to write meshes, implementing a COLLADA (.dae, .xml) writer
/** This writer implementation has been originally developed for irrEdit and then
merged out to the Irrlicht Engine */
class CColladaMeshWriter : public IColladaMeshWriter
{
public:

	CColladaMeshWriter(video::IVideoDriver* driver, io::IFileSystem* fs);
	virtual ~CColladaMeshWriter();

	//! Returns the type of the mesh writer
	virtual EMESH_WRITER_TYPE getType() const;

	//! writes a mesh
	virtual bool writeMesh(io::IWriteFile* file, scene::IMesh* mesh, s32 flags=EMWF_NONE);


protected:

	bool hasSecondTextureCoordinates(video::E_VERTEX_TYPE type) const;
	inline irr::core::stringw toString(const irr::core::vector3df& vec) const;
	inline irr::core::stringw toString(const irr::core::vector2df& vec) const;
	inline irr::core::stringw toString(const irr::video::SColorf& colorf) const;
	inline irr::core::stringw toString(const irr::video::ECOLOR_FORMAT format) const;
	inline irr::core::stringw toString(const irr::video::E_TEXTURE_CLAMP clamp) const;
	irr::core::stringw minTexfilterToString(bool bilinear, bool trilinear) const;
	irr::core::stringw magTexfilterToString(bool bilinear, bool trilinear) const;
	irr::core::stringw pathToNCName(const irr::io::path& path) const;
	irr::core::stringw pathToURI(const irr::io::path& path) const;
	inline bool isXmlNameStartChar(wchar_t c) const;
	inline bool isXmlNameChar(wchar_t c) const;
	void writeColorElement(const video::SColor & col);
	bool writeTextureSampler(const video::SMaterial & material, video::E_COLOR_MATERIAL cm);
	void writeFxElement(const video::SMaterial & material, E_COLLADA_TECHNIQUE_FX techFx);

	struct SComponentGlobalStartPos
	{
		SComponentGlobalStartPos() : PosStartIndex(-1), PosLastIndex(-1),
				NormalStartIndex(-1), NormalLastIndex(-1),
				TCoord0StartIndex(-1), TCoord0LastIndex(-1),
				TCoord1StartIndex(-1), TCoord1LastIndex(-1)
		{ }

		s32 PosStartIndex;
		s32 PosLastIndex;

		s32 NormalStartIndex;
		s32 NormalLastIndex;

		s32 TCoord0StartIndex;
		s32 TCoord0LastIndex;

		s32 TCoord1StartIndex;
		s32 TCoord1LastIndex;
	};

	io::IFileSystem* FileSystem;
	video::IVideoDriver* VideoDriver;
	io::IXMLWriter* Writer;
	core::array<video::ITexture*> LibraryImages;
	io::path Directory;
};


} // end namespace
} // end namespace

#endif


