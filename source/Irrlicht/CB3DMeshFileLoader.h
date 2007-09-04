// B3D mesh loader


#include "IrrCompileConfig.h"

#ifndef __C_B3D_MESH_LOADER_H_INCLUDED__
#define __C_B3D_MESH_LOADER_H_INCLUDED__

#include "IMeshLoader.h"
#include "ISceneManager.h"
#include "CSkinnedMesh.h"
#include "IReadFile.h"

namespace irr
{

namespace scene
{

//! Meshloader for B3D format
class CB3DMeshFileLoader : public IMeshLoader
{
public:

	//! Constructor
	CB3DMeshFileLoader(scene::ISceneManager* smgr);

	//! destructor
	virtual ~CB3DMeshFileLoader();

	//! returns true if the file maybe is able to be loaded by this class
	//! based on the file extension (e.g. ".bsp")
	virtual bool isALoadableFileExtension(const c8* fileName);

	//! creates/loads an animated mesh from the file.
	//! \return Pointer to the created mesh. Returns 0 if loading failed.
	//! If you no longer need the mesh, you should call IAnimatedMesh::drop().
	//! See IUnknown::drop() for more information.
	virtual IAnimatedMesh* createMesh(io::IReadFile* file);

private:

	struct SB3dChunkHeader
	{
		c8 name[4];
		s32 size;
	};

	struct SB3dChunk
	{
		c8 name[4];
		s32 length;
		s32 startposition;
	};

	struct SB3dTexture
	{
		irr::video::ITexture* Texture;
		s32 Flags;
		s32 Blend;
		f32 Xpos;
		f32 Ypos;
		f32 Xscale;
		f32 Yscale;
		f32 Angle;
	};

	struct SB3dMaterial
	{
		irr::video::SMaterial* Material;
		f32 red, green, blue, alpha;
		f32 shininess;
		s32 blend,fx;
		SB3dTexture *Textures[2];
	};

	bool load();
	bool readChunkNODE(CSkinnedMesh::SJoint* InJoint);
	bool readChunkMESH(CSkinnedMesh::SJoint* InJoint);
	bool readChunkVRTS(CSkinnedMesh::SJoint* InJoint, scene::SSkinMeshBuffer *MeshBuffer, s32 Vertices_Start);
	bool readChunkTRIS(CSkinnedMesh::SJoint* InJoint, scene::SSkinMeshBuffer *MeshBuffer, u32 MeshBufferID, s32 Vertices_Start);
	bool readChunkBONE(CSkinnedMesh::SJoint* InJoint);
	bool readChunkKEYS(CSkinnedMesh::SJoint* InJoint);
	bool readChunkANIM(CSkinnedMesh::SJoint* InJoint);
	bool readChunkTEXS();
	bool readChunkBRUS();

	core::stringc readString();
	core::stringc stripPathFromString(core::stringc string, bool returnPath=false);
	void readFloats(f32* vec, u32 count);

	core::array<SB3dChunk> B3dStack;

	bool NormalsInFile;

	core::array<SB3dMaterial> Materials;
	core::array<SB3dTexture> Textures;

	core::array<s32> AnimatedVertices_VertexID;

	core::array<s32> AnimatedVertices_BufferID;

	core::array<video::S3DVertex2TCoords*> BaseVertices;


	core::array<scene::SSkinMeshBuffer*> *Buffers;
	core::array<CSkinnedMesh::SJoint*> *AllJoints;

	//
	ISceneManager*	SceneManager;
	CSkinnedMesh*	AnimatedMesh;
	io::IReadFile*	file;



};

} // end namespace scene
} // end namespace irr

#endif // __C_B3D_MESH_LOADER_H_INCLUDED__

