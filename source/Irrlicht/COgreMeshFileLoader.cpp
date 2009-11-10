// Copyright (C) 2002-2009 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h
// orginally written by Christian Stehno, modified by Nikolaus Gebhardt

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_OGRE_LOADER_

#include "COgreMeshFileLoader.h"
#include "os.h"
#include "SMeshBuffer.h"
#include "SAnimatedMesh.h"
#include "IReadFile.h"
#include "fast_atof.h"
#include "coreutil.h"

#ifdef _DEBUG
#define IRR_OGRE_LOADER_DEBUG
#endif

namespace irr
{
namespace scene
{

namespace
{
	enum OGRE_CHUNKS
	{
		// Main Chunks
		COGRE_HEADER= 0x1000,
		COGRE_SKELETON= 0x2000,
		COGRE_MESH= 0x3000,

		// sub chunks of COGRE_MESH
		COGRE_SUBMESH= 0x4000,
		COGRE_GEOMETRY= 0x5000,
		COGRE_SKELETON_LINK= 0x6000,
		COGRE_BONE_ASSIGNMENT= 0x7000,
		COGRE_MESH_LOD= 0x8000,
		COGRE_MESH_BOUNDS= 0x9000,
		COGRE_MESH_SUBMESH_NAME_TABLE= 0xA000,
		COGRE_MESH_EDGE_LISTS= 0xB000,

		// sub chunks of COGRE_SKELETON
		COGRE_BONE_PARENT= 0x3000,
		COGRE_ANIMATION= 0x4000,
		COGRE_ANIMATION_TRACK= 0x4100,
		COGRE_ANIMATION_KEYFRAME= 0x4110,
		COGRE_ANIMATION_LINK= 0x5000,

		// sub chunks of COGRE_SUBMESH
		COGRE_SUBMESH_OPERATION= 0x4010,
		COGRE_SUBMESH_BONE_ASSIGNMENT= 0x4100,
		COGRE_SUBMESH_TEXTURE_ALIAS= 0x4200,

