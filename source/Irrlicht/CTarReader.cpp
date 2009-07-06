// Copyright (C) 2009 Gaz Davidson
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CTarReader.h"
#include "CFileList.h"
#include "CLimitReadFile.h"
#include "os.h"
#include "coreutil.h"

#include "IrrCompileConfig.h"

namespace irr
{
namespace io
{

//! Constructor
CArchiveLoaderTAR::CArchiveLoaderTAR(io::IFileSystem* fs)
: FileSystem(fs)
{
	#ifdef _DEBUG
	setDebugName("CArchiveLoaderTAR");
	#endif
}


//! destructor
CArchiveLoaderTAR::~CArchiveLoaderTAR()
{
}


//! returns true if the file maybe is able to be loaded by this class
bool CArchiveLoaderTAR::isALoadableFileFormat(const core::string<c16>& filename) const
{
	return core::hasFileExtension(filename, "tar");
}


//! Creates an archive from the filename
/** \param file File handle to check.
\return Pointer to newly created archive, or 0 upon error. */
IFileArchive* CArchiveLoaderTAR::createArchive(const core::string<c16>& filename, bool ignoreCase, bool ignorePaths) const
{
	IFileArchive *archive = 0;
	io::IReadFile* file = FileSystem->createAndOpenFile(filename);

	if (file)
	{
		archive = createArchive(file, ignoreCase, ignorePaths);
		file->drop();
	}

	return archive;
}

//! creates/loads an archive from the file.
//! \return Pointer to the created archive. Returns 0 if loading failed.
IFileArchive* CArchiveLoaderTAR::createArchive(io::IReadFile* file, bool ignoreCase, bool ignorePaths) const
{
	IFileArchive *archive = 0;
	if (file)
	{
		file->seek(0);
		archive = new CTarReader(file, ignoreCase, ignorePaths);
	}
	return archive;
}

//! Check if the file might be loaded by this class
/** Check might look into the file.
\param file File handle to check.
\return True if file seems to be loadable. */
bool CArchiveLoaderTAR::isALoadableFileFormat(io::IReadFile* file) const
{
	// TAR files consist of blocks of 512 bytes
	// if it isn't a multiple of 512 then it's not a TAR file.
	if (file->getSize() % 512)
		return false;

	file->seek(0);

	// read header of first file
	STarHeader fHead;
	file->read(&fHead, sizeof(STarHeader));

#ifdef __BIG_ENDIAN__	
	for (u32* p = (u32*)&fHead; p < &fHead + sizeof(fHead); ++p)
		os::Byteswap::byteswap(*p);
#endif

	u32 checksum = 0;
	sscanf(fHead.Checksum, "%lo", &checksum);


	// verify checksum

	// some old TAR writers assume that chars are signed, others assume unsigned
	// USTAR archives have a longer header, old TAR archives end after linkname

	u32 checksum1=0;
	s32 checksum2=0;

	// remember to blank the checksum field!
	for (u32 i=0; i<8;++i)
		fHead.Checksum[i] = ' ';

	// old header
	for (u8* p = (u8*)(&fHead); p < (u8*)(&fHead.Magic[0]); ++p)
	{
		checksum1 += *p;
		checksum2 += c8(*p);
	}

	if (!strcmp(fHead.Magic, "star"))
	{
		for (u8* p = (u8*)(&fHead.Magic[0]); p < (u8*)(&fHead) + sizeof(fHead); ++p)
		{
			checksum1 += *p;
			checksum2 += c8(*p);
		}
	}
	return checksum1 == checksum || checksum2 == (s32)checksum;
}

/*
	TAR Archive
*/
CTarReader::CTarReader(IReadFile* file, bool ignoreCase, bool ignorePaths)
: File(file), IgnoreCase(ignoreCase), IgnorePaths(ignorePaths)
{
	#ifdef _DEBUG
	setDebugName("CTarReader");
	#endif

	if (File)
	{
		File->grab();

		Base = File->getFileName();
		Base.replace('\\', '/');

		// fill the file list
		populateFileList();
	}
}

CTarReader::~CTarReader()
{
	if (File)
		File->drop();

}

u32 CTarReader::populateFileList()
{
	STarHeader fHead;
	FileList.clear();

	u32 pos = 0;
	while ( s32(pos + sizeof(STarHeader)) < File->getSize())
	{
		// seek to next file header
		File->seek(pos);

		// read the header
		File->read(&fHead, sizeof(fHead));

#ifdef __BIG_ENDIAN__
		for (u32* p = (u32*)&fHead; p < &fHead + sizeof(fHead); ++p)
			os::Byteswap::byteswap(*p);
#endif

		// only add standard files for now
		if (fHead.Link == ETLI_REGULAR_FILE || ETLI_REGULAR_FILE_OLD)
		{
			STARArchiveEntry entry;

			core::string<c16> fullPath = L"";
			fullPath.reserve(255);

			// USTAR archives have a filename prefix
			// may not be null terminated, copy carefully!
			if (!strcmp(fHead.Magic, "ustar"))
			{
				c8* np = fHead.FileNamePrefix;
				while(*np && (np - fHead.FileNamePrefix) < 155) 
					fullPath.append(*np);
				np++;
			}

			// append the file name
			c8* np = fHead.FileName;
			while(*np && (np - fHead.FileName) < 100)
			{
				fullPath.append(*np);
				np++;
			}

			fullPath.replace('\\', '/');
			s32 lastSlash = fullPath.findLast('/');

			if (IgnoreCase)
				fullPath.make_lower();

			if (lastSlash == -1)
			{
				entry.path = "";
				entry.simpleFileName = fullPath;
			}
			else
			{
				entry.path = fullPath.subString(0, lastSlash);
				if (IgnorePaths)
					entry.simpleFileName = &fullPath[lastSlash+1];
				else
					entry.simpleFileName = fullPath;
			}

			// get size
			core::stringc sSize = "";
			sSize.reserve(12);
			np = fHead.Size;
			while(*np && (np - fHead.Size) < 12)
			{
				sSize.append(*np);
				np++;
			}
			
			sscanf(sSize.c_str(), "%lo", &entry.size);

			// save start position
			entry.startPos = pos + 512;

			// move to next file header block
			pos = entry.startPos + (entry.size / 512) * 512 + 
				((entry.size % 512) ? 512 : 0);

			// add file to list
			FileList.push_back(entry);
		}
		else
		{
			// move to next block
			pos += 512;
		}

	}

	FileList.sort();

	return FileList.size();
}

//! opens a file by file name
IReadFile* CTarReader::createAndOpenFile(const core::string<c16>& filename)
{
	s32 index = findFile(filename);

	if (index != -1)
		return createAndOpenFile(index);

	return 0;
}


//! opens a file by index
IReadFile* CTarReader::createAndOpenFile(u32 index)
{
	if (index < FileList.size())
		return createLimitReadFile(FileList[index].simpleFileName, File, FileList[index].startPos, FileList[index].size);
	else
		return 0;
}


//! returns count of files in archive
u32 CTarReader::getFileCount() const
{
	return FileList.size();
}


//! returns data of file
const IFileArchiveEntry* CTarReader::getFileInfo(u32 index)
{
	return &FileList[index];
}


//! return the id of the file Archive
const core::string<c16>& CTarReader::getArchiveName()
{
	return Base;
}

//! returns fileindex
s32 CTarReader::findFile(const core::string<c16>& simpleFilename)
{
	STARArchiveEntry entry;
	entry.simpleFileName = simpleFilename;

	if (IgnoreCase)
		entry.simpleFileName.make_lower();

	if (IgnorePaths)
		core::deletePathFromFilename(entry.simpleFileName);

	s32 res = FileList.binary_search(entry);

	return res;
}

} // end namespace io
} // end namespace irr

