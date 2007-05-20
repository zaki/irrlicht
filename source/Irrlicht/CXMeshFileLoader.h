// Copyright (C) 2002-2007 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_X_MESH_FILE_LOADER_H_INCLUDED__
#define __C_X_MESH_FILE_LOADER_H_INCLUDED__

#include "IMeshLoader.h"
#include "IFileSystem.h"
#include "IVideoDriver.h"
#include "irrString.h"
#include "SMesh.h"
#include "CXFileReader.h"
#include "SMeshBuffer.h"

namespace irr
{
namespace scene
{
class IMeshManipulator;

//! Meshloader capable of loading x meshes.
class CXMeshFileLoader : public IMeshLoader
{
public:

	//! Constructor
	CXMeshFileLoader(IMeshManipulator* manip, video::IVideoDriver* driver);

	//! destructor
	virtual ~CXMeshFileLoader();

	//! returns true if the file maybe is able to be loaded by this class
	//! based on the file extension (e.g. ".cob")
	virtual bool isALoadableFileExtension(const c8* fileName);

	//! creates/loads an animated mesh from the file.
	//! \return Pointer to the created mesh. Returns 0 if loading failed.
	//! If you no longer need the mesh, you should call IAnimatedMesh::drop().
	//! See IUnknown::drop() for more information.
	virtual IAnimatedMesh* createMesh(irr::io::IReadFile* file);

private:

	IMeshManipulator* Manipulator;
	video::IVideoDriver* Driver;
};

} // end namespace scene
} // end namespace irr

#endif