		// sub chunks of COGRE_GEOMETRY
		COGRE_GEOMETRY_VERTEX_DECLARATION= 0x5100,
		COGRE_GEOMETRY_VERTEX_ELEMENT= 0x5110,
		COGRE_GEOMETRY_VERTEX_BUFFER= 0x5200,
		COGRE_GEOMETRY_VERTEX_BUFFER_DATA= 0x5210
	};
}

//! Constructor
COgreMeshFileLoader::COgreMeshFileLoader(io::IFileSystem* fs, video::IVideoDriver* driver)
: FileSystem(fs), Driver(driver), SwapEndian(false), Mesh(0), NumUV(0)
{

	#ifdef _DEBUG
	setDebugName("COgreMeshFileLoader");
	#endif

	if (FileSystem)
		FileSystem->grab();

	if (Driver)
		Driver->grab();
}


//! destructor
COgreMeshFileLoader::~COgreMeshFileLoader()
{
	clearMeshes();

	if (FileSystem)
		FileSystem->drop();

	if (Driver)
		Driver->drop();

	if (Mesh)
		Mesh->drop();
}


//! returns true if the file maybe is able to be loaded by this class
//! based on the file extension (e.g. ".bsp")
bool COgreMeshFileLoader::isALoadableFileExtension(const io::path& filename) const
{
	return core::hasFileExtension ( filename, "mesh" );
}


//! creates/loads an animated mesh from the file.
//! \return Pointer to the created mesh. Returns 0 if loading failed.
//! If you no longer need the mesh, you should call IAnimatedMesh::drop().
//! See IReferenceCounted::drop() for more information.
IAnimatedMesh* COgreMeshFileLoader::createMesh(io::IReadFile* file)
{
	s16 id;

	file->read(&id, 2);

	if (id == COGRE_HEADER)
		SwapEndian=false;
	else if (id == 0x0010)
		SwapEndian=true;
	else
		return 0;
	ChunkData data;
	readString(file, data, Version);
	if ((Version != "[MeshSerializer_v1.30]") && (Version != "[MeshSerializer_v1.40]"))
		return 0;

	clearMeshes();
	if (Mesh)
		Mesh->drop();

	CurrentlyLoadingFromPath = FileSystem->getFileDir(file->getFileName());
	loadMaterials(file);

	if (readChunk(file))
	{
		// delete data loaded from file
		clearMeshes();

		if (Skeleton.Bones.size())
		{
			ISkinnedMesh* tmp = static_cast<CSkinnedMesh*>(Mesh);
			static_cast<CSkinnedMesh*>(Mesh)->updateBoundingBox();
			Skeleton.Animations.clear();
			Skeleton.Bones.clear();
			Mesh=0;
			return tmp;
		}
		else
		{
			for (u32 i=0; i<Mesh->getMeshBufferCount(); ++i)
				((SMeshBuffer*)Mesh->getMeshBuffer(i))->recalculateBoundingBox();

			((SMesh*)Mesh)->recalculateBoundingBox();
			SAnimatedMesh* am = new SAnimatedMesh();
			am->Type = EAMT_3DS;
			am->addMesh(Mesh);
			am->recalculateBoundingBox();
			Mesh->drop();
			Mesh = 0;
        	return am;
		}
	}

	Mesh->drop();
	Mesh = 0;

	return 0;
}


bool COgreMeshFileLoader::readChunk(io::IReadFile* file)
{
	while(file->getPos() < file->getSize())
	{
		ChunkData data;
		readChunkData(file, data);

		switch(data.header.id)
		{
		case COGRE_MESH:
			{
				Meshes.push_back(OgreMesh());
				readObjectChunk(file, data, Meshes.getLast());
				if (Skeleton.Bones.size())
					Mesh = new CSkinnedMesh();
				else
					Mesh = new SMesh();
				composeObject();
			}
			break;
		default:
			return true;
		}
	}

	return true;
}


bool COgreMeshFileLoader::readObjectChunk(io::IReadFile* file, ChunkData& parent, OgreMesh& mesh)
{
#ifdef IRR_OGRE_LOADER_DEBUG
	os::Printer::log("Read Object Chunk");
#endif
	readBool(file, parent, mesh.SkeletalAnimation);
	while ((parent.read < parent.header.length)&&(file->getPos() < file->getSize()))
	{
		ChunkData data;
		readChunkData(file, data);

		switch(data.header.id)
		{
			case COGRE_GEOMETRY:
			{
				readGeometry(file, data, mesh.Geometry);
			}
			break;
			case COGRE_SUBMESH:
				mesh.SubMeshes.push_back(OgreSubMesh());
				readSubMesh(file, data, mesh.SubMeshes.getLast());
			break;
			case COGRE_MESH_BOUNDS:
			{
#ifdef IRR_OGRE_LOADER_DEBUG
				os::Printer::log("Read Mesh Bounds");
#endif
				readVector(file, data, mesh.BBoxMinEdge);
				readVector(file, data, mesh.BBoxMaxEdge);
				readFloat(file, data, &mesh.BBoxRadius);
			}
			break;
			case COGRE_SKELETON_LINK:
			{
#ifdef IRR_OGRE_LOADER_DEBUG
				os::Printer::log("Read Skeleton link");
#endif
				core::stringc name;
				readString(file, data, name);
				loadSkeleton(file, name);
			}
			break;
			case COGRE_BONE_ASSIGNMENT:
			{
				mesh.BoneAssignments.push_back(OgreBoneAssignment());
				readInt(file, data, &mesh.BoneAssignments.getLast().VertexID);
				readShort(file, data, &mesh.BoneAssignments.getLast().BoneID);
				readFloat(file, data, &mesh.BoneAssignments.getLast().Weight);
			}
			break;
			case COGRE_MESH_LOD:
			case COGRE_MESH_SUBMESH_NAME_TABLE:
			case COGRE_MESH_EDGE_LISTS:
			default:
				// ignore chunk
				file->seek(data.header.length-data.read, true);
				data.read += data.header.length-data.read;
				break;
		}
		parent.read += data.read;
	}
	return true;
}


bool COgreMeshFileLoader::readGeometry(io::IReadFile* file, ChunkData& parent, OgreGeometry& geometry)
{
#ifdef IRR_OGRE_LOADER_DEBUG
	os::Printer::log("Read Geometry");
#endif
	readInt(file, parent, &geometry.NumVertex);
	while(parent.read < parent.header.length)
	{
		ChunkData data;
		readChunkData(file, data);

		switch(data.header.id)
		{
		case COGRE_GEOMETRY_VERTEX_DECLARATION:
			readVertexDeclaration(file, data, geometry);
			break;
		case COGRE_GEOMETRY_VERTEX_BUFFER:
			readVertexBuffer(file, data, geometry);
			break;
		default:
			// ignore chunk
			file->seek(data.header.length-data.read, true);
			data.read += data.header.length-data.read;
		}
		parent.read += data.read;
	}
	return true;
}


bool COgreMeshFileLoader::readVertexDeclaration(io::IReadFile* file, ChunkData& parent, OgreGeometry& geometry)
{
#ifdef IRR_OGRE_LOADER_DEBUG
	os::Printer::log("Read Vertex Declaration");
#endif
	NumUV = 0;
	while(parent.read < parent.header.length)
	{
		ChunkData data;
		readChunkData(file, data);

		switch(data.header.id)
		{
		case COGRE_GEOMETRY_VERTEX_ELEMENT:
		{
			geometry.Elements.push_back(OgreVertexElement());
			OgreVertexElement& elem = geometry.Elements.getLast();
			readShort(file, data, &elem.Source);
			readShort(file, data, &elem.Type);
			readShort(file, data, &elem.Semantic);
			if (elem.Semantic == 7) //Tex coords
			{
				++NumUV;
			}
			readShort(file, data, &elem.Offset);
			elem.Offset /= sizeof(f32);
			readShort(file, data, &elem.Index);
		}
			break;
		default:
			// ignore chunk
			file->seek(data.header.length-data.read, true);
			data.read += data.header.length-data.read;
		}
		parent.read += data.read;
	}
	return true;
}


bool COgreMeshFileLoader::readVertexBuffer(io::IReadFile* file, ChunkData& parent, OgreGeometry& geometry)
{
#ifdef IRR_OGRE_LOADER_DEBUG
	os::Printer::log("Read Vertex Buffer");
#endif
	OgreVertexBuffer buf;
	readShort(file, parent, &buf.BindIndex);
	readShort(file, parent, &buf.VertexSize);
	buf.VertexSize /= sizeof(f32);
	ChunkData data;
	readChunkData(file, data);

	if (data.header.id == COGRE_GEOMETRY_VERTEX_BUFFER_DATA)
	{
		buf.Data.set_used(geometry.NumVertex*buf.VertexSize);
		readFloat(file, data, buf.Data.pointer(), geometry.NumVertex*buf.VertexSize);
	}

	geometry.Buffers.push_back(buf);
	parent.read += data.read;
	return true;
}


bool COgreMeshFileLoader::readSubMesh(io::IReadFile* file, ChunkData& parent, OgreSubMesh& subMesh)
{
#ifdef IRR_OGRE_LOADER_DEBUG
	os::Printer::log("Read Submesh");
#endif
	readString(file, parent, subMesh.Material);
	readBool(file, parent, subMesh.SharedVertices);

	s32 numIndices;
	readInt(file, parent, &numIndices);
	subMesh.Indices.set_used(numIndices);

	readBool(file, parent, subMesh.Indices32Bit);

	if (subMesh.Indices32Bit)
		readInt(file, parent, subMesh.Indices.pointer(), numIndices);
	else
		for (s32 i=0; i<numIndices; ++i)
		{
			u16 num;
			readShort(file, parent, &num);
			subMesh.Indices[i]=num;
		}

	if (!subMesh.SharedVertices)
	{
		ChunkData data;
		readChunkData(file, data);

		if (data.header.id==COGRE_GEOMETRY)
		{
			readGeometry(file, data, subMesh.Geometry);
		}
		parent.read += data.read;
	}

	while(parent.read < parent.header.length)
	{
		ChunkData data;
		readChunkData(file, data);

		switch(data.header.id)
		{
		case COGRE_SUBMESH_OPERATION:
			readShort(file, data, &subMesh.Operation);
#ifdef IRR_OGRE_LOADER_DEBUG
			os::Printer::log("Read Submesh Operation",core::stringc(subMesh.Operation));
#endif
			if (subMesh.Operation != 4)
				os::Printer::log("Primitive type != trilist not yet implemented", ELL_WARNING);
			break;
		case COGRE_SUBMESH_TEXTURE_ALIAS:
		{
#ifdef IRR_OGRE_LOADER_DEBUG
				os::Printer::log("Read Submesh Texture Alias");
#endif
			core::stringc texture, alias;
			readString(file, data, texture);
			readString(file, data, alias);
			subMesh.TextureAliases.push_back(OgreTextureAlias(texture,alias));
		}
			break;
		case COGRE_SUBMESH_BONE_ASSIGNMENT:
		{
			subMesh.BoneAssignments.push_back(OgreBoneAssignment());
			readInt(file, data, &subMesh.BoneAssignments.getLast().VertexID);
			readShort(file, data, &subMesh.BoneAssignments.getLast().BoneID);
			readFloat(file, data, &subMesh.BoneAssignments.getLast().Weight);
		}
			break;
		default:
			parent.read=parent.header.length;
			file->seek(-(long)sizeof(ChunkHeader), true);
			return true;
		}
		parent.read += data.read;
	}
	return true;
}


void COgreMeshFileLoader::composeMeshBufferMaterial(scene::IMeshBuffer* mb, const core::stringc& materialName)
{
	video::SMaterial& material=mb->getMaterial();
	for (u32 k=0; k<Materials.size(); ++k)
	{
		if ((materialName==Materials[k].Name)&&(Materials[k].Techniques.size())&&(Materials[k].Techniques[0].Passes.size()))
		{
			material=Materials[k].Techniques[0].Passes[0].Material;
			if (Materials[k].Techniques[0].Passes[0].Texture.Filename.size())
			{
				if (FileSystem->existFile(Materials[k].Techniques[0].Passes[0].Texture.Filename))
					material.setTexture(0, Driver->getTexture(Materials[k].Techniques[0].Passes[0].Texture.Filename));
				else
					material.setTexture(0, Driver->getTexture((CurrentlyLoadingFromPath+"/"+FileSystem->getFileBasename(Materials[k].Techniques[0].Passes[0].Texture.Filename))));
			}
			break;
		}
	}
}


scene::SMeshBuffer* COgreMeshFileLoader::composeMeshBuffer(const core::array<s32>& indices, const OgreGeometry& geom)
{
	scene::SMeshBuffer *mb=new scene::SMeshBuffer();

	u32 i;
	mb->Indices.set_used(indices.size());
	for (i=0; i<indices.size(); ++i)
		mb->Indices[i]=indices[i];

	mb->Vertices.set_used(geom.NumVertex);
	for (i=0; i<geom.Elements.size(); ++i)
	{
		if (geom.Elements[i].Semantic==1) //Pos
		{
			for (u32 j=0; j<geom.Buffers.size(); ++j)
			{
				if (geom.Elements[i].Source==geom.Buffers[j].BindIndex)
				{
					u32 eSize=geom.Buffers[j].VertexSize;
					u32 ePos=geom.Elements[i].Offset;
					for (s32 k=0; k<geom.NumVertex; ++k)
					{
						mb->Vertices[k].Color=mb->Material.DiffuseColor;
						mb->Vertices[k].Pos.set(geom.Buffers[j].Data[ePos],geom.Buffers[j].Data[ePos+1],geom.Buffers[j].Data[ePos+2]);
						ePos += eSize;
					}
				}
			}
		}

		if (geom.Elements[i].Semantic==4) //Normal
		{
			for (u32 j=0; j<geom.Buffers.size(); ++j)
			{
				if (geom.Elements[i].Source==geom.Buffers[j].BindIndex)
				{
					u32 eSize=geom.Buffers[j].VertexSize;
					u32 ePos=geom.Elements[i].Offset;
					for (s32 k=0; k<geom.NumVertex; ++k)
					{
						mb->Vertices[k].Normal.set(geom.Buffers[j].Data[ePos],geom.Buffers[j].Data[ePos+1],geom.Buffers[j].Data[ePos+2]);
						ePos += eSize;
					}
				}
			}
		}

		if (geom.Elements[i].Semantic==7) //TexCoord
		{
			for (u32 j=0; j<geom.Buffers.size(); ++j)
			{
				if (geom.Elements[i].Source==geom.Buffers[j].BindIndex)
				{
					u32 eSize=geom.Buffers[j].VertexSize;
					u32 ePos=geom.Elements[i].Offset;
					for (s32 k=0; k<geom.NumVertex; ++k)
					{
						mb->Vertices[k].TCoords.set(geom.Buffers[j].Data[ePos],geom.Buffers[j].Data[ePos+1]);
						ePos += eSize;
					}
				}
			}
		}
	}
	return mb;
}


scene::SMeshBufferLightMap* COgreMeshFileLoader::composeMeshBufferLightMap(const core::array<s32>& indices, const OgreGeometry& geom)
{
	scene::SMeshBufferLightMap *mb=new scene::SMeshBufferLightMap();

	u32 i;
	mb->Indices.set_used(indices.size());
	for (i=0; i<indices.size(); ++i)
		mb->Indices[i]=indices[i];

	mb->Vertices.set_used(geom.NumVertex);

	for (i=0; i<geom.Elements.size(); ++i)
	{
		if (geom.Elements[i].Semantic==1) //Pos
		{
			for (u32 j=0; j<geom.Buffers.size(); ++j)
			{
				if (geom.Elements[i].Source==geom.Buffers[j].BindIndex)
				{
					u32 eSize=geom.Buffers[j].VertexSize;
					u32 ePos=geom.Elements[i].Offset;
					for (s32 k=0; k<geom.NumVertex; ++k)
					{
						mb->Vertices[k].Color=mb->Material.DiffuseColor;
						mb->Vertices[k].Pos.set(geom.Buffers[j].Data[ePos],geom.Buffers[j].Data[ePos+1],geom.Buffers[j].Data[ePos+2]);
						ePos += eSize;
					}
				}
			}
		}

		if (geom.Elements[i].Semantic==4) //Normal
		{
			for (u32 j=0; j<geom.Buffers.size(); ++j)
			{
				if (geom.Elements[i].Source==geom.Buffers[j].BindIndex)
				{
					u32 eSize=geom.Buffers[j].VertexSize;
					u32 ePos=geom.Elements[i].Offset;
					for (s32 k=0; k<geom.NumVertex; ++k)
					{
						mb->Vertices[k].Normal.set(geom.Buffers[j].Data[ePos],geom.Buffers[j].Data[ePos+1],geom.Buffers[j].Data[ePos+2]);
						ePos += eSize;
					}
				}
			}
		}

		if (geom.Elements[i].Semantic==7) //TexCoord
		{
			for (u32 j=0; j<geom.Buffers.size(); ++j)
			{
				if (geom.Elements[i].Source==geom.Buffers[j].BindIndex)
				{
					u32 eSize=geom.Buffers[j].VertexSize;
					u32 ePos=geom.Elements[i].Offset;
					for (s32 k=0; k<geom.NumVertex; ++k)
					{
						mb->Vertices[k].TCoords.set(geom.Buffers[j].Data[ePos], geom.Buffers[j].Data[ePos+1]);
						mb->Vertices[k].TCoords2.set(geom.Buffers[j].Data[ePos+2], geom.Buffers[j].Data[ePos+3]);

						ePos += eSize;
					}
				}
			}
		}
	}

	return mb;
}


scene::IMeshBuffer* COgreMeshFileLoader::composeMeshBufferSkinned(scene::CSkinnedMesh& mesh, const core::array<s32>& indices, const OgreGeometry& geom)
{
	scene::SSkinMeshBuffer *mb=mesh.addMeshBuffer();
	if (NumUV>1)
	{
		mb->convertTo2TCoords();
		mb->Vertices_2TCoords.set_used(geom.NumVertex);
	}
	else
		mb->Vertices_Standard.set_used(geom.NumVertex);

	u32 i;
	mb->Indices.set_used(indices.size());
	for (i=0; i<indices.size(); i+=3)
	{
		mb->Indices[i+0]=indices[i+2];
		mb->Indices[i+1]=indices[i+1];
		mb->Indices[i+2]=indices[i+0];
	}

	for (i=0; i<geom.Elements.size(); ++i)
	{
		if (geom.Elements[i].Semantic==1) //Pos
		{
			for (u32 j=0; j<geom.Buffers.size(); ++j)
			{
				if (geom.Elements[i].Source==geom.Buffers[j].BindIndex)
				{
					u32 eSize=geom.Buffers[j].VertexSize;
					u32 ePos=geom.Elements[i].Offset;
					for (s32 k=0; k<geom.NumVertex; ++k)
					{
						if (NumUV>1)
							mb->Vertices_2TCoords[k].Color=mb->Material.DiffuseColor;
						else
							mb->Vertices_Standard[k].Color=mb->Material.DiffuseColor;
						mb->getPosition(k).set(-geom.Buffers[j].Data[ePos],geom.Buffers[j].Data[ePos+1],geom.Buffers[j].Data[ePos+2]);
						ePos += eSize;
					}
				}
			}
		}

		if (geom.Elements[i].Semantic==4) //Normal
		{
			for (u32 j=0; j<geom.Buffers.size(); ++j)
			{
				if (geom.Elements[i].Source==geom.Buffers[j].BindIndex)
				{
					u32 eSize=geom.Buffers[j].VertexSize;
					u32 ePos=geom.Elements[i].Offset;
					for (s32 k=0; k<geom.NumVertex; ++k)
					{
						mb->getNormal(k).set(-geom.Buffers[j].Data[ePos],geom.Buffers[j].Data[ePos+1],geom.Buffers[j].Data[ePos+2]);
						ePos += eSize;
					}
				}
			}
		}

		if (geom.Elements[i].Semantic==7) //TexCoord
		{
			for (u32 j=0; j<geom.Buffers.size(); ++j)
			{
				if (geom.Elements[i].Source==geom.Buffers[j].BindIndex)
				{
					u32 eSize=geom.Buffers[j].VertexSize;
					u32 ePos=geom.Elements[i].Offset;
					for (s32 k=0; k<geom.NumVertex; ++k)
					{
						mb->getTCoords(k).set(geom.Buffers[j].Data[ePos], geom.Buffers[j].Data[ePos+1]);
						if (NumUV>1)
							mb->Vertices_2TCoords[k].TCoords2.set(geom.Buffers[j].Data[ePos+2], geom.Buffers[j].Data[ePos+3]);

						ePos += eSize;
					}
				}
			}
		}
	}

	return mb;
}


void COgreMeshFileLoader::composeObject(void)
{
	for (u32 i=0; i<Meshes.size(); ++i)
	{
		for (u32 j=0; j<Meshes[i].SubMeshes.size(); ++j)
		{
			IMeshBuffer* mb;
			if (Meshes[i].SubMeshes[j].SharedVertices)
			{
				if (Skeleton.Bones.size())
				{
					mb = composeMeshBufferSkinned(*(CSkinnedMesh*)Mesh, Meshes[i].SubMeshes[j].Indices, Meshes[i].Geometry);
				}
				else if (NumUV < 2)
				{
					mb = composeMeshBuffer(Meshes[i].SubMeshes[j].Indices, Meshes[i].Geometry);
				}
				else
				{
					mb = composeMeshBufferLightMap(Meshes[i].SubMeshes[j].Indices, Meshes[i].Geometry);
				}
			}
			else
			{
				if (Skeleton.Bones.size())
				{
					mb = composeMeshBufferSkinned(*(CSkinnedMesh*)Mesh, Meshes[i].SubMeshes[j].Indices, Meshes[i].SubMeshes[j].Geometry);
				}
				else if (NumUV < 2)
				{
					mb = composeMeshBuffer(Meshes[i].SubMeshes[j].Indices, Meshes[i].SubMeshes[j].Geometry);
				}
				else
				{
					mb = composeMeshBufferLightMap(Meshes[i].SubMeshes[j].Indices, Meshes[i].SubMeshes[j].Geometry);
				}
			}

			if (mb != 0)
			{
				composeMeshBufferMaterial(mb, Meshes[i].SubMeshes[j].Material);
				if (!Skeleton.Bones.size())
				{
					((SMesh*)Mesh)->addMeshBuffer(mb);
					mb->drop();
				}
			}
		}
	}
	if (Skeleton.Bones.size())
	{
		CSkinnedMesh* m = (CSkinnedMesh*)Mesh;
		// Create Joints
		for (u32 i=0; i<Skeleton.Bones.size(); ++i)
		{
			ISkinnedMesh::SJoint* joint = m->addJoint();
			joint->Name=Skeleton.Bones[i].Name;

			joint->LocalMatrix = Skeleton.Bones[i].Orientation.getMatrix();
			if (Skeleton.Bones[i].Scale != core::vector3df(1,1,1))
			{
				core::matrix4 scaleMatrix;
				scaleMatrix.setScale( Skeleton.Bones[i].Scale );
				joint->LocalMatrix *= scaleMatrix;
			}
			/*
			joint->LocalMatrix[2]=-joint->LocalMatrix[2];
			joint->LocalMatrix[6]=-joint->LocalMatrix[6];
			joint->LocalMatrix[8]=-joint->LocalMatrix[8];
			joint->LocalMatrix[9]=-joint->LocalMatrix[9];
			*/
			joint->LocalMatrix.setTranslation( Skeleton.Bones[i].Position );
		}
		// Joints hierarchy
		core::array<bool> isRoot;
		isRoot.set_used(Skeleton.Bones.size());
		memset(isRoot.pointer(), true, Skeleton.Bones.size());
		for (u32 i=0; i<Skeleton.Bones.size(); ++i)
		{
			if (Skeleton.Bones[i].Parent<m->getJointCount())
			{
				m->getAllJoints()[Skeleton.Bones[i].Parent]->Children.push_back(m->getAllJoints()[Skeleton.Bones[i].Handle]);
				isRoot[Skeleton.Bones[i].Handle]=false;
			}
		}

		// Weights
		u32 bufCount=0;
		for (u32 i=0; i<Meshes.size(); ++i)
		{
			for (u32 j=0; j<Meshes[i].SubMeshes.size(); ++j)
			{
				for (u32 k=0; k<Meshes[i].SubMeshes[j].BoneAssignments.size(); ++k)
				{
					ISkinnedMesh::SWeight* w = m->addWeight(m->getAllJoints()[Meshes[i].SubMeshes[j].BoneAssignments[k].BoneID]);
					w->strength=Meshes[i].SubMeshes[j].BoneAssignments[k].Weight;
					w->vertex_id=Meshes[i].SubMeshes[j].BoneAssignments[k].VertexID;
					w->buffer_id=bufCount;
				}
				++bufCount;
			}
		}

#if 0
		// currently not working correctly
		for (u32 i=0; i<Skeleton.Animations.size(); ++i)
		{
			for (u32 j=0; j<Skeleton.Animations[i].Keyframes.size(); ++j)
			{
				ISkinnedMesh::SPositionKey* poskey = m->addPositionKey(m->getAllJoints()[Skeleton.Animations[i].Keyframes[j].BoneID]);
				poskey->frame=Skeleton.Animations[i].Keyframes[j].Time;
				poskey->position=Skeleton.Animations[i].Keyframes[j].Position;
				ISkinnedMesh::SRotationKey* rotkey = m->addRotationKey(m->getAllJoints()[Skeleton.Animations[i].Keyframes[j].BoneID]);
				rotkey->frame=Skeleton.Animations[i].Keyframes[j].Time;
				rotkey->rotation=Skeleton.Animations[i].Keyframes[j].Orientation;
				ISkinnedMesh::SScaleKey* scalekey = m->addScaleKey(m->getAllJoints()[Skeleton.Animations[i].Keyframes[j].BoneID]);
				scalekey->frame=Skeleton.Animations[i].Keyframes[j].Time;
				scalekey->scale=Skeleton.Animations[i].Keyframes[j].Scale;
			}
		}
#endif
		m->finalize();
	}
}


core::stringc COgreMeshFileLoader::getTextureFileName(const core::stringc& texture,
						 core::stringc& model)
{
	s32 idx = -1;
	idx = model.findLast('/');

	if (idx == -1)
		idx = model.findLast('\\');

	if (idx == -1)
		return core::stringc();

	core::stringc p = model.subString(0, idx+1);
	p.append(texture);
	return p;
}


void COgreMeshFileLoader::getMaterialToken(io::IReadFile* file, core::stringc& token, bool noNewLine)
{
	bool parseString=false;
	c8 c=0;
	token = "";

	if (file->getPos() >= file->getSize())
		return;

	file->read(&c, sizeof(c8));
	// search for word beginning
	while ( core::isspace(c) && (file->getPos() < file->getSize()))
	{
		if (noNewLine && c=='\n')
		{
			file->seek(-1, true);
			return;
		}
		file->read(&c, sizeof(c8));
	}
	// check if we read a string
	if (c=='"')
	{
		parseString = true;
		file->read(&c, sizeof(c8));
	}
	do
	{
		if (c=='/')
		{
			file->read(&c, sizeof(c8));
			// check for comments, cannot be part of strings
			if (!parseString && (c=='/'))
			{
				// skip comments
				while(c!='\n')
					file->read(&c, sizeof(c8));
				if (!token.size())
				{
					// if we start with a comment we need to skip
					// following whitespaces, so restart
					getMaterialToken(file, token, noNewLine);
					return;
				}
				else
				{
					// else continue with next character
					file->read(&c, sizeof(c8));
					continue;
				}
			}
			else
			{
				// else append first slash and check if second char
				// ends this token
				token.append('/');
				if ((!parseString && core::isspace(c)) ||
						(parseString && (c=='"')))
					return;
			}
		}
		token.append(c);
		file->read(&c, sizeof(c8));
		// read until a token delimiter is found
	}
	while (((!parseString && !core::isspace(c)) || (parseString && (c!='"'))) &&
			(file->getPos() < file->getSize()));
	// we want to skip the last quotes of a string , but other chars might be the next
	// token already.
	if (!parseString)
		file->seek(-1, true);
}


bool COgreMeshFileLoader::readColor(io::IReadFile* file, video::SColor& col)
{
	core::stringc token;

	getMaterialToken(file, token);
	if (token!="vertexcolour")
	{
		video::SColorf col_f;
		col_f.r=core::fast_atof(token.c_str());
		getMaterialToken(file, token);
		col_f.g=core::fast_atof(token.c_str());
		getMaterialToken(file, token);
		col_f.b=core::fast_atof(token.c_str());
		getMaterialToken(file, token, true);
		if (token.size())
			col_f.a=core::fast_atof(token.c_str());
		else
			col_f.a=1.0f;
		if ((col_f.r==0.0f)&&(col_f.g==0.0f)&&(col_f.b==0.0f))
			col.set(255,255,255,255);
		else
			col=col_f.toSColor();
		return false;
	}
	return true;
}


void COgreMeshFileLoader::readPass(io::IReadFile* file, OgreTechnique& technique)
{
#ifdef IRR_OGRE_LOADER_DEBUG
	os::Printer::log("Read Pass");
#endif
	core::stringc token;
	technique.Passes.push_back(OgrePass());
	OgrePass& pass=technique.Passes.getLast();

	getMaterialToken(file, token); //open brace or name
	if (token != "{")
		getMaterialToken(file, token); //open brace

	getMaterialToken(file, token);
	u32 inBlocks=1;
	u32 textureUnit=0;
	while(inBlocks)
	{
		if (token=="ambient")
			pass.AmbientTokenColor=readColor(file, pass.Material.AmbientColor);
		else if (token=="diffuse")
			pass.DiffuseTokenColor=readColor(file, pass.Material.DiffuseColor);
		else if (token=="specular")
		{
			pass.SpecularTokenColor=readColor(file, pass.Material.SpecularColor);
			getMaterialToken(file, token);
			pass.Material.Shininess=core::fast_atof(token.c_str());
		}
		else if (token=="emissive")
			pass.EmissiveTokenColor=readColor(file, pass.Material.EmissiveColor);
		else if (token=="scene_blend")
		{ // TODO: Choose correct values
			getMaterialToken(file, token);
			if (token=="add")
				pass.Material.MaterialType=video::EMT_TRANSPARENT_ADD_COLOR;
			else if (token=="modulate")
				pass.Material.MaterialType=video::EMT_SOLID;
			else if (token=="alpha_blend")
				pass.Material.MaterialType=video::EMT_TRANSPARENT_ALPHA_CHANNEL;
			else if (token=="colour_blend")
				pass.Material.MaterialType=video::EMT_TRANSPARENT_VERTEX_ALPHA;
			else
				getMaterialToken(file, token);
		}
		else if (token=="depth_check")
		{
			getMaterialToken(file, token);
			if (token!="on")
				pass.Material.ZBuffer=video::ECFN_NEVER;
		}
		else if (token=="depth_write")
		{
			getMaterialToken(file, token);
			pass.Material.ZWriteEnable=(token=="on");
		}
		else if (token=="depth_func")
		{
			getMaterialToken(file, token); // Function name
			if (token=="always_fail")
				pass.Material.ZBuffer=video::ECFN_NEVER;
			else if (token=="always_pass")
				pass.Material.ZBuffer=video::ECFN_ALWAYS;
			else if (token=="equal")
				pass.Material.ZBuffer=video::ECFN_EQUAL;
			else if (token=="greater")
				pass.Material.ZBuffer=video::ECFN_GREATER;
			else if (token=="greater_equal")
				pass.Material.ZBuffer=video::ECFN_GREATEREQUAL;
			else if (token=="less")
				pass.Material.ZBuffer=video::ECFN_LESS;
			else if (token=="less_equal")
				pass.Material.ZBuffer=video::ECFN_LESSEQUAL;
			else if (token=="not_equal")
				pass.Material.ZBuffer=video::ECFN_NOTEQUAL;
		}
		else if (token=="normalise_normals")
		{
			getMaterialToken(file, token);
			pass.Material.NormalizeNormals=(token=="on");
		}
		else if (token=="depth_bias")
		{
			getMaterialToken(file, token); // bias value
		}
		else if (token=="alpha_rejection")
		{
			getMaterialToken(file, token); // function name
			getMaterialToken(file, token); // value
			pass.Material.MaterialTypeParam=core::fast_atof(token.c_str());
		}
		else if (token=="alpha_to_coverage")
		{
			getMaterialToken(file, token);
			if (token=="on")
				pass.Material.AntiAliasing |= video::EAAM_ALPHA_TO_COVERAGE;
		}
		else if (token=="colour_write")
		{
			getMaterialToken(file, token);
			pass.Material.ColorMask = (token=="on")?video::ECP_ALL:video::ECP_NONE;
		}
		else if (token=="cull_hardware")
		{
			getMaterialToken(file, token); // rotation name
		}
		else if (token=="cull_software")
		{
			getMaterialToken(file, token); // culling side
		}
		else if (token=="lighting")
		{
			getMaterialToken(file, token);
			pass.Material.Lighting=(token=="on");
		}
		else if (token=="shading")
		{
			getMaterialToken(file, token);
			// We take phong as gouraud
			pass.Material.GouraudShading=(token!="flat");
		}
		else if (token=="polygon_mode")
		{
			getMaterialToken(file, token);
			pass.Material.Wireframe=(token=="wireframe");
			pass.Material.PointCloud=(token=="points");
		}
		else if (token=="max_lights")
		{
			getMaterialToken(file, token);
			pass.MaxLights=strtol(token.c_str(),NULL,10);
		}
		else if (token=="point_size")
		{
			getMaterialToken(file, token);
			pass.PointSize=core::fast_atof(token.c_str());
		}
		else if (token=="point_sprites")
		{
			getMaterialToken(file, token);
			pass.PointSprites=(token=="on");
		}
		else if (token=="point_size_min")
		{
			getMaterialToken(file, token);
			pass.PointSizeMin=strtol(token.c_str(),NULL,10);
		}
		else if (token=="point_size_max")
		{
			getMaterialToken(file, token);
			pass.PointSizeMax=strtol(token.c_str(),NULL,10);
		}
		else if (token=="texture_unit")
		{
#ifdef IRR_OGRE_LOADER_DEBUG
			os::Printer::log("Read Texture unit");
#endif
			getMaterialToken(file, token); //open brace
			getMaterialToken(file, token);
			while(token != "}")
			{
				if (token=="texture")
				{
					getMaterialToken(file, pass.Texture.Filename);
#ifdef IRR_OGRE_LOADER_DEBUG
					os::Printer::log("Read Texture", pass.Texture.Filename.c_str());
#endif
					getMaterialToken(file, pass.Texture.CoordsType, true);
					getMaterialToken(file, pass.Texture.MipMaps, true);
					getMaterialToken(file, pass.Texture.Alpha, true);
				}
				else if (token=="filtering")
				{
					getMaterialToken(file, token);
					pass.Material.TextureLayer[textureUnit].AnisotropicFilter=0;
					if (token=="point")
					{
						pass.Material.TextureLayer[textureUnit].BilinearFilter=false;
						pass.Material.TextureLayer[textureUnit].TrilinearFilter=false;
						getMaterialToken(file, token);
						getMaterialToken(file, token);
					}
					else if (token=="linear")
					{
						getMaterialToken(file, token);
						if (token=="point")
						{
							pass.Material.TextureLayer[textureUnit].BilinearFilter=false;
							pass.Material.TextureLayer[textureUnit].TrilinearFilter=false;
							getMaterialToken(file, token);
						}
						else
						{
							pass.Material.TextureLayer[textureUnit].BilinearFilter=true;
							getMaterialToken(file, token);
							pass.Material.TextureLayer[textureUnit].TrilinearFilter=(token=="linear");
						}
					}
					else
					{
						pass.Material.TextureLayer[textureUnit].BilinearFilter=(token=="bilinear");
						pass.Material.TextureLayer[textureUnit].TrilinearFilter=(token=="trilinear");
						pass.Material.TextureLayer[textureUnit].AnisotropicFilter=(token=="anisotropic")?2:1;
					}
				}
				else if (token=="max_anisotropy")
				{
					getMaterialToken(file, token);
					pass.Material.TextureLayer[textureUnit].AnisotropicFilter=(u8)core::strtol10(token.c_str());
				}
				else if (token=="texture_alias")
				{
					getMaterialToken(file, pass.Texture.Alias);
				}
				else if (token=="mipmap_bias")
				{
					getMaterialToken(file, token);
					pass.Material.TextureLayer[textureUnit].LODBias=(s8)core::fast_atof(token.c_str());
				}
				else if (token=="colour_op")
				{ // TODO: Choose correct values
					getMaterialToken(file, token);
					if (token=="add")
						pass.Material.MaterialType=video::EMT_TRANSPARENT_ADD_COLOR;
					else if (token=="modulate")
						pass.Material.MaterialType=video::EMT_SOLID;
					else if (token=="alpha_blend")
						pass.Material.MaterialType=video::EMT_TRANSPARENT_ALPHA_CHANNEL;
					else if (token=="colour_blend")
						pass.Material.MaterialType=video::EMT_TRANSPARENT_VERTEX_ALPHA;
					else
						getMaterialToken(file, token);
				}
				getMaterialToken(file, token);
			}
			++textureUnit;
		}
		else if (token=="shadow_caster_program_ref")
		{
			do
			{
				getMaterialToken(file, token);
			} while (token != "}");
			getMaterialToken(file, token);
		}
		else if (token=="vertex_program_ref")
		{
			do
			{
				getMaterialToken(file, token);
			} while (token != "}");
			getMaterialToken(file, token);
		}
		//fog_override, iteration, point_size_attenuation
		//not considered yet!
		getMaterialToken(file, token);
		if (token=="{")
			++inBlocks;
		else if (token=="}")
			--inBlocks;
	}
}


void COgreMeshFileLoader::readTechnique(io::IReadFile* file, OgreMaterial& mat)
{
#ifdef IRR_OGRE_LOADER_DEBUG
	os::Printer::log("Read Technique");
#endif
	core::stringc token;
	mat.Techniques.push_back(OgreTechnique());
	OgreTechnique& technique=mat.Techniques.getLast();

	getMaterialToken(file, technique.Name); //open brace or name
	if (technique.Name != "{")
		getMaterialToken(file, token); //open brace
	else
		technique.Name=core::stringc((int)mat.Techniques.size());

	getMaterialToken(file, token);
	while (token != "}")
	{
		if (token == "pass")
			readPass(file, technique);
		else if (token == "scheme")
			getMaterialToken(file, token);
		else if (token == "lod_index")
			getMaterialToken(file, token);
		getMaterialToken(file, token);
	}
}


void COgreMeshFileLoader::loadMaterials(io::IReadFile* meshFile)
{
#ifdef IRR_OGRE_LOADER_DEBUG
	os::Printer::log("Load Materials");
#endif
	core::stringc token;
	io::IReadFile* file = 0;
	io::path filename = FileSystem->getFileBasename(meshFile->getFileName(), false) + ".material";
	if (FileSystem->existFile(filename))
		file = FileSystem->createAndOpenFile(filename);
	else
		file = FileSystem->createAndOpenFile(FileSystem->getFileDir(meshFile->getFileName())+"/"+filename);

	if (!file)
	{
		os::Printer::log("Could not load OGRE material", filename);
		return;
	}

	getMaterialToken(file, token);

	while (file->getPos() < file->getSize())
	{
		if ((token == "fragment_program") || (token == "vertex_program"))
		{
			// skip whole block
			u32 blocks=1;
			do
			{
				getMaterialToken(file, token);
			} while (token != "{");
			do
			{
				getMaterialToken(file, token);
				if (token == "{")
					++blocks;
				else if (token == "}")
					--blocks;
			} while (blocks);
			getMaterialToken(file, token);
			continue;
		}
		if (token != "material")
		{
			if (token.trim().size())
				os::Printer::log("Unknown material group", token.c_str());
			break;
		}

		Materials.push_back(OgreMaterial());
		OgreMaterial& mat = Materials.getLast();

		getMaterialToken(file, mat.Name);
#ifdef IRR_OGRE_LOADER_DEBUG
	os::Printer::log("Load Material", mat.Name.c_str());
#endif
		getMaterialToken(file, token); //open brace
		getMaterialToken(file, token);
		while(token != "}")
		{
			if (token=="lod_distances") // can have several items
				getMaterialToken(file, token);
			else if (token=="receive_shadows")
			{
				getMaterialToken(file, token);
				mat.ReceiveShadows=(token=="on");
			}
			else if (token=="transparency_casts_shadows")
			{
				getMaterialToken(file, token);
				mat.TransparencyCastsShadows=(token=="on");
			}
			else if (token=="set_texture_alias")
			{
				getMaterialToken(file, token);
				getMaterialToken(file, token);
			}
			else if (token=="technique")
				readTechnique(file, mat);
			getMaterialToken(file, token);
		}
		getMaterialToken(file, token);
	}

	file->drop();
#ifdef IRR_OGRE_LOADER_DEBUG
	os::Printer::log("Finished loading Materials");
#endif
}


bool COgreMeshFileLoader::loadSkeleton(io::IReadFile* meshFile, const core::stringc& name)
{
#ifdef IRR_OGRE_LOADER_DEBUG
	os::Printer::log("Load Skeleton", name);
#endif
	io::IReadFile* file = 0;
	io::path filename;
	if (FileSystem->existFile(name))
		file = FileSystem->createAndOpenFile(name);
	else if (FileSystem->existFile(filename = FileSystem->getFileDir(meshFile->getFileName())+"/"+name))
		file = FileSystem->createAndOpenFile(filename);
	else if (FileSystem->existFile(filename = FileSystem->getFileBasename(meshFile->getFileName(), false) + ".skeleton"))
		file = FileSystem->createAndOpenFile(filename);
	else
		file = FileSystem->createAndOpenFile(FileSystem->getFileDir(meshFile->getFileName())+"/"+filename);
	if (!file)
	{
		os::Printer::log("Could not load matching skeleton", name);
		return false;
	}

	s16 id;
	file->read(&id, 2);
	if (SwapEndian)
		id = os::Byteswap::byteswap(id);
	if (id != COGRE_HEADER)
	{
		file->drop();
		return false;
	}

	core::stringc skeletonVersion;
	ChunkData head;
	readString(file, head, skeletonVersion);
	if (skeletonVersion != "[Serializer_v1.10]")
	{
		file->drop();
		return false;
	}

	u16 bone=0;
	f32 animationTotal=0.f;
	while(file->getPos() < file->getSize())
	{
		ChunkData data;
		readChunkData(file, data);

		switch(data.header.id)
		{
		case COGRE_SKELETON:
			{
				Skeleton.Bones.push_back(OgreBone());
				OgreBone& bone = Skeleton.Bones.getLast();
				readString(file, data, bone.Name);
				readShort(file, data, &bone.Handle);
				readVector(file, data, bone.Position);
				readQuaternion(file, data, bone.Orientation);
#ifdef IRR_OGRE_LOADER_DEBUG
				os::Printer::log("Bone", bone.Name+" ("+core::stringc(bone.Handle)+")");
//				os::Printer::log("Position", core::stringc(bone.Position.X)+" "+core::stringc(bone.Position.Y)+" "+core::stringc(bone.Position.Z));
//				os::Printer::log("Rotation quat", core::stringc(bone.Orientation.W)+" "+core::stringc(bone.Orientation.X)+" "+core::stringc(bone.Orientation.Y)+" "+core::stringc(bone.Orientation.Z));
//				core::vector3df rot;
//				bone.Orientation.toEuler(rot);
//				rot *= core::RADTODEG;
//				os::Printer::log("Rotation", core::stringc(rot.X)+" "+core::stringc(rot.Y)+" "+core::stringc(rot.Z));
#endif
				if (data.read<(data.header.length-bone.Name.size()))
				{
					readVector(file, data, bone.Scale);
					bone.Scale.X *= -1.f;
				}
				else
					bone.Scale=core::vector3df(1,1,1);
				bone.Parent=0xffff;
			}
			break;
		case COGRE_BONE_PARENT:
			{
				u16 parent;
				readShort(file, data, &bone);
				readShort(file, data, &parent);
				if (bone<Skeleton.Bones.size() && parent<Skeleton.Bones.size())
					Skeleton.Bones[bone].Parent=parent;
			}
			break;
		case COGRE_ANIMATION:
			{
				if (Skeleton.Animations.size())
					animationTotal+=Skeleton.Animations.getLast().Length;
				Skeleton.Animations.push_back(OgreAnimation());
				OgreAnimation& anim = Skeleton.Animations.getLast();
				readString(file, data, anim.Name);
				readFloat(file, data, &anim.Length);
#ifdef IRR_OGRE_LOADER_DEBUG
				os::Printer::log("Animation", anim.Name);
				os::Printer::log("Length", core::stringc(anim.Length));
#endif
			}
			break;
		case COGRE_ANIMATION_TRACK:
#ifdef IRR_OGRE_LOADER_DEBUG
//			os::Printer::log("for Bone ", core::stringc(bone));
#endif
				readShort(file, data, &bone); // store current bone
			break;
		case COGRE_ANIMATION_KEYFRAME:
			{
				Skeleton.Animations.getLast().Keyframes.push_back(OgreKeyframe());
				OgreKeyframe& keyframe = Skeleton.Animations.getLast().Keyframes.getLast();
				readFloat(file, data, &keyframe.Time);
				keyframe.Time+=animationTotal;
				readVector(file, data, keyframe.Position);
				readQuaternion(file, data, keyframe.Orientation);
				if (data.read<data.header.length)
				{
					readVector(file, data, keyframe.Scale);
					keyframe.Scale.X *= -1.f;
				}
				else
					keyframe.Scale=core::vector3df(1,1,1);
				keyframe.BoneID=bone;
#ifdef IRR_OGRE_LOADER_DEBUG
//				os::Printer::log("Keyframe time", core::stringc(keyframe.Time));
#endif
			}
			break;
		case COGRE_ANIMATION_LINK:
#ifdef IRR_OGRE_LOADER_DEBUG
			os::Printer::log("Animation link");
#endif
			break;
		default:
			break;
		}
	}
	file->drop();
	return true;
}


void COgreMeshFileLoader::readChunkData(io::IReadFile* file, ChunkData& data)
{
	file->read(&data.header, sizeof(ChunkHeader));
	if (SwapEndian)
	{
		data.header.id = os::Byteswap::byteswap(data.header.id);
		data.header.length = os::Byteswap::byteswap(data.header.length);
	}
	data.read += sizeof(ChunkHeader);
}


void COgreMeshFileLoader::readString(io::IReadFile* file, ChunkData& data, core::stringc& out)
{
	c8 c = 0;
	out = "";

	while (c!='\n')
	{
		file->read(&c, sizeof(c8));
		if (c!='\n')
			out.append(c);

	}
	data.read+=out.size()+1;
}


void COgreMeshFileLoader::readBool(io::IReadFile* file, ChunkData& data, bool& out)
{
	// normal C type because we read a bit string
	char c = 0;
	file->read(&c, sizeof(char));
	out=(c!=0);
	++data.read;
}


void COgreMeshFileLoader::readInt(io::IReadFile* file, ChunkData& data, s32* out, u32 num)
{
	// normal C type because we read a bit string
	file->read(out, sizeof(int));
	if (SwapEndian)
	{
		for (u32 i=0; i<num; ++i)
			out[i] = os::Byteswap::byteswap(out[i]);
	}
	data.read+=sizeof(int)*num;
}


void COgreMeshFileLoader::readShort(io::IReadFile* file, ChunkData& data, u16* out, u32 num)
{
	// normal C type because we read a bit string
	file->read(out, sizeof(short)*num);
	if (SwapEndian)
	{
		for (u32 i=0; i<num; ++i)
			out[i] = os::Byteswap::byteswap(out[i]);
	}
	data.read+=sizeof(short)*num;
}


void COgreMeshFileLoader::readFloat(io::IReadFile* file, ChunkData& data, f32* out, u32 num)
{
	// normal C type because we read a bit string
	file->read(out, sizeof(float)*num);
	if (SwapEndian)
	{
		for (u32 i=0; i<num; ++i)
			out[i] = os::Byteswap::byteswap(out[i]);
	}
	data.read+=sizeof(float)*num;
}


void COgreMeshFileLoader::readVector(io::IReadFile* file, ChunkData& data, core::vector3df& out)
{
	readFloat(file, data, &out.X);
	readFloat(file, data, &out.Y);
	readFloat(file, data, &out.Z);
	out.X *= -1.f;
}


void COgreMeshFileLoader::readQuaternion(io::IReadFile* file, ChunkData& data, core::quaternion& out)
{
	readVector(file, data, *((core::vector3df*)&out.X));
	readFloat(file, data, &out.W);
}


void COgreMeshFileLoader::clearMeshes()
{
	for (u32 i=0; i<Meshes.size(); ++i)
	{
		for (int k=0; k<(int)Meshes[i].Geometry.Buffers.size(); ++k)
			Meshes[i].Geometry.Buffers[k].Data.clear();

		for (u32 j=0; j<Meshes[i].SubMeshes.size(); ++j)
		{
			for (int h=0; h<(int)Meshes[i].SubMeshes[j].Geometry.Buffers.size(); ++h)
				Meshes[i].SubMeshes[j].Geometry.Buffers[h].Data.clear();
		}
	}

	Meshes.clear();
}


} // end namespace scene
} // end namespace irr

#endif // _IRR_COMPILE_WITH_OGRE_LOADER_

