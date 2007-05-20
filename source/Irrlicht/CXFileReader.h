// Copyright (C) 2002-2007 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_X_FILE_READER_H_INCLUDED__
#define __C_X_FILE_READER_H_INCLUDED__

#include "IReadFile.h"
#include "irrString.h"
#include "matrix4.h"
#include "SColor.h"
#include "irrArray.h"
#include "vector2d.h"
#include "quaternion.h"

//#define _XREADER_DEBUG

namespace irr
{
namespace scene
{

//! This is a .x file reader. It only reads data from uncompressed .x files.
class CXFileReader : public virtual IUnknown
{
public:

	struct SXAnimationSet;
	struct SXFrame;

	CXFileReader(io::IReadFile* file);
	~CXFileReader();

	//! Returns an error occured during reading the file
	bool errorHappened() const;

	//! Returns if the loaded mesh is static
	bool isStaticMesh() const;

	//! returns count of animations
	s32 getAnimationSetCount() const;

	//! returns a specific animation set
	SXAnimationSet& getAnimationSet(s32 i);

	//! returns array of root frames
	core::array<SXFrame> & getRootFrames();

	struct SXMaterial
	{
		video::SColorf FaceColor; // note: RGBA
		f32 Power;
		video::SColorf Specular; // RGB
		video::SColorf Emissive; // RGB
		core::stringc TextureFileName;
	};

	struct SXMeshMaterialList
	{
		core::array<s32> FaceIndices; // index of material for each face
		core::array<SXMaterial> Materials; // material array
	};

	struct SXTemplateMaterial
	{
		core::stringc Name; // template name from Xfile
		SXMaterial Material; // material
	};

	struct SXIndexedColor
	{
		s32 Index;
		video::SColorf Color;
	};

	struct SXSkinMeshHeader
	{
		s32 MaxSkinWeightsPerVertex; // Maximum number of transforms that affect a vertex in the mesh.
		s32 MaxSkinWeightsPerFace; // Maximum number of unique transforms that affect the three vertices of any face.
		s32 BoneCount; // Number of bones that affect vertices in this mesh.
	};

	struct SXWeight
	{
		SXWeight() {};
		SXWeight(s32 vtidx): VertexIndex(vtidx) {}

		s32 VertexIndex;
		f32 Weight;

		bool operator <(const SXWeight& other) const
		{
			return VertexIndex < other.VertexIndex;
		}
	};

	struct SXSkinWeight
	{
		core::stringc TransformNodeName; // name of the bone
		core::array< SXWeight > Weights;
		core::matrix4 MatrixOffset; // transforms the mesh vertices to the space of the bone
					// When concatenated to the bone's transform, this provides the
					// world space coordinates of the mesh as affected by the bone
	};

	struct SXMesh
	{
		// this mesh contains triangulated texture data.
		// because in an .x file, faces can be made of more than 3
		// vertices, the indices data structure is triangulated during the
		// loading process. The IndexCountPerFace array is filled during
		// this triangulation process and stores how much indices belong to
		// every face. This data structure can be ignored, because all data
		// in this structure is triangulated.

		core::stringc Name;
		core::array<core::vector3df> Vertices;
		core::array< s32 > Indices; // triangle indices

		// optional:

		core::array<core::vector2df> TextureCoords;
		core::array<core::vector3df> Normals;
		core::array<s32> NormalIndices; // amount is equal to Indices amount

		core::array< s32 > IndexCountPerFace; // default 3, but could be more
		core::array< SXIndexedColor > VertexColors;
		core::array< SXSkinWeight > SkinWeights;
		SXMeshMaterialList MaterialList;
		SXSkinMeshHeader SkinMeshHeader;
	};

	struct SXFrame
	{
		SXFrame() : iLevel(0), pParent(0) { }

		int iLevel;
		SXFrame * pParent;
		core::stringc Name;
		core::matrix4 LocalMatrix;
		core::matrix4 GlobalMatrix;

		core::array<SXMesh> Meshes;
		core::array<SXFrame> ChildFrames;
	};

	struct SXAnimationKey
	{
		SXAnimationKey() : keyType(-1), numberOfKeys(0), time(0), data(0) { }

		void del()
		{
			delete [] time;

			switch(keyType)
			{
			case -1: break;
			case 0 : delete [] (core::quaternion*)data; break;
			case 1 :
			case 2 : delete [] (core::vector3df*)data; break;
			case 3 :
			case 4 : delete [] (core::matrix4*)data; break;
			}
		}

