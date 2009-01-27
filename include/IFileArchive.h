// Copyright (C) 2002-2009 Nikolaus Gebhardt/ Thomas Alten
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_FILE_ARCHIVE_H_INCLUDED__
#define __I_FILE_ARCHIVE_H_INCLUDED__

#include "IReadFile.h"

namespace irr
{

namespace io
{

//! FileSystemType: which Filesystem should be used for e.q browsing
enum eFileSystemType
{
	FILESYSTEM_NATIVE = 0,		// Native OS FileSystem
	FILESYSTEM_VIRTUAL,			// Virtual FileSystem
};


//! Base Info which all File archives must support on browsing
struct IFileArchiveEntry
{
	IFileArchiveEntry () {}

	core::string<c16> simpleFileName;
	core::string<c16> path;

	bool operator < (const IFileArchiveEntry& other) const
	{
		return simpleFileName < other.simpleFileName;
	}

	bool operator == (const IFileArchiveEntry& other) const
	{
		return simpleFileName == other.simpleFileName;
	}
};

//! The FileArchive manages files and archives and provides access to them.
/** It manages where files are, so that modules which use the the IO do not
need to know where every file is located. A file could be in a .zip-Archive or
as file on disk, using the IFileSystem makes no difference to this. */
struct IFileArchive : public virtual IReferenceCounted
{

	//! return the id of the file Archive
	virtual const core::string<c16>& getArchiveName () = 0;

	//! get the class Type
	virtual const core::string<c16>& getArchiveType() = 0;

	//! opens a file by file name
	virtual IReadFile* openFile(const core::string<c16>& filename) = 0;

	//! returns fileindex
	virtual s32 findFile(const core::string<c16>& filename) = 0;

	//! Returns the amount of files in the filelist.
	/** \return Amount of files and directories in the file list. */
	virtual u32 getFileCount() = 0;

	//! returns data of known file
	virtual const IFileArchiveEntry* getFileInfo(u32 index) = 0;

};

//! Class which is able to create an archive from a file.
/** If you want the Irrlicht Engine be able to load archives of
currently unsupported file formats (e.g .wad), then implement
this and add your new Archive loader with
IFileSystem::addArchiveLoader() to the engine. */
struct IArchiveLoader : public virtual IReferenceCounted
{
	//! Check if the file might be loaded by this class
	/** Check is based on the file extension (e.g. ".zip")
	\param fileName Name of file to check.
	\return True if file seems to be loadable. */
	virtual bool isALoadableFileFormat(const core::string<c16>& filename) const = 0;

	//! Creates an archive from the filename
	/** \param file File handle to check.
	\return Pointer to newly created archive, or 0 upon error. */
	virtual IFileArchive* createArchive(const core::string<c16>& filename, bool ignoreCase, bool ignorePaths) const = 0;

	//! Check if the file might be loaded by this class
	/** Check might look into the file.
	\param file File handle to check.
	\return True if file seems to be loadable. */
	virtual bool isALoadableFileFormat(io::IReadFile* file) const = 0;

	//! Creates an archive from the file
	/** \param file File handle to check.
	\return Pointer to newly created archive, or 0 upon error. */
	virtual IFileArchive* createArchive(io::IReadFile* file, bool ignoreCase, bool ignorePaths) const = 0;


};


} // end namespace io
} // end namespace irr


#endif

