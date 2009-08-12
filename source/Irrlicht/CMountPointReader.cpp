// Copyright (C) 2002-2009 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CMountPointReader.h"
#include "CReadFile.h"

#include "os.h"

namespace irr
{
namespace io
{

//! Constructor
CArchiveLoaderMount::CArchiveLoaderMount( io::IFileSystem* fs)
: FileSystem(fs)
{
	#ifdef _DEBUG
	setDebugName("CArchiveLoaderMount");
	#endif
}


//! destructor
CArchiveLoaderMount::~CArchiveLoaderMount()
{
}


//! returns true if the file maybe is able to be loaded by this class
bool CArchiveLoaderMount::isALoadableFileFormat(const core::string<c16>& filename) const
{
	bool ret = false;
	core::string<c16> fname(filename);
	deletePathFromFilename(fname);

	if (!fname.size())
	{
		ret = true;
	}

	return ret;
}

//! Check to see if the loader can create archives of this type.
bool CArchiveLoaderMount::isALoadableFileFormat(E_FILE_ARCHIVE_TYPE fileType) const
{
	return fileType == EFAT_FOLDER;
}

//! Check if the file might be loaded by this class
bool CArchiveLoaderMount::isALoadableFileFormat(io::IReadFile* file) const
{
	return false;
}

//! Creates an archive from the filename
IFileArchive* CArchiveLoaderMount::createArchive(const core::string<c16>& filename, bool ignoreCase, bool ignorePaths) const
{
	IFileArchive *archive = 0;

	EFileSystemType current = FileSystem->setFileListSystem(FILESYSTEM_NATIVE);

	core::string<c16> save = FileSystem->getWorkingDirectory();
	core::string<c16> fullPath = FileSystem->getAbsolutePath(filename);
	FileSystem->flattenFilename(fullPath);

	if ( FileSystem->changeWorkingDirectoryTo ( fullPath ) )
	{
		archive = new CMountPointReader(FileSystem, fullPath, ignoreCase, ignorePaths);
	}

	FileSystem->changeWorkingDirectoryTo(save);
	FileSystem->setFileListSystem(current);

	return archive;
}

//! creates/loads an archive from the file.
//! \return Pointer to the created archive. Returns 0 if loading failed.
IFileArchive* CArchiveLoaderMount::createArchive(io::IReadFile* file, bool ignoreCase, bool ignorePaths) const
{
	return 0;
}

//! compatible Folder Archticture
//
CMountPointReader::CMountPointReader( IFileSystem * parent, const core::string<c16>& basename, bool ignoreCase, bool ignorePaths)
	: CFileList(basename, ignoreCase, ignorePaths), Parent(parent)
{
	//! ensure CFileList path ends in a slash
	if (core::lastChar(Path) != '/' )
		Path.append ('/');

	core::string<c16> work = Parent->getWorkingDirectory();

	Parent->changeWorkingDirectoryTo(basename);
	buildDirectory();
	Parent->changeWorkingDirectoryTo(work);

	sort();
}

CMountPointReader::~CMountPointReader()
{

}

//! returns the list of files
const IFileList* CMountPointReader::getFileList() const
{
	return this;
}

void CMountPointReader::buildDirectory()
{
	IFileList * list = Parent->createFileList();

	const u32 size = list->getFileCount();
	for (u32 i = 0; i!= size; ++i)
	{
		if (!list->isDirectory(i))
		{
			addItem(list->getFullFileName(i), list->getFileSize(i), false, RealFileNames.size());
			RealFileNames.push_back(list->getFullFileName(i));
		}
		else
		{
			const core::string<c16>& full = list->getFullFileName(i);
			const core::string<c16> rel = list->getFileName(i);
			core::string<c16> pwd  = Parent->getWorkingDirectory() + "/";
			pwd += rel;

			if ( rel != "." && rel != ".." )
			{
				addItem(full, 0, true, 0);
				Parent->changeWorkingDirectoryTo(pwd);
				buildDirectory ();
				Parent->changeWorkingDirectoryTo("..");
			}
		}
	}

	list->drop();
}

//! opens a file by index
IReadFile* CMountPointReader::createAndOpenFile(u32 index)
{
	if (index >= Files.size())
		return 0;

	return createReadFile( RealFileNames[Files[index].ID] );
}

//! opens a file by file name
IReadFile* CMountPointReader::createAndOpenFile(const core::string<c16>& filename)
{
	s32 index = findFile(filename, false);

	if (index == -1)
		return 0;

	return createReadFile( RealFileNames[Files[index].ID] );
}


} // io
} // irr
