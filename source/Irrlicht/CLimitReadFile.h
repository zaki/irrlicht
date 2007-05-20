// Copyright (C) 2002-2007 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_LIMIT_READ_FILE_H_INCLUDED__
#define __C_LIMIT_READ_FILE_H_INCLUDED__

#include "IReadFile.h"
#include "irrString.h"

namespace irr
{
	class CUnicodeConverter;

namespace io
{

	/*! this is a read file, which is limited to some boundaries, 
		so that it may only start from a certain file position
		and may only read until a certain file position.
		This can be useful, for example for reading uncompressed files
		in an archive (zip).
	!*/
	class CLimitReadFile : public IReadFile
	{
	public:

		CLimitReadFile(IReadFile* alreadyOpenedFile, s32 areaSize, const c8* name);

		virtual ~CLimitReadFile();

		//! returns how much was read
		virtual s32 read(void* buffer, u32 sizeToRead);

		//! changes position in file, returns true if successful
		//! if relativeMovement==true, the pos is changed relative to current pos,
		//! otherwise from begin of file
		virtual bool seek(s32 finalPos, bool relativeMovement = false);

		//! returns size of file
		virtual s32 getSize();

		//! returns where in the file we are.
		virtual s32 getPos();

		//! returns name of file
		virtual const c8* getFileName();

	private:

		void init();

		core::stringc Filename;
		s32 AreaSize;
		s32 AreaStart;
		s32 AreaEnd;
		IReadFile* File;
	};

} // end namespace io
} // end namespace irr

#endif

