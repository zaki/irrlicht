// Copyright (C) 2002-2007 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "IrrCompileConfig.h"

#ifdef _IRR_COMPILE_WITH_X_LOADER_

#include "CXMeshFileLoader.h"
#include "os.h"

#include "fast_atof.h"
#include "coreutil.h"

namespace irr
{
namespace scene
{

//! Constructor
CXMeshFileLoader::CXMeshFileLoader(scene::ISceneManager* smgr)
: SceneManager(smgr), Buffers(0), AllJoints(0), AnimatedMesh(0), file(0),
	MajorVersion(0), MinorVersion(0), binary(false), binaryNumCount(0),
	Buffer(0), Size(0), FloatSize(0), P(0), End(0), ErrorHappened(false),
	CurFrame(0)
{
}


//! destructor
CXMeshFileLoader::~CXMeshFileLoader()
{
	TemplateMaterials.clear();
}


//! returns true if the file maybe is able to be loaded by this class
//! based on the file extension (e.g. ".bsp")
bool CXMeshFileLoader::isALoadableFileExtension(const c8* filename)
{
	return strstr(filename, ".x") != 0;
}


//! creates/loads an animated mesh from the file.
//! \return Pointer to the created mesh. Returns 0 if loading failed.
//! If you no longer need the mesh, you should call IAnimatedMesh::drop().
//! See IUnknown::drop() for more information.
IAnimatedMesh* CXMeshFileLoader::createMesh(irr::io::IReadFile* f)
{
	if (!f)
		return 0;

	file = f;
	AnimatedMesh = new CSkinnedMesh();

	Buffers = &AnimatedMesh->getMeshBuffers();
	AllJoints = &AnimatedMesh->getAllJoints();

	if ( load() )
	{
		AnimatedMesh->finalize();
	}
	else
	{
		AnimatedMesh->drop();
		AnimatedMesh = 0;
	}

	//Clear up

	file=0;
	MajorVersion=0;
	MinorVersion=0;
	binary=0;
	binaryNumCount=0;
	Buffer=0;
	Size=0;
	FloatSize=0;
	P=0;
	End=0;
	ErrorHappened=0;
	CurFrame=0;
	TemplateMaterials.clear();

	Meshes.clear();

	return AnimatedMesh;
}


bool CXMeshFileLoader::load()
{
	if (!readFileIntoMemory())
		return false;

	if (!parseFile())
		return false;

	for (u32 n=0;n<Meshes.size();++n)
	{
		SXMesh *Mesh=Meshes[n];

		if (!Mesh->Materials.size()) Mesh->Materials.push_back(video::SMaterial());

		u32 i;

		for (i=0;i<Mesh->Materials.size();++i)
		{
			Mesh->Buffers.push_back( AnimatedMesh->createBuffer() );
			Mesh->Buffers.getLast()->Material=Mesh->Materials[i];
		}


		#ifdef BETTER_MESHBUFFER_SPLITTING_FOR_X

			//the same vertex can be used in many different meshbuffers, but it's slow to work out

			core::array< core::array< u32 > > VerticesLink;
			core::array< core::array< u32 > > VerticesLinkBuffer;

			for (i=0;i<Mesh->Vertices.size();++i)
			{
				VerticesLinkBuffer.push_back( core::array< u32 >() );
				VerticesLink.push_back( core::array< u32 >() );
			}

			for (i=0;i<Mesh->FaceIndices.size();++i)
			{
				for (u32 id=i*3+0;id<=i*3+2;++id)
				{
					core::array< u32 > &Array=VerticesLinkBuffer[ Mesh->Indices[id] ];
					bool found=false;

					for (u32 j=0;j<  Array.size() ;++j)
						if (Array[j]==Mesh->FaceIndices[i]) found=true;

					if (!found)
						Array.push_back( Mesh->FaceIndices[i] );
				}
			}


			for (i=0;i<VerticesLinkBuffer.size();++i)
			{
				if (!VerticesLinkBuffer[i].size())
					VerticesLinkBuffer[i].push_back(0);
			}

			for (i=0;i<Mesh->Vertices.size();++i)
			{
				core::array< u32 > &Array=VerticesLinkBuffer[i];
				for (u32 j=0;j < Array.size(); ++j)
				{
					scene::SSkinMeshBuffer *Buffer=Mesh->Buffers[ Array[j] ];
					VerticesLink[i].push_back( Buffer->Vertices_Standard.size() );
					Buffer->Vertices_Standard.push_back(  Mesh->Vertices[i] );
				}
			}

			for (i=0;i<Mesh->FaceIndices.size();++i)
			{
				scene::SSkinMeshBuffer *Buffer=Mesh->Buffers[ Mesh->FaceIndices[i] ];

				for (u32 id=i*3+0;id<=i*3+2;++id)
				{
					core::array< u32 > &Array=VerticesLinkBuffer[ Mesh->Indices[id] ];

					for (u32 j=0;j<  Array.size() ;++j)
					{
						if ( Array[j]== Mesh->FaceIndices[i] )
							Buffer->Indices.push_back( VerticesLink[ Mesh->Indices[id] ][j] );
					}
				}
			}

			for (i=0;i<AnimatedMesh->getAllJoints().size();++i)
			{

				ISkinnedMesh::SJoint *Joint=AnimatedMesh->getAllJoints()[i];

				for (u32 j=0;j<Joint->Weights.size();++j)
				{
					u32 id;

					ISkinnedMesh::SWeight *Weight=&Joint->Weights[j];
					id=Weight->vertex_id;

					if (VerticesLinkBuffer[id].size()==1)
					{
						Weight->vertex_id=VerticesLink[id][0];
						Weight->buffer_id=VerticesLinkBuffer[id][0];
					}

					if (VerticesLinkBuffer[id].size()>1)
					{
						for (u32 k=1;k<  VerticesLinkBuffer[id].size() ;++k)
						{
							ISkinnedMesh::SWeight *WeightClone=AnimatedMesh->createWeight(Joint);
							WeightClone->strength=Weight->strength;
							WeightClone->vertex_id=VerticesLink[id][k];
							WeightClone->buffer_id=VerticesLinkBuffer[id][k];
						}
					}

				}

			}
		#else

			core::array< u32 > VerticesLink;
			core::array< u32 > VerticesLinkBuffer;

			VerticesLinkBuffer.set_used(Mesh->Vertices.size());
			VerticesLink.set_used(Mesh->Vertices.size());

			for (i=0;i<Mesh->Vertices.size();++i)
			{
				VerticesLinkBuffer[i]=0;
				VerticesLink[i]=0;
			}

			for (i=0;i<Mesh->FaceIndices.size();++i)
			{
				for (u32 id=i*3+0;id<=i*3+2;++id)
				{
					VerticesLinkBuffer[ Mesh->Indices[id] ] = Mesh->FaceIndices[i];
				}
			}


			for (i=0;i<Mesh->Vertices.size();++i)
			{

				scene::SSkinMeshBuffer *Buffer=Mesh->Buffers[ VerticesLinkBuffer[i] ];

				VerticesLink[i] = Buffer->Vertices_Standard.size();
				Buffer->Vertices_Standard.push_back(  Mesh->Vertices[i] );

			}

			for (i=0;i<Mesh->FaceIndices.size();++i)
			{
				scene::SSkinMeshBuffer *Buffer=Mesh->Buffers[ Mesh->FaceIndices[i] ];

				for (u32 id=i*3+0;id<=i*3+2;++id)
				{
					Buffer->Indices.push_back( VerticesLink[ Mesh->Indices[id] ] );
				}
			}

			for (i=0;i<AnimatedMesh->getAllJoints().size();++i)
			{

				ISkinnedMesh::SJoint *Joint=AnimatedMesh->getAllJoints()[i];

				for (u32 j=0;j<Joint->Weights.size();++j)
				{
					u32 id;

					ISkinnedMesh::SWeight *Weight=&Joint->Weights[j];

					id=Weight->vertex_id;

					Weight->vertex_id=VerticesLink[id];
					Weight->buffer_id=VerticesLinkBuffer[id];
				}
			}
		#endif
	}

	return true;
}



//! Reads file into memory
bool CXMeshFileLoader::readFileIntoMemory()
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
bool CXMeshFileLoader::parseFile()
{
	while(parseDataObject())
	{
		// loop
	}

	return true;
}


//! Parses the next Data object in the file
bool CXMeshFileLoader::parseDataObject()
{
	core::stringc objectName = getNextToken();

	if (objectName.size() == 0)
		return false;

	// parse specific object
#ifdef _DEBUG
	os::Printer::log("debug DataObject:", objectName.c_str() );
#endif

	if (objectName == "template")
		return parseDataObjectTemplate();
	else
	if (objectName == "Frame")
	{

		return parseDataObjectFrame( 0 );
	}
	else
	if (objectName == "Mesh")
	{
		// some meshes have no frames at all
		//CurFrame = AnimatedMesh->createJoint(0);

		//CurFrame->Meshes.push_back(SXMesh());
		//return parseDataObjectMesh(CurFrame->Meshes.getLast());

		SXMesh *Mesh=new SXMesh;

		//Mesh->Buffer=AnimatedMesh->createBuffer();
		Meshes.push_back(Mesh);


		return parseDataObjectMesh ( *Mesh );
	}
	else
	if (objectName == "AnimationSet")
	{
		return parseDataObjectAnimationSet();
	}
	else
	if (objectName == "Material")
	{
		// template materials now available thanks to joeWright
		TemplateMaterials.push_back(SXTemplateMaterial());
		TemplateMaterials.getLast().Name = getNextToken();
		return parseDataObjectMaterial(TemplateMaterials.getLast().Material);
	}
	else
	if (objectName == "}")
	{
		os::Printer::log("} found in dataObject", ELL_WARNING);
		return true;
	}

	os::Printer::log("Unknown data object in x file", objectName.c_str(), ELL_WARNING);

	return parseUnknownDataObject();
}


bool CXMeshFileLoader::parseDataObjectTemplate()
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



bool CXMeshFileLoader::parseDataObjectFrame( CSkinnedMesh::SJoint *Parent )
{
#ifdef _XREADER_DEBUG
	os::Printer::log("CXFileReader: Reading frame");
#endif

	// A coordinate frame, or "frame of reference." The Frame template
	// is open and can contain any object. The Direct3D extensions (D3DX)
	// mesh-loading functions recognize Mesh, FrameTransformMatrix, and
	// Frame template instances as child objects when loading a Frame
	// instance.

	core::stringc Name;

	if (!readHeadOfDataObject(&Name))
	{
		os::Printer::log("No opening brace in Frame found in x file", ELL_WARNING);
		return false;
	}

	CSkinnedMesh::SJoint *joint=0;

	if (Name!="")
	{
		for (u32 n=0;n < AnimatedMesh->getAllJoints().size();++n)
		{
			if (AnimatedMesh->getAllJoints()[n]->Name==Name)
				joint=AnimatedMesh->getAllJoints()[n];
		}
	}

	if (!joint)
	{
#ifdef _DEBUG
		os::Printer::log("creating joint ", Name.c_str());
#endif
		joint=AnimatedMesh->createJoint(Parent);
		joint->Name=Name;
	}
	else
	{
#ifdef _DEBUG
		os::Printer::log("using joint ", Name.c_str());
#endif
		if (Parent)
			Parent->Children.push_back(joint);
	}

	// Now inside a frame.
	// read tokens until closing brace is reached.

	while(true)
	{
		core::stringc objectName = getNextToken();

#ifdef _DEBUG
		os::Printer::log("debug DataObject in frame:", objectName.c_str() );
#endif

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

			if (!parseDataObjectFrame(joint))
				return false;
		}
		else
		if (objectName == "FrameTransformMatrix")
		{
			//if (!parseDataObjectTransformationMatrix(joint->LocalMatrix))

			if (!parseDataObjectTransformationMatrix(joint->LocalMatrix))
				return false;

			//joint->LocalAnimatedMatrix


			//joint->LocalAnimatedMatrix.makeInverse();

			//joint->LocalMatrix=tmp*joint->LocalAnimatedMatrix;


		}
		else
		if (objectName == "Mesh")
		{
			/*
			frame.Meshes.push_back(SXMesh());
			if (!parseDataObjectMesh(frame.Meshes.getLast()))
				return false;
			*/
			SXMesh *Mesh=new SXMesh;

			Meshes.push_back(Mesh);

			return parseDataObjectMesh ( *Mesh );
		}
		else
		{
			os::Printer::log("Unknown data object in frame in x file", objectName.c_str(), ELL_WARNING);
			if (!parseUnknownDataObject())
				return false;
		}

	}

