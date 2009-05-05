// Copyright (C) 2002-2009 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CZipReader.h"
#include "CFileList.h"
#include "CReadFile.h"
#include "os.h"
#include "coreutil.h"

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_ZLIB_
    #ifndef _IRR_USE_NON_SYSTEM_ZLIB_
    #include <zlib.h> // use system lib
    #else // _IRR_USE_NON_SYSTEM_ZLIB_
    #include "zlib/zlib.h" 
    #endif // _IRR_USE_NON_SYSTEM_ZLIB_
#endif // _IRR_COMPILE_WITH_ZLIB_

namespace irr
{
namespace io
{

//! Constructor
CArchiveLoaderZIP::CArchiveLoaderZIP( io::IFileSystem* fs)
: FileSystem(fs)
{
	#ifdef _DEBUG
	setDebugName("CArchiveLoaderZIP");
	#endif
}


//! destructor
CArchiveLoaderZIP::~CArchiveLoaderZIP()
{
}


//! returns true if the file maybe is able to be loaded by this class
bool CArchiveLoaderZIP::isALoadableFileFormat(const core::string<c16>& filename) const
{
	return core::hasFileExtension ( filename, "zip", "pk3", "dat" );
}


//! Creates an archive from the filename
/** \param file File handle to check.
\return Pointer to newly created archive, or 0 upon error. */
IFileArchive* CArchiveLoaderZIP::createArchive(const core::string<c16>& filename, bool ignoreCase, bool ignorePaths) const
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
IFileArchive* CArchiveLoaderZIP::createArchive(io::IReadFile* file, bool ignoreCase, bool ignorePaths) const
{
	IFileArchive *archive = 0;
	if ( file )
	{
		file->seek ( 0 );
		archive = new CZipReader(file, ignoreCase, ignorePaths);
	}
	return archive;
}

//! Check if the file might be loaded by this class
/** Check might look into the file.
\param file File handle to check.
\return True if file seems to be loadable. */
bool CArchiveLoaderZIP::isALoadableFileFormat(io::IReadFile* file) const
{
	SZIPFileHeader header;

	file->read( &header.Sig, 4 );
#ifdef __BIG_ENDIAN__
	os::Byteswap::byteswap(header.Sig);
#endif
	return header.Sig == 0x04034b50;
}

/*
	ZIP Archive
*/
CZipReader::CZipReader(IReadFile* file, bool ignoreCase, bool ignorePaths)
: File(file), IgnoreCase(ignoreCase), IgnorePaths(ignorePaths), Type ( "zip" )
{
	#ifdef _DEBUG
	setDebugName("CZipReader");
	#endif

	if (File)
	{
		File->grab();

		Base = File->getFileName();
		Base.replace ( '\\', '/' );

		// scan local headers
		while (scanLocalHeader());
		//while (scanLocalHeader2());

		// prepare file index for binary search
		FileList.sort();
	}
}

CZipReader::~CZipReader()
{
	if (File)
		File->drop();
}



//! splits filename from zip file into useful filenames and paths
void CZipReader::extractFilename(SZipFileEntry* entry)
{
	s32 lorfn = entry->header.FilenameLength; // length of real file name

	if (!lorfn)
		return;

	if (IgnoreCase)
		entry->zipFileName.make_lower();

	const c16* p = entry->zipFileName.c_str() + lorfn;
	
	// suche ein slash oder den anfang.

	while (*p!='/' && p!=entry->zipFileName.c_str())
	{
		--p;
		--lorfn;
	}

	bool thereIsAPath = p != entry->zipFileName.c_str();

	if (thereIsAPath)
	{
		// there is a path
		++p;
		++lorfn;
	}

	entry->simpleFileName = p;
	entry->path = "";

	// pfad auch kopieren
	if (thereIsAPath)
	{
		lorfn = (s32)(p - entry->zipFileName.c_str());
		
		entry->path = entry->zipFileName.subString ( 0, lorfn );

		//entry->path.append(entry->zipFileName, lorfn);
		//entry->path.append ( "" );
	}

	if (!IgnorePaths)
		entry->simpleFileName = entry->zipFileName; // thanks to Pr3t3nd3r for this fix
}

#if 0
#include <windows.h>

const c8 *sigName( u32 sig )
{
	switch ( sig )
	{
		case 0x04034b50: return "PK0304";
		case 0x02014b50: return "PK0102";
		case 0x06054b50: return "PK0506";
	}
	return "unknown";
}

bool CZipReader::scanLocalHeader2()
{
	c8 buf [ 128 ];
	c8 *c;

	File->read( &temp.header.Sig, 4 );

#ifdef __BIG_ENDIAN__
	os::Byteswap::byteswap(temp.header.Sig);
#endif

	sprintf ( buf, "sig: %08x,%s,", temp.header.Sig, sigName ( temp.header.Sig ) );
	OutputDebugStringA ( buf );

	// Local File Header
	if ( temp.header.Sig == 0x04034b50 )
	{
		File->read( &temp.header.VersionToExtract, sizeof( temp.header ) - 4 );

		temp.zipFileName.reserve( temp.header.FilenameLength+2);
		c = (c8*) temp.zipFileName.c_str();
		File->read( c, temp.header.FilenameLength);
		c [ temp.header.FilenameLength ] = 0;
		temp.zipFileName.verify();

		sprintf ( buf, "%d,'%s'\n", temp.header.CompressionMethod, c );
		OutputDebugStringA ( buf );

		if (temp.header.ExtraFieldLength)
		{
			File->seek( temp.header.ExtraFieldLength, true);
		}

		if (temp.header.GeneralBitFlag & ZIP_INFO_IN_DATA_DESCRITOR)
		{
			// read data descriptor
			File->seek(sizeof(SZIPFileDataDescriptor), true );
		}

		// compressed data
		temp.fileDataPosition = File->getPos();
		File->seek( temp.header.DataDescriptor.CompressedSize, true);
		FileList.push_back( temp );
		return true;
	}

	// Central directory structure
	if ( temp.header.Sig == 0x04034b50 )
	{
		//SZIPFileCentralDirFileHeader h;
		//File->read( &h, sizeof( h ) - 4 );
		return true;
	}

	// End of central dir
	if ( temp.header.Sig == 0x06054b50 )
	{
		return true;
	}

	// eof
	if ( temp.header.Sig == 0x02014b50 )
	{
		return false;
	}

	return false;
}

#endif

//! scans for a local header, returns false if there is no more local file header.
bool CZipReader::scanLocalHeader()
{
	//c8 tmp[1024];

	SZipFileEntry entry;
	entry.fileDataPosition = 0;
	memset(&entry.header, 0, sizeof(SZIPFileHeader));

	File->read(&entry.header, sizeof(SZIPFileHeader));

#ifdef __BIG_ENDIAN__
		entry.header.Sig = os::Byteswap::byteswap(entry.header.Sig);
		entry.header.VersionToExtract = os::Byteswap::byteswap(entry.header.VersionToExtract);
		entry.header.GeneralBitFlag = os::Byteswap::byteswap(entry.header.GeneralBitFlag);
		entry.header.CompressionMethod = os::Byteswap::byteswap(entry.header.CompressionMethod);
		entry.header.LastModFileTime = os::Byteswap::byteswap(entry.header.LastModFileTime);
		entry.header.LastModFileDate = os::Byteswap::byteswap(entry.header.LastModFileDate);
		entry.header.DataDescriptor.CRC32 = os::Byteswap::byteswap(entry.header.DataDescriptor.CRC32);
		entry.header.DataDescriptor.CompressedSize = os::Byteswap::byteswap(entry.header.DataDescriptor.CompressedSize);
		entry.header.DataDescriptor.UncompressedSize = os::Byteswap::byteswap(entry.header.DataDescriptor.UncompressedSize);
		entry.header.FilenameLength = os::Byteswap::byteswap(entry.header.FilenameLength);
		entry.header.ExtraFieldLength = os::Byteswap::byteswap(entry.header.ExtraFieldLength);
#endif

	if (entry.header.Sig != 0x04034b50)
		return false; // local file headers end here.

	// read filename
	{
		c8 *tmp = new c8 [ entry.header.FilenameLength + 2 ];
		File->read(tmp, entry.header.FilenameLength);
		tmp[entry.header.FilenameLength] = 0x0;
		entry.zipFileName = tmp;
		delete [] tmp;
	}

	extractFilename(&entry);

	// move forward length of extra field.

	if (entry.header.ExtraFieldLength)
		File->seek(entry.header.ExtraFieldLength, true);

	// if bit 3 was set, read DataDescriptor, following after the compressed data
	if (entry.header.GeneralBitFlag & ZIP_INFO_IN_DATA_DESCRITOR)
	{
		// read data descriptor
		File->read(&entry.header.DataDescriptor, sizeof(entry.header.DataDescriptor));
#ifdef __BIG_ENDIAN__
		entry.header.DataDescriptor.CRC32 = os::Byteswap::byteswap(entry.header.DataDescriptor.CRC32);
		entry.header.DataDescriptor.CompressedSize = os::Byteswap::byteswap(entry.header.DataDescriptor.CompressedSize);
		entry.header.DataDescriptor.UncompressedSize = os::Byteswap::byteswap(entry.header.DataDescriptor.UncompressedSize);
#endif
	}

	// store position in file
	entry.fileDataPosition = File->getPos();
	// move forward length of data
	File->seek(entry.header.DataDescriptor.CompressedSize, true);

	#ifdef _DEBUG
	//os::Debuginfo::print("added file from archive", entry.simpleFileName.c_str());
	#endif

	FileList.push_back(entry);

	return true;
}


//! opens a file by file name
IReadFile* CZipReader::openFile(const core::string<c16>& filename)
{
	s32 index = findFile(filename);

	if (index != -1)
		return openFile(index);

	return 0;
}


//! opens a file by index
IReadFile* CZipReader::openFile(s32 index)
{
	//0 - The file is stored (no compression)
	//1 - The file is Shrunk
	//2 - The file is Reduced with compression factor 1
	//3 - The file is Reduced with compression factor 2
	//4 - The file is Reduced with compression factor 3
	//5 - The file is Reduced with compression factor 4
	//6 - The file is Imploded
	//7 - Reserved for Tokenizing compression algorithm
	//8 - The file is Deflated
	//9 - Reserved for enhanced Deflating
	//10 - PKWARE Date Compression Library Imploding

	const SZipFileEntry &e = FileList[index];
	wchar_t buf[64];
	switch(e.header.CompressionMethod)
	{
	case 0: // no compression
		{
			return createLimitReadFile( e.simpleFileName, File, e.fileDataPosition, e.header.DataDescriptor.CompressedSize);
		}
	case 8:
		{
  			#ifdef _IRR_COMPILE_WITH_ZLIB_
			
			const u32 uncompressedSize = e.header.DataDescriptor.UncompressedSize;			
			const u32 compressedSize = e.header.DataDescriptor.CompressedSize;

			void* pBuf = new c8[ uncompressedSize ];
			if (!pBuf)
			{
				swprintf ( buf, 64, L"Not enough memory for decompressing %s", e.simpleFileName.c_str() );
				os::Printer::log( buf, ELL_ERROR);
				return 0;
			}

			c8 *pcData = new c8[ compressedSize ];
			if (!pcData)
			{
				swprintf ( buf, 64, L"Not enough memory for decompressing %s", e.simpleFileName.c_str() );
				os::Printer::log( buf, ELL_ERROR);
				return 0;
			}

			//memset(pcData, 0, compressedSize );
			File->seek( e.fileDataPosition );
			File->read(pcData, compressedSize );
			
			// Setup the inflate stream.
			z_stream stream;
			s32 err;

			stream.next_in = (Bytef*)pcData;
			stream.avail_in = (uInt)compressedSize;
			stream.next_out = (Bytef*)pBuf;
			stream.avail_out = uncompressedSize;
			stream.zalloc = (alloc_func)0;
			stream.zfree = (free_func)0;

			// Perform inflation. wbits < 0 indicates no zlib header inside the data.
			err = inflateInit2(&stream, -MAX_WBITS);
			if (err == Z_OK)
			{
				err = inflate(&stream, Z_FINISH);
				inflateEnd(&stream);
				if (err == Z_STREAM_END)
					err = Z_OK;
				err = Z_OK;
				inflateEnd(&stream);
			}


			delete[] pcData;
			
			if (err != Z_OK)
			{
				swprintf ( buf, 64, L"Error decompressing %s", e.simpleFileName.c_str() );
				os::Printer::log( buf, ELL_ERROR);
				delete [] (c8*)pBuf;
				return 0;
			}
			else
				return io::createMemoryReadFile(pBuf, uncompressedSize, e.zipFileName, true);
			
			#else
			return 0; // zlib not compiled, we cannot decompress the data.
			#endif
		}
	default:
		swprintf ( buf, 64, L"file has unsupported compression method. %s", e.simpleFileName.c_str() );
		os::Printer::log( buf, ELL_ERROR);
		return 0;
	};
}


//! returns count of files in archive
u32 CZipReader::getFileCount() const
{
	return FileList.size();
}


//! returns data of file
const IFileArchiveEntry* CZipReader::getFileInfo(u32 index)
{
	return &FileList[index];
}


//! return the id of the file Archive
const core::string<c16>& CZipReader::getArchiveName ()
{ 
	return Base;
}


//! returns fileindex
s32 CZipReader::findFile( const core::string<c16>& simpleFilename)
{
	SZipFileEntry entry;
	entry.simpleFileName = simpleFilename;

	if (IgnoreCase)
		entry.simpleFileName.make_lower();

	if (IgnorePaths)
		core::deletePathFromFilename(entry.simpleFileName);

	s32 res = FileList.binary_search(entry);

	#ifdef _DEBUG
	if (res == -1)
	{
		for (u32 i=0; i<FileList.size(); ++i)
			if (FileList[i].simpleFileName == entry.simpleFileName)
			{
				os::Printer::log("File in archive but not found.", entry.simpleFileName.c_str(), ELL_ERROR);
				break;
			}
	}
	#endif

	return res;
}


// -----------------------------------------------------------------------------

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
	core::string<c16> fname ( filename );
	deletePathFromFilename ( fname );

