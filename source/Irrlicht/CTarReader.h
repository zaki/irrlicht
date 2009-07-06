// Copyright (C) 2009 Gaz Davidson
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_TAR_READER_H_INCLUDED__
#define __C_TAR_READER_H_INCLUDED__

#include "IReferenceCounted.h"
#include "IReadFile.h"
#include "irrArray.h"
#include "irrString.h"
#include "IFileSystem.h"
#include "IFileList.h"

namespace irr
{
namespace io
{

#if defined(_MSC_VER) || defined(__BORLANDC__) || defined (__BCPLUSPLUS__) 
#	pragma pack( push, packing )
#	pragma pack( 1 )
#	define PACK_STRUCT
#elif defined( __GNUC__ )
#	define PACK_STRUCT	__attribute__((packed))
#else
#	error compiler not supported
#endif

	enum E_TAR_LINK_INDICATOR
	{
		ETLI_REGULAR_FILE_OLD      =  0 ,
		ETLI_REGULAR_FILE          = '0',
		ETLI_LINK_TO_ARCHIVED_FILE = '1',
		ETLI_SYMBOLIC_LINK         = '2',
		ETLI_CHAR_SPECIAL_DEVICE   = '3',
		ETLI_BLOCK_SPECIAL_DEVICE  = '4',
		ETLI_DIRECTORY             = '5',
		ETLI_FIFO_SPECIAL_FILE     = '6'
	};

	struct STarHeader
	{
		c8 FileName[100];
		c8 FileMode[8];
		c8 UserID[8];
		c8 GroupID[8];
		c8 Size[12];
		c8 ModifiedTime[12];
		c8 Checksum[8];
		c8 Link;
		c8 LinkName[100];
		c8 Magic[6];
		c8 USTARVersion[2];
		c8 UserName[32];
		c8 GroupName[32];
		c8 DeviceMajor[8];
		c8 DeviceMinor[8];
		c8 FileNamePrefix[155];
	} PACK_STRUCT;


// Default alignment
#if defined(_MSC_VER) || defined(__BORLANDC__) || defined (__BCPLUSPLUS__) 
#	pragma pack( pop, packing )
#endif

#undef PACK_STRUCT

	//! Archiveloader capable of loading ZIP Archives
	class CArchiveLoaderTAR : public IArchiveLoader
	{
	public:

		//! Constructor
		CArchiveLoaderTAR(io::IFileSystem* fs);

		//! destructor
		virtual ~CArchiveLoaderTAR();

		//! returns true if the file maybe is able to be loaded by this class
		//! based on the file extension (e.g. ".tar")
		virtual bool isALoadableFileFormat(const core::string<c16>& filename) const;

		//! Creates an archive from the filename
		/** \param file File handle to check.
		\return Pointer to newly created archive, or 0 upon error. */
		virtual IFileArchive* createArchive(const core::string<c16>& filename, bool ignoreCase, bool ignorePaths) const;

		//! Check if the file might be loaded by this class
		/** Check might look into the file.
		\param file File handle to check.
		\return True if file seems to be loadable. */
		virtual bool isALoadableFileFormat(io::IReadFile* file) const;

		//! creates/loads an archive from the file.
		//! \return Pointer to the created archive. Returns 0 if loading failed.
		virtual io::IFileArchive* createArchive(io::IReadFile* file, bool ignoreCase, bool ignorePaths) const;

		//! Returns the type of archive created by this loader
		virtual E_FILE_ARCHIVE_TYPE getType() const { return EFAT_TAR; }

	private:
		io::IFileSystem* FileSystem;
	};



	class CTarReader : public IFileArchive
	{
	public:

		CTarReader(IReadFile* file, bool ignoreCase, bool ignorePaths);
		virtual ~CTarReader();

		//! opens a file by file name
		virtual IReadFile* createAndOpenFile(const core::string<c16>& filename);

		//! opens a file by index
		virtual IReadFile* createAndOpenFile(u32 index);

		//! returns count of files in archive
		virtual u32 getFileCount() const;

		//! returns data of file
		virtual const IFileArchiveEntry* getFileInfo(u32 index);

		//! returns fileindex
		virtual s32 findFile(const core::string<c16>& filename);

		//! return the id of the file Archive
		virtual const core::string<c16>& getArchiveName();

		//! get the class Type
		virtual E_FILE_ARCHIVE_TYPE getType() const { return EFAT_TAR; }

	private:

		struct STARArchiveEntry : public IFileArchiveEntry
		{
			u32 size;
			u32 startPos;
		};

		u32 populateFileList();
		
		IReadFile* File;

		bool IgnoreCase;
		bool IgnorePaths;
		core::array<STARArchiveEntry> FileList;

		core::string<c16> Base;
	};

} // end namespace io
} // end namespace irr

#endif