	return true;
}


bool CXMeshFileLoader::parseDataObjectTransformationMatrix(core::matrix4 &mat)
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



bool CXMeshFileLoader::parseDataObjectMesh(SXMesh &mesh)
{
#ifdef _XREADER_DEBUG
	os::Printer::log("CXFileReader: Reading mesh");
#endif

	core::stringc name;

	if (!readHeadOfDataObject(&name))
	{
		os::Printer::log("No opening brace in Mesh found in x file", ELL_WARNING);
		return false;
	}

	// read vertex count
	s32 nVertices = readInt();


	// read vertices
	mesh.Vertices.set_used(nVertices); //luke: change
	for (s32 nums=0; nums<nVertices; ++nums)
		mesh.Vertices[nums].Color=0xFFFFFFFF;


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
	{
		readVector3(mesh.Vertices[n].Pos);
	}


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

#ifdef _DEBUG
		os::Printer::log("debug DataObject in mesh:", objectName.c_str() );
#endif

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
			if (!parseDataObjectMeshNormals(mesh))
				return false;
		}
		else
		if (objectName == "MeshTextureCoords")
		{
			if (!parseDataObjectMeshTextureCoords(mesh))
				return false;
		}
		else
		if (objectName == "MeshVertexColors")
		{
			if (!parseDataObjectMeshVertexColors(mesh))
				return false;
		}
		else
		if (objectName == "MeshMaterialList")
		{
			if (!parseDataObjectMeshMaterialList(mesh))
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
			if (!parseDataObjectSkinMeshHeader())
				return false;
		}
		else
		if (objectName == "SkinWeights")
		{
			//mesh.SkinWeights.push_back(SXSkinWeight());
			//if (!parseDataObjectSkinWeights(mesh.SkinWeights.getLast()))
			if (!parseDataObjectSkinWeights(mesh))
				return false;
		}
		else
		{
			os::Printer::log("Unknown data object in mesh in x file", objectName.c_str(), ELL_WARNING);
			if (!parseUnknownDataObject())
				return false;
		}
	}

	return true;
}


