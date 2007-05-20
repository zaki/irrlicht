// Copyright (C) 2002-2007 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CMemoryReadFile.h"
#include "irrString.h"

namespace irr
{
namespace io
{


CMemoryReadFile::CMemoryReadFile(void* memory, s32 len, const c8* fileName, bool d)
: Buffer(memory), Len(len), Pos(0), deleteMemoryWhenDropped(d)
{
	#ifdef _DEBUG
	setDebugName("CReadFile");
	#endif

	Filename = fileName;
}



CMemoryReadFile::~CMemoryReadFile()
{
	if (deleteMemoryWhenDropped)
		delete [] (c8*)Buffer;
}



//! returns how much was read
s32 CMemoryReadFile::read(void* buffer, u32 sizeToRead)
{
	s32 amount = sizeToRead;
	if (Pos + amount > Len)
		amount -= Pos + amount - Len;

	if (amount < 0)
		amount = 0;

	c8* p = (c8*)Buffer;
	memcpy(buffer, p + Pos, amount);
	
	Pos += static_cast<u32> ( amount );

	return amount;
}



//! changes position in file, returns true if successful
//! if relativeMovement==true, the pos is changed relative to current pos,
//! otherwise from begin of file
bool CMemoryReadFile::seek(s32 finalPos, bool relativeMovement)
{
	if (relativeMovement)
	{
		if (Pos + finalPos > Len)
			return false;

		Pos += finalPos;
	}
	else
	{
		if ( (unsigned) finalPos > Len)
			return false;
		
        Pos = finalPos;
	}

	return true;
}



//! returns size of file
s32 CMemoryReadFile::getSize()
{
	return Len;
}



//! returns where in the file we are.
s32 CMemoryReadFile::getPos()
{
	return Pos;
}



//! returns name of file
const c8* CMemoryReadFile::getFileName()
{
	return Filename.c_str();
}



IReadFile* createMemoryReadFile(void* memory, s32 size, const c8* fileName, bool deleteMemoryWhenDropped)
{
	CMemoryReadFile* file = new CMemoryReadFile(memory, size, fileName, deleteMemoryWhenDropped);
	return file;
}


} // end namespace io
} // end namespace irr

