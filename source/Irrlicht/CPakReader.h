// Copyright (C) 2002-2009 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_PAK_READER_H_INCLUDED__
#define __C_PAK_READER_H_INCLUDED__

#include "IReferenceCounted.h"
#include "IReadFile.h"
#include "irrArray.h"
#include "irrString.h"
#include "IFileSystem.h"

namespace irr
{
namespace io
{
	struct SPAKFileHeader
	{
		c8 tag[4];
		u32 offset;
		u32 length;
	};


	struct SPakFileEntry: public IFileArchiveEntry
	{
		core::string<c16> pakFileName;
		u32 pos;
		u32 length;
	};

	//! Archiveloader capable of loading PAK Archives
	class CArchiveLoaderPAK : public IArchiveLoader
	{
	public:

		//! Constructor
		CArchiveLoaderPAK(io::IFileSystem* fs);

		//! returns true if the file maybe is able to be loaded by this class
		//! based on the file extension (e.g. ".zip")
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

	private:
		io::IFileSystem* FileSystem;
	};


	//! reads from pak
	class CPakReader : public IFileArchive
	{
	public:

		CPakReader(IReadFile* file, bool ignoreCase, bool ignorePaths);
		virtual ~CPakReader();

		//! opens a file by file name
		virtual IReadFile* openFile(const core::string<c16>& filename);

		//! opens a file by index
		virtual IReadFile* openFile(s32 index);

		//! returns count of files in archive
		virtual u32 getFileCount() const;

		//! returns data of file
		virtual const IFileArchiveEntry* getFileInfo(u32 index);

		//! returns fileindex
		virtual s32 findFile(const core::string<c16>& filename);

		//! get the class Type
		virtual const core::string<c16>& getArchiveType() { return Type; }

		//! return the id of the file Archive
		virtual const core::string<c16>& getArchiveName ()
		{ 
			return File->getFileName();
		}


	private:
		
		core::string<c16> Type;

		//! scans for a local header, returns false if there is no more local file header.
		bool scanLocalHeader();

		//! splits filename from zip file into useful filenames and paths
		void extractFilename(SPakFileEntry* entry);


		IReadFile* File;

		SPAKFileHeader header;

		core::array<SPakFileEntry> FileList;

		bool IgnoreCase;
		bool IgnorePaths;
	};

} // end namespace io
} // end namespace irr

#endif

