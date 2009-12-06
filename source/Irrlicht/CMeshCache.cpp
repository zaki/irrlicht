// Copyright (C) 2002-2009 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CMeshCache.h"
#include "IAnimatedMesh.h"
#include "IMesh.h"

namespace irr
{
namespace scene
{

static const io::SNamedPath emptyNamedPath;


CMeshCache::~CMeshCache()
{
	clear();
}


//! adds a mesh to the list
void CMeshCache::addMesh(const io::path& filename, IAnimatedMesh* mesh)
{
	mesh->grab();

	MeshEntry e ( filename );
	e.Mesh = mesh;

	Meshes.push_back(e);
}


//! Removes a mesh from the cache.
void CMeshCache::removeMesh(const IAnimatedMesh* const mesh)
{
	if ( !mesh )
		return;
	for (u32 i=0; i<Meshes.size(); ++i)
	{
		if (Meshes[i].Mesh == mesh)
		{
			Meshes[i].Mesh->drop();
			Meshes.erase(i);
			return;
		}
	}
}


//! Removes a mesh from the cache.
void CMeshCache::removeMesh(const IMesh* const mesh)
{
	if ( !mesh )
		return;
	for (u32 i=0; i<Meshes.size(); ++i)
	{
		if (Meshes[i].Mesh && Meshes[i].Mesh->getMesh(0) == mesh)
		{
			Meshes[i].Mesh->drop();
			Meshes.erase(i);
			return;
		}
	}
}


//! Returns amount of loaded meshes
u32 CMeshCache::getMeshCount() const
{
	return Meshes.size();
}


//! Returns current number of the mesh
s32 CMeshCache::getMeshIndex(const IAnimatedMesh* const mesh) const
{
	for (u32 i=0; i<Meshes.size(); ++i)
		if (Meshes[i].Mesh == mesh)
			return (s32)i;

	return -1;
}



//! Returns current index number of the mesh, and -1 if it is not in the cache.
s32 CMeshCache::getMeshIndex(const IMesh* const mesh) const
{
	for (u32 i=0; i<Meshes.size(); ++i)
	{
		if (Meshes[i].Mesh && Meshes[i].Mesh->getMesh(0) == mesh)
			return (s32)i;
	}

	return -1;
}


//! Returns a mesh based on its index number
IAnimatedMesh* CMeshCache::getMeshByIndex(u32 number)
{
	if (number >= Meshes.size())
		return 0;

	return Meshes[number].Mesh;
}


//! Returns a mesh based on its file name.
IAnimatedMesh* CMeshCache::getMeshByFilename(const io::path& filename)
{
	return getMeshByName(filename);
}


//! Returns name of a mesh based on its index number
const io::path& CMeshCache::getMeshFilename(u32 number) const
{
	return getMeshName(number).getName();
}



//! Returns the filename of a loaded mesh, if there is any.
const io::path& CMeshCache::getMeshFilename(const IAnimatedMesh* const mesh) const
{
	return getMeshName(mesh).getName();
}


//! Returns the filename of a loaded mesh, if there is any. Returns 0 if there is none.
const io::path& CMeshCache::getMeshFilename(const IMesh* const mesh) const
{
	return getMeshName(mesh).getName();
}



//! Renames a loaded mesh, if possible.
bool CMeshCache::setMeshFilename(u32 index, const io::path& filename)
{
	return renameMesh(index, filename);
}


//! Renames a loaded mesh, if possible.
bool CMeshCache::setMeshFilename(const IAnimatedMesh* const mesh, const io::path& filename)
{
	return renameMesh(mesh, filename);
}


//! Renames a loaded mesh, if possible.
bool CMeshCache::setMeshFilename(const IMesh* const mesh, const io::path& filename)
{
	return renameMesh(mesh, filename);
}

//! Returns a mesh based on its name.
IAnimatedMesh* CMeshCache::getMeshByName(const io::path& name)
{
	MeshEntry e ( name );
	s32 id = Meshes.binary_search(e);
	return (id != -1) ? Meshes[id].Mesh : 0;
}

//! Get the name of a loaded mesh, based on its index.
const io::SNamedPath& CMeshCache::getMeshName(u32 index) const
{
	if (index >= Meshes.size())
		return emptyNamedPath;

	return Meshes[index].NamedPath;
}

//! Get the name of a loaded mesh, if there is any.
const io::SNamedPath& CMeshCache::getMeshName(const IAnimatedMesh* const mesh) const
{
	if(!mesh)
		return emptyNamedPath;

	for (u32 i=0; i<Meshes.size(); ++i)
	{
		if (Meshes[i].Mesh == mesh)
			return Meshes[i].NamedPath;
	}

	return emptyNamedPath;
}

//! Get the name of a loaded mesh, if there is any.
const io::SNamedPath& CMeshCache::getMeshName(const IMesh* const mesh) const
{
	if (!mesh)
		return emptyNamedPath;

	for (u32 i=0; i<Meshes.size(); ++i)
	{
		// IMesh may actually be an IAnimatedMesh, so do a direct comparison
		// as well as getting an IMesh from our stored IAnimatedMeshes
		if (Meshes[i].Mesh && (Meshes[i].Mesh == mesh || Meshes[i].Mesh->getMesh(0) == mesh))
			return Meshes[i].NamedPath;
	}

	return emptyNamedPath;
}

//! Renames a loaded mesh.
bool CMeshCache::renameMesh(u32 index, const io::path& name)
{
	if (index >= Meshes.size())
		return false;

	Meshes[index].NamedPath.rename(name);
	Meshes.sort();
	return true;
}

//! Renames a loaded mesh.
bool CMeshCache::renameMesh(const IAnimatedMesh* const mesh, const io::path& name)
{
	for (u32 i=0; i<Meshes.size(); ++i)
	{
		if (Meshes[i].Mesh == mesh)
		{
			Meshes[i].NamedPath.rename(name);
			Meshes.sort();
			return true;
		}
	}

	return false;
}

//! Renames a loaded mesh.
bool CMeshCache::renameMesh(const IMesh* const mesh, const io::path& name)
{
	for (u32 i=0; i<Meshes.size(); ++i)
	{
		if (Meshes[i].Mesh && Meshes[i].Mesh->getMesh(0) == mesh)
		{
			Meshes[i].NamedPath.rename(name);
			Meshes.sort();
			return true;
		}
	}

	return false;
}


//! returns if a mesh already was loaded
bool CMeshCache::isMeshLoaded(const io::path& name)
{
	return getMeshByName(name) != 0;
}


//! Clears the whole mesh cache, removing all meshes.
void CMeshCache::clear()
{
	for (u32 i=0; i<Meshes.size(); ++i)
		Meshes[i].Mesh->drop();

	Meshes.clear();
}

//! Clears all meshes that are held in the mesh cache but not used anywhere else.
void CMeshCache::clearUnusedMeshes()
{
	for (u32 i=0; i<Meshes.size(); ++i)
	{
		if (Meshes[i].Mesh->getReferenceCount() == 1)
		{
			Meshes[i].Mesh->drop();
			Meshes.erase(i);
			--i;
		}
	}
}


} // end namespace scene
} // end namespace irr

