
#include "CImageWriterJPG.h"
#include "CColorConverter.h"
#include "IWriteFile.h"
#include "CImage.h"
#include "CColorConverter.h"
#include "irrString.h"

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_LIBJPEG_
#include <stdio.h> // required for jpeglib.h
extern "C"
{
#ifndef _IRR_USE_NON_SYSTEM_JPEG_LIB_
	#include <jpeglib.h>
	#include <jerror.h>
#else
	#include "jpeglib/jpeglib.h"
	#include "jpeglib/jerror.h"
#endif
#include <setjmp.h>
}


namespace irr
{
namespace video
{


typedef struct
{
  struct jpeg_destination_mgr pub; /* public fields */

  JOCTET * buffer;              /* image buffer */
  u32 buffer_size;				/* image buffer size */
} mem_destination_mgr;


typedef mem_destination_mgr * mem_dest_ptr;

void init_destination (j_compress_ptr cinfo)
{
	mem_dest_ptr dest = (mem_dest_ptr) cinfo->dest;

	/* image buffer must be allocated before mem_dest routines are called.  */
	if(dest->buffer == NULL) {
		//fprintf(stderr, "jmem_dest: init_destination: buffer not allocated\n");
	}

	dest->pub.next_output_byte = dest->buffer;
	dest->pub.free_in_buffer = dest->buffer_size;
}


boolean empty_output_buffer (j_compress_ptr cinfo)
{
	/*
	mem_dest_ptr dest = (mem_dest_ptr) cinfo->dest;
	*/
	// empty_output_buffer: buffer should not ever be full\n");
	return FALSE;
}


void term_destination (j_compress_ptr cinfo)
{
	mem_dest_ptr dest = (mem_dest_ptr) cinfo->dest;
	size_t datacount = dest->buffer_size - dest->pub.free_in_buffer;
}

void jpeg_memory_dest (j_compress_ptr cinfo, u8 *jfif_buffer,
                  s32 buf_size)
{
  mem_dest_ptr dest;

  if(jfif_buffer == NULL) {
    //fprintf(stderr, "jpeg_memory_dest: memory buffer needs to be allocated\n");
    //ERREXIT(cinfo, JERR_BUFFER_SIZE);
	  return;
  }

  if (cinfo->dest == NULL) {    /* first time for this JPEG object? */
    cinfo->dest = (struct jpeg_destination_mgr *)
      (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
                                  (size_t) sizeof(mem_destination_mgr));
  }

  dest = (mem_dest_ptr) cinfo->dest;  /* for casting */

  /* Initialize method pointers */
  dest->pub.init_destination = init_destination;
  dest->pub.empty_output_buffer = empty_output_buffer;
  dest->pub.term_destination = term_destination;

  /* Initialize private member */
  dest->buffer = (JOCTET*)jfif_buffer;
  dest->buffer_size = buf_size;
}

/* write_JPEG_memory: store JPEG compressed image into memory.
*/
void write_JPEG_memory (void *img_buf, s32 width, s32 height, u32 bpp, u32 pitch,
					u8 *jpeg_buffer, u32 jpeg_buffer_size,
					s32 quality, u32 *jpeg_comp_size)
{
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;

	/* More stuff */
	JSAMPROW row_pointer[1];      /* pointer to JSAMPLE row[s] */

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);
	jpeg_memory_dest(&cinfo, jpeg_buffer, jpeg_buffer_size);
	cinfo.image_width = width;
	cinfo.image_height = height;
	cinfo.input_components = bpp;
	cinfo.in_color_space = JCS_RGB;

	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, quality, TRUE);
	jpeg_start_compress(&cinfo, TRUE);

	while (cinfo.next_scanline < cinfo.image_height)
	{
		row_pointer[0] = (u8*) img_buf + (cinfo.next_scanline * pitch );
		jpeg_write_scanlines(&cinfo, row_pointer, 1);
	}

	/* Step 6: Finish compression */
	jpeg_finish_compress(&cinfo);

	{
		mem_dest_ptr dest = (mem_dest_ptr) cinfo.dest;
		*jpeg_comp_size = dest->buffer_size - dest->pub.free_in_buffer;
	}

	jpeg_destroy_compress(&cinfo);
}

} // namespace video
} // namespace irr

#endif // _IRR_COMPILE_WITH_LIBJPEG_

namespace irr
{
namespace video
{

IImageWriter* createImageWriterJPG()
{
	return new CImageWriterJPG;
}

CImageWriterJPG::CImageWriterJPG()
{
#ifdef _DEBUG
	setDebugName("CImageWriterJPG");
#endif
}


bool CImageWriterJPG::isAWriteableFileExtension(const c8* fileName)
{
	return strstr(fileName, ".jpg") != 0 || strstr(fileName, ".jpeg") != 0;
}


bool CImageWriterJPG::writeImage(io::IWriteFile *file, IImage *input,u32 quality )
{
#ifndef _IRR_COMPILE_WITH_LIBJPEG_
	return false;
#else

	core::dimension2di dim = input->getDimension();
	IImage * image = new CImage(ECF_R8G8B8, dim );

	void (*format)(const void*, s32, void*) = 0;
	switch( input->getColorFormat () )
	{
		case ECF_R8G8B8:	format = CColorConverter::convert_R8G8B8toR8G8B8; break;
		case ECF_A8R8G8B8:	format = CColorConverter::convert_A8R8G8B8toB8G8R8; break;
		case ECF_A1R5G5B5:	format = CColorConverter::convert_A1R5G5B5toB8G8R8; break;
		case ECF_R5G6B5:	format = CColorConverter::convert_R5G6B5toR8G8B8; break;
	}

	// couldn't find a color converter
	if ( 0 == format )
		return false;

	s32 y;
	void *src = input->lock();
	void *dst = image->lock();
	for ( y = 0; y!= dim.Height; ++y )
	{
		format( src, dim.Width, dst );
		src = (void*) ( (u8*) src + input->getPitch () );
		dst = (void*) ( (u8*) dst + image->getPitch () );
	}
	input->unlock ();
	image->unlock ();

	// temp buffer
	u32 destSize = image->getImageDataSizeInBytes ();
	u8 * dest = new u8 [ destSize ];
	
	if ( 0 == quality )
		quality = 75;

	write_JPEG_memory ( image->lock (), dim.Width, dim.Height,
				image->getBytesPerPixel(), image->getPitch(),
				dest, destSize,
				quality,
				&destSize);

	file->write ( dest, destSize );

	image->drop ();
	delete [] dest;

	return true;
#endif
}

} // namespace video
} // namespace irr

