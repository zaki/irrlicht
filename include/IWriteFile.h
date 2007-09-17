// Copyright (C) 2002-2007 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_WRITE_FILE_H_INCLUDED__
#define __I_WRITE_FILE_H_INCLUDED__

#include "IReferenceCounted.h"

namespace irr
{
namespace io
{

	//! Interface providing write acess to a file.
	class IWriteFile : public virtual IReferenceCounted
	{
	public:

		virtual ~IWriteFile() {}

		//! Writes an amount of bytes to the file.
		//! \param buffer: Pointer to buffer of bytes to write.
		//! \param sizeToWrite: Amount of bytes to write to the file.
		//! \return Returns how much bytes were written.
		virtual s32 write(const void* buffer, u32 sizeToWrite) = 0;

		//! Changes position in file, returns true if successful.
		//! \param finalPos: Destination position in the file.
		//! \param relativeMovement: If set to true, the position in the file is
		//! changed relative to current position. Otherwise the position is changed 
		//! from begin of file.		
		//! \return Returns true if successful, otherwise false.
		virtual bool seek(long finalPos, bool relativeMovement = false) = 0;

		//! Returns the current position in the file.
		//! \return Returns the current position in the file in bytes.
		virtual long getPos() const = 0;

		//! Returns name of file.
		//! \return Returns the file name as zero terminated character string.
		virtual const c8* getFileName() const = 0;
	};

	//! Internal function, please do not use.
	IWriteFile* createWriteFile(const c8* fileName, bool append);

} // end namespace io
} // end namespace irr

#endif

