// Copyright (C) 2002-2007 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_SURFACE_LOADER_H_INCLUDED__
#define __I_SURFACE_LOADER_H_INCLUDED__

#include "IUnknown.h"
#include "IImage.h"

namespace irr
{
namespace io
{
	class IReadFile;
} // end namespace io
namespace video
{

//!	Class which is able to create a image from a file.
/** If you want the Irrlicht Engine be able to load textures of 
currently unsupported file formats (e.g .gif), then implement
this and add your new Surface loader with 
IVideoDriver::addExternalImageLoader() to the engine. */
class IImageLoader : public virtual IUnknown
{
public:

	//! destructor
	virtual ~IImageLoader() {};

	//! returns true if the file maybe is able to be loaded by this class
	//! based on the file extension (e.g. ".tga")
	virtual bool isALoadableFileExtension(const c8* fileName) = 0;

	//! returns true if the file maybe is able to be loaded by this class
	virtual bool isALoadableFileFormat(irr::io::IReadFile* file) = 0;

	//! creates a surface from the file
	virtual IImage* loadImage(irr::io::IReadFile* file) = 0;
};


} // end namespace video
} // end namespace irr

#endif

