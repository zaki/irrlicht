// Copyright (C) 2002-2007 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_MESH_CACHE_H_INCLUDED__
#define __I_MESH_CACHE_H_INCLUDED__

#include "IUnknown.h"
#include "irrString.h"

namespace irr
{

namespace scene
{
	class IMesh;
	class IAnimatedMesh;
	class IAnimatedMeshSceneNode;
	class IMeshLoader;

	//!	The mesh cache stores already loaded meshes and provides an interface to them.
	/** You can access it using ISceneManager::getMeshCache(). All existing scene managers
	will return a pointer to the same mesh cache, because it is shared between them. With
	this interface, it is possible to manually add new loaded meshes (if
	ISceneManager::getMesh() is not sufficient), to remove them and to iterate through
	already loaded meshes.  */
	class IMeshCache : public virtual IUnknown
	{
	public:

		//! destructor
		virtual ~IMeshCache() {};

		//! Adds a mesh to the internal list of loaded meshes.
		/** Usually, ISceneManager::getMesh() is called to load a mesh from a file.
		That method searches the list of loaded meshes if a mesh has already been loaded and
		returns a pointer to if	it is in that list and already in memory. Otherwise it loads
		the mesh. With IMeshCache::addMesh(), it is possible to pretend that a mesh already
		has been loaded. This method can be used for example by mesh loaders who need to
		load more than one mesh with one call. They can add additional meshes with this
		method to the scene manager. The COLLADA loader for example uses this method.
		\param filename: Filename of the mesh. When called ISceneManager::getMesh() with this
		parameter, the method will return the mesh parameter given with this method.
		\param mesh: Pointer to a mesh which will now be referenced by this name. */
		virtual void addMesh(const c8* filename, IAnimatedMesh* mesh) = 0;

		//! Removes a mesh from the cache.
		/** After loading a mesh with getMesh(), the mesh can be removed from the cache
		using this method, freeing a lot of memory. */
		virtual void removeMesh(const IAnimatedMesh* const mesh) = 0;

		//! Removes a mesh from the cache.
		/** After loading a mesh with getMesh(), the mesh can be removed from the cache
		using this method, freeing a lot of memory. */
		virtual void removeMesh(const IMesh* const mesh) = 0;

		//! Returns amount of loaded meshes in the cache.
		/** You can load new meshes into the cache using getMesh() and addMesh().
		If you ever need to access the internal mesh cache, you can do this using
		removeMesh(), getMeshNumber(), getMeshByIndex() and getMeshFilename() */
		virtual u32 getMeshCount() const = 0;

		//! Returns current index number of the mesh, and -1 if it is not in the cache.
		virtual s32 getMeshIndex(const IAnimatedMesh* const mesh) const = 0;

		//! Returns current index number of the mesh, and -1 if it is not in the cache.
		virtual s32 getMeshIndex(const IMesh* const mesh) const = 0;

		//! Returns a mesh based on its index number.
		/** \param index: Index of the mesh, number between 0 and getMeshCount()-1.
		Note that this number is only valid until a new mesh is loaded or removed *
		\return Returns pointer to the mesh or 0 if there is none with this number. */
		virtual IAnimatedMesh* getMeshByIndex(u32 index) = 0;

		//! Returns a mesh based on its file name.
		/** \return Returns pointer to the mesh or 0 if there is none with this number. */
		virtual IAnimatedMesh* getMeshByFilename(const c8* filename) = 0;

		//! Returns name of a mesh based on its index number.
		/** \param index: Index of the mesh, number between 0 and getMeshCount()-1.
		Note that this is only valid until a new mesh is loaded */
		virtual const c8* getMeshFilename(u32 index) const = 0;

		//! Returns the filename of a loaded mesh, if there is any.
		/** Returns 0 if there is none. */
		virtual const c8* getMeshFilename(const IAnimatedMesh* const mesh) const = 0;

		//! Returns the filename of a loaded mesh, if there is any.
		/* Returns 0 if there is none.*/
		virtual const c8* getMeshFilename(const IMesh* const mesh) const = 0;

		//! Renames a loaded mesh, if possible.
		/** Returns true if sucessful. Note that renaming meshes might change
		the ordering of the meshes, and so the index of the meshes as returned by
		getMeshIndex() or taken by some methods will change. */
		virtual bool setMeshFilename(u32 index, const c8* filename) = 0;

		//! Renames a loaded mesh, if possible.
		/** Returns true if sucessful. Note that renaming meshes might change
		the ordering of the meshes, and so the index of the meshes as returned by
		getMeshIndex() or taken by some methods will change. */
		virtual bool setMeshFilename(const IAnimatedMesh* const mesh, const c8* filename) = 0;

		//! Renames a loaded mesh, if possible.
		/** Returns true if sucessful. Note that renaming meshes might change
		the ordering of the meshes, and so the index of the meshes as returned by
		getMeshIndex() or taken by some methods will change. */
		virtual bool setMeshFilename(const IMesh* const mesh, const c8* filename) = 0;

		//! returns if a mesh already was loaded
		virtual bool isMeshLoaded(const c8* filename) = 0;

		//! Clears the whole mesh cache, removing all meshes.
		/** All meshes will be reloaded completely when using ISceneManager::getMesh()
		after calling this method. */
		virtual void clear() = 0;
	};


} // end namespace scene
} // end namespace irr

#endif