bool CXMeshFileLoader::parseDataObjectSkinWeights(SXMesh &mesh)
{
#ifdef _XREADER_DEBUG
	os::Printer::log("CXFileReader: Reading mesh skin weights");
#endif

	if (!readHeadOfDataObject())
	{
		os::Printer::log("No opening brace in Skin Weights found in x file", ELL_WARNING);
		return false;
	}

	core::stringc TransformNodeName;

	if (!getNextTokenAsString(TransformNodeName))
	{
		os::Printer::log("Unknown syntax while reading transfrom node name string in x file", ELL_WARNING);
		return false;
	}

	CSkinnedMesh::SJoint *joint=0;

	for (u32 n=0;n < AnimatedMesh->getAllJoints().size();++n)
	{
		if (AnimatedMesh->getAllJoints()[n]->Name==TransformNodeName)
			joint=AnimatedMesh->getAllJoints()[n];
	}

	if (!joint)
	{
		//os::Printer::log("no joints with correct name for weights,", TransformNodeName.c_str());
		//return false;

#ifdef _XREADER_DEBUG
		os::Printer::log("pre-creating joint for skinning ", TransformNodeName.c_str());
#endif
		joint=AnimatedMesh->createJoint(0);
		joint->Name=TransformNodeName;
	}


	core::array<u32> Weights_Index;
	core::array<f32> Weights_Strength;

	// read vertex weights
	s32 nWeights = readInt();

	Weights_Index.set_used(nWeights);
	Weights_Strength.set_used(nWeights);

	// read vertex indices

	s32 i;

	for (i=0; i<nWeights; ++i)
		Weights_Index[i] = readInt();

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
		Weights_Strength[i] = readFloat();

	// sort weights

	//weights.Weights.set_sorted(false);
	//weights.Weights.sort();

	// read matrix offset

	core::matrix4 MatrixOffset; // transforms the mesh vertices to the space of the bone
					// When concatenated to the bone's transform, this provides the
					// world space coordinates of the mesh as affected by the bone



	for (i=0; i<4; ++i)
	{
		for (u32 j=0; j<4; ++j)
			MatrixOffset(i,j) = readFloat();
	}

	joint->GlobalInversedMatrix=MatrixOffset;

	for (i=0; i<nWeights; ++i)
	{
		CSkinnedMesh::SWeight *weight=AnimatedMesh->createWeight(joint);

		weight->buffer_id=0;
		weight->vertex_id=Weights_Index[i];
		weight->strength=Weights_Strength[i];
	}

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


bool CXMeshFileLoader::parseDataObjectSkinMeshHeader()
{
#ifdef _XREADER_DEBUG
	os::Printer::log("CXFileReader: Reading skin mesh header");
#endif

	if (!readHeadOfDataObject())
	{
		os::Printer::log("No opening brace in Skin Mesh header found in x file", ELL_WARNING);
		return false;
	}

	readInt(); //MaxSkinWeightsPerVertex
	readInt(); //MaxSkinWeightsPerFace
	readInt(); //BoneCount
	if (!binary)
		getNextToken(); // skip semicolon
	core::stringc objectName = getNextToken();

	if (objectName != "}")
	{
		os::Printer::log("No closing brace in skin mesh header in x file", objectName.c_str(), ELL_WARNING);
		return false;
	}

	return true;
}


bool CXMeshFileLoader::parseDataObjectMeshNormals(SXMesh &mesh)
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
	//normals.set_used(nNormals);

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
		readVector3(mesh.Vertices[i].Normal);

	if (!checkForTwoFollowingSemicolons())
	{
		os::Printer::log("No finishing semicolon in Mesh Normals Array found in x file", ELL_WARNING);
		return false;
	}

	core::array<s32> normalIndices;

	s32 triangulatedIndexCount=mesh.Indices.size();

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

		if (indexcount != mesh.IndexCountPerFace[k])
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


bool CXMeshFileLoader::parseDataObjectMeshTextureCoords(SXMesh &mesh)
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
	//textureCoords.set_used(nCoords);

	for (s32 i=0; i<nCoords; ++i)
		readVector2(mesh.Vertices[i].TCoords);

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




bool CXMeshFileLoader::parseDataObjectMeshVertexColors(SXMesh &mesh)
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
	//vertexColors.set_used(nColors);

	for (s32 i=0; i<nColors; ++i)
	{
		u32 Index=readInt();
		if (Index>=mesh.Vertices.size() )
			{
				os::Printer::log("index value in parseDataObjectMeshVertexColors out of bounds", ELL_WARNING);
				return false;
			}
		readRGBA(mesh.Vertices[i].Color);
	}

	core::stringc tmp=getNextToken();
	if (tmp != ";")
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



bool CXMeshFileLoader::parseDataObjectMeshMaterialList(SXMesh &mesh)
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

	if (nFaceIndices != (s32)mesh.IndexCountPerFace.size())
	{
		os::Printer::log("Index count per face not equal to face material index count in x file.", ELL_WARNING);
		return false;
	}

	mesh.FaceIndices.set_used( mesh.Indices.size() / 3);
	s32 triangulatedindex = 0;
	for (s32 tfi=0; tfi<nFaceIndices; ++tfi)
		for (s32 k=0; k<mesh.IndexCountPerFace[tfi]/3; ++k)
			mesh.FaceIndices[triangulatedindex++] = nonTriFaceIndices[tfi];

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
					mesh.Materials.push_back(TemplateMaterials[i].Material);
			getNextToken(); // skip }
		}
		else
		if (objectName == "Material")
		{
			mesh.Materials.push_back(video::SMaterial());
			if (!parseDataObjectMaterial(mesh.Materials.getLast()))
				return false;
		}
		else
		if (objectName == ";")
		{
			// ignore
		}
		else
		{
			os::Printer::log("Unknown data object in material list in x file", objectName.c_str(), ELL_WARNING);
			if (!parseUnknownDataObject())
				return false;
		}
	}
	return true;
}



