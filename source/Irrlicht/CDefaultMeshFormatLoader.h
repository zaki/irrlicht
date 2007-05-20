// Copyright (C) 2002-2007 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_DEFAULT_MESH_FORMAT_LOADER_H_INCLUDED__
#define __C_DEFAULT_MESH_FORMAT_LOADER_H_INCLUDED__

#include "IMeshLoader.h"
#include "IFileSystem.h"
#include "IVideoDriver.h"
#include "ISceneManager.h"

namespace irr
{
namespace scene
{

//! Meshloader capable of loading all Irrlicht default build in formats.
/** Which are: Quake 3 Bsp level, Quake 2 MD2 model, Milkshape .ms3d model. */
class CDefaultMeshFormatLoader : public IMeshLoader
{
public:

	//! Constructor
	CDefaultMeshFormatLoader(io::IFileSystem* fs, video::IVideoDriver* driver, scene::ISceneManager* smgr);

	//! destructor
	virtual ~CDefaultMeshFormatLoader();

	//! returns true if the file maybe is able to be loaded by this class
	//! based on the file extension (e.g. ".bsp")
	virtual bool isALoadableFileExtension(const c8* fileName);

	//! creates/loads an animated mesh from the file.
	//! \return Pointer to the created mesh. Returns 0 if loading failed.
	//! If you no longer need the mesh, you should call IAnimatedMesh::drop().
	//! See IUnknown::drop() for more information.
	virtual IAnimatedMesh* createMesh(irr::io::IReadFile* file);

private:

	io::IFileSystem* FileSystem;
	video::IVideoDriver* Driver;
	scene::ISceneManager* SceneManager;
};

} // end namespace scene
} // end namespace irr

#endif

