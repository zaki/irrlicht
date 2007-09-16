#ifndef _I_IMAGE_WRITER_H_INCLUDED__
#define _I_IMAGE_WRITER_H_INCLUDED__

#include "IReferenceCounted.h"

namespace irr
{
namespace io
{
	class IWriteFile;
} // end namespace io

namespace video
{
	class IImage;


//! Interface for writing software image data.
class IImageWriter : public IReferenceCounted
{
public:
	//! destructor
	virtual ~IImageWriter() { }

	//! return true if this writer can write a file with the given extension
	virtual bool isAWriteableFileExtension(const c8* fileName) const = 0;

	//! write image to file
	virtual bool writeImage(io::IWriteFile *file, IImage *image, u32 param = 0) const = 0;
};

} // namespace video
} // namespace irr

#endif // _I_IMAGE_WRITER_H_INCLUDED__