bool CXMeshFileLoader::parseDataObjectMaterial(video::SMaterial& material)
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
	readRGBA(material.DiffuseColor); checkForOneFollowingSemicolons();

	// read power
	//material.Power = readFloat();
	readFloat();

	// read specular
	readRGB(material.SpecularColor); checkForOneFollowingSemicolons();

	// read emissive
	readRGB(material.EmissiveColor); checkForOneFollowingSemicolons();

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
			core::stringc TextureFileName;
			if (!parseDataObjectTextureFilename(TextureFileName))
				return false;

			TextureFileName=stripPathFromString(file->getFileName(),true) + stripPathFromString(TextureFileName,false);

			material.Textures[0]=SceneManager->getVideoDriver()->getTexture ( TextureFileName.c_str() );
		}
		else
		{
			os::Printer::log("Unknown data object in material in x file", objectName.c_str(), ELL_WARNING);
			if (!parseUnknownDataObject())
				return false;
		}

	}

	return true;
}


bool CXMeshFileLoader::parseDataObjectAnimationSet()
{
	#ifdef _XREADER_DEBUG
	os::Printer::log("CXFileReader: Reading animation set");
	#endif

	os::Printer::log("parseDataObjectAnimationSet()", ELL_WARNING);

	core::stringc AnimationName;

	if (!readHeadOfDataObject(&AnimationName))
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
			if (!parseDataObjectAnimation())
				return false;
		}
		else
		{
			os::Printer::log("Unknown data object in animation set in x file", objectName.c_str(), ELL_WARNING);
			if (!parseUnknownDataObject())
				return false;
		}
	}
	return true;
}


