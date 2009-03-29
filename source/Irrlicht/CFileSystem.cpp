// Copyright (C) 2002-2009 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "IrrCompileConfig.h"

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
#include "CAttributes.h"
#include "CMemoryFile.h"
#include "CLimitReadFile.h"

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

	setFileListSystem(FILESYSTEM_NATIVE);

	addArchiveLoader(new CArchiveLoaderZIP(this));
	addArchiveLoader(new CArchiveLoaderMount(this));
	addArchiveLoader(new CArchiveLoaderPAK(this));
}


//! destructor
CFileSystem::~CFileSystem()
{
	u32 i;

	for ( i=0; i < FileArchives.size(); ++i)
	{
		FileArchives[i]->drop();
	}

	for ( i=0; i < ArchiveLoader.size(); ++i)
	{
		ArchiveLoader[i]->drop();
	}
}


//! opens a file for read access
IReadFile* CFileSystem::createAndOpenFile(const core::string<c16>& filename)
{
	IReadFile* file = 0;
	u32 i;

	for (i=0; i< FileArchives.size(); ++i)
	{
		file = FileArchives[i]->openFile(filename);
		if (file)
			return file;
	}

	// Create the file using an absolute path so that it matches
	// the scheme used by CNullDriver::getTexture().
	return createReadFile(getAbsolutePath(filename));
}


//! Creates an IReadFile interface for treating memory like a file.
IReadFile* CFileSystem::createMemoryReadFile(void* memory, s32 len,
		const core::string<c16>& fileName, bool deleteMemoryWhenDropped)
{
	if (!memory)
		return 0;
	else
		return new CMemoryFile(memory, len, fileName, deleteMemoryWhenDropped);
}


//! Creates an IReadFile interface for reading files inside files
IReadFile* CFileSystem::createLimitReadFile(const core::string<c16>& fileName,
		IReadFile* alreadyOpenedFile, long pos, long areaSize)
{
	if (!alreadyOpenedFile)
		return 0;
	else
		return new CLimitReadFile(alreadyOpenedFile, pos, areaSize, fileName);
}


//! Creates an IReadFile interface for treating memory like a file.
IWriteFile* CFileSystem::createMemoryWriteFile(void* memory, s32 len,
		const core::string<c16>& fileName, bool deleteMemoryWhenDropped)
{
	if (!memory)
		return 0;
	else
		return new CMemoryFile(memory, len, fileName, deleteMemoryWhenDropped);
}


//! Opens a file for write access.
IWriteFile* CFileSystem::createAndWriteFile(const core::string<c16>& filename, bool append)
{
	return createWriteFile(filename, append);
}


//! Adds an external archive loader to the engine.
void CFileSystem::addArchiveLoader(IArchiveLoader* loader)
{
	if (!loader)
		return;

	//loader->grab();
	ArchiveLoader.push_back(loader);
}


//! move the hirarchy of the filesystem. moves sourceIndex relative up or down
bool CFileSystem::moveFileArchive(u32 sourceIndex, s32 relative)
{
	bool r = false;
	const s32 dest = (s32) sourceIndex + relative;
	const s32 dir = relative < 0 ? -1 : 1;
	const s32 sourceEnd = ((s32) FileArchives.size() ) - 1;
	IFileArchive *t;

	for (s32 s = (s32) sourceIndex;s != dest; s += dir)
	{
		if (s < 0 || s > sourceEnd || s + dir < 0 || s + dir > sourceEnd)
			continue;

		t = FileArchives[s + dir];
		FileArchives[s + dir] = FileArchives[s];
		FileArchives[s] = t;
		r = true;
	}
	return r;
}


//! Adds an archive to the file system.
bool CFileSystem::registerFileArchive(const core::string<c16>& filename, bool ignoreCase, bool ignorePaths)
{
	IFileArchive* archive = 0;
	bool ret = false;
	u32 i;

	// check if the archive was already loaded
	for (i = 0; i < FileArchives.size(); ++i)
	{
		if (filename == FileArchives[i]->getArchiveName())
			return true;
	}

	// try to load archive based on file name
	for (i = 0; i < ArchiveLoader.size(); ++i)
	{
		if (ArchiveLoader[i]->isALoadableFileFormat(filename))
		{
			archive = ArchiveLoader[i]->createArchive(filename, ignoreCase, ignorePaths);
			if (archive)
				break;
		}
	}

	// try to load archive based on content
	if (0 == archive)
	{
		io::IReadFile* file = createAndOpenFile(filename);
		if (file)
		{
			for (i = 0; i < ArchiveLoader.size(); ++i)
			{
				file->seek(0);
				if (ArchiveLoader[i]->isALoadableFileFormat(file))
				{
					file->seek(0);
					archive = ArchiveLoader[i]->createArchive(file, ignoreCase, ignorePaths);
					if (archive)
						break;
				}
			}
			file->drop ();
		}
	}

	if (archive)
	{
		FileArchives.push_back(archive);
		ret = true;
	}
	else
	{
		os::Printer::log("Could not create archive for", filename, ELL_ERROR);
	}

	_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
	return ret;
}


