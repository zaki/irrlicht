// Copyright (C) 2002-2011 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

// TODO: second UV-coordinates currently ignored in textures

#include "IrrCompileConfig.h"

#ifdef _IRR_COMPILE_WITH_COLLADA_WRITER_

#include "CColladaMeshWriter.h"
#include "os.h"
#include "IFileSystem.h"
#include "IWriteFile.h"
#include "IXMLWriter.h"
#include "IMesh.h"
#include "IAttributes.h"
#include "IAnimatedMeshSceneNode.h"
#include "IMeshSceneNode.h"
#include "ITerrainSceneNode.h"
#include "ILightSceneNode.h"
#include "ISceneManager.h"

namespace irr
{
namespace scene
{

//! Which lighting model should be used in the technique (FX) section when exporting effects (materials)
E_COLLADA_TECHNIQUE_FX CColladaMeshWriterProperties::getTechniqueFx(const video::SMaterial& material) const
{
	return ECTF_BLINN;
}

//! Which texture index should be used when writing the texture of the given sampler color.
s32 CColladaMeshWriterProperties::getTextureIdx(const video::SMaterial & material, E_COLLADA_COLOR_SAMPLER cs) const
{
	// So far we just export in a way which is similar to how we import colladas.
	// There might be better ways to do this, but I suppose it depends a lot for which target
	// application we export, so in most cases it will have to be done in user-code anyway.
	switch ( cs )
	{
		case ECCS_DIFFUSE:
			return 2;
		case ECCS_AMBIENT:
			return 1;
		case ECCS_EMISSIVE:
			return 0;
		case ECCS_SPECULAR:
			return 3;
		case ECCS_TRANSPARENT:
			return -1;
		case ECCS_REFLECTIVE:
			return -1;
	};
	return -1;
}

E_COLLADA_IRR_COLOR CColladaMeshWriterProperties::getColorMapping(const video::SMaterial & material, E_COLLADA_COLOR_SAMPLER cs) const
{
	switch ( cs )
	{
		case ECCS_DIFFUSE:
			return ECIC_DIFFUSE;
		case ECCS_AMBIENT:
			return ECIC_AMBIENT;
		case ECCS_EMISSIVE:
			return ECIC_EMISSIVE;
		case ECCS_SPECULAR:
			return ECIC_SPECULAR;
		case ECCS_TRANSPARENT:
			return ECIC_NONE;
		case ECCS_REFLECTIVE:
			return ECIC_CUSTOM;
	};

	return ECIC_NONE;
}

//! Return custom colors for certain color types requested by collada.
video::SColor CColladaMeshWriterProperties::getCustomColor(const video::SMaterial & material, E_COLLADA_COLOR_SAMPLER cs) const
{
	return video::SColor(255, 0, 0, 0);
}


//! Return the settings for transparence
E_COLLADA_TRANSPARENT_FX CColladaMeshWriterProperties::getTransparentFx(const video::SMaterial& material) const
{
	// TODO: figure out best default mapping
	return ECOF_A_ONE;
}

//! Transparency value for the material.
f32 CColladaMeshWriterProperties::getTransparency(const video::SMaterial& material) const
{
	// TODO: figure out best default mapping
	return -1.f;
}

//! Reflectivity value for that material
f32 CColladaMeshWriterProperties::getReflectivity(const video::SMaterial& material) const
{
	// TODO: figure out best default mapping
	return 0.f;
}

//! Return index of refraction for that material
f32 CColladaMeshWriterProperties::getIndexOfRefraction(const video::SMaterial& material) const
{
	return -1.f;
}

bool CColladaMeshWriterProperties::isExportable(const irr::scene::ISceneNode * node) const
{
	return node && node->isVisible();
}

IMesh* CColladaMeshWriterProperties::getMesh(irr::scene::ISceneNode * node)
{
	if ( !node )
		return 0;
	if ( node->getType() == ESNT_ANIMATED_MESH )
		return static_cast<IAnimatedMeshSceneNode*>(node)->getMesh()->getMesh(0);
	if (	node->getType() == ESNT_MESH
		||	node->getType() == ESNT_CUBE
		||	node->getType() == ESNT_SPHERE
		||	node->getType() == ESNT_WATER_SURFACE
		||	node->getType() == ESNT_Q3SHADER_SCENE_NODE
		)
		return static_cast<IMeshSceneNode*>(node)->getMesh();
	if ( node->getType() == ESNT_TERRAIN )
		return static_cast<ITerrainSceneNode*>(node)->getMesh();
	return 0;
}



CColladaMeshWriter::CColladaMeshWriter(	ISceneManager * smgr, video::IVideoDriver* driver,
					io::IFileSystem* fs)
	: FileSystem(fs), VideoDriver(driver), Writer(0)
{

	#ifdef _DEBUG
	setDebugName("CColladaMeshWriter");
	#endif

	if (VideoDriver)
		VideoDriver->grab();

	if (FileSystem)
		FileSystem->grab();

	if ( smgr )
		setAmbientLight( smgr->getAmbientLight() );

	CColladaMeshWriterProperties * p = new CColladaMeshWriterProperties();
	setDefaultProperties(p);
	setProperties(p);
	p->drop();
}


CColladaMeshWriter::~CColladaMeshWriter()
{
	if (VideoDriver)
		VideoDriver->drop();

	if (FileSystem)
		FileSystem->drop();
}


//! Returns the type of the mesh writer
EMESH_WRITER_TYPE CColladaMeshWriter::getType() const
{
	return EMWT_COLLADA;
}

//! writes a scene starting with the given node
bool CColladaMeshWriter::writeScene(io::IWriteFile* file, scene::ISceneNode* root)
{
	if (!file || !root)
		return false;

	Writer = FileSystem->createXMLWriter(file);

	if (!Writer)
	{
		os::Printer::log("Could not write file", file->getFileName());
		return false;
	}

	Directory = FileSystem->getFileDir(FileSystem->getAbsolutePath( file->getFileName() ));

	// make names for all nodes with exportable meshes
	Meshes.clear();
	makeMeshNames(root);

	os::Printer::log("Writing scene", file->getFileName());

	// write COLLADA header

	Writer->writeXMLHeader();

	Writer->writeElement(L"COLLADA", false,
		L"xmlns", L"http://www.collada.org/2005/11/COLLADASchema",
		L"version", L"1.4.1");
	Writer->writeLineBreak();

	// write asset data
	writeAsset();

	// write all materials
	Writer->writeElement(L"library_materials", false);
	Writer->writeLineBreak();
	writeNodeMaterials(root);
	Writer->writeClosingTag(L"library_materials");
	Writer->writeLineBreak();

	LibraryImages.clear();
	Writer->writeElement(L"library_effects", false);
	Writer->writeLineBreak();
	writeNodeEffects(root);
	Writer->writeClosingTag(L"library_effects");
	Writer->writeLineBreak();


	// images
	writeLibraryImages();

	// lights
	Writer->writeElement(L"library_lights", false);
	Writer->writeLineBreak();

	writeAmbientLightElement( getAmbientLight() );
	writeNodeLights(root);

	Writer->writeClosingTag(L"library_lights");
	Writer->writeLineBreak();


	// write meshes
	Writer->writeElement(L"library_geometries", false);
	Writer->writeLineBreak();
	writeNodeGeometries(root);
	Writer->writeClosingTag(L"library_geometries");
	Writer->writeLineBreak();

	// write scene
	Writer->writeElement(L"library_visual_scenes", false);
	Writer->writeLineBreak();
	Writer->writeElement(L"visual_scene", false, L"id", L"default_scene");
	Writer->writeLineBreak();

		// ambient light
		Writer->writeElement(L"node", false);
		Writer->writeLineBreak();
		Writer->writeElement(L"instance_light", true, L"url", L"#ambientlight");
		Writer->writeLineBreak();

			// scenegraph
			writeSceneNode(root);

		Writer->writeClosingTag(L"node");
		Writer->writeLineBreak();

	Writer->writeClosingTag(L"visual_scene");
	Writer->writeLineBreak();
	Writer->writeClosingTag(L"library_visual_scenes");
	Writer->writeLineBreak();


	// instance scene
	Writer->writeElement(L"scene", false);
	Writer->writeLineBreak();

		Writer->writeElement(L"instance_visual_scene", true, L"url", L"#default_scene");
		Writer->writeLineBreak();

	Writer->writeClosingTag(L"scene");
	Writer->writeLineBreak();


	// close everything

	Writer->writeClosingTag(L"COLLADA");
	Writer->drop();

	return true;
}

void CColladaMeshWriter::makeMeshNames(irr::scene::ISceneNode * node)
{
	if ( !node || !getProperties() || !getProperties()->isExportable(node) )
		return;

	IMesh* mesh = getProperties()->getMesh(node);
	if ( mesh )
	{
		if ( !Meshes.find(mesh) )
		{
			ColladaMesh cm;
			cm.Name = uniqueNameForMesh(mesh);
			Meshes.insert(mesh, cm);
		}
	}

	const core::list<ISceneNode*>& children = node->getChildren();
	for ( core::list<ISceneNode*>::ConstIterator it = children.begin(); it != children.end(); ++it )
	{
		makeMeshNames(*it);
	}
}

void CColladaMeshWriter::writeNodeMaterials(irr::scene::ISceneNode * node)
{
	if ( !node || !getProperties() || !getProperties()->isExportable(node) )
		return;

	IMesh* mesh = getProperties()->getMesh(node);
	if ( mesh )
	{
		MeshNode * n = Meshes.find(mesh);
		if ( n && !n->getValue().MaterialWritten )
		{
			writeMeshMaterials(n->getValue().Name, mesh);
			n->getValue().MaterialWritten = true;
		}
	}

	const core::list<ISceneNode*>& children = node->getChildren();
	for ( core::list<ISceneNode*>::ConstIterator it = children.begin(); it != children.end(); ++it )
	{
		writeNodeMaterials( *it );
	}
}

void CColladaMeshWriter::writeNodeEffects(irr::scene::ISceneNode * node)
{
	if ( !node || !getProperties() || !getProperties()->isExportable(node) )
		return;

	IMesh* mesh = getProperties()->getMesh(node);
	if ( mesh )
	{
		MeshNode * n = Meshes.find(mesh);
		if ( n  && !n->getValue().EffectWritten )
		{
			irr::core::stringw meshname(n->getValue().Name);
			for (u32 i=0; i<node->getMaterialCount(); ++i)
			{
				core::stringw strMat = "mat";
				strMat += meshname;
				strMat += i;
				strMat += L"-fx";

				video::SMaterial & material = node->getMaterial(i);
				writeMaterialEffect(meshname, strMat, material);
			}
			n->getValue().EffectWritten = true;
		}
	}

	const core::list<ISceneNode*>& children = node->getChildren();
	for ( core::list<ISceneNode*>::ConstIterator it = children.begin(); it != children.end(); ++it )
	{
		writeNodeEffects( *it );
	}
}

void CColladaMeshWriter::writeNodeLights(irr::scene::ISceneNode * node)
{
	if ( !node )
		return;

	if ( node->getType() == ESNT_LIGHT )
	{
		ILightSceneNode * lightNode = static_cast<ILightSceneNode*>(node);
		const video::SLight& lightData = lightNode->getLightData();

		ColladaLight cLight;
		cLight.Name = uniqueNameForLight(node);
		LightNodes.insert(node, cLight);

		Writer->writeElement(L"light", false, L"id", cLight.Name.c_str());
		Writer->writeLineBreak();

		Writer->writeElement(L"technique_common", false);
		Writer->writeLineBreak();			

		switch ( lightNode->getLightType() )
		{
			case video::ELT_POINT:
				Writer->writeElement(L"point", false);
				Writer->writeLineBreak();

				writeColorElement(lightData.DiffuseColor, false);

				Writer->writeElement(L"constant_attenuation ", false);
				Writer->writeText( irr::core::stringw(lightData.Attenuation.X).c_str() );
				Writer->writeClosingTag(L"constant_attenuation ");
				Writer->writeLineBreak();
				Writer->writeElement(L"linear_attenuation  ", false);
				Writer->writeText( irr::core::stringw(lightData.Attenuation.Y).c_str() );
				Writer->writeClosingTag(L"linear_attenuation  ");
				Writer->writeLineBreak();
				Writer->writeElement(L"quadratic_attenuation", false);
				Writer->writeText( irr::core::stringw(lightData.Attenuation.Z).c_str() );
				Writer->writeClosingTag(L"quadratic_attenuation");
				Writer->writeLineBreak();

				Writer->writeClosingTag(L"point");
				Writer->writeLineBreak();
				break;

			case video::ELT_SPOT:
				Writer->writeElement(L"spot", false);
				Writer->writeLineBreak();

				writeColorElement(lightData.DiffuseColor, false);

				Writer->writeElement(L"constant_attenuation ", false);
				Writer->writeText( irr::core::stringw(lightData.Attenuation.X).c_str() );
				Writer->writeClosingTag(L"constant_attenuation ");
				Writer->writeLineBreak();
				Writer->writeElement(L"linear_attenuation  ", false);
				Writer->writeText( irr::core::stringw(lightData.Attenuation.Y).c_str() );
				Writer->writeClosingTag(L"linear_attenuation  ");
				Writer->writeLineBreak();
				Writer->writeElement(L"quadratic_attenuation", false);
				Writer->writeText( irr::core::stringw(lightData.Attenuation.Z).c_str() );
				Writer->writeClosingTag(L"quadratic_attenuation");
				Writer->writeLineBreak();

				Writer->writeElement(L"falloff_angle", false);
				Writer->writeText( irr::core::stringw(lightData.OuterCone * core::RADTODEG).c_str() );
				Writer->writeClosingTag(L"falloff_angle");
				Writer->writeLineBreak();
				Writer->writeElement(L"falloff_exponent", false);
				Writer->writeText( irr::core::stringw(lightData.Falloff).c_str() );
				Writer->writeClosingTag(L"falloff_exponent");
				Writer->writeLineBreak();
					
				Writer->writeClosingTag(L"spot");
				Writer->writeLineBreak();
				break;

			case video::ELT_DIRECTIONAL:
				Writer->writeElement(L"directional", false);
				Writer->writeLineBreak();

				writeColorElement(lightData.DiffuseColor, false);

				Writer->writeClosingTag(L"directional");
				Writer->writeLineBreak();
				break;
		}

		Writer->writeClosingTag(L"technique_common");
		Writer->writeLineBreak();

		Writer->writeClosingTag(L"light");
		Writer->writeLineBreak();

	}

	const core::list<ISceneNode*>& children = node->getChildren();
	for ( core::list<ISceneNode*>::ConstIterator it = children.begin(); it != children.end(); ++it )
	{
		writeNodeLights( *it );
	}
}

void CColladaMeshWriter::writeNodeGeometries(irr::scene::ISceneNode * node)
{
	if ( !node || !getProperties() || !getProperties()->isExportable(node) )
		return;

	IMesh* mesh = getProperties()->getMesh(node);
	if ( mesh )
	{
		MeshNode * n = Meshes.find(mesh);
		if ( n && !n->getValue().GeometryWritten )
		{
			writeMeshGeometry(n->getValue().Name, mesh);
			n->getValue().GeometryWritten = true;
		}
	}

	const core::list<ISceneNode*>& children = node->getChildren();
	for ( core::list<ISceneNode*>::ConstIterator it = children.begin(); it != children.end(); ++it )
	{
		writeNodeGeometries( *it );
	}
}

void CColladaMeshWriter::writeSceneNode(irr::scene::ISceneNode * node )
{
	if ( !node || !getProperties() || !getProperties()->isExportable(node) )
		return;

	// Collada doesn't require to set the id, but some other tools have problems if none exists, so we just add it).
	irr::core::stringw nameId(uniqueNameForNode(node));
	Writer->writeElement(L"node", false, L"id", nameId.c_str());
	Writer->writeLineBreak();

	irr::core::vector3df rot(node->getRotation());
	writeTranslateElement( node->getPosition() );
	writeRotateElement( irr::core::vector3df(1.f, 0.f, 0.f), rot.X );
	writeRotateElement( irr::core::vector3df(0.f, 1.f, 0.f), rot.Y );
	writeRotateElement( irr::core::vector3df(0.f, 0.f, 1.f), rot.Z );
	writeScaleElement( node->getScale() );

	// instance geometry
	IMesh* mesh = getProperties()->getMesh(node);
	if ( mesh )
	{
		MeshNode * n = Meshes.find(mesh);
		if ( n )
			writeMeshInstanceGeometry(n->getValue().Name, mesh);
	}

	// instance light
	if ( node->getType() == ESNT_LIGHT )
	{
		LightNode * n = LightNodes.find(node);
		if ( n )
			writeLightInstance(n->getValue().Name);
	}

	// TODO instance cameras

	const core::list<ISceneNode*>& children = node->getChildren();
	for ( core::list<ISceneNode*>::ConstIterator it = children.begin(); it != children.end(); ++it )
	{
		writeSceneNode( *it );
	}

	Writer->writeClosingTag(L"node");
	Writer->writeLineBreak();
}

//! writes a mesh
bool CColladaMeshWriter::writeMesh(io::IWriteFile* file, scene::IMesh* mesh, s32 flags)
{
	if (!file)
		return false;

	Writer = FileSystem->createXMLWriter(file);

	if (!Writer)
	{
		os::Printer::log("Could not write file", file->getFileName());
		return false;
	}

	Directory = FileSystem->getFileDir(FileSystem->getAbsolutePath( file->getFileName() ));

	os::Printer::log("Writing mesh", file->getFileName());

	// write COLLADA header

	Writer->writeXMLHeader();

	Writer->writeElement(L"COLLADA", false,
		L"xmlns", L"http://www.collada.org/2005/11/COLLADASchema",
		L"version", L"1.4.1");
	Writer->writeLineBreak();

	// write asset data
	writeAsset();

	// write all materials

	Writer->writeElement(L"library_materials", false);
	Writer->writeLineBreak();

	irr::core::stringw meshname(uniqueNameForMesh(mesh));
	writeMeshMaterials(meshname, mesh);

	Writer->writeClosingTag(L"library_materials");
	Writer->writeLineBreak();

	LibraryImages.clear();
	Writer->writeElement(L"library_effects", false);
	Writer->writeLineBreak();

	writeMeshEffects(meshname, mesh);

	Writer->writeClosingTag(L"library_effects");
	Writer->writeLineBreak();

	// images
	writeLibraryImages();

	// write mesh

	Writer->writeElement(L"library_geometries", false);
	Writer->writeLineBreak();

	writeMeshGeometry(meshname, mesh);

	Writer->writeClosingTag(L"library_geometries");
	Writer->writeLineBreak();

	// write scene_library
	if ( getWriteDefaultScene() )
	{
		Writer->writeElement(L"library_visual_scenes", false);
		Writer->writeLineBreak();

		Writer->writeElement(L"visual_scene", false, L"id", L"default_scene");
		Writer->writeLineBreak();

			Writer->writeElement(L"node", false);
			Writer->writeLineBreak();

				writeMeshInstanceGeometry(meshname, mesh);

			Writer->writeClosingTag(L"node");
			Writer->writeLineBreak();

		Writer->writeClosingTag(L"visual_scene");
		Writer->writeLineBreak();

		Writer->writeClosingTag(L"library_visual_scenes");
		Writer->writeLineBreak();


		// write scene
		Writer->writeElement(L"scene", false);
		Writer->writeLineBreak();

			Writer->writeElement(L"instance_visual_scene", true, L"url", L"#default_scene");
			Writer->writeLineBreak();

		Writer->writeClosingTag(L"scene");
		Writer->writeLineBreak();
	}


	// close everything

	Writer->writeClosingTag(L"COLLADA");
	Writer->drop();

	return true;
}

void CColladaMeshWriter::writeMeshInstanceGeometry(const irr::core::stringw& meshname, scene::IMesh* mesh)
{
	//<instance_geometry url="#mesh">
	core::stringw meshId(meshname);
	Writer->writeElement(L"instance_geometry", false, L"url", toRef(meshId).c_str());
	Writer->writeLineBreak();

		Writer->writeElement(L"bind_material", false);
		Writer->writeLineBreak();

			Writer->writeElement(L"technique_common", false);
			Writer->writeLineBreak();

			// instance materials
			// <instance_material symbol="leaf" target="#MidsummerLeaf01"/>
			for (u32 i=0; i<mesh->getMeshBufferCount(); ++i)
			{
				core::stringw strMat = "mat";
				strMat += meshname;
				strMat += i;
				core::stringw strMatInst(L"#");
				strMatInst += strMat;
				Writer->writeElement(L"instance_material", false, L"symbol", strMat.c_str(), L"target", strMatInst.c_str());
				Writer->writeLineBreak();

					// <bind_vertex_input semantic="mesh-TexCoord0" input_semantic="TEXCOORD" input_set="0"/>
					core::stringw meshTexCoordId(meshname);
					meshTexCoordId += L"-TexCoord0";	// TODO: need to handle second UV-set
					Writer->writeElement(L"bind_vertex_input", true, L"semantic", meshTexCoordId.c_str(), L"input_semantic", L"TEXCOORD", L"input_set", L"0" );
					Writer->writeLineBreak();

				Writer->writeClosingTag(L"instance_material");
				Writer->writeLineBreak();
			}

			Writer->writeClosingTag(L"technique_common");
			Writer->writeLineBreak();

		Writer->writeClosingTag(L"bind_material");
		Writer->writeLineBreak();

	Writer->writeClosingTag(L"instance_geometry");
	Writer->writeLineBreak();
}

void CColladaMeshWriter::writeLightInstance(const irr::core::stringw& lightName)
{
	Writer->writeElement(L"instance_light", true, L"url", toRef(lightName).c_str());
	Writer->writeLineBreak();
}

bool CColladaMeshWriter::hasSecondTextureCoordinates(video::E_VERTEX_TYPE type) const
{
	return type == video::EVT_2TCOORDS;
}

irr::core::stringw CColladaMeshWriter::toString(const irr::core::vector3df& vec) const
{
	c8 tmpbuf[255];
	snprintf(tmpbuf, 255, "%f %f %f", vec.X, vec.Y, vec.Z);
	core::stringw str  = tmpbuf;

	return str;
}

irr::core::stringw CColladaMeshWriter::uvToString(const irr::core::vector2df& vec) const
{
	// change handedness
	return toString( core::vector2df(vec.X, 1.f-vec.Y) );
}

irr::core::stringw CColladaMeshWriter::toString(const irr::core::vector2df& vec) const
{
	c8 tmpbuf[255];
	snprintf(tmpbuf, 255, "%f %f", vec.X, vec.Y);
	core::stringw str  = tmpbuf;

	return str;
}

irr::core::stringw CColladaMeshWriter::toString(const irr::video::SColorf& colorf, bool writeAlpha) const
{
	c8 tmpbuf[255];
	if ( writeAlpha )
		snprintf(tmpbuf, 255, "%f %f %f %f", colorf.getRed(), colorf.getGreen(), colorf.getBlue(), colorf.getAlpha());
	else
		snprintf(tmpbuf, 255, "%f %f %f", colorf.getRed(), colorf.getGreen(), colorf.getBlue());
	core::stringw str = tmpbuf;

	return str;
}

irr::core::stringw CColladaMeshWriter::toString(const irr::video::ECOLOR_FORMAT format) const
{
	switch ( format )
	{
		case video::ECF_A1R5G5B5:	return irr::core::stringw(L"A1R5G5B5");
		case video::ECF_R5G6B5:		return irr::core::stringw(L"R5G6B5");
		case video::ECF_R8G8B8:		return irr::core::stringw(L"R8G8B8");
		case video::ECF_A8R8G8B8:	return irr::core::stringw(L"A8R8G8B8");
		default:					return irr::core::stringw(L"");
	}
}

irr::core::stringw CColladaMeshWriter::toString(const irr::video::E_TEXTURE_CLAMP clamp) const
{
	switch ( clamp )
	{
		case video::ETC_REPEAT:
			return core::stringw(L"WRAP");
		case video::ETC_CLAMP:
		case video::ETC_CLAMP_TO_EDGE:
			return core::stringw(L"CLAMP");
		case video::ETC_CLAMP_TO_BORDER:
			return core::stringw(L"BORDER");
		case video::ETC_MIRROR:
		case video::ETC_MIRROR_CLAMP:
		case video::ETC_MIRROR_CLAMP_TO_EDGE:
		case video::ETC_MIRROR_CLAMP_TO_BORDER:
			return core::stringw(L"MIRROR");
	}
	return core::stringw(L"NONE");
}

irr::core::stringw CColladaMeshWriter::toString(const irr::scene::E_COLLADA_TRANSPARENT_FX transparent) const
{
	if ( transparent & ECOF_RGB_ZERO )
		return core::stringw(L"RGB_ZERO");
	else
		return core::stringw(L"A_ONE");
}

irr::core::stringw CColladaMeshWriter::toRef(const irr::core::stringw& source) const
{
	irr::core::stringw ref(L"#");
	ref += source;
	return ref;
}

irr::core::stringw CColladaMeshWriter::uniqueNameForMesh(const scene::IMesh* mesh) const
{
	irr::core::stringw name(L"mesh");
	name += irr::core::stringw((long)mesh);
	return name;
}

irr::core::stringw CColladaMeshWriter::uniqueNameForLight(const scene::ISceneNode* lightNode) const
{
	irr::core::stringw name(L"light");	// (prefix, because xs::ID can't start with a number)
	name += irr::core::stringw((long)lightNode);
	return name;
}

irr::core::stringw CColladaMeshWriter::uniqueNameForNode(const scene::ISceneNode* node) const
{
	irr::core::stringw name(L"node");	// (prefix, because xs::ID can't start with a number)
	name += irr::core::stringw((long)node);
	if ( node )
		name += irr::core::stringw(node->getName());
	return name;
}

irr::core::stringw CColladaMeshWriter::minTexfilterToString(bool bilinear, bool trilinear) const
{
	if ( trilinear )
		return core::stringw(L"LINEAR_MIPMAP_LINEAR");
	else if ( bilinear )
		return core::stringw(L"LINEAR_MIPMAP_NEAREST");

	return core::stringw(L"NONE");
}

inline irr::core::stringw CColladaMeshWriter::magTexfilterToString(bool bilinear, bool trilinear) const
{
	if ( bilinear || trilinear )
		return core::stringw(L"LINEAR");

	return core::stringw(L"NONE");
}

bool CColladaMeshWriter::isXmlNameStartChar(wchar_t c) const
{
	return		(c >= 'A' && c <= 'Z')
			||	c == L'_'
			||	(c >= 'a' && c <= 'z')
			||	(c >= 0xC0 && c <= 0xD6)
			||	(c >= 0xD8 && c <= 0xF6)
			||	(c >= 0xF8 && c <= 0x2FF)
			||  (c >= 0x370 && c <= 0x37D)
			||  (c >= 0x37F && c <= 0x1FFF)
			||  (c >= 0x200C && c <= 0x200D)
			||  (c >= 0x2070 && c <= 0x218F)
			||  (c >= 0x2C00 && c <= 0x2FEF)
			||  (c >= 0x3001 && c <= 0xD7FF)
			||  (c >= 0xF900 && c <= 0xFDCF)
			||  (c >= 0xFDF0 && c <= 0xFFFD)
			||  (c >= 0x10000 && c <= 0xEFFFF);
}

bool CColladaMeshWriter::isXmlNameChar(wchar_t c) const
{
	return isXmlNameStartChar(c)
		||	c == L'-'
		||	c == L'.'
		||	(c >= '0' && c <= '9')
		||	c == 0xB7
		||	(c >= 0x0300 && c <= 0x036F)
		||	(c >= 0x203F && c <= 0x2040);
}

// Restrict the characters to a set of allowed characters in xs::NCName.
irr::core::stringw CColladaMeshWriter::pathToNCName(const irr::io::path& path) const
{
	irr::core::stringw result(L"_NCNAME_");	// ensure id starts with a valid char and reduce chance of name-conflicts
	if ( path.empty() )
		return result;

	result.append( irr::core::stringw(path) );

	// We replace all characters not allowed by a replacement char
	const wchar_t REPLACMENT = L'-';
	for ( irr::u32 i=1; i < result.size(); ++i )
	{
		if ( result[i] == L':' || !isXmlNameChar(result[i]) )
		{
			result[i] = REPLACMENT;
		}
	}
	return result;
}

irr::core::stringw CColladaMeshWriter::pathToURI(const irr::io::path& path) const
{
	irr::core::stringw result;

	// is this a relative path?
	if ( path.size() > 1
		&& path[0] != _IRR_TEXT('/')
		&& path[0] != _IRR_TEXT('\\')
		&& path[1] != _IRR_TEXT(':') )
	{
		// not already starting with "./" ?
		if (	path[0] != _IRR_TEXT('.')
			||	path[1] != _IRR_TEXT('/') )
		{
			result.append(L"./");
		}
	}
	result.append(path);

	// TODO: make correct URI (without whitespaces)

	return result;
}

void CColladaMeshWriter::writeAsset()
{
	Writer->writeElement(L"asset", false);
	Writer->writeLineBreak();

	Writer->writeElement(L"contributor", false);
	Writer->writeLineBreak();
	Writer->writeElement(L"authoring_tool", false);
	Writer->writeText(L"Irrlicht Engine / irrEdit");  // this code has originated from irrEdit 0.7
	Writer->writeClosingTag(L"authoring_tool");
	Writer->writeLineBreak();
	Writer->writeClosingTag(L"contributor");
	Writer->writeLineBreak();

	// The next two are required
	Writer->writeElement(L"created", false);
	Writer->writeText(L"2008-01-31T00:00:00Z");
	Writer->writeClosingTag(L"created");
	Writer->writeLineBreak();

	Writer->writeElement(L"modified", false);
	Writer->writeText(L"2008-01-31T00:00:00Z");
	Writer->writeClosingTag(L"modified");
	Writer->writeLineBreak();

	Writer->writeElement(L"revision", false);
	Writer->writeText(L"1.0");
	Writer->writeClosingTag(L"revision");
	Writer->writeLineBreak();

	Writer->writeClosingTag(L"asset");
	Writer->writeLineBreak();
}

void CColladaMeshWriter::writeMeshMaterials(const irr::core::stringw& meshname, scene::IMesh* mesh)
{
	u32 i;
	for (i=0; i<mesh->getMeshBufferCount(); ++i)
	{
		core::stringw strMat = "mat";
		strMat += meshname;
		strMat += i;

		Writer->writeElement(L"material", false,
			L"id", strMat.c_str(),
			L"name", strMat.c_str());
		Writer->writeLineBreak();

		strMat += L"-fx";
		Writer->writeElement(L"instance_effect", true,
			L"url", (core::stringw(L"#") + strMat).c_str());
		Writer->writeLineBreak();

		Writer->writeClosingTag(L"material");
		Writer->writeLineBreak();
	}
}

void CColladaMeshWriter::writeMaterialEffect(const irr::core::stringw& meshname, const irr::core::stringw& materialname, const video::SMaterial & material)
{
	Writer->writeElement(L"effect", false,
		L"id", materialname.c_str(),
		L"name", materialname.c_str());
	Writer->writeLineBreak();
	Writer->writeElement(L"profile_COMMON", false);
	Writer->writeLineBreak();

	int numTextures = 0;
	if ( getWriteTextures() )
	{
		// write texture surfaces and samplers and buffer all used imagess
		for ( int t=0; t<4; ++t )
		{
			const video::SMaterialLayer& layer  = material.TextureLayer[t];
			if ( !layer.Texture )
				break;
			++numTextures;

			if ( LibraryImages.linear_search(layer.Texture) < 0 )
					LibraryImages.push_back( layer.Texture );

			irr::core::stringw texName("tex");
			texName += irr::core::stringw(t);

			// write texture surface
			//<newparam sid="tex0-surface">
			irr::core::stringw texSurface(texName);
			texSurface += L"-surface";
			Writer->writeElement(L"newparam", false, L"sid", texSurface.c_str());
			Writer->writeLineBreak();
			//  <surface type="2D">
				Writer->writeElement(L"surface", false, L"type", L"2D");
				Writer->writeLineBreak();

		//          <init_from>internal_texturename</init_from>
					Writer->writeElement(L"init_from", false);
					irr::io::path p(FileSystem->getRelativeFilename(layer.Texture->getName().getPath(), Directory));
					Writer->writeText(pathToNCName(p).c_str());
					Writer->writeClosingTag(L"init_from");
					Writer->writeLineBreak();

		//          <format>A8R8G8B8</format>
					Writer->writeElement(L"format", false);
					video::ECOLOR_FORMAT format = layer.Texture->getColorFormat();
					Writer->writeText(toString(format).c_str());
					Writer->writeClosingTag(L"format");
					Writer->writeLineBreak();
		//      </surface>
				Writer->writeClosingTag(L"surface");
				Writer->writeLineBreak();
		//  </newparam>
			Writer->writeClosingTag(L"newparam");
			Writer->writeLineBreak();

			// write texture sampler
		//  <newparam sid="tex0-sampler">
			irr::core::stringw texSampler(texName);
			texSampler += L"-sampler";
			Writer->writeElement(L"newparam", false, L"sid", texSampler.c_str());
			Writer->writeLineBreak();
		//      <sampler2D>
				Writer->writeElement(L"sampler2D", false);
				Writer->writeLineBreak();

		//          <source>tex0-surface</source>
					Writer->writeElement(L"source", false);
					Writer->writeText(texSurface.c_str());
					Writer->writeClosingTag(L"source");
					Writer->writeLineBreak();

		//			<wrap_s>WRAP</wrap_s>
					Writer->writeElement(L"wrap_s", false);
					Writer->writeText(toString((video::E_TEXTURE_CLAMP)layer.TextureWrapU).c_str());
					Writer->writeClosingTag(L"wrap_s");
					Writer->writeLineBreak();

		//			<wrap_t>WRAP</wrap_t>
					Writer->writeElement(L"wrap_t", false);
					Writer->writeText(toString((video::E_TEXTURE_CLAMP)layer.TextureWrapV).c_str());
					Writer->writeClosingTag(L"wrap_t");
					Writer->writeLineBreak();

		//			<minfilter>LINEAR_MIPMAP_LINEAR</minfilter>
					Writer->writeElement(L"minfilter", false);
					Writer->writeText(minTexfilterToString(layer.BilinearFilter, layer.TrilinearFilter).c_str());
					Writer->writeClosingTag(L"minfilter");
					Writer->writeLineBreak();

		//			<magfilter>LINEAR</magfilter>
					Writer->writeElement(L"magfilter", false);
					Writer->writeText(magTexfilterToString(layer.BilinearFilter, layer.TrilinearFilter).c_str());
					Writer->writeClosingTag(L"magfilter");
					Writer->writeLineBreak();

					// TBD - actually not sure how anisotropic should be written, so for now it writes in a way
					// that works with the way the loader reads it again.
					if ( layer.AnisotropicFilter )
					{
		//			<mipfilter>LINEAR_MIPMAP_LINEAR</mipfilter>
						Writer->writeElement(L"mipfilter", false);
						Writer->writeText(L"LINEAR_MIPMAP_LINEAR");
						Writer->writeClosingTag(L"mipfilter");
						Writer->writeLineBreak();
					}

		//     </sampler2D>
				Writer->writeClosingTag(L"sampler2D");
				Writer->writeLineBreak();
		//  </newparam>
			Writer->writeClosingTag(L"newparam");
			Writer->writeLineBreak();
		}
	}

	Writer->writeElement(L"technique", false, L"sid", L"common");
	Writer->writeLineBreak();

	E_COLLADA_TECHNIQUE_FX techFx = getProperties() ? getProperties()->getTechniqueFx(material) : ECTF_BLINN;
	writeFxElement(meshname, material, techFx);

	Writer->writeClosingTag(L"technique");
	Writer->writeLineBreak();
	Writer->writeClosingTag(L"profile_COMMON");
	Writer->writeLineBreak();
	Writer->writeClosingTag(L"effect");
	Writer->writeLineBreak();
}

void CColladaMeshWriter::writeMeshEffects(const irr::core::stringw& meshname, scene::IMesh* mesh)
{
	for (u32 i=0; i<mesh->getMeshBufferCount(); ++i)
	{
		core::stringw strMat = "mat";
		strMat += meshname;
		strMat += i;
		strMat += L"-fx";

		video::SMaterial & material = mesh->getMeshBuffer(i)->getMaterial();
		writeMaterialEffect(meshname, strMat, material);
	}
}

void CColladaMeshWriter::writeMeshGeometry(const irr::core::stringw& meshname, scene::IMesh* mesh)
{
	core::stringw meshId(meshname);

	Writer->writeElement(L"geometry", false, L"id", meshId.c_str(), L"name", meshId.c_str());
	Writer->writeLineBreak();
	Writer->writeElement(L"mesh");
	Writer->writeLineBreak();

	// do some statistics for the mesh to know which stuff needs to be saved into
	// the file:
	// - count vertices
	// - check for the need of a second texture coordinate
	// - count amount of second texture coordinates
	// - check for the need of tangents (TODO)

	u32 totalVertexCount = 0;
	u32 totalTCoords2Count = 0;
	bool needsTangents = false; // TODO: tangents not supported here yet
	u32 i=0;
	for (i=0; i<mesh->getMeshBufferCount(); ++i)
	{
		totalVertexCount += mesh->getMeshBuffer(i)->getVertexCount();

		if (hasSecondTextureCoordinates(mesh->getMeshBuffer(i)->getVertexType()))
			totalTCoords2Count += mesh->getMeshBuffer(i)->getVertexCount();

		if (!needsTangents)
			needsTangents = mesh->getMeshBuffer(i)->getVertexType() == video::EVT_TANGENTS;
	}

	SComponentGlobalStartPos* globalIndices = new SComponentGlobalStartPos[mesh->getMeshBufferCount()];

	// write positions
	core::stringw meshPosId(meshId);
	meshPosId += L"-Pos";
	Writer->writeElement(L"source", false, L"id", meshPosId.c_str());
	Writer->writeLineBreak();

		core::stringw vertexCountStr(totalVertexCount*3);
		core::stringw meshPosArrayId(meshPosId);
		meshPosArrayId += L"-array";
		Writer->writeElement(L"float_array", false, L"id", meshPosArrayId.c_str(),
					L"count", vertexCountStr.c_str());
		Writer->writeLineBreak();

		for (i=0; i<mesh->getMeshBufferCount(); ++i)
		{
			scene::IMeshBuffer* buffer = mesh->getMeshBuffer(i);
			video::E_VERTEX_TYPE vtxType = buffer->getVertexType();
			u32 vertexCount = buffer->getVertexCount();

			globalIndices[i].PosStartIndex = 0;

			if (i!=0)
				globalIndices[i].PosStartIndex = globalIndices[i-1].PosLastIndex + 1;

			globalIndices[i].PosLastIndex = globalIndices[i].PosStartIndex + vertexCount - 1;

			switch(vtxType)
			{
			case video::EVT_STANDARD:
				{
					video::S3DVertex* vtx = (video::S3DVertex*)buffer->getVertices();
					for (u32 j=0; j<vertexCount; ++j)
					{
						Writer->writeText(toString(vtx[j].Pos).c_str());
						Writer->writeLineBreak();
					}
				}
				break;
			case video::EVT_2TCOORDS:
				{
					video::S3DVertex2TCoords* vtx = (video::S3DVertex2TCoords*)buffer->getVertices();
					for (u32 j=0; j<vertexCount; ++j)
					{
						Writer->writeText(toString(vtx[j].Pos).c_str());
						Writer->writeLineBreak();
					}
				}
				break;
			case video::EVT_TANGENTS:
				{
					video::S3DVertexTangents* vtx = (video::S3DVertexTangents*)buffer->getVertices();
					for (u32 j=0; j<vertexCount; ++j)
					{
						Writer->writeText(toString(vtx[j].Pos).c_str());
						Writer->writeLineBreak();
					}
				}
				break;
			}
		}

		Writer->writeClosingTag(L"float_array");
		Writer->writeLineBreak();

		Writer->writeElement(L"technique_common", false);
		Writer->writeLineBreak();

		vertexCountStr = core::stringw(totalVertexCount);

			Writer->writeElement(L"accessor", false, L"source", toRef(meshPosArrayId).c_str(),
						L"count", vertexCountStr.c_str(), L"stride", L"3");
			Writer->writeLineBreak();

				Writer->writeElement(L"param", true, L"name", L"X", L"type", L"float");
				Writer->writeLineBreak();
				Writer->writeElement(L"param", true, L"name", L"Y", L"type", L"float");
				Writer->writeLineBreak();
				Writer->writeElement(L"param", true, L"name", L"Z", L"type", L"float");
				Writer->writeLineBreak();

				Writer->writeClosingTag(L"accessor");
				Writer->writeLineBreak();

		Writer->writeClosingTag(L"technique_common");
		Writer->writeLineBreak();

	Writer->writeClosingTag(L"source");
	Writer->writeLineBreak();

	// write texture coordinates

	core::stringw meshTexCoord0Id(meshId);
	meshTexCoord0Id += L"-TexCoord0";
	Writer->writeElement(L"source", false, L"id", meshTexCoord0Id.c_str());
	Writer->writeLineBreak();

		vertexCountStr = core::stringw(totalVertexCount*2);
		core::stringw meshTexCoordArrayId(meshTexCoord0Id);
		meshTexCoordArrayId += L"-array";
		Writer->writeElement(L"float_array", false, L"id", meshTexCoordArrayId.c_str(),
					L"count", vertexCountStr.c_str());
		Writer->writeLineBreak();

		for (i=0; i<mesh->getMeshBufferCount(); ++i)
		{
			scene::IMeshBuffer* buffer = mesh->getMeshBuffer(i);
			video::E_VERTEX_TYPE vtxType = buffer->getVertexType();
			u32 vertexCount = buffer->getVertexCount();

			globalIndices[i].TCoord0StartIndex = 0;

			if (i!=0)
				globalIndices[i].TCoord0StartIndex = globalIndices[i-1].TCoord0LastIndex + 1;

			globalIndices[i].TCoord0LastIndex = globalIndices[i].TCoord0StartIndex + vertexCount - 1;

			switch(vtxType)
			{
			case video::EVT_STANDARD:
				{
					video::S3DVertex* vtx = (video::S3DVertex*)buffer->getVertices();
					for (u32 j=0; j<vertexCount; ++j)
					{
						Writer->writeText(uvToString(vtx[j].TCoords).c_str());
						Writer->writeLineBreak();
					}
				}
				break;
			case video::EVT_2TCOORDS:
				{
					video::S3DVertex2TCoords* vtx = (video::S3DVertex2TCoords*)buffer->getVertices();
					for (u32 j=0; j<vertexCount; ++j)
					{
						Writer->writeText(uvToString(vtx[j].TCoords).c_str());
						Writer->writeLineBreak();
					}
				}
				break;
			case video::EVT_TANGENTS:
				{
					video::S3DVertexTangents* vtx = (video::S3DVertexTangents*)buffer->getVertices();
					for (u32 j=0; j<vertexCount; ++j)
					{
						Writer->writeText(uvToString(vtx[j].TCoords).c_str());
						Writer->writeLineBreak();
					}
				}
				break;
			}
		}

		Writer->writeClosingTag(L"float_array");
		Writer->writeLineBreak();

		Writer->writeElement(L"technique_common", false);
		Writer->writeLineBreak();

		vertexCountStr = core::stringw(totalVertexCount);

			Writer->writeElement(L"accessor", false, L"source", toRef(meshTexCoordArrayId).c_str(),
						L"count", vertexCountStr.c_str(), L"stride", L"2");
			Writer->writeLineBreak();

				Writer->writeElement(L"param", true, L"name", L"U", L"type", L"float");
				Writer->writeLineBreak();
				Writer->writeElement(L"param", true, L"name", L"V", L"type", L"float");
				Writer->writeLineBreak();

			Writer->writeClosingTag(L"accessor");
			Writer->writeLineBreak();

		Writer->writeClosingTag(L"technique_common");
		Writer->writeLineBreak();

	Writer->writeClosingTag(L"source");
	Writer->writeLineBreak();

	// write normals
	core::stringw meshNormalId(meshId);
	meshNormalId += L"-Normal";
	Writer->writeElement(L"source", false, L"id", meshNormalId.c_str());
	Writer->writeLineBreak();

		vertexCountStr = core::stringw(totalVertexCount*3);
		core::stringw meshNormalArrayId(meshNormalId);
		meshNormalArrayId += L"-array";
		Writer->writeElement(L"float_array", false, L"id", meshNormalArrayId.c_str(),
					L"count", vertexCountStr.c_str());
		Writer->writeLineBreak();

		for (i=0; i<mesh->getMeshBufferCount(); ++i)
		{
			scene::IMeshBuffer* buffer = mesh->getMeshBuffer(i);
			video::E_VERTEX_TYPE vtxType = buffer->getVertexType();
			u32 vertexCount = buffer->getVertexCount();

			globalIndices[i].NormalStartIndex = 0;

			if (i!=0)
				globalIndices[i].NormalStartIndex = globalIndices[i-1].NormalLastIndex + 1;

			globalIndices[i].NormalLastIndex = globalIndices[i].NormalStartIndex + vertexCount - 1;

			switch(vtxType)
			{
			case video::EVT_STANDARD:
				{
					video::S3DVertex* vtx = (video::S3DVertex*)buffer->getVertices();
					for (u32 j=0; j<vertexCount; ++j)
					{
						Writer->writeText(toString(vtx[j].Normal).c_str());
						Writer->writeLineBreak();
					}
				}
				break;
			case video::EVT_2TCOORDS:
				{
					video::S3DVertex2TCoords* vtx = (video::S3DVertex2TCoords*)buffer->getVertices();
					for (u32 j=0; j<vertexCount; ++j)
					{
						Writer->writeText(toString(vtx[j].Normal).c_str());
						Writer->writeLineBreak();
					}
				}
				break;
			case video::EVT_TANGENTS:
				{
					video::S3DVertexTangents* vtx = (video::S3DVertexTangents*)buffer->getVertices();
					for (u32 j=0; j<vertexCount; ++j)
					{
						Writer->writeText(toString(vtx[j].Normal).c_str());
						Writer->writeLineBreak();
					}
				}
				break;
			}
		}

		Writer->writeClosingTag(L"float_array");
		Writer->writeLineBreak();

		Writer->writeElement(L"technique_common", false);
		Writer->writeLineBreak();

		vertexCountStr = core::stringw(totalVertexCount);

		Writer->writeElement(L"accessor", false, L"source", toRef(meshNormalArrayId).c_str(),
									L"count", vertexCountStr.c_str(), L"stride", L"3");
			Writer->writeLineBreak();

				Writer->writeElement(L"param", true, L"name", L"X", L"type", L"float");
				Writer->writeLineBreak();
				Writer->writeElement(L"param", true, L"name", L"Y", L"type", L"float");
				Writer->writeLineBreak();
				Writer->writeElement(L"param", true, L"name", L"Z", L"type", L"float");
				Writer->writeLineBreak();

			Writer->writeClosingTag(L"accessor");
			Writer->writeLineBreak();

		Writer->writeClosingTag(L"technique_common");
		Writer->writeLineBreak();

	Writer->writeClosingTag(L"source");
	Writer->writeLineBreak();

	// write second set of texture coordinates
	core::stringw meshTexCoord1Id(meshId);
	meshTexCoord1Id += L"-TexCoord1";
	if (totalTCoords2Count)
	{
		Writer->writeElement(L"source", false, L"id", meshTexCoord1Id.c_str());
		Writer->writeLineBreak();

			vertexCountStr = core::stringw(totalTCoords2Count*2);
			core::stringw meshTexCoord1ArrayId(meshTexCoord1Id);
			meshTexCoord1ArrayId += L"-array";
			Writer->writeElement(L"float_array", false, L"id", meshTexCoord1ArrayId.c_str(),
									L"count", vertexCountStr.c_str());
			Writer->writeLineBreak();

			for (i=0; i<mesh->getMeshBufferCount(); ++i)
			{
				scene::IMeshBuffer* buffer = mesh->getMeshBuffer(i);
				video::E_VERTEX_TYPE vtxType = buffer->getVertexType();
				u32 vertexCount = buffer->getVertexCount();

				if (hasSecondTextureCoordinates(vtxType))
				{
					globalIndices[i].TCoord1StartIndex = 0;

					if (i!=0 && globalIndices[i-1].TCoord1LastIndex != -1)
						globalIndices[i].TCoord1StartIndex = globalIndices[i-1].TCoord1LastIndex + 1;

					globalIndices[i].TCoord1LastIndex = globalIndices[i].TCoord1StartIndex + vertexCount - 1;

					switch(vtxType)
					{
					case video::EVT_2TCOORDS:
						{
							video::S3DVertex2TCoords* vtx = (video::S3DVertex2TCoords*)buffer->getVertices();
							for (u32 j=0; j<vertexCount; ++j)
							{
								Writer->writeText(toString(vtx[j].TCoords2).c_str());
								Writer->writeLineBreak();
							}
						}
						break;
					default:
						break;
					}
				} // end this buffer has 2 texture coordinates
			}

			Writer->writeClosingTag(L"float_array");
			Writer->writeLineBreak();

			Writer->writeElement(L"technique_common", false);
			Writer->writeLineBreak();

			vertexCountStr = core::stringw(totalTCoords2Count);

				Writer->writeElement(L"accessor", false, L"source", toRef(meshTexCoord1ArrayId).c_str(),
										L"count", vertexCountStr.c_str(), L"stride", L"2");
				Writer->writeLineBreak();

					Writer->writeElement(L"param", true, L"name", L"U", L"type", L"float");
					Writer->writeLineBreak();
					Writer->writeElement(L"param", true, L"name", L"V", L"type", L"float");
					Writer->writeLineBreak();

				Writer->writeClosingTag(L"accessor");
				Writer->writeLineBreak();

			Writer->writeClosingTag(L"technique_common");
			Writer->writeLineBreak();

		Writer->writeClosingTag(L"source");
		Writer->writeLineBreak();
	}

	// write tangents

	// TODO

	// write vertices
	core::stringw meshVtxId(meshId);
	meshVtxId += L"-Vtx";
	Writer->writeElement(L"vertices", false, L"id", meshVtxId.c_str());
	Writer->writeLineBreak();

		Writer->writeElement(L"input", true, L"semantic", L"POSITION", L"source", toRef(meshPosId).c_str());
		Writer->writeLineBreak();

	Writer->writeClosingTag(L"vertices");
	Writer->writeLineBreak();

	// write polygons

	for (i=0; i<mesh->getMeshBufferCount(); ++i)
	{
		scene::IMeshBuffer* buffer = mesh->getMeshBuffer(i);

		const u32 polyCount = buffer->getIndexCount() / 3;
		core::stringw strPolyCount(polyCount);
		core::stringw strMat = "mat";
		strMat += meshname;
		strMat += i;

		Writer->writeElement(L"triangles", false, L"count", strPolyCount.c_str(),
								L"material", strMat.c_str());
		Writer->writeLineBreak();

		Writer->writeElement(L"input", true, L"semantic", L"VERTEX", L"source", toRef(meshVtxId).c_str(), L"offset", L"0");
		Writer->writeLineBreak();
		Writer->writeElement(L"input", true, L"semantic", L"TEXCOORD", L"source", toRef(meshTexCoord0Id).c_str(), L"offset", L"1", L"set", L"0");
		Writer->writeLineBreak();
		Writer->writeElement(L"input", true, L"semantic", L"NORMAL", L"source", toRef(meshNormalId).c_str(), L"offset", L"2");
		Writer->writeLineBreak();

		bool has2ndTexCoords = hasSecondTextureCoordinates(buffer->getVertexType());
		if (has2ndTexCoords)
		{
			Writer->writeElement(L"input", true, L"semantic", L"TEXCOORD", L"source", toRef(meshTexCoord1Id).c_str(), L"idx", L"3");
			Writer->writeLineBreak();
		}

		// write indices now

		s32 posIdx = globalIndices[i].PosStartIndex;
		s32 tCoordIdx = globalIndices[i].TCoord0StartIndex;
		s32 normalIdx = globalIndices[i].NormalStartIndex;
		s32 tCoord2Idx = globalIndices[i].TCoord1StartIndex;

		Writer->writeElement(L"p", false);

		for (u32 p=0; p<polyCount; ++p)
		{
			core::stringw strP;

			strP += buffer->getIndices()[(p*3) + 0] + posIdx;
			strP += " ";
			strP += buffer->getIndices()[(p*3) + 0] + tCoordIdx;
			strP += " ";
			strP += buffer->getIndices()[(p*3) + 0] + normalIdx;
			strP += " ";
			if (has2ndTexCoords)
			{
				strP += buffer->getIndices()[(p*3) + 0] + tCoord2Idx;
				strP += " ";
			}

			strP += buffer->getIndices()[(p*3) + 1] + posIdx;
			strP += " ";
			strP += buffer->getIndices()[(p*3) + 1] + tCoordIdx;
			strP += " ";
			strP += buffer->getIndices()[(p*3) + 1] + normalIdx;
			strP += " ";
			if (has2ndTexCoords)
			{
				strP += buffer->getIndices()[(p*3) + 1] + tCoord2Idx;
				strP += " ";
			}

			strP += buffer->getIndices()[(p*3) + 2] + posIdx;
			strP += " ";
			strP += buffer->getIndices()[(p*3) + 2] + tCoordIdx;
			strP += " ";
			strP += buffer->getIndices()[(p*3) + 2] + normalIdx;
			if (has2ndTexCoords)
			{
				strP += " ";
				strP += buffer->getIndices()[(p*3) + 2] + tCoord2Idx;
			}
			strP += " ";

			Writer->writeText(strP.c_str());
		}

		Writer->writeClosingTag(L"p");
		Writer->writeLineBreak();

		// close index buffer section

		Writer->writeClosingTag(L"triangles");
		Writer->writeLineBreak();
	}

	// close mesh and geometry
	delete [] globalIndices;
	Writer->writeClosingTag(L"mesh");
	Writer->writeLineBreak();
	Writer->writeClosingTag(L"geometry");
	Writer->writeLineBreak();
}

void CColladaMeshWriter::writeLibraryImages()
{
	if ( getWriteTextures() && !LibraryImages.empty() )
	{
		Writer->writeElement(L"library_images", false);
		Writer->writeLineBreak();

		for ( irr::u32 i=0; i<LibraryImages.size(); ++i )
		{
			irr::io::path p(FileSystem->getRelativeFilename(LibraryImages[i]->getName().getPath(), Directory));
			//<image name="rose01">
			irr::core::stringw ncname(pathToNCName(p));
			Writer->writeElement(L"image", false, L"id", ncname.c_str(), L"name", ncname.c_str());
			Writer->writeLineBreak();
			//  <init_from>../flowers/rose01.jpg</init_from>
				Writer->writeElement(L"init_from", false);
				Writer->writeText(pathToURI(p).c_str());
				Writer->writeClosingTag(L"init_from");
				Writer->writeLineBreak();
	 		//  </image>
			Writer->writeClosingTag(L"image");
			Writer->writeLineBreak();
		}

		Writer->writeClosingTag(L"library_images");
		Writer->writeLineBreak();
	}
}

void CColladaMeshWriter::writeColorElement(const video::SColorf & col, bool writeAlpha)
{
	Writer->writeElement(L"color", false);

	irr::core::stringw str( toString(col, writeAlpha) );
	Writer->writeText(str.c_str());

	Writer->writeClosingTag(L"color");
	Writer->writeLineBreak();
}

void CColladaMeshWriter::writeColorElement(const video::SColor & col, bool writeAlpha)
{
	writeColorElement( video::SColorf(col), writeAlpha );
}

void CColladaMeshWriter::writeAmbientLightElement(const video::SColorf & col)
{
	Writer->writeElement(L"light", false, L"id", L"ambientlight");
	Writer->writeLineBreak();

		Writer->writeElement(L"technique_common", false);
		Writer->writeLineBreak();			

			Writer->writeElement(L"ambient", false);
			Writer->writeLineBreak();
					
				writeColorElement(col, false);

			Writer->writeClosingTag(L"ambient");
			Writer->writeLineBreak();

		Writer->writeClosingTag(L"technique_common");
		Writer->writeLineBreak();

	Writer->writeClosingTag(L"light");
	Writer->writeLineBreak();
}

s32 CColladaMeshWriter::getCheckedTextureIdx(const video::SMaterial & material, E_COLLADA_COLOR_SAMPLER cs)
{
	if (	!getWriteTextures()
		||	!getProperties() )
		return -1;

	s32 idx = getProperties()->getTextureIdx(material, cs);
	if ( idx >= 0 && !material.TextureLayer[idx].Texture )
		return -1;

	return idx;
}

video::SColor CColladaMeshWriter::getColorMapping(const video::SMaterial & material, E_COLLADA_COLOR_SAMPLER cs, E_COLLADA_IRR_COLOR colType)
{
	switch ( colType )
	{
		case ECIC_NONE:
			return video::SColor(255, 0, 0, 0);

		case ECIC_CUSTOM:
			return getProperties()->getCustomColor(material, cs);

		case ECIC_DIFFUSE:
			return material.DiffuseColor;

		case ECIC_AMBIENT:
			return material.AmbientColor;

		case ECIC_EMISSIVE:
			return material.EmissiveColor;

		case ECIC_SPECULAR:
			return material.SpecularColor;
	}
	return video::SColor(255, 0, 0, 0);
}

void CColladaMeshWriter::writeTextureSampler(const irr::core::stringw& meshname, s32 textureIdx)
{
	irr::core::stringw sampler(L"tex");
	sampler += irr::core::stringw(textureIdx);
	sampler += L"-sampler";

	// <texture texture="sampler" texcoord="texCoord"/>
	core::stringw meshTexCoordId(meshname);
	meshTexCoordId += L"-TexCoord0";	// TODO: need to handle second UV-set
	Writer->writeElement(L"texture", true, L"texture", sampler.c_str(), L"texcoord", meshTexCoordId.c_str() );
	Writer->writeLineBreak();
}

void CColladaMeshWriter::writeFxElement(const irr::core::stringw& meshname, const video::SMaterial & material, E_COLLADA_TECHNIQUE_FX techFx)
{
	core::stringw fxLabel;
	bool writeEmission = true;
	bool writeAmbient = true;
	bool writeDiffuse = true;
	bool writeSpecular = true;
	bool writeShininess = true;
	bool writeReflective = true;
	bool writeReflectivity = true;
	bool writeTransparent = true;
	bool writeTransparency = true;
	bool writeIndexOfRefraction = true;
	switch ( techFx )
	{
		case ECTF_BLINN:
			fxLabel = L"blinn";
			break;
		case ECTF_PHONG:
			fxLabel = L"phong";
			break;
		case ECTF_LAMBERT:
			fxLabel = L"lambert";
			writeSpecular = false;
			writeShininess = false;
			break;
		case ECTF_CONSTANT:
			fxLabel = L"constant";
			writeAmbient = false;
			writeDiffuse = false;
			writeSpecular = false;
			writeShininess = false;
			break;
	}

	Writer->writeElement(fxLabel.c_str(), false);
	Writer->writeLineBreak();

	// write all interesting material parameters
	// attributes must be written in fixed order
	if ( getProperties() )
	{
		if ( writeEmission )
		{
			writeColorFx(meshname, material, L"emission", ECCS_EMISSIVE);
		}

		if ( writeAmbient )
		{
			writeColorFx(meshname, material, L"ambient", ECCS_AMBIENT);
		}

		if ( writeDiffuse )
		{
			writeColorFx(meshname, material, L"diffuse", ECCS_DIFFUSE);
		}

		if ( writeSpecular )
		{
			writeColorFx(meshname, material, L"specular", ECCS_SPECULAR);
		}

		if ( writeShininess )
		{
			Writer->writeElement(L"shininess", false);
			Writer->writeLineBreak();
			writeFloatElement(material.Shininess);
			Writer->writeClosingTag(L"shininess");
			Writer->writeLineBreak();
		}

		if ( writeReflective )
		{
			writeColorFx(meshname, material, L"reflective", ECCS_REFLECTIVE);
		}

		if ( writeReflectivity )
		{
			f32 t = getProperties()->getReflectivity(material);
			if ( t >= 0.f )
			{
				// <transparency>  <float>1.000000</float> </transparency>
				Writer->writeElement(L"reflectivity", false);
				Writer->writeLineBreak();
				writeFloatElement(t);
				Writer->writeClosingTag(L"reflectivity");
				Writer->writeLineBreak();
			}
		}

		if ( writeTransparent )
		{
			E_COLLADA_TRANSPARENT_FX transparentFx = getProperties()->getTransparentFx(material);
			writeColorFx(meshname, material, L"transparent", ECCS_TRANSPARENT, L"opaque", toString(transparentFx).c_str());
		}

		if ( writeTransparency  )
		{
			f32 t = getProperties()->getTransparency(material);
			if ( t >= 0.f )
			{
				// <transparency>  <float>1.000000</float> </transparency>
				Writer->writeElement(L"transparency", false);
				Writer->writeLineBreak();
				writeFloatElement(t);
				Writer->writeClosingTag(L"transparency");
				Writer->writeLineBreak();
			}
		}

		if ( writeIndexOfRefraction )
		{
			f32 t = getProperties()->getIndexOfRefraction(material);
			if ( t >= 0.f )
			{
				Writer->writeElement(L"index_of_refraction", false);
				Writer->writeLineBreak();
				writeFloatElement(t);
				Writer->writeClosingTag(L"index_of_refraction");
				Writer->writeLineBreak();
			}
		}
	}


	Writer->writeClosingTag(fxLabel.c_str());
	Writer->writeLineBreak();
}

void CColladaMeshWriter::writeColorFx(const irr::core::stringw& meshname, const video::SMaterial & material, const wchar_t * colorname, E_COLLADA_COLOR_SAMPLER cs, const wchar_t* attr1Name, const wchar_t* attr1Value)
{
	irr::s32 idx = getCheckedTextureIdx(material, cs);
	E_COLLADA_IRR_COLOR colType = idx < 0 ? getProperties()->getColorMapping(material, cs) : ECIC_NONE;
	if ( idx >= 0 || colType != ECIC_NONE )
	{
		Writer->writeElement(colorname, false, attr1Name, attr1Value);
		Writer->writeLineBreak();
		if ( idx >= 0 )
			writeTextureSampler( meshname, idx);
		else
			writeColorElement(getColorMapping(material, cs, colType));
		Writer->writeClosingTag(colorname);
		Writer->writeLineBreak();
	}
}

void CColladaMeshWriter::writeFloatElement(irr::f32 value)
{
	Writer->writeElement(L"float", false);
	Writer->writeText(core::stringw(value).c_str());
	Writer->writeClosingTag(L"float");
	Writer->writeLineBreak();
}

void CColladaMeshWriter::writeRotateElement(const irr::core::vector3df& axis, irr::f32 angle)
{
	Writer->writeElement(L"rotate", false);
	irr::core::stringw txt(axis.X);
	txt += L" ";
	txt += irr::core::stringw(axis.Y);
	txt += L" ";
	txt += irr::core::stringw(axis.Z);
	txt += L" ";
	txt += irr::core::stringw(angle);
	Writer->writeText(txt.c_str());
	Writer->writeClosingTag(L"rotate");
	Writer->writeLineBreak();
}

void CColladaMeshWriter::writeScaleElement(const irr::core::vector3df& scale)
{
	Writer->writeElement(L"scale", false);
	irr::core::stringw txt(scale.X);
	txt += L" ";
	txt += irr::core::stringw(scale.Y);
	txt += L" ";
	txt += irr::core::stringw(scale.Z);
	Writer->writeText(txt.c_str());
	Writer->writeClosingTag(L"scale");
	Writer->writeLineBreak();
}

void CColladaMeshWriter::writeTranslateElement(const irr::core::vector3df& translate)
{
	Writer->writeElement(L"translate", false);
	irr::core::stringw txt(translate.X);
	txt += L" ";
	txt += irr::core::stringw(translate.Y);
	txt += L" ";
	txt += irr::core::stringw(translate.Z);
	Writer->writeText(txt.c_str());
	Writer->writeClosingTag(L"translate");
	Writer->writeLineBreak();
}

} // end namespace
} // end namespace

#endif

