// Copyright (C) 2002-2007 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_FILE_LIST_H_INCLUDED__
#define __I_FILE_LIST_H_INCLUDED__

#include "IUnknown.h"

namespace irr
{
namespace io
{

//! The Filelist lists all files in a directory.
class IFileList : public virtual IUnknown
{
public:

	//! destructor
	virtual ~IFileList() {};

	//! Returns the amount of files in the filelist.
	//! \return
	//! Returns the amount of files and directories in the file list.
	virtual s32 getFileCount() = 0;

	//! Gets the name of a file in the list, based on an index.
	//! The path is not included in this name. Use getFullFileName for this.
	//! \param index is the zero based index of the file which name should
	//!   be returned. The index has to be smaller than the amount getFileCount() returns.
	//! \return
	//! Returns the file name of the file. Returns 0, if an error occured.
	virtual const c8* getFileName(s32 index) = 0;

	//! Gets the full name of a file in the list, path included, based on an index.
	//! \param index is the zero based index of the file which name should
	//!   be returned. The index has to be smaller than the amount getFileCount() returns.
	//! \return
	//! Returns the file name of the file. Returns 0, if an error occured.
	virtual const c8* getFullFileName(s32 index) = 0;

	//! Returns of the file is a directory
	//! \param
	//! index is the zero based index of the file which name should
	//!   be returned. The index has to be smaller than the amount getFileCount() returns.
	//! \return
	//! Returns true, if the file is a directory, and false, if it is not.
	//!  If an error occurs, the result is undefined.
	virtual bool isDirectory(s32 index) = 0;
};

} // end namespace irr
} // end namespace io


#endif