//! removes an archive from the file system.
bool CFileSystem::unregisterFileArchive(u32 index)
{
	bool ret = false;
	if (index < FileArchives.size())
	{
		FileArchives[index]->drop();
		FileArchives.erase(index);
		ret = true;
	}
	_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
	return ret;
}


//! removes an archive from the file system.
bool CFileSystem::unregisterFileArchive(const core::string<c16>& filename)
{
	for (u32 i=0; i < FileArchives.size(); ++i)
	{
		if (filename == FileArchives[i]->getArchiveName())
			return unregisterFileArchive(i);
	}
	_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
	return false;
}


//! gets an archive
u32 CFileSystem::getFileArchiveCount() const
{
	return FileArchives.size();
}


IFileArchive* CFileSystem::getFileArchive(u32 index)
{
	return index < getFileArchiveCount() ? FileArchives[index] : 0;
}


//! Returns the string of the current working directory
const core::string<c16>& CFileSystem::getWorkingDirectory()
{
	EFileSystemType type = FileSystemType;

	if (type != FILESYSTEM_NATIVE)
	{
		type = FILESYSTEM_VIRTUAL;
	}
	else
	{
		const s32 FILE_SYSTEM_MAX_PATH = 1024;
		WorkingDirectory[type].reserve(FILE_SYSTEM_MAX_PATH);
		c16* r = (c16*) WorkingDirectory[type].c_str();

		#if defined(_IRR_USE_WINDOWS_CE_DEVICE_)
		#elif defined(_IRR_WINDOWS_API_)
			#if defined(_IRR_WCHAR_FILESYSTEM )
				_wgetcwd(r, FILE_SYSTEM_MAX_PATH);
			#else
				_getcwd(r, FILE_SYSTEM_MAX_PATH);
			#endif
		#endif

		#if (defined(_IRR_POSIX_API_) || defined(_IRR_OSX_PLATFORM_))

			#if defined(_IRR_WCHAR_FILESYSTEM )
				wgetcwd(r, FILE_SYSTEM_MAX_PATH);
			#else
				getcwd(r, (size_t)FILE_SYSTEM_MAX_PATH);
			#endif
		#endif

		WorkingDirectory[type].validate();
	}

	return WorkingDirectory[type];
}


//! Changes the current Working Directory to the given string.
bool CFileSystem::changeWorkingDirectoryTo(const core::string<c16>& newDirectory)
{
	bool success=false;

	if (FileSystemType != FILESYSTEM_NATIVE)
	{
		WorkingDirectory[FILESYSTEM_VIRTUAL].append(newDirectory);
		flattenFilename(WorkingDirectory[FILESYSTEM_VIRTUAL], "");
		success = 1;
	}
	else
	{
		WorkingDirectory[FILESYSTEM_NATIVE] = newDirectory;

#if defined(_IRR_USE_WINDOWS_CE_DEVICE_)
		success = true;
#elif defined(_MSC_VER)
	#if defined(_IRR_WCHAR_FILESYSTEM)
		success=(_wchdir(newDirectory.c_str()) == 0);
	#else
		success=(_chdir(newDirectory.c_str()) == 0);
	#endif
#else
		success=(chdir(newDirectory.c_str()) == 0);
#endif
	}

	return success;
}


core::string<c16> CFileSystem::getAbsolutePath(const core::string<c16>& filename) const
{
	c16 *p=0;

#if defined(_IRR_USE_WINDOWS_CE_DEVICE_)
	return filename;
#elif defined(_IRR_WINDOWS_API_)

	#if defined(_IRR_WCHAR_FILESYSTEM )
		c16 fpath[_MAX_PATH];
		p = _wfullpath(fpath, filename.c_str(), _MAX_PATH);
	#else
		c8 fpath[_MAX_PATH];
		p = _fullpath(fpath, filename.c_str(), _MAX_PATH);
	#endif

#elif (defined(_IRR_POSIX_API_) || defined(_IRR_OSX_PLATFORM_))
	c8 fpath[4096];
	fpath[0]=0;
	p = realpath(filename.c_str(), fpath);
	if (!p)
	{
		// content in fpath is undefined at this point
		if ('0'==fpath[0]) // seems like fpath wasn't altered
		{
			// at least remove a ./ prefix
			if ('.'==filename[0] && '/'==filename[1])
				return filename.subString(2, filename.size()-2);
			else
				return filename;
		}
		else
			return core::string<c16>(fpath);
	}

#endif

	return core::string<c16>(p);
}


//! returns the directory part of a filename, i.e. all until the first
//! slash or backslash, excluding it. If no directory path is prefixed, a '.'
//! is returned.
core::string<c16> CFileSystem::getFileDir(const core::string<c16>& filename) const
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
core::string<c16> CFileSystem::getFileBasename(const core::string<c16>& filename, bool keepExtension) const
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


