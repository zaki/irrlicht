// Copyright (C) 2002-2007 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CAnimatedMeshMS3D.h"
#include "os.h"
#include "IVideoDriver.h"
#include "quaternion.h"

namespace irr
{
namespace scene
{


// byte-align structures
#if defined(_MSC_VER) ||  defined(__BORLANDC__) || defined (__BCPLUSPLUS__)
#	pragma pack( push, packing )
#	pragma pack( 1 )
#	define PACK_STRUCT
#elif defined( __GNUC__ )
#	define PACK_STRUCT	__attribute__((packed))
#else
#	error compiler not supported
#endif

// File header
struct MS3DHeader
{
	c8 ID[10];
	s32 Version;
} PACK_STRUCT;

// Vertex information
struct MS3DVertex
{
	u8 Flags;
	f32 Vertex[3];
	s8 BoneID;
	u8 RefCount;
} PACK_STRUCT;

// Triangle information
struct MS3DTriangle
{
	u16 Flags;
	u16 VertexIndices[3];
	f32 VertexNormals[3][3];
	f32 S[3], T[3];
	u8 SmoothingGroup;
	u8 GroupIndex;
} PACK_STRUCT;

// Material information
struct MS3DMaterial
{
    s8 Name[32];
    f32 Ambient[4];
    f32 Diffuse[4];
    f32 Specular[4];
    f32 Emissive[4];
    f32 Shininess;	// 0.0f - 128.0f
    f32 Transparency;	// 0.0f - 1.0f
    u8 Mode;	// 0, 1, 2 is unused now
    s8 Texture[128];
    s8 Alphamap[128];
} PACK_STRUCT;

// Joint information
struct MS3DJoint
{
	u8 Flags;
	s8 Name[32];
	s8 ParentName[32];
	f32 Rotation[3];
	f32 Translation[3];
	u16 NumRotationKeyframes;
	u16 NumTranslationKeyframes;
} PACK_STRUCT;

// Keyframe data
struct MS3DKeyframe
{
	f32 Time;
	f32 Parameter[3];
} PACK_STRUCT;

// Default alignment
#if defined(_MSC_VER) ||  defined(__BORLANDC__) || defined (__BCPLUSPLUS__)
#	pragma pack( pop, packing )
#endif

#undef PACK_STRUCT


//! constructor
CAnimatedMeshMS3D::CAnimatedMeshMS3D(video::IVideoDriver* driver)
: Driver(driver)
{
	if (Driver)
		Driver->grab();

	lastCalculatedFrame = -1;
}



//! destructor
CAnimatedMeshMS3D::~CAnimatedMeshMS3D()
{
	if (Driver)
		Driver->drop();
}



//! loads an ms3d file
bool CAnimatedMeshMS3D::loadFile(io::IReadFile* file)
{
	u32 i=0;

	if (!file)
		return false;

	HasAnimation = false;

	// find file size

	size_t fileSize = file->getSize();

	// read whole file

	u8* buffer = new u8[fileSize];
	size_t read = (s32)file->read(buffer, fileSize);
	if (read != fileSize)
	{
		delete [] buffer;
		os::Printer::log("Could not read full file. Loading failed", file->getFileName(), ELL_ERROR);
		return false;
	}

	// read header

	const u8 *pPtr = (u8*)((void*)buffer);
	MS3DHeader *pHeader = (MS3DHeader*)pPtr;
	pPtr += sizeof(MS3DHeader);

	if ( strncmp( pHeader->ID, "MS3D000000", 10 ) != 0 )
	{
		delete [] buffer;
		os::Printer::log("Not a valid Milkshape3D Model File. Loading failed", file->getFileName(), ELL_ERROR);
		return false;
	}

#ifdef __BIG_ENDIAN__
	pHeader->Version = os::Byteswap::byteswap(pHeader->Version);
#endif
	if ( pHeader->Version < 3 || pHeader->Version > 4 )
	{
		delete [] buffer;
		os::Printer::log("Only Milkshape3D version 1.3 and 1.4 is supported. Loading failed", file->getFileName(), ELL_ERROR);
		return false;
	}

	// get pointers to data

	// vertices
	u16 numVertices = *(u16*)pPtr;
#ifdef __BIG_ENDIAN__
	numVertices = os::Byteswap::byteswap(numVertices);
#endif
	pPtr += sizeof(u16);
	MS3DVertex *vertices = (MS3DVertex*)pPtr;
	pPtr += sizeof(MS3DVertex) * numVertices;
#ifdef __BIG_ENDIAN__
	for (i=0; i<numVertices; ++i)
		for (u32 j=0; j<3; ++j)
			vertices[i].Vertex[j] = os::Byteswap::byteswap(vertices[i].Vertex[j]);
#endif

	// triangles
	u16 numTriangles = *(u16*)pPtr;
#ifdef __BIG_ENDIAN__
	numTriangles = os::Byteswap::byteswap(numTriangles);
#endif
	pPtr += sizeof(u16);
	MS3DTriangle *triangles = (MS3DTriangle*)pPtr;
	pPtr += sizeof(MS3DTriangle) * numTriangles;
#ifdef __BIG_ENDIAN__
	for (i=0; i<numTriangles; ++i)
	{
		triangles[i].Flags = os::Byteswap::byteswap(triangles[i].Flags);
		for (u32 j=0; j<3; ++j)
		{
			triangles[i].VertexIndices[j] = os::Byteswap::byteswap(triangles[i].VertexIndices[j]);
			for (u16 k=0; k<3; ++k)
				triangles[i].VertexNormals[j][k] = os::Byteswap::byteswap(triangles[i].VertexNormals[j][k]);
			triangles[i].S[j] = os::Byteswap::byteswap(triangles[i].S[j]);
			triangles[i].T[j] = os::Byteswap::byteswap(triangles[i].T[j]);
		}
	}
#endif

	// groups
	u16 numGroups = *(u16*)pPtr;
#ifdef __BIG_ENDIAN__
	numGroups = os::Byteswap::byteswap(numGroups);
#endif
	pPtr += sizeof(u16);

	//skip groups
	for (i=0; i<numGroups; ++i)
	{
		Groups.push_back(SGroup());
		SGroup& grp = Groups.getLast();

		// The byte flag is before the name, so add 1
		grp.Name = ((const c8*) pPtr) + 1;

		pPtr += 33; // name and 1 byte flags
		u16 triangleCount = *(u16*)pPtr;
#ifdef __BIG_ENDIAN__
		triangleCount = os::Byteswap::byteswap(triangleCount);
#endif
		pPtr += sizeof(u16);

		//pPtr += sizeof(u16) * triangleCount; // triangle indices
		for (u32 j=0; j<triangleCount; ++j)
		{
#ifdef __BIG_ENDIAN__
			grp.VertexIds.push_back(os::Byteswap::byteswap(*(u16*)pPtr));
#else
			grp.VertexIds.push_back(*(u16*)pPtr);
#endif
			pPtr += sizeof (u16);
		}

		grp.MaterialIdx = *(u8*)pPtr;

		pPtr += sizeof(c8); // material index
	}

	// skip materials
	u16 numMaterials = *(u16*)pPtr;
#ifdef __BIG_ENDIAN__
	numMaterials = os::Byteswap::byteswap(numMaterials);
#endif
	pPtr += sizeof(u16);

	// MS3DMaterial *materials = (MS3DMaterial*)pPtr;
	// pPtr += sizeof(MS3DMaterial) * numMaterials;

	// if there are no materials, add at least one buffer
	if(numMaterials == 0)
		Buffers.push_back(SSharedMeshBuffer(&AnimatedVertices));

	for (i=0; i<numMaterials; ++i)
	{
		MS3DMaterial *material = (MS3DMaterial*)pPtr;
#ifdef __BIG_ENDIAN__
		for (u32 j=0; j<4; ++j)
			material->Ambient[j] = os::Byteswap::byteswap(material->Ambient[j]);
		for (u32 j=0; j<4; ++j)
			material->Diffuse[j] = os::Byteswap::byteswap(material->Diffuse[j]);
		for (u32 j=0; j<4; ++j)
			material->Specular[j] = os::Byteswap::byteswap(material->Specular[j]);
		for (u32 j=0; j<4; ++j)
			material->Emissive[j] = os::Byteswap::byteswap(material->Emissive[j]);
		material->Shininess = os::Byteswap::byteswap(material->Shininess);
		material->Transparency = os::Byteswap::byteswap(material->Transparency);
#endif
		pPtr += sizeof(MS3DMaterial);

		Buffers.push_back(SSharedMeshBuffer(&AnimatedVertices));
		SSharedMeshBuffer& tmpBuffer = Buffers.getLast();

		tmpBuffer.Material.MaterialType = video::EMT_SOLID;

		tmpBuffer.Material.AmbientColor = video::SColorf(material->Ambient[0], material->Ambient[1], material->Ambient[2], material->Ambient[3]).toSColor ();
		tmpBuffer.Material.DiffuseColor = video::SColorf(material->Diffuse[0], material->Diffuse[1], material->Diffuse[2], material->Diffuse[3]).toSColor ();
		tmpBuffer.Material.EmissiveColor = video::SColorf(material->Emissive[0], material->Emissive[1], material->Emissive[2], material->Emissive[3]).toSColor ();
		tmpBuffer.Material.SpecularColor = video::SColorf(material->Specular[0], material->Specular[1], material->Specular[2], material->Specular[3]).toSColor ();
		tmpBuffer.Material.Shininess = material->Shininess;
		tmpBuffer.Material.Textures[0] = Driver->getTexture((const c8*)material->Texture);
		if (tmpBuffer.Material.Textures[0]==0)
			tmpBuffer.Material.Textures[0] = Driver->getTexture(strrchr((const c8*)material->Texture, '/')+1);
	}

	// animation time
	f32 framesPerSecond = *(f32*)pPtr;
#ifdef __BIG_ENDIAN__
	framesPerSecond = os::Byteswap::byteswap(framesPerSecond);
#endif
	pPtr += sizeof(f32) * 2; // fps and current time

	s32 frameCount = *(s32*)pPtr;
#ifdef __BIG_ENDIAN__
	frameCount = os::Byteswap::byteswap(frameCount);
#endif
	pPtr += sizeof(s32);

	totalTime = (frameCount / framesPerSecond) * 1000.0f;

	u16 jointCount = *(u16*)pPtr;
#ifdef __BIG_ENDIAN__
	jointCount = os::Byteswap::byteswap(jointCount);
#endif
	pPtr += sizeof(u16);

	// load joints
	SJoint t;

	for (i=0; i<jointCount; ++i)
	{
		MS3DJoint *pJoint = (MS3DJoint*)pPtr;
#ifdef __BIG_ENDIAN__
		for (u32 j=0; j<3; ++j)
			pJoint->Rotation[j] = os::Byteswap::byteswap(pJoint->Rotation[j]);
		for (u32 j=0; j<3; ++j)
			pJoint->Translation[j] = os::Byteswap::byteswap(pJoint->Translation[j]);
		pJoint->NumRotationKeyframes= os::Byteswap::byteswap(pJoint->NumRotationKeyframes);
		pJoint->NumTranslationKeyframes = os::Byteswap::byteswap(pJoint->NumTranslationKeyframes);
#endif
		pPtr += sizeof(MS3DJoint);
		Joints.push_back(t);
		SJoint& jnt = Joints[Joints.size()-1];

		jnt.Name = pJoint->Name;
		jnt.Index = i;
		jnt.Rotation.X = pJoint->Rotation[0];
		jnt.Rotation.Y = pJoint->Rotation[1];
		jnt.Rotation.Z = pJoint->Rotation[2];
		jnt.Translation.X = pJoint->Translation[0];
		jnt.Translation.Y = pJoint->Translation[1];
		jnt.Translation.Z = pJoint->Translation[2];
		jnt.ParentName = pJoint->ParentName;
		jnt.Parent = -1;

		if (pJoint->NumRotationKeyframes ||
			pJoint->NumTranslationKeyframes)
			HasAnimation = true;

		// get rotation keyframes
		for (u32 j=0; j<pJoint->NumRotationKeyframes; ++j)
		{
			MS3DKeyframe* kf = (MS3DKeyframe*)pPtr;
#ifdef __BIG_ENDIAN__
			kf->Time = os::Byteswap::byteswap(kf->Time);
			for (u32 l=0; l<3; ++l)
				kf->Parameter[l] = os::Byteswap::byteswap(kf->Parameter[l]);
#endif
			pPtr += sizeof(MS3DKeyframe);

			SKeyframe k;
			k.timeindex = kf->Time * 1000.0f;
			k.data.X = kf->Parameter[0];
			k.data.Y = kf->Parameter[1];
			k.data.Z = kf->Parameter[2];
			jnt.RotationKeys.push_back(k);
		}

		// get translation keyframes
		for (u32 j=0; j<pJoint->NumTranslationKeyframes; ++j)
		{
			MS3DKeyframe* kf = (MS3DKeyframe*)pPtr;
#ifdef __BIG_ENDIAN__
			kf->Time = os::Byteswap::byteswap(kf->Time);
			for (u32 l=0; l<3; ++l)
				kf->Parameter[l] = os::Byteswap::byteswap(kf->Parameter[l]);
#endif
			pPtr += sizeof(MS3DKeyframe);

			SKeyframe k;
			k.timeindex = kf->Time * 1000.0f;
			k.data.X = kf->Parameter[0];
			k.data.Y = kf->Parameter[1];
			k.data.Z = kf->Parameter[2];
			jnt.TranslationKeys.push_back(k);
		}
	}

	//find parent of every joint
	for (i=0; i<Joints.size(); ++i)
	{
		if (Joints[i].ParentName.size() != 0)
		{
			for (u32 j=0; j<Joints.size(); ++j)
				if (i != j && Joints[i].ParentName == Joints[j].Name)
				{
					Joints[i].Parent = j;
					break;
				}

			if (Joints[i].Parent == -1)
				os::Printer::log("Found joint in model without parent.", ELL_WARNING);
		}
	}

	// sets up all joints with initial rotation and translation
	for (i=0; i<Joints.size(); ++i)
	{
		SJoint& jnt = Joints[i];

		jnt.RelativeTransformation.setRotationRadians(jnt.Rotation);
		jnt.RelativeTransformation.setTranslation(jnt.Translation);

		if (jnt.Parent == -1)
			jnt.AbsoluteTransformation  = jnt.RelativeTransformation;
		else
		{
			jnt.AbsoluteTransformation = Joints[jnt.Parent].AbsoluteTransformation;
			jnt.AbsoluteTransformation *= jnt.RelativeTransformation;
		}

		jnt.AbsoluteTransformationAnimated = jnt.AbsoluteTransformation;
	}

	// create vertices and indices, attach them to the joints.
	video::S3DVertex v;

	for (i=0; i<numTriangles; ++i)
	{
		for (u32 j = 0; j<3; ++j)
		{
			v.TCoords.X = triangles[i].S[j];
			v.TCoords.Y = triangles[i].T[j];
			v.Normal.X = triangles[i].VertexNormals[j][0];
			v.Normal.Y = triangles[i].VertexNormals[j][1];
			v.Normal.Z = triangles[i].VertexNormals[j][2];
			if(triangles[i].GroupIndex < Groups.size() && Groups[triangles[i].GroupIndex].MaterialIdx < Buffers.size())
				v.Color = Buffers[Groups[triangles[i].GroupIndex].MaterialIdx].Material.DiffuseColor;
			else
				v.Color.set(255,255,255,255);
			v.Pos.X = vertices[triangles[i].VertexIndices[j]].Vertex[0];
			v.Pos.Y = vertices[triangles[i].VertexIndices[j]].Vertex[1];
			v.Pos.Z = vertices[triangles[i].VertexIndices[j]].Vertex[2];

			//look, if we already have this vertex in our vertex array
			s32 index = -1;
			for (u32 iV = 0; iV < Vertices.size(); ++iV)
			{
				if (v == Vertices[iV])
				{
					index = (s32)iV;
					break;
				}
			}
			if (index == -1)
			{
				s32 boneid = vertices[triangles[i].VertexIndices[j]].BoneID;
				if (boneid>=0 && boneid<(s32)Joints.size())
					Joints[boneid].VertexIds.push_back(Vertices.size());

				Vertices.push_back(v);
				index = Vertices.size() - 1;
			}
			Indices.push_back(index);
		}
	}

	//create groups
	s32 iIndex = -1;
	for (i=0; i<Groups.size(); ++i)
	{
		SGroup& grp = Groups[i];

		if (grp.MaterialIdx >= Buffers.size())
			grp.MaterialIdx = 0;

		core::array<u16>& indices = Buffers[grp.MaterialIdx].Indices;

		for (u32 k=0; k<grp.VertexIds.size(); ++k)
			for (u32 l=0; l<3; ++l)
				indices.push_back(Indices[++iIndex]);
	}

	// calculate bounding box
	if (!Vertices.empty())
		BoundingBox.reset(Vertices[0].Pos);

	for (i=0; i<Vertices.size(); ++i)
		BoundingBox.addInternalPoint(Vertices[i].Pos);

	for (i=0; i<Buffers.size(); ++i)
		Buffers[i].recalculateBoundingBox();

	// inverse translate and rotate all vertices for making animation easier
	if (HasAnimation)
	{
		for (u32 k=0; k<Joints.size(); ++k)
		{
			for (u32 l=0; l<Joints[k].VertexIds.size(); ++l)
			{
				Joints[k].AbsoluteTransformation.inverseTranslateVect(
					Vertices[Joints[k].VertexIds[l]].Pos);
	
				Joints[k].AbsoluteTransformation.inverseRotateVect(
					Vertices[Joints[k].VertexIds[l]].Pos);
	
				Joints[k].AbsoluteTransformation.inverseRotateVect(
					Vertices[Joints[k].VertexIds[l]].Normal);
			}
		}
	}

	AnimatedVertices = Vertices;

	delete [] buffer;
	return true;
}



//! returns the amount of frames in milliseconds. If the amount is 1, it is a static (=non animated) mesh.
s32 CAnimatedMeshMS3D::getFrameCount()
{
	return (s32)totalTime;
}



//! returns the animated mesh based on a detail level. 0 is the lowest, 255 the highest detail. Note, that some Meshes will ignore the detail level.
IMesh* CAnimatedMeshMS3D::getMesh(s32 frame, s32 detailLevel, s32 startFrameLoop, s32 endFrameLoop)
{
	animate(frame);
	return this;
}



void CAnimatedMeshMS3D::getKeyframeData(const core::array<SKeyframe>& keys, f32 time, core::vector3df& outdata) const
{
	if (keys.size())
	{
		if (time < keys[0].timeindex)
		{
			outdata = keys[0].data;
			return;
		}
		if (time > keys.getLast().timeindex)
		{
			outdata = keys.getLast().data;
			return;
		}

		for (u32 i=0; i<keys.size()-1; ++i)
		{
			if (keys[i].timeindex <= time && keys[i+1].timeindex >= time)
			{
				f32 interpolate = (time - keys[i].timeindex)/(keys[i+1].timeindex - keys[i].timeindex);
				outdata = keys[i].data + ((keys[i+1].data - keys[i].data) * interpolate);
				return;
			}
		}
	}
}


void CAnimatedMeshMS3D::getKeyframeRotation(const core::array<SKeyframe>& keys, f32 time, core::vector3df& outdata) const
{
	if (keys.size())
	{
		if (time < keys[0].timeindex)
		{
			outdata = keys[0].data;
			return;
		}
		if (time > keys.getLast().timeindex)
		{
			outdata = keys.getLast().data;
			return;
		}

		for (u32 i=0; i<keys.size()-1; ++i)
		{
			if (keys[i].timeindex <= time && keys[i+1].timeindex >= time)
			{
//				core::quaternion q1(keys[i].data);
//				core::quaternion q2(keys[i+1].data);
				f32 interpolate = (time - keys[i].timeindex)/(keys[i+1].timeindex - keys[i].timeindex);
				core::quaternion q;
				q.slerp(keys[i].data, keys[i+1].data, interpolate);
				q.toEuler(outdata);
				return;
			}
		}
	}
}


//! Returns a pointer to a transformation matrix of a part of the
//! mesh based on a frame time.
core::matrix4* CAnimatedMeshMS3D::getMatrixOfJoint(s32 jointNumber, s32 frame)
{
	if (jointNumber < 0 || jointNumber >= (s32)Joints.size())
		return 0;

	animate(frame);

	return &Joints[jointNumber].AbsoluteTransformationAnimated;
}


//! Gets joint count.
s32 CAnimatedMeshMS3D::getJointCount() const
{
	return Joints.size();
}


//! Gets the name of a joint.
const c8* CAnimatedMeshMS3D::getJointName(s32 number) const
{
	if (number < 0 || number >= (s32)Joints.size())
		return 0;
	return Joints[number].Name.c_str();
}


//! Gets a joint number from its name
s32 CAnimatedMeshMS3D::getJointNumber(const c8* name) const
{
	for (s32 i=0; i<(s32)Joints.size(); ++i)
		if (Joints[i].Name == name)
			return i;

	return -1;
}


void CAnimatedMeshMS3D::animate(s32 frame)
{
	if (!HasAnimation || lastCalculatedFrame == frame)
		return;

	f32 time = (f32)frame;
	core::matrix4 transform;
	lastCalculatedFrame = frame;
	u32 i;

	for (i=0; i<Joints.size(); ++i)
	{
		core::vector3df translation = Joints[i].Translation;
		core::vector3df rotation = Joints[i].Rotation;

		// find keyframe translation and rotation
		getKeyframeData(Joints[i].TranslationKeys, time, translation);
		getKeyframeRotation(Joints[i].RotationKeys, time, rotation);

		transform.makeIdentity();
		transform.setRotationRadians(rotation);
		transform.setTranslation(translation);

		Joints[i].AbsoluteTransformationAnimated = Joints[i].RelativeTransformation * transform;

		if (Joints[i].Parent != -1)
			Joints[i].AbsoluteTransformationAnimated = Joints[Joints[i].Parent].AbsoluteTransformationAnimated * Joints[i].AbsoluteTransformationAnimated;

		if (i==0)
			BoundingBox.reset(Joints[0].AbsoluteTransformationAnimated.getTranslation());
		else
			BoundingBox.addInternalPoint(Joints[i].AbsoluteTransformationAnimated.getTranslation());


		// transform all vertices
		for (u32 j=0; j<Joints[i].VertexIds.size(); ++j)
		{
			Joints[i].AbsoluteTransformationAnimated.transformVect(
				AnimatedVertices[Joints[i].VertexIds[j]].Pos,
				Vertices[Joints[i].VertexIds[j]].Pos
			);

			Joints[i].AbsoluteTransformationAnimated.transformVect(
				AnimatedVertices[Joints[i].VertexIds[j]].Normal,
				Vertices[Joints[i].VertexIds[j]].Normal
			);

			// TODO: this could be done much more faster by
			// first getting 8 extreme points and putting them in.
			BoundingBox.addInternalPoint(AnimatedVertices[Joints[i].VertexIds[j]].Pos);
		}
	}

	for (i=0; i<Buffers.size(); ++i)
		Buffers[i].recalculateBoundingBox();
}



//! returns amount of mesh buffers.
u32 CAnimatedMeshMS3D::getMeshBufferCount() const
{
	return Buffers.size();
}



//! returns pointer to a mesh buffer
IMeshBuffer* CAnimatedMeshMS3D::getMeshBuffer(u32 nr) const
{
	if (nr < Buffers.size())
		return (IMeshBuffer*) &Buffers[nr];
	else
		return 0;
}

//! Returns pointer to a mesh buffer which fits a material
IMeshBuffer* CAnimatedMeshMS3D::getMeshBuffer( const video::SMaterial &material) const
{
	for (u32 i=0; i<Buffers.size(); ++i)
	{
		if (Buffers[i].getMaterial() == material)
			return (IMeshBuffer*) &Buffers[i];
	}
	return 0;
}


//! returns an axis aligned bounding box
const core::aabbox3d<f32>& CAnimatedMeshMS3D::getBoundingBox() const
{
	return BoundingBox;
}


//! set user axis aligned bounding box
void CAnimatedMeshMS3D::setBoundingBox( const core::aabbox3df& box)
{
	BoundingBox = box;
}

//! sets a flag of all contained materials to a new value
void CAnimatedMeshMS3D::setMaterialFlag(video::E_MATERIAL_FLAG flag, bool newvalue)
{
	for (u32 i=0; i<Buffers.size(); ++i)
		Buffers[i].Material.setFlag(flag, newvalue);
}


//! Returns the type of the animated mesh.
E_ANIMATED_MESH_TYPE CAnimatedMeshMS3D::getMeshType() const
{
	return EAMT_MS3D;
}


//! returns the byte size (stride, pitch) of the vertex
} // end namespace scene
} // end namespace irr


