#ifndef _C_IMAGE_WRITER_PPM_H_INCLUDED__
#define _C_IMAGE_WRITER_PPM_H_INCLUDED__

#include "IImageWriter.h"

namespace irr
{
namespace video
{

class CImageWriterPPM : public IImageWriter
{
public:
	//! constructor
	CImageWriterPPM();

	//! return true if this writer can write a file with the given extension
	virtual bool isAWriteableFileExtension(const c8* fileName);

	//! write image to file
	virtual bool writeImage(io::IWriteFile *file, IImage *image, u32 param);
};

} // namespace video
} // namespace irr

#endif // _C_IMAGE_WRITER_PPM_H_INCLUDED__