	if ( 0 == fname.size() )
	{
		ret = true;
	}

	return ret;
}


//! Creates an archive from the filename
IFileArchive* CArchiveLoaderMount::createArchive(const core::string<c16>& filename, bool ignoreCase, bool ignorePaths) const
{
	IFileArchive *archive = 0;

	EFileSystemType current = FileSystem->setFileListSystem ( FILESYSTEM_NATIVE );

	core::string<c16> save = FileSystem->getWorkingDirectory ();
	core::string<c16> fullPath = FileSystem->getAbsolutePath ( filename );
	FileSystem->flattenFilename ( fullPath );

	if ( FileSystem->changeWorkingDirectoryTo ( fullPath ) )
	{
		archive = new CMountPointReader(FileSystem, fullPath, ignoreCase, ignorePaths);
	}

	FileSystem->changeWorkingDirectoryTo ( save );
	FileSystem->setFileListSystem ( current );

	return archive;
}

//! Check if the file might be loaded by this class
/** Check might look into the file.
\param file File handle to check.
\return True if file seems to be loadable. */
bool CArchiveLoaderMount::isALoadableFileFormat(io::IReadFile* file) const
{
	return false;
}

//! creates/loads an archive from the file.
//! \return Pointer to the created archive. Returns 0 if loading failed.
IFileArchive* CArchiveLoaderMount::createArchive(io::IReadFile* file, bool ignoreCase, bool ignorePaths) const
{
	return 0;
}

#if 1

//! simple Reader ( does not handle ignorecase and ignorePath )
// its more a simple wrapper for handling relative directories
// advantage: speed
class CMountPointReadFile : public CReadFile
{
	public:
		CMountPointReadFile ( const core::string<c16>& realName,
						const core::string<c16>& hashName )
			: CReadFile( realName ), CallFileName ( hashName )
		{
		}
		virtual ~CMountPointReadFile () {}

