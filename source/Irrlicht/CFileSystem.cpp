// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CFileSystem.h"
#include "IReadFile.h"
#include "IWriteFile.h"
#include "CZipReader.h"
#include "CPakReader.h"
#include "CFileList.h"
#include "CXMLReader.h"
#include "CXMLWriter.h"
#include "stdio.h"
#include "os.h"
#include "IrrCompileConfig.h"
#include "CAttributes.h"
#include "CMemoryReadFile.h"

#if defined (_IRR_WINDOWS_API_)
	#if !defined ( _WIN32_WCE )
		#include <direct.h> // for _chdir
	#endif
#else
	#include <unistd.h>
	#include <limits.h>
	#include <stdlib.h>
#endif

namespace irr
{
namespace io
{


//! constructor
CFileSystem::CFileSystem()
{
	#ifdef _DEBUG
	setDebugName("CFileSystem");
	#endif
}


//! destructor
CFileSystem::~CFileSystem()
{
	u32 i;

	for ( i=0; i<ZipFileSystems.size(); ++i)
		ZipFileSystems[i]->drop();

	for ( i=0; i<PakFileSystems.size(); ++i)
		PakFileSystems[i]->drop();

	for ( i= 0; i<UnZipFileSystems.size(); ++i)
		UnZipFileSystems[i]->drop();
}


//! opens a file for read access
IReadFile* CFileSystem::createAndOpenFile(const c8* filename)
{
	IReadFile* file = 0;
	u32 i;

	for ( i=0; i<ZipFileSystems.size(); ++i)
	{
		file = ZipFileSystems[i]->openFile(filename);
		if (file)
			return file;
	}

	for ( i = 0; i<PakFileSystems.size(); ++i)
	{
		file = PakFileSystems[i]->openFile(filename);
		if (file)
			return file;
	}

	for ( i = 0; i<UnZipFileSystems.size(); ++i)
	{
		file = UnZipFileSystems[i]->openFile(filename);
		if (file)
			return file;
	}

	// Create the file using an absolute path so that it matches
	// the scheme used by CNullDriver::getTexture().
	return createReadFile(getAbsolutePath(filename).c_str());
}


//! Creates an IReadFile interface for treating memory like a file.
IReadFile* CFileSystem::createMemoryReadFile(void* memory, s32 len, 
			const c8* fileName, bool deleteMemoryWhenDropped)
{
	if (!memory)
		return 0;
	else
		return new CMemoryReadFile(memory, len, fileName, deleteMemoryWhenDropped);
}


//! Opens a file for write access.
IWriteFile* CFileSystem::createAndWriteFile(const c8* filename, bool append)
{
	return createWriteFile(filename, append);
}


bool CFileSystem::addFolderFileArchive(const c8* filename, bool ignoreCase, bool ignorePaths)
{
	CUnZipReader* zr = new CUnZipReader(this, filename, ignoreCase, ignorePaths);
	if (zr)
		UnZipFileSystems.push_back(zr);
	#ifdef _DEBUG
	else
	{
		os::Printer::log("Could not open file. Folderfile not added", filename, ELL_ERROR);
	}
	#endif

	_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
	return (zr!=0);
}


//! adds an zip archive to the filesystem
bool CFileSystem::addZipFileArchive(const c8* filename, bool ignoreCase, bool ignorePaths)
{
	CZipReader* zr = 0;
	IReadFile* file = createAndOpenFile(filename);
	if (file)
	{
		zr = new CZipReader(file, ignoreCase, ignorePaths);
		if (zr)
			ZipFileSystems.push_back(zr);

		file->drop();
	}
	#ifdef _DEBUG
	else
		os::Printer::log("Could not open file. Zipfile not added", filename, ELL_ERROR);
	#endif

	_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
	return (zr != 0);
}


//! adds an pak archive to the filesystem
bool CFileSystem::addPakFileArchive(const c8* filename, bool ignoreCase, bool ignorePaths)
{
	CPakReader* zr = 0;
	IReadFile* file = createAndOpenFile(filename);
	if (file)
	{
		zr = new CPakReader(file, ignoreCase, ignorePaths);
		if (zr)
			PakFileSystems.push_back(zr);

		file->drop();
	}
	#ifdef _DEBUG
	else
		os::Printer::log("Could not open file. Pakfile not added", filename, ELL_ERROR);
	#endif

	_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
	return (zr != 0);
}


//! Returns the string of the current working directory
const c8* CFileSystem::getWorkingDirectory()
{
#ifdef _IRR_WINDOWS_API_
	#if !defined ( _WIN32_WCE )
		_getcwd(WorkingDirectory, FILE_SYSTEM_MAX_PATH);
	#endif
#endif

#if (defined(_IRR_POSIX_API_) || defined(_IRR_OSX_PLATFORM_))
	getcwd(WorkingDirectory, (size_t)FILE_SYSTEM_MAX_PATH);
#endif
	return WorkingDirectory;
}


//! Changes the current Working Directory to the given string.
//! The string is operating system dependent. Under Windows it will look
//! like this: "drive:\directory\sudirectory\"
//! \return Returns true if successful, otherwise false.
bool CFileSystem::changeWorkingDirectoryTo(const c8* newDirectory)
{
	bool success=false;
#ifdef _MSC_VER
	#if !defined ( _WIN32_WCE )
		success=(_chdir(newDirectory) == 0);
	#endif
#else
	success=(chdir(newDirectory) == 0);
#endif
	return success;
}


core::stringc CFileSystem::getAbsolutePath(const core::stringc& filename) const
{
	c8 *p=0;

#ifdef _IRR_WINDOWS_API_
	#if !defined ( _WIN32_WCE )
		c8 fpath[_MAX_PATH];
		p = _fullpath( fpath, filename.c_str(), _MAX_PATH);
	#endif

#elif (defined(_IRR_POSIX_API_) || defined(_IRR_OSX_PLATFORM_))
	c8 fpath[4096];
	p = realpath(filename.c_str(), fpath);
	if (!p)
		return filename;
#endif

	return core::stringc(p);
}


//! returns the directory part of a filename, i.e. all until the first
//! slash or backslash, excluding it. If no directory path is prefixed, a '.'
//! is returned.
core::stringc CFileSystem::getFileDir(const core::stringc& filename) const
{
	// find last forward or backslash
	s32 lastSlash = filename.findLast('/');
	const s32 lastBackSlash = filename.findLast('\\');
	lastSlash = lastSlash > lastBackSlash ? lastSlash : lastBackSlash;

	if ((u32)lastSlash < filename.size())
		return filename.subString(0, lastSlash);
	else
		return ".";
}


//! returns the base part of a filename, i.e. all except for the directory
//! part. If no directory path is prefixed, the full name is returned.
core::stringc CFileSystem::getFileBasename(const core::stringc& filename, bool keepExtension) const
{
	// find last forward or backslash
	s32 lastSlash = filename.findLast('/');
	const s32 lastBackSlash = filename.findLast('\\');
	lastSlash = core::max_(lastSlash, lastBackSlash);
	s32 end = 0;
	if (!keepExtension)
	{
		end = filename.findLast('.');
		if (end == -1)
			end=0;
		else
			end = filename.size()-end;
	}

	if ((u32)lastSlash < filename.size())
		return filename.subString(lastSlash+1, filename.size()-lastSlash-1-end);
	else if (end != 0)
		return filename.subString(0, filename.size()-end);
	else
		return filename;
}


//! Creates a list of files and directories in the current working directory 
IFileList* CFileSystem::createFileList() const
{
	return new CFileList();
}


//! determines if a file exists and would be able to be opened.
bool CFileSystem::existFile(const c8* filename) const
{
	u32 i;

	for (i=0; i<ZipFileSystems.size(); ++i)
		if (ZipFileSystems[i]->findFile(filename)!=-1)
			return true;

	for (i=0; i<PakFileSystems.size(); ++i)
		if (PakFileSystems[i]->findFile(filename)!=-1)
			return true;

	for (i=0; i<UnZipFileSystems.size(); ++i)
		if (UnZipFileSystems[i]->findFile(filename)!=-1)
			return true;

	FILE* f = fopen(filename, "rb");
	if (f) 
	{
		fclose(f);
		return true;
	}

	_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
	return false;
}


//! Creates a XML Reader from a file.
IXMLReader* CFileSystem::createXMLReader(const c8* filename)
{
	IReadFile* file = createAndOpenFile(filename);
	if (!file)
		return 0;

	IXMLReader* reader = createXMLReader(file);
	file->drop();
	return reader;
}


//! Creates a XML Reader from a file.
IXMLReader* CFileSystem::createXMLReader(IReadFile* file)
{
	if (!file)
		return 0;

	return createIXMLReader(file);
}


//! Creates a XML Reader from a file.
IXMLReaderUTF8* CFileSystem::createXMLReaderUTF8(const c8* filename)
{
	IReadFile* file = createAndOpenFile(filename);
	if (!file)
		return 0;

	IXMLReaderUTF8* reader = createIXMLReaderUTF8(file);
	file->drop();
	return reader;
}


//! Creates a XML Reader from a file.
IXMLReaderUTF8* CFileSystem::createXMLReaderUTF8(IReadFile* file)
{
	if (!file)
		return 0;

	return createIXMLReaderUTF8(file);
}


//! Creates a XML Writer from a file.
IXMLWriter* CFileSystem::createXMLWriter(const c8* filename)
{
	IWriteFile* file = createAndWriteFile(filename);
	IXMLWriter* writer = createXMLWriter(file);
	file->drop();
	return writer;
}


//! Creates a XML Writer from a file.
IXMLWriter* CFileSystem::createXMLWriter(IWriteFile* file)
{
	return new CXMLWriter(file);
}


//! creates a filesystem which is able to open files from the ordinary file system,
//! and out of zipfiles, which are able to be added to the filesystem.
IFileSystem* createFileSystem()
{
	return new CFileSystem();
}


//! Creates a new empty collection of attributes, usable for serialization and more.
IAttributes* CFileSystem::createEmptyAttributes(video::IVideoDriver* driver)
{
	return new CAttributes(driver);
}


} // end namespace irr
} // end namespace io

