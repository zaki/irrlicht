#ifndef _C_IMAGE_WRITER_JPG_H_INCLUDED__
#define _C_IMAGE_WRITER_JPG_H_INCLUDED__

#include "IImageWriter.h"

namespace irr
{
namespace video
{

class CImageWriterJPG : public IImageWriter
{
public:
	//! constructor
	CImageWriterJPG();

	//! return true if this writer can write a file with the given extension
	virtual bool isAWriteableFileExtension(const c8* fileName);

	//! write image to file
	virtual bool writeImage(io::IWriteFile *file, IImage *image, u32 param);
};

}
}

#endif // _C_IMAGE_WRITER_JPG_H_INCLUDED__