		void init()
		{
			time = new s32[numberOfKeys];

			switch(keyType)
			{
			case 0 : data = new core::quaternion[numberOfKeys]; break;
			case 1 :
			case 2 : data = new core::vector3df[numberOfKeys]; break;
			case 3 :
			case 4 : data = new core::matrix4[numberOfKeys]; break;
			}

		}

		core::matrix4& getMatrix(s32 nr) const
		{
			return ((core::matrix4*)data)[nr];
		}

		core::vector3df& getVector(s32 nr) const
		{
			return ((core::vector3df*)data)[nr];
		}

		core::quaternion& getQuaternion(s32 nr) const
		{
			return ((core::quaternion*)data)[nr];
		}

		s32 keyType; // 0=rotation, 1=scale, 2=position, 3=matrix
		s32 numberOfKeys;

		s32* time;
		void* data;
	};

	struct SXAnimation
	{
		core::stringc FrameName;
		core::array<SXAnimationKey> Keys;

		// optional, from AnimationOptions:
		bool closed; // default true
		bool linearPositionQuality; // if false: spline position quality
	};

	struct SXAnimationSet
	{
		core::stringc AnimationName;
		core::array<SXAnimation> Animations;
	};

private:

	//! Parses the file
	bool parseFile();

	//! Reads file into memory
	bool readFileIntoMemory(io::IReadFile* file);

	//! Parses the next Data object in the file
	bool parseDataObject();

	//! places pointer to next begin of a token, and ignores comments
	void findNextNoneWhiteSpace();

	//! places pointer to next begin of a token, which must be a number,
	// and ignores comments
	void findNextNoneWhiteSpaceNumber();

	//! returns next parseable token. Returns empty string if no token there
	core::stringc getNextToken();

	//! reads header of dataobject including the opening brace.
	//! returns false if error happened, and writes name of object
	//! if there is one
	bool readHeadOfDataObject(core::stringc* outname=0);

	//! checks for two following semicolons, returns false if they are not there
	bool checkForTwoFollowingSemicolons();

	//! reads a x file style string
	bool getNextTokenAsString(core::stringc& out);

	inline u16 readBinWord();
	inline u32 readBinDWord();
	inline s32 readInt();
	inline f32 readFloat();
	inline bool readVector2(core::vector2df& vec);
	inline bool readVector3(core::vector3df& vec);
	inline bool readRGB(video::SColorf& color);
	inline bool readRGBA(video::SColorf& color);

	bool parseDataObjectTemplate();
	bool parseDataObjectFrame(SXFrame &frame);
	bool parseDataObjectTransformationMatrix(core::matrix4 &mat);
	bool parseDataObjectMesh(SXMesh &mesh);
	bool parseDataObjectMeshNormals(core::array<core::vector3df>& normals,
		core::array< s32 >& normalIndices, s32 triangulatedIndexCount,
		core::array< s32 >& indexCountPerFace);
	bool parseDataObjectMeshTextureCoords(core::array<core::vector2df>& textureCoords);
	bool parseDataObjectMeshVertexColors(core::array<SXIndexedColor>& vertexColors);
	bool parseDataObjectMeshMaterialList(SXMeshMaterialList& materiallist,
		s32 triangulatedIndexCount, core::array< s32 >& indexCountPerFace);
	bool parseDataObjectMaterial(SXMaterial& material);
	bool parseDataObjectTextureFilename(core::stringc& texturename);
	bool parseDataObjectSkinMeshHeader(SXSkinMeshHeader& header);
	bool parseDataObjectSkinWeights(SXSkinWeight& weights);
	bool parseDataObjectAnimationSet(SXAnimationSet& set);
	bool parseDataObjectAnimation(SXAnimation& anim);
	bool parseDataObjectAnimationKey(SXAnimationKey& animkey);
	bool parseUnknownDataObject();

	void readUntilEndOfLine();

	void computeGlobalFrameMatrices(SXFrame& frame, const SXFrame* const parent);
	void optimizeFrames( SXFrame * pgFrame,  SXFrame * pgParent );
	bool validateMesh(SXFrame* frame);

	s32 MajorVersion;
	s32 MinorVersion;
	bool binary;
	s32 binaryNumCount;

	c8* Buffer;
	s32 Size;
	c8 FloatSize;
	const c8* P;
	c8* End;

	bool ErrorHappened;

	bool m_bFrameRemoved;
	SXFrame * m_pgCurFrame;
	core::array<SXFrame>RootFrames;
	core::array<SXAnimationSet> AnimationSets;
	core::array<SXTemplateMaterial> TemplateMaterials;
};

} // end namespace scene
} // end namespace irr

#endif

