// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "IMesh.h"
#include "NativeConverter.h"

namespace Irrlicht
{
namespace Scene
{
	IMesh::IMesh(irr::scene::IMesh* realMesh)
		: Mesh(realMesh)
	{
		Mesh->grab();
	}

	IMesh::~IMesh()
	{
		Mesh->drop();
	}

	int IMesh::get_MeshBufferCount()
	{
		return Mesh->getMeshBufferCount();
	}

	Core::Box3D IMesh::get_BoundingBox()
	{
		return irr::NativeConverter::getNETBox(Mesh->getBoundingBox());
	}

	void IMesh::set_BoundingBox(Core::Box3D box)
	{
		Mesh->setBoundingBox( irr::NativeConverter::getNativeBox(box) );
	}

	void IMesh::setMaterialFlag(Video::MaterialFlag flag, bool newvalue)
	{
		Mesh->setMaterialFlag((irr::video::E_MATERIAL_FLAG)flag, newvalue);
	}

	irr::scene::IMesh* IMesh::get_NativeMesh()
	{
		return Mesh;
	}

}
}