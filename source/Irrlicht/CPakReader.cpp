// Copyright (C) 2002-2009 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h
// Code contributed by skreamz

#include "CPakReader.h"

#ifdef __IRR_COMPILE_WITH_PAK_ARCHIVE_LOADER_

#include "os.h"
#include "coreutil.h"

namespace irr
{
namespace io
{

//! Constructor
CArchiveLoaderPAK::CArchiveLoaderPAK( io::IFileSystem* fs)
: FileSystem(fs)
{
	#ifdef _DEBUG
	setDebugName("CArchiveLoaderPAK");
	#endif
}


//! returns true if the file maybe is able to be loaded by this class
bool CArchiveLoaderPAK::isALoadableFileFormat(const core::string<c16>& filename) const
{
	return core::hasFileExtension ( filename, "pak" );
}

//! Check to see if the loader can create archives of this type.
bool CArchiveLoaderPAK::isALoadableFileFormat(E_FILE_ARCHIVE_TYPE fileType) const
{
	return fileType == EFAT_PAK;
}

//! Creates an archive from the filename
/** \param file File handle to check.
\return Pointer to newly created archive, or 0 upon error. */
IFileArchive* CArchiveLoaderPAK::createArchive(const core::string<c16>& filename, bool ignoreCase, bool ignorePaths) const
{
	IFileArchive *archive = 0;
	io::IReadFile* file = FileSystem->createAndOpenFile(filename);

	if (file)
	{
		archive = createArchive ( file, ignoreCase, ignorePaths );
		file->drop ();
	}

	return archive;
}

//! creates/loads an archive from the file.
//! \return Pointer to the created archive. Returns 0 if loading failed.
IFileArchive* CArchiveLoaderPAK::createArchive(io::IReadFile* file, bool ignoreCase, bool ignorePaths) const
{
	IFileArchive *archive = 0;
	if ( file )
	{
		file->seek ( 0 );
		archive = new CPakReader(file, ignoreCase, ignorePaths);
	}
	return archive;
}


//! Check if the file might be loaded by this class
/** Check might look into the file.
\param file File handle to check.
\return True if file seems to be loadable. */
bool CArchiveLoaderPAK::isALoadableFileFormat(io::IReadFile* file) const
{
	SPAKFileHeader header;

	file->read( &header.tag, 4 );

	return header.tag[0] == 'P' && header.tag[1] == 'A';
}


/*!
	PAK Reader
*/
CPakReader::CPakReader(IReadFile* file, bool ignoreCase, bool ignorePaths)
: CFileList(file ? file->getFileName() : "", ignoreCase, ignorePaths), File(file)
{
	#ifdef _DEBUG
	setDebugName("CPakReader");
	#endif

	if (File)
	{
		File->grab();

		// scan local headers
		scanLocalHeader();

		sort();
	}
}


CPakReader::~CPakReader()
{
	if (File)
		File->drop();
}


const IFileList* CPakReader::getFileList() const
{
	return this;
}

//! scans for a local header, returns false if there is no more local file header.
bool CPakReader::scanLocalHeader()
{

	c8 tmp[1024];
	core::string<c16> PakFileName;

	memset(&header, 0, sizeof(SPAKFileHeader));
	File->read(&header, sizeof(SPAKFileHeader));

	if (header.tag[0] != 'P' && header.tag[1] != 'A')
		return false; // local file headers end here.

	File->seek(header.offset);

	const int count = header.length / ((sizeof(u32) * 2) + 56);

	for(int i = 0; i < count; i++)
	{
		// read filename
		PakFileName.reserve(56+2);
		File->read(tmp, 56);
		tmp[56] = 0x0;
		PakFileName = tmp;

		#ifdef _DEBUG
		os::Printer::log(PakFileName.c_str());
		#endif

		s32 offset;
		s32 size;

		File->read(&offset, sizeof(u32));
		File->read(&size, sizeof(u32));

#ifdef __BIG_ENDIAN__
		os::Byteswap::byteswap(offset);
		os::Byteswap::byteswap(size);
#endif

		addItem(PakFileName, size, false, Offsets.size());
		Offsets.push_back(offset);
	}
	return true;
}


//! opens a file by file name
IReadFile* CPakReader::createAndOpenFile(const core::string<c16>& filename)
{
	s32 index = findFile(filename, false);

	if (index != -1)
		return createAndOpenFile(index);

	return 0;
}


//! opens a file by index
IReadFile* CPakReader::createAndOpenFile(u32 index)
{
	if (index < Files.size())
	{
		return createLimitReadFile(Files[index].FullName, File, Offsets[Files[index].ID], Files[index].Size);
	}
	else
		return 0;
}

} // end namespace io
} // end namespace irr

#endif // __IRR_COMPILE_WITH_PAK_ARCHIVE_LOADER_
