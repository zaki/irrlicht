// Copyright (C) 2002-2011 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "IrrCompileConfig.h"

#ifdef _IRR_COMPILE_WITH_COLLADA_WRITER_

#include "CColladaMeshWriter.h"
#include "os.h"
#include "IFileSystem.h"
#include "IWriteFile.h"
#include "IXMLWriter.h"
#include "IMesh.h"
#include "IAttributes.h"

namespace irr
{
namespace scene
{


CColladaMeshWriter::CColladaMeshWriter(video::IVideoDriver* driver,
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

	os::Printer::log("Writing mesh", file->getFileName());

	// write COLLADA header

	Writer->writeXMLHeader();

	Writer->writeElement(L"COLLADA", false,
		L"xmlns", L"http://www.collada.org/2005/11/COLLADASchema",
		L"version", L"1.4.1");
	Writer->writeLineBreak();

	// write asset data 

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


	// write all materials

	Writer->writeElement(L"library_materials", false);
	Writer->writeLineBreak();

	u32 i;
	for (i=0; i<mesh->getMeshBufferCount(); ++i)
	{
		core::stringw strMat = "mat";
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

	Writer->writeClosingTag(L"library_materials");
	Writer->writeLineBreak();

	Writer->writeElement(L"library_effects", false);
	Writer->writeLineBreak();
	
	LibraryImages.clear();
	for (i=0; i<mesh->getMeshBufferCount(); ++i)
	{
		core::stringw strMat = "mat";
		strMat += i;
		strMat += L"-fx";

		Writer->writeElement(L"effect", false,
			L"id", strMat.c_str(),
			L"name", strMat.c_str());
		Writer->writeLineBreak();
		Writer->writeElement(L"profile_COMMON", false);
		Writer->writeLineBreak();

		video::SMaterial & material = mesh->getMeshBuffer(i)->getMaterial();

		// write texture surfaces and samplers and buffer all used imagess
		int numTextures = 0;
		for ( int t=0; t<4; ++t )
		{
			video::SMaterialLayer& layer  = material.TextureLayer[t];
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
					Writer->writeText(irr::core::stringw(layer.Texture->getName().getInternalName()).c_str());
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

		Writer->writeElement(L"technique", false, L"sid", L"common");
		Writer->writeLineBreak();
		Writer->writeElement(L"blinn", false);
		Writer->writeLineBreak();

		{
			// write all interesting material parameters 
			// attributes must be written in fixed order
			Writer->writeElement(L"emission", false);
			Writer->writeLineBreak();
			if ( numTextures < 1 )
				writeColorElement(material.EmissiveColor);
			else
			{
				// <texture texture="sampler" texcoord="texCoord"/>
				Writer->writeElement(L"texture", true, L"texture", L"tex0-sampler", L"texcoord", L"mesh-TexCoord0" );
				Writer->writeLineBreak();
			}
			Writer->writeClosingTag(L"emission");
			Writer->writeLineBreak();

			Writer->writeElement(L"ambient", false);
			Writer->writeLineBreak();
			if ( numTextures < 2 )
				writeColorElement(material.AmbientColor);
			else
			{
				// <texture texture="sampler" texcoord="texCoord"/>
				Writer->writeElement(L"texture", true, L"texture", L"tex1-sampler", L"texcoord", L"mesh-TexCoord0" );
				Writer->writeLineBreak();
			}
			Writer->writeClosingTag(L"ambient");
			Writer->writeLineBreak();

			Writer->writeElement(L"diffuse", false);
			Writer->writeLineBreak();
			if ( numTextures < 3 )
				writeColorElement(material.DiffuseColor);
			else
			{
				// <texture texture="sampler" texcoord="texCoord"/>
				Writer->writeElement(L"texture", true, L"texture", L"tex2-sampler", L"texcoord", L"mesh-TexCoord0" );
				Writer->writeLineBreak();
			}
			Writer->writeClosingTag(L"diffuse");
			Writer->writeLineBreak();

			Writer->writeElement(L"specular", false);
			Writer->writeLineBreak();
			if ( numTextures < 4 )
				writeColorElement(material.SpecularColor);
			else
			{
				// <texture texture="sampler" texcoord="texCoord"/>
				Writer->writeElement(L"texture", true, L"texture", L"tex3-sampler", L"texcoord", L"mesh-TexCoord0" );
				Writer->writeLineBreak();
			}
			Writer->writeClosingTag(L"specular");
			Writer->writeLineBreak();

			Writer->writeElement(L"shininess", false);
			Writer->writeLineBreak();
			Writer->writeElement(L"float", false);
			Writer->writeText(core::stringw(material.Shininess).c_str());
			Writer->writeClosingTag(L"float");
			Writer->writeLineBreak();
			Writer->writeClosingTag(L"shininess");
			Writer->writeLineBreak();
		}

		Writer->writeClosingTag(L"blinn");
		Writer->writeLineBreak();
		Writer->writeClosingTag(L"technique");
		Writer->writeLineBreak();
		Writer->writeClosingTag(L"profile_COMMON");
		Writer->writeLineBreak();
		Writer->writeClosingTag(L"effect");
		Writer->writeLineBreak();
	}

	Writer->writeClosingTag(L"library_effects");
	Writer->writeLineBreak();

	// images
	if ( !LibraryImages.empty() )
	{
		Writer->writeElement(L"library_images", false);
		Writer->writeLineBreak();

		for ( irr::u32 i=0; i<LibraryImages.size(); ++i )
		{
			//<image name="rose01"> 
			Writer->writeElement(L"image", false, L"name", irr::core::stringw(LibraryImages[i]->getName().getInternalName()).c_str());
			Writer->writeLineBreak();
			//  <init_from>../flowers/rose01.jpg</init_from> 
				Writer->writeElement(L"init_from", false);
				// TODO: path might need some conversion into collada URI-format to replace whitespaces etc.
				Writer->writeText(irr::core::stringw(LibraryImages[i]->getName().getPath()).c_str());
				Writer->writeClosingTag(L"init_from");
				Writer->writeLineBreak();
	 		//  </image> 
			Writer->writeClosingTag(L"image");
			Writer->writeLineBreak();
		}

		Writer->writeClosingTag(L"library_images");
		Writer->writeLineBreak();
	}

	// write mesh

	Writer->writeElement(L"library_geometries", false);
	Writer->writeLineBreak();

	Writer->writeElement(L"geometry", false, L"id", L"mesh", L"name", L"mesh");
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

	Writer->writeElement(L"source", false, L"id", L"mesh-Pos");
	Writer->writeLineBreak();

	core::stringw vertexCountStr(totalVertexCount*3);
	Writer->writeElement(L"float_array", false, L"id", L"mesh-Pos-array",
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

		Writer->writeElement(L"accessor", false, L"source", L"#mesh-Pos-array",
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

	Writer->writeElement(L"source", false, L"id", L"mesh-TexCoord0");
	Writer->writeLineBreak();

	vertexCountStr = core::stringw(totalVertexCount*2);
	Writer->writeElement(L"float_array", false, L"id", L"mesh-TexCoord0-array",
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
					Writer->writeText(toString(vtx[j].TCoords).c_str());
					Writer->writeLineBreak();
				}
			}
			break;
		case video::EVT_2TCOORDS:
			{
				video::S3DVertex2TCoords* vtx = (video::S3DVertex2TCoords*)buffer->getVertices();
				for (u32 j=0; j<vertexCount; ++j)
				{
					Writer->writeText(toString(vtx[j].TCoords).c_str());
					Writer->writeLineBreak();
				}
			}
			break;
		case video::EVT_TANGENTS:
			{
				video::S3DVertexTangents* vtx = (video::S3DVertexTangents*)buffer->getVertices();
				for (u32 j=0; j<vertexCount; ++j)
				{
					Writer->writeText(toString(vtx[j].TCoords).c_str());
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

		Writer->writeElement(L"accessor", false, L"source", L"#mesh-TexCoord0-array",
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

	Writer->writeElement(L"source", false, L"id", L"mesh-Normal");
	Writer->writeLineBreak();

	vertexCountStr = core::stringw(totalVertexCount*3);
	Writer->writeElement(L"float_array", false, L"id", L"mesh-Normal-array",
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

		Writer->writeElement(L"accessor", false, L"source", L"#mesh-Normal-array",
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

	if (totalTCoords2Count)
	{
		Writer->writeElement(L"source", false, L"id", L"mesh-TexCoord1");
		Writer->writeLineBreak();

		vertexCountStr = core::stringw(totalTCoords2Count*2);
		Writer->writeElement(L"float_array", false, L"id", L"mesh-TexCoord1-array",
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

			Writer->writeElement(L"accessor", false, L"source", L"#mesh-TexCoord1-array",
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

	Writer->writeElement(L"vertices", false, L"id", L"mesh-Vtx");
	Writer->writeLineBreak();

	Writer->writeElement(L"input", true, L"semantic", L"POSITION", L"source", L"#mesh-Pos");
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
		strMat += i;

		Writer->writeElement(L"triangles", false, L"count", strPolyCount.c_str(),
								L"material", strMat.c_str());
		Writer->writeLineBreak();

		Writer->writeElement(L"input", true, L"semantic", L"VERTEX", L"source", L"#mesh-Vtx", L"offset", L"0");
		Writer->writeLineBreak();
		Writer->writeElement(L"input", true, L"semantic", L"TEXCOORD", L"source", L"#mesh-TexCoord0", L"offset", L"1");
		Writer->writeLineBreak();
		Writer->writeElement(L"input", true, L"semantic", L"NORMAL", L"source", L"#mesh-Normal", L"offset", L"2");
		Writer->writeLineBreak();

		bool has2ndTexCoords = hasSecondTextureCoordinates(buffer->getVertexType());
		if (has2ndTexCoords)
		{
			Writer->writeElement(L"input", true, L"semantic", L"TEXCOORD", L"source", L"#mesh-TexCoord1", L"idx", L"3");
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

	Writer->writeClosingTag(L"mesh");
	Writer->writeLineBreak();
	Writer->writeClosingTag(L"geometry");
	Writer->writeLineBreak();

	Writer->writeClosingTag(L"library_geometries");
	Writer->writeLineBreak();

	// close everything

	Writer->writeClosingTag(L"COLLADA");
	Writer->drop();

	delete [] globalIndices;

	return true;
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

irr::core::stringw CColladaMeshWriter::toString(const irr::core::vector2df& vec) const
{
	c8 tmpbuf[255];
	snprintf(tmpbuf, 255, "%f %f", vec.X, vec.Y);
	core::stringw str  = tmpbuf;

	return str;
}

inline irr::core::stringw CColladaMeshWriter::toString(const irr::video::SColorf& colorf) const
{
	c8 tmpbuf[255];
	snprintf(tmpbuf, 255, "%f %f %f %f", colorf.getRed(), colorf.getGreen(), colorf.getBlue(), colorf.getAlpha());
	core::stringw str = tmpbuf;

	return str;
}

inline irr::core::stringw CColladaMeshWriter::toString(const irr::video::ECOLOR_FORMAT format) const
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

inline irr::core::stringw CColladaMeshWriter::toString(const irr::video::E_TEXTURE_CLAMP clamp) const
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

inline irr::core::stringw CColladaMeshWriter::minTexfilterToString(bool bilinear, bool trilinear) const
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

void CColladaMeshWriter::writeColorElement(const video::SColor & col)
{
	Writer->writeElement(L"color", false);

	irr::core::stringw str( toString(video::SColorf(col)) );
	Writer->writeText(str.c_str());

	Writer->writeClosingTag(L"color");
	Writer->writeLineBreak();
}


} // end namespace
} // end namespace

#endif

