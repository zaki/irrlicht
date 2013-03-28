// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_MEMORY_READ_WRITE_FILE_H_INCLUDED__
#define __C_MEMORY_READ_WRITE_FILE_H_INCLUDED__

#include "IWriteFile.h"
#include "IReadFile.h"
#include "irrArray.h"
#include "irrString.h"
#include "memory.h"

namespace irr
{
namespace io
{

	//! Provides write acess to an array as if it is a file.
	class CMemoryReadWriteFile : public virtual IWriteFile, public virtual IReadFile
	{
	public:

		CMemoryReadWriteFile(const c8* filename=0);

		//! Reads an amount of bytes from the file.
		//! \param buffer: Pointer to buffer of bytes to write.
		//! \param sizeToWrite: Amount of bytes to wrtie to the file.
		//! \return Returns how much bytes were written.
		virtual s32 write(const void* buffer, u32 sizeToWrite);

		//! Changes position in file, returns true if successful.
		//! \param finalPos: Destination position in the file.
		//! \param relativeMovement: If set to true, the position in the file is
		//! changed relative to current position. Otherwise the position is changed
		//! from begin of file.
		//! \return Returns true if successful, otherwise false.
		virtual bool seek(long finalPos, bool relativeMovement = false);

		//! Returns size of file.
		//! \return Returns the size of the file in bytes.
		virtual long getSize() const;

		//! Reads an amount of bytes from the file.
		//! \param buffer: Pointer to buffer where to read bytes will be written to.
		//! \param sizeToRead: Amount of bytes to read from the file.
		//! \return Returns how much bytes were read.
		virtual s32 read(void* buffer, u32 sizeToRead);

		//! Returns the current position in the file.
		//! \return Returns the current position in the file in bytes.
		virtual long getPos() const;

		//! Returns name of file.
		//! \return Returns the file name as zero terminated character string.
		virtual const io::path& getFileName() const;

		//! Returns file data as an array
		core::array<c8>& getData();

	private:

		core::array<c8> Data;
		io::path FileName;
		long Pos;
	};



} // end namespace io
} // end namespace irr

#endif // __C_MEMORY_READ_WRITE_FILE_H_INCLUDED__
