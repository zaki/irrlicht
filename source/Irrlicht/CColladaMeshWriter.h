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
	// (Implementer note: keep namespace labels here to make it easier for users copying this one)
	class CColladaMeshWriterProperties  : public virtual IColladaMeshWriterProperties
	{
	public:
		//! Which lighting model should be used in the technique (FX) section when exporting effects (materials)
		virtual irr::scene::E_COLLADA_TECHNIQUE_FX getTechniqueFx(const irr::video::SMaterial& material) const;

		//! Which texture index should be used when writing the texture of the given sampler color.
		virtual irr::s32 getTextureIdx(const irr::video::SMaterial & material, irr::scene::E_COLLADA_COLOR_SAMPLER cs) const;

		//! Return which color from Irrlicht should be used for the color requested by collada
		virtual irr::scene::E_COLLADA_IRR_COLOR getColorMapping(const irr::video::SMaterial & material, irr::scene::E_COLLADA_COLOR_SAMPLER cs) const;

		//! Return custom colors for certain color types requested by collada. 
		virtual irr::video::SColor getCustomColor(const irr::video::SMaterial & material, irr::scene::E_COLLADA_COLOR_SAMPLER cs) const;

		//! Return the settings for transparence
		virtual irr::scene::E_COLLADA_TRANSPARENT_FX getTransparentFx(const irr::video::SMaterial& material) const;

		//! Transparency value for that material. 
		virtual irr::f32 getTransparency(const irr::video::SMaterial& material) const;

		//! Reflectivity value for that material
		virtual irr::f32 getReflectivity(const irr::video::SMaterial& material) const;

		//! Return index of refraction for that material
		virtual irr::f32 getIndexOfRefraction(const irr::video::SMaterial& material) const;

		//! Should node be used in scene export? By default all visible nodes are exported.
		virtual bool isExportable(const irr::scene::ISceneNode * node) const;

		//! Return the mesh for the given nod. If it has no mesh or shouldn't export it's mesh return 0.
		virtual irr::scene::IMesh* getMesh(irr::scene::ISceneNode * node);
	};


//! class to write meshes, implementing a COLLADA (.dae, .xml) writer
/** This writer implementation has been originally developed for irrEdit and then
merged out to the Irrlicht Engine */
class CColladaMeshWriter : public IColladaMeshWriter
{
public:

	CColladaMeshWriter(ISceneManager * smgr, video::IVideoDriver* driver, io::IFileSystem* fs);
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
	inline irr::core::stringw uvToString(const irr::core::vector2df& vec) const;
	inline irr::core::stringw toString(const irr::core::vector2df& vec) const;
	inline irr::core::stringw toString(const irr::video::SColorf& colorf, bool writeAlpha=true) const;
	inline irr::core::stringw toString(const irr::video::ECOLOR_FORMAT format) const;
	inline irr::core::stringw toString(const irr::video::E_TEXTURE_CLAMP clamp) const;
	inline irr::core::stringw toString(const irr::scene::E_COLLADA_TRANSPARENT_FX opaque) const;
	inline irr::core::stringw toRef(const irr::core::stringw& source) const;
	irr::core::stringw uniqueNameForMesh(const scene::IMesh* mesh) const;
	irr::core::stringw uniqueNameForLight(const scene::ISceneNode* lightNode) const;
	irr::core::stringw uniqueNameForNode(const scene::ISceneNode* node) const;
	irr::core::stringw minTexfilterToString(bool bilinear, bool trilinear) const;
	irr::core::stringw magTexfilterToString(bool bilinear, bool trilinear) const;
	irr::core::stringw pathToNCName(const irr::io::path& path) const;
	irr::core::stringw pathToURI(const irr::io::path& path) const;
	inline bool isXmlNameStartChar(wchar_t c) const;
	inline bool isXmlNameChar(wchar_t c) const;
	s32 getCheckedTextureIdx(const video::SMaterial & material, E_COLLADA_COLOR_SAMPLER cs);
	video::SColor getColorMapping(const video::SMaterial & material, E_COLLADA_COLOR_SAMPLER cs, E_COLLADA_IRR_COLOR colType);
	void writeAsset();
	void makeMeshNames(irr::scene::ISceneNode * node);
	void writeNodeMaterials(irr::scene::ISceneNode * node);
	void writeNodeEffects(irr::scene::ISceneNode * node);
	void writeNodeLights(irr::scene::ISceneNode * node);
	void writeNodeGeometries(irr::scene::ISceneNode * node);
	void writeSceneNode(irr::scene::ISceneNode * node);
	void writeMeshMaterials(const irr::core::stringw& meshname, scene::IMesh* mesh);
	void writeMeshEffects(const irr::core::stringw& meshname, scene::IMesh* mesh);
	void writeMaterialEffect(const irr::core::stringw& meshname, const irr::core::stringw& materialname, const video::SMaterial & material);
	void writeMeshGeometry(const irr::core::stringw& meshname, scene::IMesh* mesh);
	void writeMeshInstanceGeometry(const irr::core::stringw& meshname, scene::IMesh* mesh);
	void writeLightInstance(const irr::core::stringw& lightName);
	void writeLibraryImages();
	void writeColorFx(const irr::core::stringw& meshname, const video::SMaterial & material, const wchar_t * colorname, E_COLLADA_COLOR_SAMPLER cs, const wchar_t* attr1Name=0, const wchar_t* attr1Value=0);
	void writeAmbientLightElement(const video::SColorf & col);
	void writeColorElement(const video::SColor & col, bool writeAlpha=true);
	void writeColorElement(const video::SColorf & col, bool writeAlpha=true);
	void writeTextureSampler(const irr::core::stringw& meshname, s32 textureIdx);
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

	struct ColladaLight
	{
		ColladaLight()	{}
		irr::core::stringw Name;
	};
	typedef core::map<ISceneNode*, ColladaLight>::Node LightNode;
	core::map<ISceneNode*, ColladaLight> LightNodes;
};


} // end namespace
} // end namespace

#endif


