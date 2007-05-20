// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#using <mscorlib.dll>
using namespace System;

#pragma unmanaged
#include "..\\..\\include\\irrlicht.h"
#pragma managed

namespace Irrlicht
{
public __gc class IrrlichtDevice;

namespace IO
{
	public __gc class IFileSystem
	{
	public:

		/// <summary>
		/// You should access the IFileSystem 
		/// through the Irrlicht::IrrlichtDevice.FileSystem property. Simply don't use
		/// this constructor.
		///</summary>
		///<param name="fs">The real, unmanaged C++ file system</param>
		IFileSystem(irr::io::IFileSystem* fs);

		~IFileSystem();

		/// <summary>
		/// Adds an zip archive to the file system. After calling this, the Irrlicht Engine will search and open files directly from this archive too. 
		/// This is useful for hiding data from the end user, speeding up file access and making it possible to
		/// access for example Quake3 .pk3 files, which are nothing different than .zip files.
		/// </summary>
		/// <param name="filename"> Filename of the zip archive to add to the file system.</param>
		/// <param name="ignoreCase"> If set to true, files in the archive can be accessed without
		/// writing all letters in the right case.</param>
		/// <param name="ignorePaths"> If set to true, files in the added archive can be accessed
		/// without its complete path.</param>
		/// <returns> Returns true if the archive was added successful, false if not.</returns>
		bool AddZipFileArchive(System::String* filename, bool ignoreCase,
			bool ignorePaths);

		/// <summary>
		/// Adds an zip archive to the file system. After calling this, the Irrlicht Engine will search and open files directly from this archive too. 
		/// This is useful for hiding data from the end user, speeding up file access and making it possible to
		/// access for example Quake3 .pk3 files, which are nothing different than .zip files.
		/// </summary>
		/// <param name="filename"> Filename of the zip archive to add to the file system.</param>
		/// <returns> Returns true if the archive was added successful, false if not.</returns>
		bool AddZipFileArchive(System::String* filename);

	private:

		irr::io::IFileSystem* FileSystem;
	};

}
}
