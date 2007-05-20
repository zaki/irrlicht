// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "IAnimatedMesh.h"
#include "NativeConverter.h"

namespace Irrlicht
{
namespace Scene
{

	IAnimatedMesh::IAnimatedMesh(irr::scene::IAnimatedMesh* mesh)
		: AnimatedMesh(mesh)
	{
		AnimatedMesh->grab();
	}

	IAnimatedMesh::~IAnimatedMesh()
	{
		AnimatedMesh->drop();
	}

	int IAnimatedMesh::get_FrameCount()
	{
		return AnimatedMesh->getFrameCount();
	}

	IMesh* IAnimatedMesh::GetMesh(int frame)
	{
		return GetMesh(frame, -1, -1, 255);
	}

	IMesh* IAnimatedMesh::GetMesh(int frame, int startFrameLoop, int endFrameLoop, int detailLevel)
	{
		irr::scene::IMesh* mesh = AnimatedMesh->getMesh(frame, startFrameLoop, endFrameLoop, detailLevel);
		if (!mesh)
			return 0;

		return new IMesh(mesh);
	}

	Core::Box3D IAnimatedMesh::get_BoundingBox()
	{
		return irr::NativeConverter::getNETBox(AnimatedMesh->getBoundingBox());	
	}

	AnimatedMeshType IAnimatedMesh::get_MeshType()
	{
		return (Scene::AnimatedMeshType)AnimatedMesh->getMeshType();
	}

	irr::scene::IAnimatedMesh* IAnimatedMesh::get_NativeAnimatedMesh()
	{
		return AnimatedMesh;
	}
}
}