		virtual const core::string<c16>& getFileName() const
		{
			return CallFileName;
		}

		core::string<c16> CallFileName;
};

/*!
*/
CMountPointReader::CMountPointReader( IFileSystem * parent, const core::string<c16>& basename, bool ignoreCase, bool ignorePaths)
:CZipReader ( 0, ignoreCase, ignorePaths ), Parent ( parent )
{
	Base = basename;
	Base.replace ( '\\', '/' );
	if ( core::lastChar ( Base ) != '/' )
		Base.append ( '/' );
	Type = "mount";
}

void CMountPointReader::buildDirectory ( )
{
}

//! opens a file by file name
IReadFile* CMountPointReader::openFile(const core::string<c16>& filename)
{
	core::string<c16> fname ( Base );
	fname += filename;


	CMountPointReadFile* file = new CMountPointReadFile( fname, filename);
	if (file->isOpen())
		return file;

	file->drop();
	return 0;

}

//! returns fileindex
s32 CMountPointReader::findFile(const core::string<c16>& filename)
{
	IReadFile *file = openFile ( filename );
	if ( 0 == file )
		return -1;
	file->drop ();
	return 1;
}

#else

//! compatible Folder Archticture
// 
CMountPointReader::CMountPointReader( IFileSystem * parent, const core::string<c16>& basename, bool ignoreCase, bool ignorePaths)
	: CZipReader( 0, ignoreCase, ignorePaths ), Parent ( parent )
{
	Type = "mount";
	core::string<c16> work = Parent->getWorkingDirectory ();

	Parent->changeWorkingDirectoryTo ( basename );
	FileList.clear();
	buildDirectory ( );
	Parent->changeWorkingDirectoryTo ( work );

	FileList.sort();
}

void CMountPointReader::buildDirectory ( )
{
	IFileList * list = new CFileList();

	SZipFileEntry entry;

	const u32 size = list->getFileCount();
	for (u32 i = 0; i!= size; ++i)
	{
		if ( false == list->isDirectory( i ) )
		{
			entry.zipFileName = list->getFullFileName ( i );
			entry.header.FilenameLength = entry.zipFileName.size ();
			extractFilename(&entry);
			FileList.push_back(entry);
		}
		else
		{
			const core::string<c16>& rel = list->getFileName ( i );

			if ( rel != "." && rel != ".." )
			{
				Parent->changeWorkingDirectoryTo ( rel );
				buildDirectory ();
				Parent->changeWorkingDirectoryTo ( ".." );
			}
		}
	}

	list->drop ();
}

s32 CMountPointReader::findFile(const core::string<c16>& simpleFilename)
{
	return CZipReader::findFile( simpleFilename);
}

//! opens a file by file name
IReadFile* CMountPointReader::openFile(const core::string<c16>& filename)
{
	s32 index = -1;

	if ( IgnorePaths )
	{
		index = findFile(filename);
	}
	else
	if ( FileList.size () )
	{
		core::string<c16> search = FileList[0].path + filename;
		index = findFile( search );
	}

	if (index == -1)
		return 0;

	return createReadFile(FileList[index].zipFileName.c_str() );
}
#endif


} // end namespace io
} // end namespace irr

