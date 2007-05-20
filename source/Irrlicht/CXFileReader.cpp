// Copyright (C) 2002-2007 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CXFileReader.h"
#include "os.h"
#include "fast_atof.h"
#include "coreutil.h"

namespace irr
{
namespace scene
{

CXFileReader::CXFileReader(io::IReadFile* file)
: MajorVersion(0), MinorVersion(0), binary(false), binaryNumCount(0),
	Buffer(0), Size(0), FloatSize(0), P(0), End(0), ErrorHappened(false),
	m_bFrameRemoved(false), m_pgCurFrame(0)
{
	if (!file)
	{
		ErrorHappened = true;
		return;
	}

	if (!readFileIntoMemory(file))
	{
		ErrorHappened = true;
		return;
	}

	if (!parseFile())
	{
		ErrorHappened = true;
		return;
	}

#ifdef _XREADER_DEBUG
	for( unsigned int i = 0; i < RootFrames.size(); i++ )
		validateMesh(&RootFrames[i]);
#endif

	for( unsigned int i = 0; i < RootFrames.size(); i++ )
	{
		computeGlobalFrameMatrices(RootFrames[i], 0 );
	}
}


CXFileReader::~CXFileReader()
{
	if (Buffer)
		delete [] Buffer;

	for (u32 i=0; i<AnimationSets.size(); ++i)
		for (u32 j=0; j<AnimationSets[i].Animations.size(); ++j)
			for (u32 k=0; k<AnimationSets[i].Animations[j].Keys.size(); ++k)
				AnimationSets[i].Animations[j].Keys[k].del();
}


//! Returns an error occured during reading the file
bool CXFileReader::errorHappened() const
{
	return ErrorHappened;
}


//! checks if there was an error loading the mesh
bool CXFileReader::validateMesh(SXFrame* frame)
{
	bool error = false;
	char tmp[1024];

	// validate all meshes in this frame

	for (u32 m=0; m<frame->Meshes.size(); ++m)
	{
		s32 vcnt = frame->Meshes[m].Vertices.size();
		s32 icnt = frame->Meshes[m].Indices.size();
		s32 ncnt = frame->Meshes[m].Normals.size();
		s32 nicnt = frame->Meshes[m].NormalIndices.size();

		// validate indices in mesh
		s32 i;

		for (i=0; i<icnt; ++i)
		{
			if (frame->Meshes[m].Indices[i] < 0)
			{
				sprintf(tmp, "XLoader error: index %d smaller than 0 in mesh %u: %d, frame '%s'",
					i, m, frame->Meshes[m].Indices[i], frame->Name.c_str());
				os::Printer::log(tmp, ELL_ERROR);
				error = true;
			}

			if (frame->Meshes[m].Indices[i] > vcnt-1)
			{
				sprintf(tmp, "XLoader error: invalid index %d in mesh %u: %d, frame '%s'",
					i, m, frame->Meshes[m].Indices[i], frame->Name.c_str());
				os::Printer::log(tmp, ELL_ERROR);
				error = true;
			}
		}

		// validate normal indices

		for (i=0; i<nicnt; ++i)
		{
			if (frame->Meshes[m].NormalIndices[i] < 0)
			{
				sprintf(tmp, "XLoader error: normal index %d smaller than 0 in mesh %u: %d, frame '%s'",
					i, m, frame->Meshes[m].NormalIndices[i], frame->Name.c_str());
				os::Printer::log(tmp, ELL_ERROR);
				error = true;
			}

			if (frame->Meshes[m].NormalIndices[i] > ncnt-1)
			{
				sprintf(tmp, "XLoader error: invalid normal index %d in mesh %u: %d, frame '%s'",
					i, m, frame->Meshes[m].NormalIndices[i], frame->Name.c_str());
				os::Printer::log(tmp, ELL_ERROR);
				error = true;
			}
		}
	}


	// validate child frames
	for (u32 i=0; i<frame->ChildFrames.size(); ++i)
		if (!validateMesh(&frame->ChildFrames[i]))
			error = true;

	return error;
}


//! Reads file into memory
bool CXFileReader::readFileIntoMemory(io::IReadFile* file)
{
	s32 Size = file->getSize();
	if (Size < 12)
	{
		os::Printer::log("X File is too small.", ELL_WARNING);
		return false;
	}

	Buffer = new c8[Size];

	//! read all into memory
	file->seek(0); // apparently sometimes files have been read already, so reset it
	if (file->read(Buffer, Size) != Size)
	{
		os::Printer::log("Could not read from x file.", ELL_WARNING);
		return false;
	}

	End = Buffer + Size;

	//! check header "xof "
	if (strncmp(Buffer, "xof ", 4)!=0)
	{
		os::Printer::log("Not an x file, wrong header.", ELL_WARNING);
		return false;
	}

	//! read minor and major version, e.g. 0302 or 0303
	c8 tmp[3];
	tmp[2] = 0x0;
	tmp[0] = Buffer[4];
	tmp[1] = Buffer[5];
	MajorVersion = strtol(tmp, (char**) &P, 10);

	tmp[0] = Buffer[6];
	tmp[1] = Buffer[7];
	MinorVersion = strtol(tmp, (char**) &P, 10);

	//! read format
	if (strncmp(&Buffer[8], "txt ", 4) ==0)
		binary = false;
	else if (strncmp(&Buffer[8], "bin ", 4) ==0)
		binary = true;
	else
	{
		os::Printer::log("Only uncompressed x files currently supported.", ELL_WARNING);
		return false;
	}
	binaryNumCount=0;

	//! read float size
	if (strncmp(&Buffer[12], "0032", 4) ==0)
		FloatSize = 4;
	else if (strncmp(&Buffer[12], "0064", 4) ==0)
		FloatSize = 8;
	else
	{
		os::Printer::log("Float size not supported.", ELL_WARNING);
		return false;
	}

	P = &Buffer[16];

	readUntilEndOfLine();

	return true;
}

//! Parses the file
bool CXFileReader::parseFile()
{
	u32 u32Idx;
	while(parseDataObject())
	{
		// loop
	}
	// loop through hiearchy and combine frames that have no mesh
	// and no name into its parent

	m_bFrameRemoved = false;
	for( u32Idx = 0; u32Idx < RootFrames.size(); u32Idx++ )
	{
		optimizeFrames( &RootFrames[ u32Idx ], 0 );
	}
	while( m_bFrameRemoved )
	{
		m_bFrameRemoved = false;
		for( u32Idx = 0; u32Idx < RootFrames.size(); u32Idx++ )
		{
			optimizeFrames( &RootFrames[ u32Idx ], 0 );
		}
	}
	return true;
}

//! loop through hiearchy and combine frames that have no mesh or name into parent frame
void CXFileReader::optimizeFrames( SXFrame * pgFrame,  SXFrame * pgParent )
{
	if( pgParent )
	{
		if( (0 == pgParent->Meshes.size()) &&
			(0 == strlen( pgFrame->Name.c_str() )) &&
			strlen( pgParent->Name.c_str() ) )
		{
			// combine this frame with parent
			// add child frames to parent
			pgParent->LocalMatrix *= pgFrame->LocalMatrix;

			u32 c;

			for( c=0; c<pgFrame->ChildFrames.size(); ++c )
			{
				// add child frames to parent
				pgParent->ChildFrames.push_back(pgFrame->ChildFrames[c]);
			}

			// add meshes to parent

			for( c=0; c<pgFrame->Meshes.size(); ++c )
			{
				// add meshes frames to parent
				pgParent->Meshes.push_back( pgFrame->Meshes[c] );
			}

			// remove child frames in our list
			pgFrame->ChildFrames.clear();

			// remove meshes
			pgFrame->Meshes.clear();

			// find ourselve and remove from parent frame
			for( c=0; c< pgParent->ChildFrames.size(); ++c )
			{
				if( &pgParent->ChildFrames[c] == pgFrame )
				{
					//found ourself
					pgParent->ChildFrames.erase( c, 1 );
					m_bFrameRemoved = true;
					return;
				}
			}
		}
	}
	for (u32 c=0; c<pgFrame->ChildFrames.size(); ++c)
		optimizeFrames( &pgFrame->ChildFrames[c], pgFrame );
}


//! Parses the next Data object in the file
bool CXFileReader::parseDataObject()
{
	core::stringc objectName = getNextToken();

	if (objectName.size() == 0)
		return false;

	// parse specific object

	if (objectName == "template")
		return parseDataObjectTemplate();
	else
	if (objectName == "Frame")
	{
		if (!m_pgCurFrame)
		{
			RootFrames.push_back(SXFrame());
			m_pgCurFrame = &RootFrames.getLast();
		}
		else
		{
			m_pgCurFrame->ChildFrames.push_back(SXFrame());
			m_pgCurFrame = &(m_pgCurFrame->ChildFrames.getLast());
		}
		return parseDataObjectFrame( * m_pgCurFrame );
	}
	else
	if (objectName == "Mesh")
	{
		// some meshes have no frames at all
		if (!m_pgCurFrame)
		{
			RootFrames.push_back(SXFrame());
			m_pgCurFrame = &RootFrames.getLast();
		}
		m_pgCurFrame->Meshes.push_back(SXMesh());
		return parseDataObjectMesh(m_pgCurFrame->Meshes.getLast());
	}
	else
	if (objectName == "AnimationSet")
	{
		AnimationSets.push_back(SXAnimationSet());
		return parseDataObjectAnimationSet(AnimationSets.getLast());
	}
	else
	if (objectName == "Material")
	{
		// template materials now available thanks to joeWright
		TemplateMaterials.push_back(SXTemplateMaterial());
		TemplateMaterials.getLast().Name = getNextToken();
		return parseDataObjectMaterial(TemplateMaterials.getLast().Material);
	}

	os::Printer::log("Unknown data object in x file", objectName.c_str());

	return parseUnknownDataObject();
}


bool CXFileReader::parseDataObjectFrame(SXFrame& frame)
{
#ifdef _XREADER_DEBUG
	os::Printer::log("CXFileReader: Reading frame");
#endif

	// A coordinate frame, or "frame of reference." The Frame template
	// is open and can contain any object. The Direct3D extensions (D3DX)
	// mesh-loading functions recognize Mesh, FrameTransformMatrix, and
	// Frame template instances as child objects when loading a Frame
	// instance.

	if (!readHeadOfDataObject(&frame.Name))
	{
		os::Printer::log("No opening brace in Frame found in x file", ELL_WARNING);
		return false;
	}

	// Now inside a frame.
	// read tokens until closing brace is reached.

	while(true)
	{
		core::stringc objectName = getNextToken();

		if (objectName.size() == 0)
		{
			os::Printer::log("Unexpected ending found in Frame in x file.", ELL_WARNING);
			return false;
		}
		else
		if (objectName == "}")
		{
			break; // frame finished
		}
		else
		if (objectName == "Frame")
		{
			frame.ChildFrames.push_back(SXFrame());
			if (!parseDataObjectFrame(frame.ChildFrames.getLast()))
				return false;
		}
		else
		if (objectName == "FrameTransformMatrix")
		{
			if (!parseDataObjectTransformationMatrix(frame.LocalMatrix))
				return false;
		}
		else
		if (objectName == "Mesh")
		{
			frame.Meshes.push_back(SXMesh());
			if (!parseDataObjectMesh(frame.Meshes.getLast()))
				return false;
		}
		else
		{
			os::Printer::log("Unknown data object in frame in x file", objectName.c_str());
			if (!parseUnknownDataObject())
				return false;
		}

	}

	return true;
}


bool CXFileReader::parseDataObjectTransformationMatrix(core::matrix4 &mat)
{
#ifdef _XREADER_DEBUG
	os::Printer::log("CXFileReader: Reading Transformation Matrix");
#endif

	if (!readHeadOfDataObject())
	{
		os::Printer::log("No opening brace in Transformation Matrix found in x file", ELL_WARNING);
		return false;
	}

	if (binary)
	{
		// read matrix in binary format
		if (readBinWord() != 7)
		{
			os::Printer::log("Binary X: Mesh: Expecting float list (for matrix)", ELL_WARNING);
			return false;
		}

		if (readBinDWord() != 0x10)
		{
			os::Printer::log("Binary X: Mesh: Should be 16 floats in matrix", ELL_WARNING);
			return false;
		}
	}

	for (s32 i=0; i<4; ++i)
		for (s32 j=0; j<4; ++j)
			mat(i,j)=readFloat();

	if (!checkForTwoFollowingSemicolons())
	{
		os::Printer::log("No finishing semicolon in Transformation Matrix found in x file", ELL_WARNING);
		return false;
	}

	if (getNextToken() != "}")
	{
		os::Printer::log("No closing brace in Transformation Matrix found in x file", ELL_WARNING);
		return false;
	}

	return true;
}


bool CXFileReader::parseDataObjectTemplate()
{
#ifdef _XREADER_DEBUG
	os::Printer::log("CXFileReader: Reading template");
#endif

	// parse a template data object. Currently not stored.
	core::stringc name = getNextToken();

	// ignore left delimiter
	if (getNextToken() != "{")
	{
		os::Printer::log("Left delimiter in template data object missing.",
			name.c_str(), ELL_ERROR);
		return false;
	}

	// read GUID
	core::stringc guid = getNextToken();

	// read and ignore data members
	while(true)
	{
		core::stringc s = getNextToken();

		if (s == "}")
			break;

		if (s.size() == 0)
			return false;
	}

	return true;
}


bool CXFileReader::parseDataObjectMesh(SXMesh &mesh)
{
#ifdef _XREADER_DEBUG
	os::Printer::log("CXFileReader: Reading mesh");
#endif

	if (!readHeadOfDataObject(&mesh.Name))
	{
		os::Printer::log("No opening brace in Mesh found in x file", ELL_WARNING);
		return false;
	}

	// read vertex count
	s32 nVertices = readInt();

	// read vertices
	mesh.Vertices.set_used(nVertices);

	s32 count=0;
	if (binary)
	{
		// read vertices in binary format
		if (readBinWord() != 7)
		{
			os::Printer::log("Binary X: Mesh: Expecting float list (for vertices)", ELL_WARNING);
			return false;
		}
		count = readBinDWord();
		if (count != (nVertices * 3))
		{
			os::Printer::log("Binary X: Mesh: Value count not matching vertices count", ELL_WARNING);
			return false;
		}
	}

	for (s32 n=0; n<nVertices; ++n)
		readVector3(mesh.Vertices[n]);

	if (!checkForTwoFollowingSemicolons())
	{
		os::Printer::log("No finishing semicolon in Mesh Vertex Array found in x file", ELL_WARNING);
		return false;
	}

	// read faces
	s32 nFaces = readInt();

	mesh.Indices.set_used(nFaces * 3);
	mesh.IndexCountPerFace.set_used(nFaces);

	core::array<s32> polygonfaces;
	s32 currentIndex = 0;

	for (s32 k=0; k<nFaces; ++k)
	{
		s32 fcnt = readInt();

		if (fcnt != 3)
		{
			if (fcnt < 3)
			{
				os::Printer::log("Invalid face count (<3) found in Mesh x file reader.", ELL_WARNING);
				return false;
			}

			// read face indices
			polygonfaces.set_used(fcnt);
			s32 triangles = (fcnt-2);
			mesh.Indices.set_used(mesh.Indices.size() + ((triangles*3)-3));
			mesh.IndexCountPerFace[k] = triangles * 3;

			for (int f=0; f<fcnt; ++f)
				polygonfaces[f] = readInt();

			for (s32 jk=0; jk<triangles; ++jk)
			{
				mesh.Indices[currentIndex++] = polygonfaces[0];
				mesh.Indices[currentIndex++] = polygonfaces[jk+1];
				mesh.Indices[currentIndex++] = polygonfaces[jk+2];
			}

			// TODO: change face indices in material list
		}
		else
		{
			mesh.Indices[currentIndex++] = readInt();
			mesh.Indices[currentIndex++] = readInt();
			mesh.Indices[currentIndex++] = readInt();
			mesh.IndexCountPerFace[k] = 3;
		}
	}

	if (binary && binaryNumCount)
	{
		os::Printer::log("Binary X: Mesh: Integer count mismatch", ELL_WARNING);
		return false;
	}
	else if (!checkForTwoFollowingSemicolons())
	{
		os::Printer::log("No finishing semicolon in Mesh Face Array found in x file", ELL_WARNING);
		return false;
	}

	// here, other data objects may follow

	while(true)
	{
		core::stringc objectName = getNextToken();

		if (objectName.size() == 0)
		{
			os::Printer::log("Unexpected ending found in Mesh in x file.", ELL_WARNING);
			return false;
		}
		else
		if (objectName == "}")
		{
			break; // mesh finished
		}
		else
		if (objectName == "MeshNormals")
		{
			if (!parseDataObjectMeshNormals(mesh.Normals, mesh.NormalIndices,
				mesh.Indices.size(), mesh.IndexCountPerFace))
				return false;
		}
		else
		if (objectName == "MeshTextureCoords")
		{
			if (!parseDataObjectMeshTextureCoords(mesh.TextureCoords))
				return false;
		}
		else
		if (objectName == "MeshVertexColors")
		{
			if (!parseDataObjectMeshVertexColors(mesh.VertexColors))
				return false;
		}
		else
		if (objectName == "MeshMaterialList")
		{
			if (!parseDataObjectMeshMaterialList(mesh.MaterialList,
				mesh.Indices.size(), mesh.IndexCountPerFace))
					return false;
		}
		else
		if (objectName == "VertexDuplicationIndices")
		{
			// we'll ignore vertex duplication indices
			// TODO: read them
			if (!parseUnknownDataObject())
				return false;
		}
		else
		if (objectName == "XSkinMeshHeader")
		{
			if (!parseDataObjectSkinMeshHeader(mesh.SkinMeshHeader))
				return false;
		}
		else
		if (objectName == "SkinWeights")
		{
			mesh.SkinWeights.push_back(SXSkinWeight());
			if (!parseDataObjectSkinWeights(mesh.SkinWeights.getLast()))
				return false;
		}
		else
		{
			os::Printer::log("Unknown data object in mesh in x file", objectName.c_str());
			if (!parseUnknownDataObject())
				return false;
		}
	}

	return true;
}


bool CXFileReader::parseDataObjectSkinWeights(SXSkinWeight& weights)
{
#ifdef _XREADER_DEBUG
	os::Printer::log("CXFileReader: Reading mesh skin weights");
#endif

	if (!readHeadOfDataObject())
	{
		os::Printer::log("No opening brace in Skin Weights found in x file", ELL_WARNING);
		return false;
	}

	if (!getNextTokenAsString(weights.TransformNodeName))
	{
		os::Printer::log("Unknown syntax while reading transfrom node name string in x file", ELL_WARNING);
		return false;
	}

	// read vertex weights

	s32 nWeights = readInt();
	weights.Weights.set_used(nWeights);

	// read vertex indices

	s32 i;

	for (i=0; i<nWeights; ++i)
		weights.Weights[i].VertexIndex = readInt();

	// read vertex weights

	if (binary)
	{
		// read float list in binary format
		if (readBinWord() != 7)
		{
			os::Printer::log("Binary X: Mesh: Expecting float list (for SkinWeights)", ELL_WARNING);
			return false;
		}

		if (readBinDWord() != (u32)(nWeights+16))
		{
			os::Printer::log("Binary X: Mesh: Wrong number of floats", ELL_WARNING);
			return false;
		}
	}

	for (i=0; i<nWeights; ++i)
		weights.Weights[i].Weight = readFloat();

	// sort weights

	weights.Weights.set_sorted(false);
	weights.Weights.sort();

	// read matrix offset

	for (i=0; i<4; ++i)
		for (u32 j=0; j<4; ++j)
			weights.MatrixOffset(i,j) = readFloat();

	if (!checkForTwoFollowingSemicolons())
	{
		os::Printer::log("No finishing semicolon in Skin Weights found in x file", ELL_WARNING);
		return false;
	}

	if (getNextToken() != "}")
	{
		os::Printer::log("No closing brace in Skin Weights found in x file", ELL_WARNING);
		return false;
	}

	return true;
}


bool CXFileReader::parseDataObjectSkinMeshHeader(SXSkinMeshHeader& header)
{
#ifdef _XREADER_DEBUG
	os::Printer::log("CXFileReader: Reading skin mesh header");
#endif

	if (!readHeadOfDataObject())
	{
		os::Printer::log("No opening brace in Skin Mesh header found in x file", ELL_WARNING);
		return false;
	}

	header.MaxSkinWeightsPerVertex = readInt();
	header.MaxSkinWeightsPerFace = readInt();
	header.BoneCount = readInt();
	if (!binary)
		getNextToken(); // skip semicolon
	core::stringc objectName = getNextToken();

	if (objectName != "}")
	{
		os::Printer::log("No closing brace in skin mesh header in x file", objectName.c_str());
		return false;
	}

	return true;
}


bool CXFileReader::parseDataObjectMeshMaterialList(SXMeshMaterialList& mlist,
					s32 triangulatedIndexCount,
					core::array< s32 >& indexCountPerFace)
{
#ifdef _XREADER_DEBUG
	os::Printer::log("CXFileReader: Reading mesh material list");
#endif

	if (!readHeadOfDataObject())
	{
		os::Printer::log("No opening brace in Mesh Material List found in x file", ELL_WARNING);
		return false;
	}

	// read material count
	readInt();

	// read non triangulated face material index count
	s32 nFaceIndices = readInt();

	// read non triangulated face indices

	core::array<s32> nonTriFaceIndices;
	nonTriFaceIndices.set_used(nFaceIndices);

	for (s32 i=0; i<nFaceIndices; ++i)
		nonTriFaceIndices[i] = readInt();

	// create triangulated face indices

	if (nFaceIndices != (s32)indexCountPerFace.size())
	{
		os::Printer::log("Index count per face not equal to face material index count in x file.", ELL_WARNING);
		return false;
	}

	mlist.FaceIndices.set_used(triangulatedIndexCount / 3);
	s32 triangulatedindex = 0;
	for (s32 tfi=0; tfi<nFaceIndices; ++tfi)
		for (s32 k=0; k<indexCountPerFace[tfi]/3; ++k)
			mlist.FaceIndices[triangulatedindex++] = nonTriFaceIndices[tfi];

	// in version 03.02, the face indices end with two semicolons.
	// commented out version check, as version 03.03 exported from blender also has 2 semicolons
	if (!binary) // && MajorVersion == 3 && MinorVersion <= 2)
	{
		if (P[0] == ';')
			++P;
	}

	// read following data objects

	while(true)
	{
		core::stringc objectName = getNextToken();

		if (objectName.size() == 0)
		{
			os::Printer::log("Unexpected ending found in Mesh Material list in x file.", ELL_WARNING);
			return false;
		}
		else
		if (objectName == "}")
		{
			break; // material list finished
		}
		else
		if (objectName == "{")
		{
			// template materials now available thanks to joeWright
			objectName = getNextToken();
			for (u32 i=0; i<TemplateMaterials.size(); ++i)
				if (TemplateMaterials[i].Name == objectName)
					mlist.Materials.push_back(TemplateMaterials[i].Material);
			getNextToken(); // skip }
		}
		else
		if (objectName == "Material")
		{
			mlist.Materials.push_back(SXMaterial());
			if (!parseDataObjectMaterial(mlist.Materials.getLast()))
				return false;
		}
		else
		if (objectName == ";")
		{
			// ignore
		}
		else
		{
			os::Printer::log("Unknown data object in material list in x file", objectName.c_str());
			if (!parseUnknownDataObject())
				return false;
		}
	}
	return true;
}



bool CXFileReader::parseDataObjectMaterial(SXMaterial& material)
{
#ifdef _XREADER_DEBUG
	os::Printer::log("CXFileReader: Reading mesh material");
#endif

	if (!readHeadOfDataObject())
	{
		os::Printer::log("No opening brace in Mesh Material found in x file", ELL_WARNING);
		return false;
	}

	u32 count = 0;
	if (binary)
	{
		if (readBinWord() != 7)
		{
			os::Printer::log("Binary X: Material: Expecting float list", ELL_WARNING);
			return false;
		}
		count = readBinDWord();
		if (count != 11)
		{
			os::Printer::log("Binary X: Material: Float list length not equal to 11", ELL_WARNING);
			return false;
		}
	}

	// read RGBA
	readRGBA(material.FaceColor);

	// read power
	material.Power = readFloat();

	// read specular
	readRGB(material.Specular);

	// read emissive
	readRGB(material.Emissive);

	// read other data objects
	while(true)
	{
		core::stringc objectName = getNextToken();

		if (objectName.size() == 0)
		{
			os::Printer::log("Unexpected ending found in Mesh Material in x file.", ELL_WARNING);
			return false;
		}
		else
		if (objectName == "}")
		{
			break; // material finished
		}
		else
		if (objectName.equals_ignore_case("TextureFilename"))
		{
			// some exporters write "TextureFileName" instead.

			if (!parseDataObjectTextureFilename(material.TextureFileName))
				return false;
		}
		else
		{
			os::Printer::log("Unknown data object in material in x file", objectName.c_str());
			if (!parseUnknownDataObject())
				return false;
		}

	}

	return true;
}


//! reads a x file style string
bool CXFileReader::getNextTokenAsString(core::stringc& out)
{
	if (binary)
	{
		out=getNextToken();
		return true;
	}
	findNextNoneWhiteSpace();

	if (P >= End)
		return false;

	if (P[0] != '"')
		return false;
	++P;

	while(P < End && P[0]!='"')
	{
		out.append(P[0]);
		++P;
	}

	if ( P[1] != ';' || P[0] != '"')
		return false;
	P+=2;

	return true;
}


bool CXFileReader::parseDataObjectAnimationSet(SXAnimationSet& set)
{
	#ifdef _XREADER_DEBUG
	os::Printer::log("CXFileReader: Reading animation set");
	#endif

	if (!readHeadOfDataObject(&set.AnimationName))
	{
		os::Printer::log("No opening brace in Animation Set found in x file", ELL_WARNING);
		return false;
	}

	while(true)
	{
		core::stringc objectName = getNextToken();

		if (objectName.size() == 0)
		{
			os::Printer::log("Unexpected ending found in Animation set in x file.", ELL_WARNING);
			return false;
		}
		else
		if (objectName == "}")
		{
			break; // animation set finished
		}
		else
		if (objectName == "Animation")
		{
			set.Animations.push_back(SXAnimation());
			if (!parseDataObjectAnimation(set.Animations.getLast()))
				return false;
		}
		else
		{
			os::Printer::log("Unknown data object in animation set in x file", objectName.c_str());
			if (!parseUnknownDataObject())
				return false;
		}
	}
	return true;
}



bool CXFileReader::parseDataObjectAnimation(SXAnimation& anim)
{
#ifdef _XREADER_DEBUG
	os::Printer::log("CXFileReader: reading animation");
#endif

	if (!readHeadOfDataObject())
	{
		os::Printer::log("No opening brace in Animation found in x file", ELL_WARNING);
		return false;
	}

	anim.closed = true;
	anim.linearPositionQuality = true;

	while(true)
	{
		core::stringc objectName = getNextToken();

		if (objectName.size() == 0)
		{
			os::Printer::log("Unexpected ending found in Animation in x file.", ELL_WARNING);
			return false;
		}
		else
		if (objectName == "}")
		{
			break; // animation finished
		}
		else
		if (objectName == "AnimationKey")
		{
			anim.Keys.push_back(SXAnimationKey());
			if (!parseDataObjectAnimationKey(anim.Keys.getLast()))
				return false;
		}
		else
		if (objectName == "AnimationOptions")
		{
			//TODO: parse options.
			if (!parseUnknownDataObject())
				return false;
		}
		else
		if (objectName == "{")
		{
			// read frame name
			anim.FrameName = getNextToken();
			core::stringc end = getNextToken();
			if (end.size() == 0 || end != "}")
			{
				os::Printer::log("Unexpected ending found in Animation in x file.", ELL_WARNING);
				return false;
			}
		}
		else
		{
			if (objectName.size()>2 && objectName[0] == '{' &&
				objectName[objectName.size()-1] == '}')
			{
				anim.FrameName = objectName.subString(1,objectName.size()-2);
			}
			else
			{
				os::Printer::log("Unknown data object in animation in x file", objectName.c_str());
				if (!parseUnknownDataObject())
					return false;
			}
		}
	}
	return true;
}



bool CXFileReader::parseDataObjectAnimationKey(SXAnimationKey& animkey)
{
#ifdef _XREADER_DEBUG
	os::Printer::log("CXFileReader: reading animation key");
#endif

	if (!readHeadOfDataObject())
	{
		os::Printer::log("No opening brace in Animation Key found in x file", ELL_WARNING);
		return false;
	}

	// read key type
	animkey.keyType = readInt();

	if ((animkey.keyType < 0) || (animkey.keyType > 4))
	{
		os::Printer::log("Unknown key type found in Animation Key in x file", ELL_WARNING);
		return false;
	}

	// read number of keys
	animkey.numberOfKeys = readInt();

	// eat the semicolon after the "0".  if there are keys present, readInt()
	// does this for us.  If there aren't, we need to do it explicitly
	if (!binary && animkey.numberOfKeys == 0)
		getNextToken(); // skip semicolon

	animkey.init();

	// read keys
	switch(animkey.keyType)
	{
	case 0:
		{
			//read quaternions
			for (s32 i=0; i<animkey.numberOfKeys; ++i)
			{
				// read time
				animkey.time[i] = readInt();

				// read count
				if (readInt() != 4)
				{
					os::Printer::log("Expected 4 numbers in animation key in x file", ELL_WARNING);
					return false;
				}

				if (binary)
				{
					if (readBinWord() != 7)
					{
						os::Printer::log("Binary X: Animation Key: Expecting float list", ELL_WARNING);
						return false;
					}

					if (readBinDWord() != 4)
					{
						os::Printer::log("Binary X: Animation Key : Value count not correct", ELL_WARNING);
						return false;
					}
				}
				animkey.getQuaternion(i).W = -readFloat();
				animkey.getQuaternion(i).X = -readFloat();
				animkey.getQuaternion(i).Y = -readFloat();
				animkey.getQuaternion(i).Z = -readFloat();

				if (!checkForTwoFollowingSemicolons())
				{
					os::Printer::log("No finishing semicolon after quaternion animation key in x file", ELL_WARNING);
					return false;
				}
			}
		}
		break;
	case 1:
	case 2:
		{
			// read vectors
			for (s32 i=0; i<animkey.numberOfKeys; ++i)
			{
				// read time
				animkey.time[i] = readInt();

				// read count
				if (readInt() != 3)
				{
					os::Printer::log("Expected 3 numbers in animation key in x file", ELL_WARNING);
					return false;
				}

				if (binary)
				{
					if (readBinWord() != 7)
					{
						os::Printer::log("Binary X: Animation Key: Expecting float list", ELL_WARNING);
						return false;
					}

					if (readBinDWord() != 3)
					{
						os::Printer::log("Binary X: Animation Key : Value count not correct", ELL_WARNING);
						return false;
					}
				}
				readVector3(animkey.getVector(i));

				if (!checkForTwoFollowingSemicolons())
				{
					os::Printer::log("No finishing semicolon after vector animation key in x file", ELL_WARNING);
					return false;
				}
			}
		}
		break;
	case 3:
	case 4:
		{
			// read matrix
			for (s32 i=0; i<animkey.numberOfKeys; ++i)
			{
				// read time
				animkey.time[i] = readInt();

				// read count
				if (readInt() != 16)
				{
					os::Printer::log("Expected 16 numbers in animation key in x file", ELL_WARNING);
					return false;
				}

				// read matrix
				if (binary)
				{
					if (readBinWord() != 7)
					{
						os::Printer::log("Binary X: Animation Key: Expecting float list", ELL_WARNING);
						return false;
					}

					if (readBinDWord() != 16)
					{
						os::Printer::log("Binary X: Animation Key : Value count not correct", ELL_WARNING);
						return false;
					}
				}
				for (s32 m=0; m<4; ++m)
					for (s32 n=0; n<4; ++n)
						animkey.getMatrix(i)(m,n) = readFloat();

				if (!checkForTwoFollowingSemicolons())
				{
					os::Printer::log("No finishing semicolon after matrix animation key in x file", ELL_WARNING);
					return false;
				}
			}
		}
		break;
	} // end switch

	if (!binary)
		getNextToken(); // skip another semicolon
	core::stringc objectName = getNextToken();

	if (objectName != "}")
	{
		os::Printer::log("No closing brace in animation key in x file", objectName.c_str());
		return false;
	}

	return true;
}


bool CXFileReader::parseDataObjectTextureFilename(core::stringc& texturename)
{
#ifdef _XREADER_DEBUG
	os::Printer::log("CXFileReader: reading texture filename");
#endif

	if (!readHeadOfDataObject())
	{
		os::Printer::log("No opening brace in Texture filename found in x file", ELL_WARNING);
		return false;
	}

	if (!getNextTokenAsString(texturename))
	{
		os::Printer::log("Unknown syntax while reading texture filename string in x file", ELL_WARNING);
		return false;
	}

	if (getNextToken() != "}")
	{
		os::Printer::log("No closing brace in Texture filename found in x file", ELL_WARNING);
		return false;
	}

	return true;
}


bool CXFileReader::parseDataObjectMeshNormals(core::array<core::vector3df>& normals,
					core::array< s32 >& normalIndices,
					s32 triangulatedIndexCount,
					core::array< s32 >& indexCountPerFace)
{
#ifdef _XREADER_DEBUG
	os::Printer::log("CXFileReader: reading mesh normals");
#endif

	if (!readHeadOfDataObject())
	{
		os::Printer::log("No opening brace in Mesh Normals found in x file", ELL_WARNING);
		return false;
	}

	// read count
	s32 nNormals;
	s32 count;
	nNormals = readInt();
	normals.set_used(nNormals);

	// read normals
	if (binary)
	{
		if (readBinWord() != 7)
		{
			os::Printer::log("Binary X: MeshNormals: Expecting float list", ELL_WARNING);
			return false;
		}
		count = readBinDWord();
		if (count != nNormals * 3)
		{
			os::Printer::log("Binary X: MeshNormals: Value count not equal to normal count", ELL_WARNING);
			return false;
		}
	}
	for (s32 i=0; i<nNormals; ++i)
		readVector3(normals[i]);

	if (!checkForTwoFollowingSemicolons())
	{
		os::Printer::log("No finishing semicolon in Mesh Normals Array found in x file", ELL_WARNING);
		return false;
	}

	// read face normal indices
	s32 nFNormals = readInt();
	normalIndices.set_used(triangulatedIndexCount);

	s32 normalidx = 0;
	core::array<s32> polygonfaces;
	for (s32 k=0; k<nFNormals; ++k)
	{
		s32 fcnt = readInt();
		s32 triangles = fcnt - 2;
		s32 indexcount = triangles * 3;

		if (indexcount != indexCountPerFace[k])
		{
			os::Printer::log("Not matching normal and face index count found in x file", ELL_WARNING);
			return false;
		}

		if (indexcount == 3)
		{
			// default, only one triangle in this face
			for (s32 h=0; h<3; ++h)
				normalIndices[normalidx++] = readInt();
		}
		else
		{
			// multiple triangles in this face
			polygonfaces.set_used(fcnt);

			for (s32 h=0; h<fcnt; ++h)
				polygonfaces[h] = readInt();

			for (s32 jk=0; jk<triangles; ++jk)
			{
				normalIndices[normalidx++] = polygonfaces[0];
				normalIndices[normalidx++] = polygonfaces[jk+1];
				normalIndices[normalidx++] = polygonfaces[jk+2];
			}
		}
	}

	if (!checkForTwoFollowingSemicolons())
	{
		os::Printer::log("No finishing semicolon in Mesh Face Normals Array found in x file", ELL_WARNING);
		return false;
	}
	if (getNextToken() != "}")
	{
		os::Printer::log("No closing brace in Mesh Normals found in x file", ELL_WARNING);
		return false;
	}

	return true;
}


bool CXFileReader::parseDataObjectMeshTextureCoords(
	core::array<core::vector2df>& textureCoords)
{
#ifdef _XREADER_DEBUG
	os::Printer::log("CXFileReader: reading mesh texture coordinates");
#endif

	if (!readHeadOfDataObject())
	{
		os::Printer::log("No opening brace in Mesh Texture Coordinates found in x file", ELL_WARNING);
		return false;
	}

	s32 nCoords;
	u32 count;
	nCoords = readInt();
	if (binary)
	{
		if (readBinWord() != 7)
		{
			os::Printer::log("Binary X: MeshTextureCoords: Expecting float list", ELL_WARNING);
			return false;
		}
		count = readBinDWord();
	}
	textureCoords.set_used(nCoords);

	for (s32 i=0; i<nCoords; ++i)
		readVector2(textureCoords[i]);

	if (!checkForTwoFollowingSemicolons())
	{
		os::Printer::log("No finishing semicolon in Mesh Texture Coordinates Array found in x file", ELL_WARNING);
		return false;
	}

	if (getNextToken() != "}")
	{
		os::Printer::log("No closing brace in Mesh Texture Coordinates Array found in x file", ELL_WARNING);
		return false;
	}

	return true;
}



bool CXFileReader::parseDataObjectMeshVertexColors(
	core::array<SXIndexedColor>& vertexColors)
{
#ifdef _XREADER_DEBUG
	os::Printer::log("CXFileReader: reading mesh vertex colors");
#endif

	if (!readHeadOfDataObject())
	{
		os::Printer::log("No opening brace for Mesh Vertex Colors found in x file", ELL_WARNING);
		return false;
	}

	s32 nColors;
	u32 count;
	nColors = readInt();
	if (binary)
	{
		if (readBinWord() != 7)
		{
			os::Printer::log("Binary X: MeshVertexColors: Expecting float list", ELL_WARNING);
			return false;
		}
		count = readBinDWord();
	}
	vertexColors.set_used(nColors);

	for (s32 i=0; i<nColors; ++i)
	{
		vertexColors[i].Index=readInt();
		readRGBA(vertexColors[i].Color);
	}

	if (getNextToken() != ";")
	{
		os::Printer::log("No finishing semicolon in Mesh Vertex Colors Array found in x file", ELL_WARNING);
		return false;
	}

	if (getNextToken() != "}")
	{
		os::Printer::log("No closing brace in Mesh Texture Coordinates Array found in x file", ELL_WARNING);
		return false;
	}

	return true;
}



bool CXFileReader::parseUnknownDataObject()
{
	// find opening delimiter
	while(true)
	{
		core::stringc t = getNextToken();

		if (t.size() == 0)
			return false;

		if (t == "{")
			break;
	}

	s32 counter = 1;

	// parse until closing delimiter

	while(counter)
	{
		core::stringc t = getNextToken();

		if (t.size() == 0)
			return false;

		if (t == "{")
			++counter;
		else
		if (t == "}")
			--counter;
	}

	return true;
}



//! checks for two following semicolons, returns false if they are not there
bool CXFileReader::checkForTwoFollowingSemicolons()
{
	if (binary)
		return true;

	for (s32 k=0; k<2; ++k)
	{
		findNextNoneWhiteSpace();
		if (P[0] != ';')
			return false;
		++P;
	}

	return true;
}


//! reads header of dataobject including the opening brace.
//! returns false if error happened, and writes name of object
//! if there is one
bool CXFileReader::readHeadOfDataObject(core::stringc* outname)
{
	core::stringc nameOrBrace = getNextToken();
	if (nameOrBrace != "{")
	{
		if (outname)
			(*outname) = nameOrBrace;

		if (nameOrBrace.size() != 0 &&
			nameOrBrace[nameOrBrace.size()-1] == '{')
		{
			(*outname) = nameOrBrace.subString(0, nameOrBrace.size()-1);
			return true;
		}


		if (getNextToken() != "{")
			return false;
	}

	return true;
}


//! returns next parseable token. Returns empty string if no token there
core::stringc CXFileReader::getNextToken()
{
	core::stringc s;

	// process binary-formatted file
	if (binary)
	{
		// in binary mode it will only return NAME and STRING token
		// and (correctly) skip over other tokens.

		s16 tok = readBinWord();
		s32 len;

		// standalone tokens
		switch (tok) {
			case 1:
				// name token
				len = readBinDWord();
				s = core::stringc(P, len);
				P += len;
				return s;
			case 2:
				// string token
				len = readBinDWord();
				s = core::stringc(P, len);
				P += (len + 2);
				return s;
			case 3:
				// integer token
				P += 4;
				return "<integer>";
			case 5:
				// GUID token
				P += 16;
				return "<guid>";
			case 6:
				len = readBinDWord();
				P += (len * 4);
				return "<int_list>";
			case 7:
				len = readBinDWord();
				P += (len * FloatSize);
				return "<flt_list>";
			case 0x0a:
				return "{";
			case 0x0b:
				return "}";
			case 0x0c:
				return "(";
			case 0x0d:
				return ")";
			case 0x0e:
				return "[";
			case 0x0f:
				return "]";
			case 0x10:
				return "<";
			case 0x11:
				return ">";
			case 0x12:
				return ".";
			case 0x13:
				return ",";
			case 0x14:
				return ";";
			case 0x1f:
				return "template";
			case 0x28:
				return "WORD";
			case 0x29:
				return "DWORD";
			case 0x2a:
				return "FLOAT";
			case 0x2b:
				return "DOUBLE";
			case 0x2c:
				return "CHAR";
			case 0x2d:
				return "UCHAR";
			case 0x2e:
				return "SWORD";
			case 0x2f:
				return "SDWORD";
			case 0x30:
				return "void";
			case 0x31:
				return "string";
			case 0x32:
				return "unicode";
			case 0x34:
				return "array";
		}
	}
	// process text-formatted file
	else
	{
		findNextNoneWhiteSpace();

		if (P >= End)
			return s;

		while(P < End && !core::isspace(P[0]))
		{
			s.append(P[0]);
			++P;
		}
	}
	return s;
}


//! places pointer to next begin of a token, which must be a number,
// and ignores comments
void CXFileReader::findNextNoneWhiteSpaceNumber()
{
	if (binary)
		return;

	while(true)
	{
		while((P < End) && (P[0] != '-') && (P[0] != '.') &&
			!( core::isdigit(P[0])))
			++P;

		if (P >= End)
			return;

		// check if this is a comment
		if ((P[0] == '/' && P[1] == '/') || P[0] == '#')
			readUntilEndOfLine();
		else
			break;
	}

}

// places pointer to next begin of a token, and ignores comments
void CXFileReader::findNextNoneWhiteSpace()
{
	if (binary)
		return;

	while(true)
	{
		while(P < End && (P[0]==' ' || P[0]=='\n' || P[0]=='\r' || P[0]=='\t'))
			++P;

		if (P >= End)
			return;

		// check if this is a comment
		if ((P[0] == '/' && P[1] == '/') ||
			P[0] == '#')
			readUntilEndOfLine();
		else
			break;
	}
}


void CXFileReader::readUntilEndOfLine()
{
	if (binary)
		return;

	while(P < End)
	{
		if (P[0] == '\n')
		{
			++P;
			return;
		}

		++P;
	}
}


//! Returns if the loaded mesh is static
bool CXFileReader::isStaticMesh() const
{
	return AnimationSets.empty();
}

//! returns count of animations
s32 CXFileReader::getAnimationSetCount() const
{
	return AnimationSets.size();
}

//! returns a specific animation set
CXFileReader::SXAnimationSet& CXFileReader::getAnimationSet(s32 i)
{
	return AnimationSets[i];
}

//! returns the root frame of the mesh
core::array<CXFileReader::SXFrame> & CXFileReader::getRootFrames()
{
	return RootFrames;
}


void CXFileReader::computeGlobalFrameMatrices(SXFrame& frame, const SXFrame* const parent)
{
	if (!parent)
		frame.GlobalMatrix = frame.LocalMatrix;
	else
		frame.GlobalMatrix = parent->GlobalMatrix * frame.LocalMatrix;

#ifdef _XREADER_DEBUG
	char tmp[255];
	sprintf(tmp, "CXFileReader: Frame %s ", frame.Name.c_str());
	os::Printer::log(tmp);
	for (int i=0; i<4; ++i)
	{
		sprintf(tmp, "    %f, %f, %f, %f",
			frame.LocalMatrix(i,0),
			frame.LocalMatrix(i,1),
			frame.LocalMatrix(i,2),
			frame.LocalMatrix(i,3));
		os::Printer::log(tmp);
	}
#endif
	for (u32 c=0; c<frame.ChildFrames.size(); ++c)
		computeGlobalFrameMatrices(frame.ChildFrames[c], &frame);
}

inline u16 CXFileReader::readBinWord()
{
	u8 *Q = (u8 *)P;
	u16 tmp = 0;
	tmp = Q[0] + (Q[1] << 8);
	P += 2;
	return tmp;
}

inline u32 CXFileReader::readBinDWord()
{
	u8 *Q = (u8 *)P;
	u32 tmp = 0;
	tmp = Q[0] + (Q[1] << 8) + (Q[2] << 16) + (Q[3] << 24);
	P += 4;
	return tmp;
}

inline s32 CXFileReader::readInt()
{
	if (binary)
	{
		if (!binaryNumCount)
		{
			readBinWord(); // 0x06
			binaryNumCount=readBinDWord(); // 0x0001
		}
		--binaryNumCount;
		return readBinDWord();
	}
	else
	{
		f32 ftmp;
		findNextNoneWhiteSpaceNumber();
		P = core::fast_atof_move(P, ftmp);
		return (s32)ftmp;
	}
}

inline f32 CXFileReader::readFloat()
{
	if (binary)
	{
		if (FloatSize == 8)
		{
			char tmp[8];
			memcpy(tmp, P, 8);
			P += 8;
			return (f32)(*(f64 *)tmp);
		}
		else
		{
			char tmp[4];
			memcpy(tmp, P, 4);
			P += 4;
			return *(f32 *)tmp;
		}
	}
	findNextNoneWhiteSpaceNumber();
	f32 ftmp;
	P = core::fast_atof_move(P, ftmp);
	return ftmp;
}

// read 2-dimensional vector. Stops at semicolon after second value for text file format
inline bool CXFileReader::readVector2(core::vector2df& vec)
{
	vec.X = readFloat();
	vec.Y = readFloat();
	return true;
}

// read 3-dimensional vector. Stops at semicolon after third value for text file format
inline bool CXFileReader::readVector3(core::vector3df& vec)
{
	vec.X = readFloat();
	vec.Y = readFloat();
	vec.Z = readFloat();
	return true;
}

// read color without alpha value. Stops after second semicolon after blue value
inline bool CXFileReader::readRGB(video::SColorf& color)
{
	color.r = readFloat();
	color.g = readFloat();
	color.b = readFloat();
	color.a = 1.0f;
	return checkForTwoFollowingSemicolons();
}

// read color with alpha value. Stops after second semicolon after blue value
inline bool CXFileReader::readRGBA(video::SColorf& color)
{
	color.r = readFloat();
	color.g = readFloat();
	color.b = readFloat();
	color.a = readFloat();
	return checkForTwoFollowingSemicolons();
}

} // end namespace scene
} // end namespace irr

