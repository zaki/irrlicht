// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CMemoryReadWriteFile.h"

using namespace irr;
using namespace io;

CMemoryReadWriteFile::CMemoryReadWriteFile(const c8* filename) 
: Pos(0), Data(), FileName(filename)
{

}


s32 CMemoryReadWriteFile::write(const void* buffer, u32 sizeToWrite)
{

	// no point in writing 0 bytes
	if (sizeToWrite < 1)
		return 0;

	// expand size
	if (Pos + sizeToWrite > (s32)Data.size())
		Data.set_used(Pos+sizeToWrite);

	// copy data
	memcpy( (void*) &Data[Pos], buffer, (size_t) sizeToWrite);

	Pos += sizeToWrite;

	return sizeToWrite;
	
}

bool CMemoryReadWriteFile::seek(long finalPos, bool relativeMovement)
{
	if (relativeMovement)
	{
		if (finalPos + Pos < 0)
			return 0;
		else
			Pos += finalPos;
	}
	else
	{
		Pos = finalPos;
	}

	if (Pos > (s32)Data.size())
		Data.set_used(Pos+1);

	return true;

}

const c8* CMemoryReadWriteFile::getFileName() const
{
	return FileName.c_str();
}

long CMemoryReadWriteFile::getPos() const
{
	return Pos;
}

core::array<c8>& CMemoryReadWriteFile::getData()
{
	return Data;
}


long CMemoryReadWriteFile::getSize() const
{
	return Data.size();
}


s32 CMemoryReadWriteFile::read(void* buffer, u32 sizeToRead)
{
	// cant read past the end
	if (Pos + sizeToRead >= (s32)Data.size())
		sizeToRead = Data.size() - Pos;

	// cant read 0 bytes
	if (!sizeToRead)
		return 0;

	// copy data
	memcpy( buffer, (void*) &Data[Pos], (size_t) sizeToRead);
	
	Pos += sizeToRead;

	return sizeToRead;
}

