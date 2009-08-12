// Copyright (C) 2002-2009 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_FILE_LIST_H_INCLUDED__
#define __I_FILE_LIST_H_INCLUDED__

#include "IReferenceCounted.h"
#include "irrString.h"

namespace irr
{
namespace io
{

//! Provides a list of files and folders.
/** File lists usually contain a list of all files in a given folder,
but can also contain a complete directory structure. */
class IFileList : public virtual IReferenceCounted
{
public:
	//! Get the number of files in the filelist.
	/** \return Amount of files and directories in the file list. */
	virtual u32 getFileCount() const = 0;

	//! Gets the name of a file in the list, based on an index.
	/** The path is not included in this name. Use getFullFileName for this.
	\param index is the zero based index of the file which name should
	be returned. The index must be less than the amount getFileCount() returns.
	\return File name of the file. Returns 0, if an error occured. */
	virtual const core::string<c16>& getFileName(u32 index) const = 0;

	//! Gets the full name of a file in the list including the path, based on an index.
	/** \param index is the zero based index of the file which name should
	be returned. The index must be less than the amount getFileCount() returns.
	\return File name of the file. Returns 0, if an error occured. */
	virtual const core::string<c16>& getFullFileName(u32 index) const = 0;

	//! Returns the size of a file in the file list, based on an index.
	/** \param index is the zero based index of the file which should be returned.
	The index must be less than the amount getFileCount() returns.
	\return The size of the file in bytes. */
	virtual u32 getFileSize(u32 index) const = 0;

	//! Check if the file is a directory
	/** \param index The zero based index which will be checked. The index
	must be less than the amount getFileCount() returns.
	\return True if the file is a directory, else false. */
	virtual bool isDirectory(u32 index) const = 0;

	//! Searches for a file or folder in the list
	/** Searches for a file by name
	\param filename The name of the file to search for.
	\param isFolder True if you are searching for a file, false if you want a dir.
	\return Returns the index of the file in the file list, or -1 if
	no matching name name was found. */
	virtual s32 findFile(const core::string<c16>& filename, bool isFolder=false) const = 0;

	//! Returns the base path of the file list
	virtual const core::string<c16>& getPath() const = 0;
};

} // end namespace irr
} // end namespace io


#endif