bool CXMeshFileLoader::parseDataObjectAnimation()
{
#ifdef _XREADER_DEBUG
	os::Printer::log("CXFileReader: reading animation");
#endif

	if (!readHeadOfDataObject())
	{
		os::Printer::log("No opening brace in Animation found in x file", ELL_WARNING);
		return false;
	}

	//anim.closed = true;
	//anim.linearPositionQuality = true;
	CSkinnedMesh::SJoint *joint=0;

	CSkinnedMesh::SJoint animationDump;

	core::stringc FrameName="";

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
			if (!joint)
			{
				os::Printer::log("no joint to write animation to, dumping in temp joint", ELL_WARNING);

				if (!parseDataObjectAnimationKey(&animationDump))
					return false;
			}
			else
			{
				if (!parseDataObjectAnimationKey(joint))
					return false;
			}
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

			FrameName = getNextToken();

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
				FrameName = objectName.subString(1,objectName.size()-2);
			}
			else
			{
				os::Printer::log("Unknown data object in animation in x file", objectName.c_str(), ELL_WARNING);
				if (!parseUnknownDataObject())
					return false;
			}
		}

		if (FrameName!="" && !joint)
		{
#ifdef _DEBUG
			os::Printer::log("getting name: ", FrameName.c_str());
#endif
			for (u32 n=0;n < AnimatedMesh->getAllJoints().size();++n)
			{
				if (AnimatedMesh->getAllJoints()[n]->Name==FrameName)
					joint=AnimatedMesh->getAllJoints()[n];
			}
			if (!joint)
			{
				//os::Printer::log("no joints with correct name for animation,", FrameName.c_str());
				//return false;

				os::Printer::log("pre-creating joint for animation ", FrameName.c_str(), ELL_WARNING);
				joint=AnimatedMesh->createJoint(0);
				joint->Name=FrameName;
			}

			u32 n;

			for (n=0;n<animationDump.PositionKeys.size();++n)
			{
				ISkinnedMesh::SPositionKey *key=&animationDump.PositionKeys[n];

				//key->position+=joint->LocalMatrix.getTranslation();

				joint->PositionKeys.push_back(*key);
			}

			for (n=0;n<animationDump.ScaleKeys.size();++n)
			{
				ISkinnedMesh::SScaleKey *key=&animationDump.ScaleKeys[n];

				//key->scale*=joint->LocalMatrix.getScale();

				joint->ScaleKeys.push_back(*key);
			}

			for (n=0;n<animationDump.RotationKeys.size();++n)
			{
				ISkinnedMesh::SRotationKey *key=&animationDump.RotationKeys[n];


				core::matrix4 tmpMatrix;

				tmpMatrix.setRotationRadians(
					core::vector3df(key->rotation.X, key->rotation.Y, key->rotation.Z) );

				tmpMatrix=joint->LocalMatrix*tmpMatrix;

				//key->rotation  = core::quaternion(tmpMatrix);

				joint->RotationKeys.push_back(*key);
			}
		}
	}

	if (FrameName=="")
		os::Printer::log("joint name was never given", ELL_WARNING);

	return true;
}


