// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "IFileSystem.h"
#include "NativeConverter.h"

namespace Irrlicht
{
namespace IO
{

	IFileSystem::IFileSystem(irr::io::IFileSystem* fs)
		: FileSystem(fs)
	{
		FileSystem->grab();
	}


	IFileSystem::~IFileSystem()
	{
		FileSystem->drop();
	}

	bool IFileSystem::AddZipFileArchive(System::String* filename, bool ignoreCase,
			bool ignorePaths)
	{
		char* str = (char*)(void*)
			System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(filename);

		bool ret = FileSystem->addZipFileArchive(str, ignoreCase, ignorePaths);

		System::Runtime::InteropServices::Marshal::FreeHGlobal(str);
		return ret;
	}

	bool IFileSystem::AddZipFileArchive(System::String* filename)
	{
		return AddZipFileArchive(filename, true, true);
	}
}
}