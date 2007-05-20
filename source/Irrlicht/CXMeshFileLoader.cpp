// Copyright (C) 2002-2007 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CXMeshFileLoader.h"
#include "os.h"
#include "SMeshBuffer.h"
#include "SAnimatedMesh.h"
#include "CXFileReader.h"
#include "CXAnimationPlayer.h"
#include "IMeshManipulator.h"

namespace irr
{
namespace scene
{

//! Constructor
CXMeshFileLoader::CXMeshFileLoader(IMeshManipulator* manip,
				   video::IVideoDriver* driver)
: Manipulator(manip), Driver(driver)
{
	if (Manipulator)
		Manipulator->grab();

	if (Driver)
		Driver->grab();
}



//! destructor
CXMeshFileLoader::~CXMeshFileLoader()
{
	if (Manipulator)
		Manipulator->drop();

	if (Driver)
		Driver->drop();
}



//! returns true if the file maybe is able to be loaded by this class
//! based on the file extension (e.g. ".bsp")
bool CXMeshFileLoader::isALoadableFileExtension(const c8* filename)
{
	return strstr(filename, ".x")!=0;
}



//! creates/loads an animated mesh from the file.
//! \return Pointer to the created mesh. Returns 0 if loading failed.
//! If you no longer need the mesh, you should call IAnimatedMesh::drop().
//! See IUnknown::drop() for more information.
IAnimatedMesh* CXMeshFileLoader::createMesh(io::IReadFile* file)
{
	if (!file)
		return 0;

	IAnimatedMesh* mesh = 0;
	CXFileReader *xreader = new CXFileReader(file);

	if (xreader->errorHappened())
	{
		xreader->drop();
		return 0;
	}

	mesh = new CXAnimationPlayer(xreader, Driver, Manipulator,
		file->getFileName());

	xreader->drop();
	return mesh;
}


} // end namespace scene
} // end namespace irr