bool CXMeshFileLoader::parseDataObjectAnimationKey(ISkinnedMesh::SJoint *joint)
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

	s32 keyType = readInt();

	if ((keyType < 0) || (keyType > 4))
	{
		os::Printer::log("Unknown key type found in Animation Key in x file", ELL_WARNING);
		return false;
	}

	// read number of keys
	s32 numberOfKeys = readInt();

	// eat the semicolon after the "0".  if there are keys present, readInt()
	// does this for us.  If there aren't, we need to do it explicitly
	if (!binary && numberOfKeys == 0)
		getNextToken(); // skip semicolon


	for (s32 i=0; i<numberOfKeys; ++i)
	{

		// read keys
		switch(keyType)
		{
		case 0: //rotation
			{
				//read quaternions

				// read time
				s32 time = readInt();

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
				f32 W = -readFloat();
				f32 X = -readFloat();
				f32 Y = -readFloat();
				f32 Z = -readFloat();

				if (!checkForTwoFollowingSemicolons())
				{
					os::Printer::log("No finishing semicolon after quaternion animation key in x file", ELL_WARNING);
					return false;
				}


				ISkinnedMesh::SRotationKey *key=AnimatedMesh->createRotationKey(joint);
				key->frame=(f32)time;
				key->rotation.set(X,Y,Z,W);
			}
			break;
		case 1: //scale
		case 2: //position
			{
				// read vectors

					// read time
					s32 time = readInt();

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

					core::vector3df vector;
					readVector3(vector);

					if (!checkForTwoFollowingSemicolons())
					{
						os::Printer::log("No finishing semicolon after vector animation key in x file", ELL_WARNING);
						return false;
					}

					if (keyType==2)
					{
						ISkinnedMesh::SPositionKey *key=AnimatedMesh->createPositionKey(joint);
						key->frame=(f32)time;
						key->position=vector;
					}
					else
					{
						ISkinnedMesh::SScaleKey *key=AnimatedMesh->createScaleKey(joint);
						key->frame=(f32)time;
						key->scale=vector;
					}
			}
			break;
		case 3:
		case 4:
			{
				// read matrix

				// read time
				s32 time = readInt();

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

				core::matrix4 Matrix;

				for (s32 m=0; m<4; ++m)
					for (s32 n=0; n<4; ++n)
						Matrix(m,n) = readFloat();


				//Matrix=joint->LocalMatrix*Matrix;

				if (!checkForTwoFollowingSemicolons())
				{
					os::Printer::log("No finishing semicolon after matrix animation key in x file", ELL_WARNING);
					return false;
				}

				//core::vector3df rotation = Matrix.getRotationDegrees();

				ISkinnedMesh::SRotationKey *keyR=AnimatedMesh->createRotationKey(joint);
				keyR->frame=(f32)time;
				//keyR->rotation.set(rotation.X*core::DEGTORAD,rotation.Y*core::DEGTORAD,rotation.Z*core::DEGTORAD);
				keyR->rotation= core::quaternion(Matrix);


				ISkinnedMesh::SPositionKey *keyP=AnimatedMesh->createPositionKey(joint);
				keyP->frame=(f32)time;
				keyP->position=Matrix.getTranslation();

				core::vector3df scale=Matrix.getScale();

				//if (scale.X==0) scale.X=1;
				//if (scale.Y==0) scale.Y=1;
				//if (scale.Z==0) scale.Z=1;
/*
				ISkinnedMesh::SScaleKey *keyS=AnimatedMesh->createScaleKey(joint);
				keyS->frame=(f32)time;
				keyS->scale=scale;
*/

				//os::Printer::log("x ", core::stringc(Matrix.getScale().X).c_str());
				//os::Printer::log("y ", core::stringc(Matrix.getScale().Y).c_str());
				//os::Printer::log("z ", core::stringc(Matrix.getScale().Z).c_str());

			}
			break;
		} // end switch
	}

	if (!binary)
		getNextToken(); // skip another semicolon
	core::stringc objectName = getNextToken();

	if (objectName != "}")
	{
		os::Printer::log("No closing brace in animation key in x file", objectName.c_str(), ELL_WARNING);
		return false;
	}

	return true;
}


