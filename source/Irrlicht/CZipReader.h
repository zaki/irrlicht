// Copyright (C) 2002-2009 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_ZIP_READER_H_INCLUDED__
#define __C_ZIP_READER_H_INCLUDED__

#include "IrrCompileConfig.h"

#ifdef __IRR_COMPILE_WITH_ZIP_ARCHIVE_LOADER_

#include "IReadFile.h"
#include "irrArray.h"
#include "irrString.h"
#include "IFileSystem.h"
#include "CFileList.h"

namespace irr
{
namespace io
{
	// set if the file is encrypted
	const s16 ZIP_FILE_ENCRYPTED =		0x0001;
	// the fields crc-32, compressed size and uncompressed size are set to
	// zero in the local header
	const s16 ZIP_INFO_IN_DATA_DESCRIPTOR =	0x0008;

#if defined(_MSC_VER) || defined(__BORLANDC__) || defined (__BCPLUSPLUS__)
#	pragma pack( push, packing )
#	pragma pack( 1 )
#	define PACK_STRUCT
#elif defined( __GNUC__ )
#	define PACK_STRUCT	__attribute__((packed))
#else
#	error compiler not supported
#endif


	struct SZIPFileDataDescriptor
	{
		u32 CRC32;
		u32 CompressedSize;
		u32 UncompressedSize;
	} PACK_STRUCT;

	struct SZIPFileHeader
	{
		u32 Sig;				// 'PK0304' little endian (0x04034b50)
		s16 VersionToExtract;
		s16 GeneralBitFlag;
		s16 CompressionMethod;
		s16 LastModFileTime;
		s16 LastModFileDate;
		SZIPFileDataDescriptor DataDescriptor;
		s16 FilenameLength;
		s16 ExtraFieldLength;
		// filename (variable size)
		// extra field (variable size )
	} PACK_STRUCT;

	struct SZIPFileCentralDirFileHeader
	{
		u32 central_file_header_signature;	// 'PK0102' (0x02014b50)
		u16 version_made_by;
		u16 version_needed_to_extract;
		u16 general_purpose_bit_flag;
		u16 compression_method;
		u16 last_mod_file_time;
		u16 last_mod_file_date;
		u32 crc_32;
		u32 compressed_size;
		u32 uncompressed_size;
		u16 filename_length;
		u16 extra_field_length;
		u16 file_comment_length;
		u16 disk_number_start;
		u16 internal_file_attributes;
		u32 external_file_attributes;
		u32 relative_offset_of_local_header;

		// filename (variable size)
		// extra field (variable size)
		// file comment (variable size)

	} PACK_STRUCT;

	struct SZIPFileCentralDirEnd
	{
		u32 sig;			// 'PK0506' end_of central dir signature			// (0x06054b50)
		u16 numberDisk;		// number of this disk
		u16 numberStart;	// number of the disk with the start of the central directory
		u16 totalDisk;		// total number of entries in the central dir on this disk
		u16 totalEntries;	// total number of entries in the central dir
		u32 size;			// size of the central directory
		u32 offset;			// offset of start of centraldirectory with respect to the starting disk number
		u16 comment_length;	// zipfile comment length
		// zipfile comment (variable size)
	} PACK_STRUCT;

	enum E_GZIP_FLAGS
	{
		EGZF_TEXT_DAT      = 1,
		EGZF_CRC16         = 2,
		EGZF_EXTRA_FIELDS  = 4,
		EGZF_FILE_NAME     = 8,
		EGZF_COMMENT       = 16
	};

	struct SGZIPMemberHeader
	{
		u16 sig; // 0x8b1f
		u8  compressionMethod; // 8 = deflate
		u8  flags;
		u32 time;
		u8  extraFlags; // slow compress = 2, fast compress = 4
		u8  operatingSystem;
	} PACK_STRUCT;

// Default alignment
#if defined(_MSC_VER) || defined(__BORLANDC__) || defined (__BCPLUSPLUS__)
#	pragma pack( pop, packing )
#endif

#undef PACK_STRUCT

	//! Contains extended info about zip files in the archive
	struct SZipFileEntry
	{
		//! Position of data in the archive file
		s32 Offset;

		//! The header for this file containing compression info etc
		SZIPFileHeader header;
	};

	//! Archiveloader capable of loading ZIP Archives
	class CArchiveLoaderZIP : public IArchiveLoader
	{
	public:

		//! Constructor
		CArchiveLoaderZIP(io::IFileSystem* fs);

		//! returns true if the file maybe is able to be loaded by this class
		//! based on the file extension (e.g. ".zip")
		virtual bool isALoadableFileFormat(const io::path& filename) const;

		//! Check if the file might be loaded by this class
		/** Check might look into the file.
		\param file File handle to check.
		\return True if file seems to be loadable. */
		virtual bool isALoadableFileFormat(io::IReadFile* file) const;

		//! Check to see if the loader can create archives of this type.
		/** Check based on the archive type.
		\param fileType The archive type to check.
		\return True if the archile loader supports this type, false if not */
		virtual bool isALoadableFileFormat(E_FILE_ARCHIVE_TYPE fileType) const;

		//! Creates an archive from the filename
		/** \param file File handle to check.
		\return Pointer to newly created archive, or 0 upon error. */
		virtual IFileArchive* createArchive(const io::path& filename, bool ignoreCase, bool ignorePaths) const;

		//! creates/loads an archive from the file.
		//! \return Pointer to the created archive. Returns 0 if loading failed.
		virtual io::IFileArchive* createArchive(io::IReadFile* file, bool ignoreCase, bool ignorePaths) const;

	private:
		io::IFileSystem* FileSystem;
	};

/*!
	Zip file Reader written April 2002 by N.Gebhardt.
*/
	class CZipReader : public virtual IFileArchive, virtual CFileList
	{
	public:

		//! constructor
		CZipReader(IReadFile* file, bool ignoreCase, bool ignorePaths, bool isGZip=false);

		//! destructor
		virtual ~CZipReader();

		//! opens a file by file name
		virtual IReadFile* createAndOpenFile(const io::path& filename);

		//! opens a file by index
		virtual IReadFile* createAndOpenFile(u32 index);

		//! returns the list of files
		virtual const IFileList* getFileList() const;

		//! get the archive type
		virtual E_FILE_ARCHIVE_TYPE getType() const;

	protected:

		IReadFile* File;

		//! reads the next file header from a ZIP file, returns false if there are no more headers.
		bool scanZipHeader();

		//! the same but for gzip files
		bool scanGZipHeader();

		bool IsGZip;

		// holds extended info about files
		core::array<SZipFileEntry> FileInfo;
	};


} // end namespace io
} // end namespace irr

#endif // __IRR_COMPILE_WITH_ZIP_ARCHIVE_LOADER_
#endif // __C_ZIP_READER_H_INCLUDED__