//! flaten a path and file name for example: "/you/me/../." becomes "/you"
core::string<c16>& CFileSystem::flattenFilename(core::string<c16>& directory, const core::string<c16>& root) const
{
	directory.replace('\\', '/');
	if (lastChar(directory) != '/')
		directory.append('/');

	core::string<c16> dir;
	core::string<c16> subdir;

	s32 lastpos = 0;
	s32 pos = 0;

	while ((pos = directory.findNext('/', lastpos)) >= 0)
	{
		subdir = directory.subString(lastpos, pos - lastpos + 1);

		if (subdir == "../")
		{
			deletePathFromPath(dir, 2);
		}
		else if (subdir == "/")
		{
			dir = root;
		}
		else if (subdir != "./" )
		{
			dir.append(subdir);
		}

		lastpos = pos + 1;
	}
	directory = dir;
	return directory;
}


//! Creates a list of files and directories in the current working directory
EFileSystemType CFileSystem::setFileListSystem(EFileSystemType listType)
{
	EFileSystemType current = FileSystemType;
	FileSystemType = listType;
	return current;
}


//! Creates a list of files and directories in the current working directory
IFileList* CFileSystem::createFileList()
{
	FileEntry e2;
	FileEntry e3;

	if ( FileSystemType == FILESYSTEM_NATIVE )
		return new CFileList();

	CFileList* r = new CFileList( "virtual" );
	r->Path = WorkingDirectory [ FILESYSTEM_VIRTUAL ];

	for ( u32 i = 0; i != FileArchives.size(); ++i)
	{
		CFileList* flist[2] = { 0, 0 };

		//! merge relative folder file archives
		if ( FileArchives[i]->getArchiveType() == "mount" )
		{
			EFileSystemType currentType = setFileListSystem ( FILESYSTEM_NATIVE );

			core::string<c16> save ( getWorkingDirectory () );
			core::string<c16> current;

			current = FileArchives[i]->getArchiveName() + WorkingDirectory [ FILESYSTEM_VIRTUAL ];
			flattenFilename ( current );

			if ( changeWorkingDirectoryTo ( current ) )
			{
				flist[0] = new CFileList( "mountpoint" );
				flist[0]->constructNative ();
				changeWorkingDirectoryTo ( save );
			}

			setFileListSystem ( currentType );
		}
		else
		if ( FileArchives[i]->getArchiveType() == "zip" )
		{
			flist[0] = new CFileList( "zip" );
			flist[1] = new CFileList( "zip directory" );

			// add relative navigation
			e2.isDirectory = 1;
			e2.Name = ".";
			e2.FullName = r->Path + e2.Name;
			e2.Size = 0;
			flist[1]->Files.push_back ( e2 );

			e2.Name = "..";
			e2.FullName = r->Path + e2.Name;
			flist[1]->Files.push_back ( e2 );

			for ( u32 g = 0; g < FileArchives[i]->getFileCount(); ++g)
			{
				const SZipFileEntry *e = (SZipFileEntry*) FileArchives[i]->getFileInfo(g);
				s32 test = isInSameDirectory ( r->Path, e->zipFileName );
				if ( test < 0 || test > 1 )
					continue;

				e2.Size = e->header.DataDescriptor.UncompressedSize;
				e2.isDirectory = e2.Size == 0;

				// check missing directories
				if ( !e2.isDirectory && test == 1 )
				{
					e3.Size = 0;
					e3.isDirectory = 1;
					e3.FullName = e->path;
					e3.Name = e->path.subString ( r->Path.size(), e->path.size() - r->Path.size() - 1 );

					if ( flist[1]->Files.binary_search ( e3 ) < 0 )
						flist[1]->Files.push_back ( e3 );
				}
				else
				{
					e2.FullName = e->zipFileName;
					e2.Name = e->simpleFileName;

					if ( !e2.isDirectory )
						core::deletePathFromFilename ( e2.Name );
					flist[0]->Files.push_back ( e2 );
				}
			}
		}

		// add file to virtual directory
		for ( u32 g = 0; g < 2; ++g )
		{
			if ( !flist[g] )
				continue;
			for ( u32 j = 0; j != flist[g]->Files.size(); ++j )
				r->Files.push_back ( flist[g]->Files[j] );

			flist[g]->drop();
		}
	}

	r->Files.sort();
	return r;
}


//! determines if a file exists and would be able to be opened.
bool CFileSystem::existFile(const core::string<c16>& filename) const
{
	for (u32 i=0; i < FileArchives.size(); ++i)
		if ( FileArchives[i]->findFile(filename)!=-1)
			return true;

#if defined ( _IRR_WCHAR_FILESYSTEM )
	FILE* f = _wfopen(filename.c_str(), L"rb");
#else
	FILE* f = fopen(filename.c_str(), "rb");
#endif

	if (f)
	{
		fclose(f);
		return true;
	}

	_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
	return false;
}


//! Creates a XML Reader from a file.
IXMLReader* CFileSystem::createXMLReader(const core::string<c16>& filename)
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
IXMLReaderUTF8* CFileSystem::createXMLReaderUTF8(const core::string<c16>& filename)
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
IXMLWriter* CFileSystem::createXMLWriter(const core::string<c16>& filename)
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