bool CXMeshFileLoader::parseDataObjectTextureFilename(core::stringc& texturename)
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


bool CXMeshFileLoader::parseUnknownDataObject()
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
bool CXMeshFileLoader::checkForOneFollowingSemicolons()
{
	if (binary)
		return true;

	findNextNoneWhiteSpace();
	if (P[0] != ';')
		return false;
	++P;

	return true;
}


//! checks for two following semicolons, returns false if they are not there
bool CXMeshFileLoader::checkForTwoFollowingSemicolons()
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
bool CXMeshFileLoader::readHeadOfDataObject(core::stringc* outname)
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
core::stringc CXMeshFileLoader::getNextToken()
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


		char last=0;

		//&& last!=';' && last!='}' && last!='{' && last!=','

		while(P < End && !core::isspace(P[0]) ) //Luke:Not only space?
		{
			last=P[0];

			s.append(P[0]);
			++P;
		}
	}
	return s;
}


//! places pointer to next begin of a token, which must be a number,
// and ignores comments
void CXMeshFileLoader::findNextNoneWhiteSpaceNumber()
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
void CXMeshFileLoader::findNextNoneWhiteSpace()
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


//! reads a x file style string
bool CXMeshFileLoader::getNextTokenAsString(core::stringc& out)
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


void CXMeshFileLoader::readUntilEndOfLine()
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


