// Copyright (C) 2002-2011 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __IRR_C_COLLADA_MESH_WRITER_H_INCLUDED__
#define __IRR_C_COLLADA_MESH_WRITER_H_INCLUDED__

#include "IColladaMeshWriter.h"
#include "S3DVertex.h"
#include "irrMap.h"
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

		//! Which texture index should be used when writing the texture of the given sampler color.
		/** \return the index to the texture-layer or -1 if that texture should never be exported */
		virtual s32 getTextureIdx(const video::SMaterial & material, E_COLLADA_COLOR_SAMPLER cs) const;

		//! Return the settings for transparence
		virtual E_COLLADA_TRANSPARENT_FX getTransparentFx(const video::SMaterial& material) const;

		//! Transparency value for the material. 
		/** This value is additional to transparent settings, if both are set they will be multiplicated.
		\return 1.0 for fully transparent, 0.0 for not transparent and not written at all when < 0.f */
		virtual f32 getTransparency(const video::SMaterial& material) const;

		//! Should node be used in scene export? By default all visible nodes are exported.
		virtual bool isExportable(const irr::scene::ISceneNode * node) const;

		//! Return the mesh for the given nod. If it has no mesh or shouldn't export it's mesh return 0.
		virtual IMesh* getMesh(irr::scene::ISceneNode * node);
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

	//! writes a scene starting with the given node
	virtual bool writeScene(io::IWriteFile* file, scene::ISceneNode* root);

	//! writes a mesh
	virtual bool writeMesh(io::IWriteFile* file, scene::IMesh* mesh, s32 flags=EMWF_NONE);


protected:
	
	bool hasSecondTextureCoordinates(video::E_VERTEX_TYPE type) const;
	inline irr::core::stringw toString(const irr::core::vector3df& vec) const;
	inline irr::core::stringw toString(const irr::core::vector2df& vec) const;
	inline irr::core::stringw toString(const irr::video::SColorf& colorf) const;
	inline irr::core::stringw toString(const irr::video::ECOLOR_FORMAT format) const;
	inline irr::core::stringw toString(const irr::video::E_TEXTURE_CLAMP clamp) const;
	inline irr::core::stringw toString(const irr::scene::E_COLLADA_TRANSPARENT_FX opaque) const;
	inline irr::core::stringw toRef(const irr::core::stringw& source) const;
	irr::core::stringw uniqueNameForMesh(const scene::IMesh* mesh) const;
	irr::core::stringw uniqueNameForNode(const scene::ISceneNode* node) const;
	irr::core::stringw minTexfilterToString(bool bilinear, bool trilinear) const;
	irr::core::stringw magTexfilterToString(bool bilinear, bool trilinear) const;
	irr::core::stringw pathToNCName(const irr::io::path& path) const;
	irr::core::stringw pathToURI(const irr::io::path& path) const;
	inline bool isXmlNameStartChar(wchar_t c) const;
	inline bool isXmlNameChar(wchar_t c) const;
	s32 getTextureIdx(const video::SMaterial & material, E_COLLADA_COLOR_SAMPLER cs);
	void writeAsset();
	void makeMeshNames(irr::scene::ISceneNode * node);
	void writeNodeMaterials(irr::scene::ISceneNode * node);
	void writeNodeEffects(irr::scene::ISceneNode * node);
	void writeNodeGeometries(irr::scene::ISceneNode * node);
	void writeSceneNode(irr::scene::ISceneNode * node);
	void writeMeshMaterials(const irr::core::stringw& meshname, scene::IMesh* mesh);
	void writeMeshEffects(const irr::core::stringw& meshname, scene::IMesh* mesh);
	void writeMaterialEffect(const irr::core::stringw& meshname, const irr::core::stringw& materialname, const video::SMaterial & material);
	void writeMeshGeometry(const irr::core::stringw& meshname, scene::IMesh* mesh);
	void writeMeshInstanceGeometry(const irr::core::stringw& meshname, scene::IMesh* mesh);
	void writeLibraryImages();
	void writeColorElement(const video::SColor & col);
	bool writeTextureSampler(const irr::core::stringw& meshname, s32 textureIdx);
	void writeFxElement(const irr::core::stringw& meshname, const video::SMaterial & material, E_COLLADA_TECHNIQUE_FX techFx);
	void writeFloatElement(irr::f32 value);
	void writeRotateElement(const irr::core::vector3df& axis, irr::f32 angle);
	void writeScaleElement(const irr::core::vector3df& scale);
	void writeTranslateElement(const irr::core::vector3df& translate);

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

	struct ColladaMesh
	{
		ColladaMesh() : MaterialWritten(false), EffectWritten(false), GeometryWritten(false)
		{
		}
			 
		irr::core::stringw Name;
		bool MaterialWritten;
		bool EffectWritten;
		bool GeometryWritten;
	};
	typedef core::map<IMesh*, ColladaMesh>::Node MeshNode;
	core::map<IMesh*, ColladaMesh> Meshes;
};


} // end namespace
} // end namespace

#endif


