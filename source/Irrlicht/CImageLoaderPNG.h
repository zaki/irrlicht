// Copyright (C) 2002-2007 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

// this file was created by rt (www.tomkorp.com), based on ttk's png-reader
// i wanted to be able to read in PNG images with irrlicht :)
// why?  lossless compression with 8-bit alpha channel!

#ifndef __C_IMAGE_LOADER_PNG_H_INCLUDED__
#define __C_IMAGE_LOADER_PNG_H_INCLUDED__

#include "IImageLoader.h"

namespace irr
{
namespace video
{

//!  Surface Loader for PNG files
class CImageLoaderPng : public IImageLoader
{
public:

   //! constructor
   CImageLoaderPng();

   //! destructor
   virtual ~CImageLoaderPng();

   //! returns true if the file maybe is able to be loaded by this class
   //! based on the file extension (e.g. ".png")
   virtual bool isALoadableFileExtension(const c8* fileName);

   //! returns true if the file maybe is able to be loaded by this class
   virtual bool isALoadableFileFormat(irr::io::IReadFile* file);

   //! creates a surface from the file
   virtual IImage* loadImage(irr::io::IReadFile* file);

private:
	//some variables
	u32 Width;
	u32 Height;
	s32 BitDepth;
	s32 ColorType;
	video::IImage* Image;
	u8** RowPointers; //Used to point to image rows
};


} // end namespace video
} // end namespace irr

#endif