u16 CXMeshFileLoader::readBinWord()
{
	u8 *Q = (u8 *)P;
	u16 tmp = 0;
	tmp = Q[0] + (Q[1] << 8);
	P += 2;
	return tmp;
}


u32 CXMeshFileLoader::readBinDWord()
{
	u8 *Q = (u8 *)P;
	u32 tmp = 0;
	tmp = Q[0] + (Q[1] << 8) + (Q[2] << 16) + (Q[3] << 24);
	P += 4;
	return tmp;
}


s32 CXMeshFileLoader::readInt()
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


f32 CXMeshFileLoader::readFloat()
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
bool CXMeshFileLoader::readVector2(core::vector2df& vec)
{
	vec.X = readFloat();
	vec.Y = readFloat();
	return true;
}


// read 3-dimensional vector. Stops at semicolon after third value for text file format
bool CXMeshFileLoader::readVector3(core::vector3df& vec)
{
	vec.X = readFloat();
	vec.Y = readFloat();
	vec.Z = readFloat();
	return true;
}


// read color without alpha value. Stops after second semicolon after blue value
bool CXMeshFileLoader::readRGB(video::SColorf& color)
{
	color.r = readFloat();
	color.g = readFloat();
	color.b = readFloat();
	color.a = 1.0f;
	return checkForOneFollowingSemicolons();
}


// read color with alpha value. Stops after second semicolon after blue value
bool CXMeshFileLoader::readRGBA(video::SColorf& color)
{
	color.r = readFloat();
	color.g = readFloat();
	color.b = readFloat();
	color.a = readFloat();
	return checkForOneFollowingSemicolons();
}


// read color without alpha value. Stops after second semicolon after blue value
bool CXMeshFileLoader::readRGB(video::SColor& color)
{
	color.setRed( (u32)(readFloat()*255)) ;
	color.setGreen( (u32)(readFloat()*255)) ;
	color.setBlue( (u32)(readFloat()*255)) ;
	color.setAlpha( 255 );
	return checkForOneFollowingSemicolons();
}


// read color with alpha value. Stops after second semicolon after blue value
bool CXMeshFileLoader::readRGBA(video::SColor& color)
{
	color.setRed( (u32)(readFloat()*255)) ;
	color.setGreen( (u32)(readFloat()*255)) ;
	color.setBlue( (u32)(readFloat()*255)) ;
	color.setAlpha( (u32)(readFloat()*255)) ;
	return checkForOneFollowingSemicolons();
}


core::stringc CXMeshFileLoader::stripPathFromString(core::stringc string, bool returnPath)
{
	s32 slashIndex=string.findLast('/'); // forward slash
	s32 backSlash=string.findLast('\\'); // back slash

	if (backSlash>slashIndex) slashIndex=backSlash;

	if (slashIndex==-1)//no slashes found
		if (returnPath)
			return core::stringc(); //no path to return
		else
			return string;

	if (returnPath)
		return string.subString(0, slashIndex + 1);
	else
		return string.subString(slashIndex+1, string.size() - (slashIndex+1));
}


} // end namespace scene
} // end namespace irr

#endif // _IRR_COMPILE_WITH_X_LOADER_

