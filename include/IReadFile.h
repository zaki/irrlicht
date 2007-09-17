// Copyright (C) 2002-2007 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_READ_FILE_H_INCLUDED__
#define __I_READ_FILE_H_INCLUDED__

#include "IReferenceCounted.h"

namespace irr
{
namespace io
{

	//! Interface providing read acess to a file.
	class IReadFile : public virtual IReferenceCounted
	{
	public:

		virtual ~IReadFile() {}

		//! Reads an amount of bytes from the file.
		//! \param buffer: Pointer to buffer where to read bytes will be written to.
		//! \param sizeToRead: Amount of bytes to read from the file.
		//! \return Returns how much bytes were read.
		virtual s32 read(void* buffer, u32 sizeToRead) = 0;

		//! Changes position in file, returns true if successful.
		//! \param finalPos: Destination position in the file.
		//! \param relativeMovement: If set to true, the position in the file is
		//! changed relative to current position. Otherwise the position is changed 
		//! from beginning of file.
		//! \return Returns true if successful, otherwise false.
		virtual bool seek(long finalPos, bool relativeMovement = false) = 0;

		//! Returns size of file.
		//! \return Returns the size of the file in bytes.
		virtual long getSize() const = 0;

		//! Returns the current position in the file.
		//! \return Returns the current position in the file in bytes.
		virtual long getPos() const = 0;

		//! Returns name of file.
		//! \return Returns the file name as zero terminated character string.
		virtual const c8* getFileName() const = 0;
	};

	//! Internal function, please do not use.
	IReadFile* createReadFile(const c8* fileName);
	//! Internal function, please do not use.
	IReadFile* createLimitReadFile(const c8* fileName, IReadFile* alreadyOpenedFile, long areaSize);
	//! Internal function, please do not use.
	IReadFile* createMemoryReadFile(void* memory, long size, const c8* fileName, bool deleteMemoryWhenDropped);

} // end namespace io
} // end namespace irr

#endif

