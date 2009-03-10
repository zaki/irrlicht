//! Copyright (C) 2009 Gary Conway
//! This file is part of the "Irrlicht Engine".
//! For conditions of distribution and use, see copyright notice in irrlicht.h

/*
	Author:	Gary Conway (Viper) - co-author of the ZIP file format, Feb 1989, 
						 see the story at http://www.idcnet.us/ziphistory.html
	Website:	http://idcnet.us
	Email:		codeslinger@vipergc.com
	Created:	March 1, 2009
	Version:	1.0
	Updated:

	This module will load SGI .rgb files (along with the other extensions). The module complies 
	with version 1.0 of the SGI Image File Format by Paul Haeberli of Silicon Graphics Computer Systems
	The module handles BW, RGB and RGBA images.

	RGB images are stored with either 8 bits per COLOR VALUE, one each for red,green,blue (24bpp)
	or 16 bits per COLOR VALUE, again one each for red,green,blue  (48 bpp), not including the alpha channel


	OPTIONS NOT SUPPORTED

	1.	16 bit COLOR VALUES (48bpp modes)
	2.	COLORMAP = DITHERED mode



For non- run length encoded files, this is the structure

 	The Header
 	The Image Data

If the image is run length encoded, this is the structure: 
 	The Header
 	The Offset Tables
 	The Image Data

The header consists of the following: 

        Size  | Type   | Name      | Description   
 
      2 bytes | short  | MAGIC     | IRIS image file magic number
      1 byte  | char   | STORAGE   | Storage format
      1 byte  | char   | BPC       | Number of bytes per pixel channel 
      2 bytes | ushort | DIMENSION | Number of dimensions
      2 bytes | ushort | XSIZE     | X size in pixels 
      2 bytes | ushort | YSIZE     | Y size in pixels 
      2 bytes | ushort | ZSIZE     | Number of channels
      4 bytes | long   | PIXMIN    | Minimum pixel value
      4 bytes | long   | PIXMAX    | Maximum pixel value
      4 bytes | char   | DUMMY     | Ignored
     80 bytes | char   | IMAGENAME | Image name
      4 bytes | long   | COLORMAP  | Colormap ID
    404 bytes | char   | DUMMY     | Ignored

Here is a description of each field in the image file header: 

MAGIC - This is the decimal value 474 saved as a short. This identifies the file as an SGI image file. 

STORAGE -	specifies whether the image is stored using run length encoding (RLE) or not (VERBATIM). 
			If RLE is used, the value of this byte will be 1. Otherwise the value of this byte will 
			be 0. The only allowed values for this field are 0 or 1. 

BPC -		describes the precision that is used to store each channel of an image. This is the number of 
			bytes per pixel component. The majority of SGI image files use 1 byte per pixel component, 
			giving 256 levels. Some SGI image files use 2 bytes per component. The only allowed values 
			for this field are 1 or 2. 

DIMENSION - described the number of dimensions in the data stored in the image file. 
			The only allowed values are 1, 2, or 3. If this value is 1, the image file 
			consists of only 1 channel and only 1 scanline (row). The length of this 
			scanline is given by the value of XSIZE below. If this value is 2, the file 
			consists of a single channel with a number of scanlines. The width and height 
			of the image are given by the values of XSIZE and YSIZE below. 
			If this value is 3, the file consists of a number of channels. 
			The width and height of the image are given by the values of XSIZE and YSIZE below. 
			The number of channels is given by the value of ZSIZE below. 

XSIZE -		The width of the image in pixels 

YSIZE -		The height of the image in pixels 

ZSIZE -		The number of channels in the image. B/W (greyscale) images are stored as 2 dimensional 
			images with a ZSIZE of 1. RGB color images are stored as 3 dimensional images with a 
			ZSIZE of 3. An RGB image with an ALPHA channel is stored as a 3 dimensional image with 
			a ZSIZE of 4. There are no inherent limitations in the SGI image file format that would 
			preclude the creation of image files with more than 4 channels. 

PINMIN -	The minimum pixel value in the image. The value of 0 may be used if no pixel has a value 
			that is smaller than 0. 

PINMAX -	The maximum pixel value in the image. The value of 255 may be used if no pixel has a 
			value that is greater than 255. This is the value that is considered to be full 
			brightness in the image. 

DUMMY -		This 4 bytes of data should be set to 0. 

IMAGENAME - An null terminated ascii string of up to 79 characters terminated by a null may be 
			included here. This is not commonly used. 

COLORMAP -	This controls how the pixel values in the file should be interpreted. It can have one 
			of these four values: 

0: NORMAL - The data in the channels represent B/W values for images with 1 channel, RGB values 
			for images with 3 channels, and RGBA values for images with 4 channels. Almost all 
			the SGI image files are of this type. 

1: DITHERED - The image will have only 1 channel of data. For each pixel, RGB data is packed 
			into one 8 bit value. 3 bits are used for red and green, while blue uses 2 bits. 
			Red data is found in bits[2..0], green data in bits[5..3], and blue data in 
			bits[7..6]. This format is obsolete. 

2: SCREEN - The image will have only 1 channel of data. This format was used to store 
			color-indexed pixels. To convert the pixel values into RGB values a colormap 
			must be used. The appropriate color map varies from image to image. This format is obsolete. 

3: COLORMAP - The image is used to store a color map from an SGI machine. In this case the 
			image is not displayable in the conventional sense. 

DUMMY -		This 404 bytes of data should be set to 0. This makes the header exactly 512 bytes. 
*/

#include "CImageLoaderRGB.h"

#ifdef _IRR_COMPILE_WITH_RGB_LOADER_

#include "IReadFile.h"
#include "SColor.h"
#include "CColorConverter.h"
#include "CImage.h"
#include "os.h"
#include "irrString.h"


namespace irr
{
namespace video
{

//! constructor

CImageLoaderRGB::CImageLoaderRGB()
{
	#ifdef _DEBUG
	setDebugName("CImageLoaderRGB");
	#endif
}


//! returns true if the file maybe is able to be loaded by this class
//! based on the file extensions listed here
bool CImageLoaderRGB::isALoadableFileExtension(const irr::core::stringc &fileName) const
{
	return core::hasFileExtension( fileName, "rgb", "rgba", "sgi" ) || 
	       core::hasFileExtension( fileName, "int", "inta", "bw" );
}



//! returns true if the file maybe is able to be loaded by this class
bool CImageLoaderRGB::isALoadableFileFormat(io::IReadFile* file) const
{
	rgbStruct *rgb = new rgbStruct;
	bool retVal = checkFormat(file, rgb);
	delete rgb;
	return retVal;
}



/*
	The main entry point, read and format the image file.

	RETURNS:	pointer to the image data on success
				null pointer on fail

*/

//! creates a surface from the file
IImage* CImageLoaderRGB::loadImage(io::IReadFile* file) const
{
	IImage* image = 0;
	s32* paletteData = 0;

	rgbStruct *rgb = new rgbStruct();   // construct our structure for holding data

	// read header information
	if (checkFormat(file, rgb))
	{
		// 16 bits per COLOR VALUE, not supported, this is 48bpp mode
		if (rgb->header.BPC != 1)
		{
			os::Printer::log("Only one byte per pixel RGB files are supported", file->getFileName(), ELL_ERROR);
		}
		else if (rgb->header.Colormap != 0)
		{
			os::Printer::log("Dithered, Screen and Colormap RGB files are not supported", file->getFileName(), ELL_ERROR);
		}
		else if (rgb->header.Storage == 1 && !readOffsetTables(file, rgb))
		{
			os::Printer::log("Failed to read RLE table in RGB file", file->getFileName(), ELL_ERROR);
		}
		else if (!rgb->allocateTemps())
		{
			os::Printer::log("Out of memory in RGB file loader", file->getFileName(), ELL_ERROR);
		}
		else
		{

			// read and process the file to rgbData
			processFile(rgb, file);

/*

		  ZSIZE		Description
			1		BW (grayscale) image
			3		RGB image
			4		RGBa image with one alpha channel

			When the Alpha channel is present, I am not sure with RGB files if
			it's a precomputed RGB color or it needs to be completely calculated. My guess
			would be that it's not precomputed for two reasons. 

			1. the loss of precision when calculating the fraction, then storing the result as an int
			2. the loss of the original color data when the image might be composited with another. Yes
				the original color data could be computed, however, not without another loss in precision

			Also, I don't know where to find the background color
			Pixmin and Pixmax are apparently the min and max alpha blend values (0-100%)

			Complete Alpha blending computation
			The actual resulting merged color is computed this way: 
			(image color ◊ alpha) + (background color ◊ (100% - alpha)).

			Using precomputed blending
			(image color) + (background color ◊ (100% - alpha)).

			Alternatively, the RGB files could use another blending technique entirely

*/

			switch (rgb->header.Zsize)
			{
			case 1:
				// BW (grayscale) image
				paletteData = new s32[256];
				for (int n=0; n<256; n++) 
					paletteData[n] = n;

				image = new CImage(ECF_A1R5G5B5, core::dimension2d<u32>(rgb->header.Xsize, rgb->header.Ysize));
				if (image)
					CColorConverter::convert8BitTo16Bit(rgb->rgbData, (s16*)image->lock(), rgb->header.Xsize, rgb->header.Ysize, paletteData, 0, true);
				break;
			case 3:
				// RGB image 
				// one byte per COLOR VALUE, eg, 24bpp
				image = new CImage(ECF_R8G8B8, core::dimension2d<u32>(rgb->header.Xsize, rgb->header.Ysize));
				if (image)
					CColorConverter::convert24BitTo24Bit(rgb->rgbData, (u8*)image->lock(), rgb->header.Xsize, rgb->header.Ysize, 0, true, false);
				break;
			case 4:
				// RGBa image with one alpha channel (32bpp)
				// image is stored in rgbData as RGBA

				converttoARGB(rgb->rgbData, rgb);

				image = new CImage(ECF_A8R8G8B8, core::dimension2d<u32>(rgb->header.Xsize, rgb->header.Ysize));
				if (image)
					CColorConverter::convert32BitTo32Bit((s32*)rgb->rgbData, (s32*)image->lock(), rgb->header.Xsize, rgb->header.Ysize, 0, true);

				break;
			default:
				// Format unknown
				os::Printer::log("Unsupported pixel format in RGB file", file->getFileName(), ELL_ERROR);
			}

			if (image)
				image->unlock();
		}
	}


	// and tidy up allocated memory

	if (paletteData)
		delete [] paletteData;

	if (rgb)
		delete rgb;

	return image;
}

// returns true on success
bool CImageLoaderRGB::readHeader(io::IReadFile* file, rgbStruct* rgb) const
{
	if ( file->read(&rgb->header, sizeof(rgb->header)) < sizeof(rgb->header) )
		return false;

	// test for INTEL or BIG ENDIAN processor
	// if INTEL, then swap the byte order on 16 bit INT's to make them BIG ENDIAN
	// because that is the native format for the .rgb file
#ifndef __BIG_ENDIAN__
	rgb->header.Magic     = os::Byteswap::byteswap(rgb->header.Magic);
	rgb->header.Storage   = os::Byteswap::byteswap(rgb->header.Storage);
	rgb->header.Dimension = os::Byteswap::byteswap(rgb->header.Dimension);
	rgb->header.Xsize     = os::Byteswap::byteswap(rgb->header.Xsize);
	rgb->header.Ysize     = os::Byteswap::byteswap(rgb->header.Ysize);
	rgb->header.Zsize     = os::Byteswap::byteswap(rgb->header.Zsize);
	rgb->header.Pixmin    = os::Byteswap::byteswap(rgb->header.Pixmin);
	rgb->header.Pixmax    = os::Byteswap::byteswap(rgb->header.Pixmax);
	rgb->header.Colormap  = os::Byteswap::byteswap(rgb->header.Colormap);
#endif

	// calculate the size of the buffer needed: XSIZE * YSIZE * ZSIZE * BPC
	rgb->ImageSize = (rgb->header.Xsize)*(rgb->header.Ysize)*(rgb->header.Zsize)*(rgb->header.BPC);


	// allocate our buffer
	//if( !(rgb->rgbData = new u8 [rgb->ImageSize]) )
	//	return false;

	return true;
}


bool CImageLoaderRGB::checkFormat(io::IReadFile* file, rgbStruct* rgb) const
{
	if (!readHeader(file, rgb))
		return false;

	if (rgb->header.Magic == 0x1DA)
		return true;
	else
		return false;
}

/*
If the image is stored using run length encoding, offset tables follow the header that 
describe what the file offsets are to the RLE for each scanline. This information only 
applies if the value for STORAGE above is 1. 

         Size  | Type   | Name      | Description   

  tablen longs | long   | STARTTAB  | Start table
  tablen longs | long   | LENGTHTAB | Length table

One entry in each table is needed for each scanline of RLE data. The total number of scanlines in the image (tablen) is determined by the product of the YSIZE and ZSIZE. There are two tables of longs that are written. Each consists of tablen longs of data. The first table has the file offsets to the RLE data for each scanline in the image. In a file with more than 1 channel (ZSIZE > 1) this table first has all the offsets for the scanlines in the first channel, followed be offsets for the scanlines in the second channel, etc. The second table has the RLE data length for each scanline in the image. In a file with more than 1 channel (ZSIZE > 1) this table first has all the RLE data lengths for the scanlines in the first channel, followed be RLE data lengths for the scanlines in the second channel, etc. 

To find the the file offset, and the number of bytes in the RLE data for a particular scanline, these 
two arrays may be read in and indexed as follows: 

To read in the tables: 

    unsigned long *starttab, *lengthtab;

    tablen = YSIZE*ZSIZE*sizeof(long);
    starttab = (unsigned long *)mymalloc(tablen);
    lengthtab = (unsigned long *)mymalloc(tablen);
    fseek(rgb->inf,512,SEEK_SET);
    readlongtab(rgb->inf,starttab);
    readlongtab(rgb->inf,lengthtab);

To find the file offset and RLE data length for a scanline: 

rowno is an integer in the range 0 to YSIZE-1 channo is an integer in the range 0 to ZSIZE-1 

    rleoffset = starttab[rowno+channo*YSIZE]
    rlelength = lengthtab[rowno+channo*YSIZE]

It is possible for two identical rows (scanlines) to share compressed data. A completely 
white image could be written as a single compressed row and having all table entries point 
to that row. Another little hack that should work is if you are writing out a RGB RLE file, 
and a particular scanline is achromatic (greyscale), you could just make the r, g and b rows 
point to the same data!! 



  RETURNS:	on success true, else returns false

*/	

bool CImageLoaderRGB::readOffsetTables(io::IReadFile* file, rgbStruct *rgb) const
{
	u32 x;

	rgb->TableLen = rgb->header.Ysize * rgb->header.Zsize ;		// calc size of tables

	// return error if unable to allocate tables
	if ( !(rgb->StartTable = new u32[rgb->TableLen]) ) 
		return false;
	if ( !(rgb->LengthTable = new u32[rgb->TableLen]) ) 
		return false;

	file->seek(512);
	file->read(rgb->StartTable, rgb->TableLen* sizeof(u32));
	file->read(rgb->LengthTable, rgb->TableLen* sizeof(u32));

	// if we are on an INTEL platform, swap the bytes
#ifndef __BIG_ENDIAN__
	x= rgb->TableLen * sizeof(u32);
	convertLong(rgb->StartTable, (long) (x/sizeof(u32)));
	convertLong((u32 *)rgb->LengthTable, (long) (x/sizeof(u32)));
#endif

	return true;
}



/*
	The header has already been read into rgb structure
	The Tables have been read if necessary
	Now process the actual data
*/
void CImageLoaderRGB::processFile(rgbStruct *rgb, io::IReadFile* file) const
{
	u8 *ptr;
	int i, j;
	u16 *tempShort;

	// calculate the size of the buffer needed: XSIZE * YSIZE * ZSIZE * BPC
	rgb->rgbData = new u8 [(rgb->header.Xsize)*(rgb->header.Ysize)*(rgb->header.Zsize)*(rgb->header.BPC)];
	ptr = rgb->rgbData;

	// cycle through all scanlines

#ifdef _IRR_RGB_FILE_INVERTED_IMAGE_
	// preserve the image as stored, eg, inverted
	for (i = 0; i < (int)(rgb->header.Ysize); i++)
#else
	// invert the image to make it upright
	for (i =  (int)(rgb->header.Ysize)-1; i>=0; i--)
#endif
	{
		// check the number of channels and read a row of data
		if( rgb->header.Zsize >= 1 )
			readRGBrow( rgb->tmpR, i, 0, file, rgb);
		if( rgb->header.Zsize >= 2 )
			readRGBrow( rgb->tmpG, i, 1, file, rgb);
		if( rgb->header.Zsize >= 3 )
			readRGBrow( rgb->tmpB, i, 2, file, rgb);
		if( rgb->header.Zsize >= 4 )
			readRGBrow( rgb->tmpA, i, 3, file, rgb);

		// cycle thru all values for this row
		for (j = 0; j < (int)(rgb->header.Xsize); j++)
		{
			if(rgb->header.BPC == 1)
			{
				// ONE byte per color
				if( rgb->header.Zsize >= 1 )  *ptr++ = *(rgb->tmpR + j);
				if( rgb->header.Zsize >= 2 )  *ptr++ = *(rgb->tmpG + j);
				if( rgb->header.Zsize >= 3 )  *ptr++ = *(rgb->tmpB + j);
				if( rgb->header.Zsize >= 4 )  *ptr++ = *(rgb->tmpA + j);

			}
			else
			{
				// TWO bytes per color
				if( rgb->header.Zsize >= 1 )
				{
					// two bytes of color data
					tempShort  = (u16 *) (ptr);
					*tempShort = *(  (u16 *) (rgb->tmpR) + j);
					tempShort++;
					ptr = ( u8 *)(tempShort);
				}
				if( rgb->header.Zsize >= 2 )
				{
					tempShort  = ( u16 *) (ptr);
					*tempShort = *( ( u16 *) (rgb->tmpG) + j);
					tempShort++;
					ptr = ( u8 *) (tempShort);
				}
				if( rgb->header.Zsize >= 3 )
				{
					tempShort  = ( u16 *) (ptr);
					*tempShort = *( ( u16 *) (rgb->tmpB) + j);
					tempShort++;
					ptr = ( u8 *)(tempShort);
				}
				if( rgb->header.Zsize >= 4 )
				{
					tempShort  = ( u16 *) (ptr);
					*tempShort = *( ( u16 *) (rgb->tmpA) + j);
					tempShort++;
					ptr = ( u8 *)(tempShort);
				}

			} // end if(rgb->header.BPC == 1)
        	} // end for

		//         // pad the image width with blanks to bring it up to the rounded width.
		//         for(;j<width;++j) *ptr++ = 0;

	} // end for
}



/*
	This information only applies if the value for STORAGE is 1. If the image is 
	stored using run length encoding, the image data follows the offset/length tables. 
	The RLE data is not in any particular order. The offset tables are used to 
	locate the rle data for any scanline. 

	The RLE data must be read in from the file and expanded into pixel data in the following manner: 

	If BPC is 1, then there is one byte per pixel. In this case the RLE data should be 
	read into an array of chars. To expand data, the low order seven bits of the first 
	byte: bits[6..0] are used to form a count. If the high order bit of the first byte 
	is 1: bit[7], then the count is used to specify how many bytes to copy from the RLE 
	data buffer to the destination. Otherwise, if the high order bit of the first byte 
	is 0: bit[7], then the count is used to specify how many times to repeat the value 
	of the following byte, in the destination. This process continues until a count 
	of 0 is found. This should decompress exactly XSIZE pixels. 


	One entry in each table is needed for each scanline of RLE data. The total number of 
	scanlines in the image (tablen) is determined by the product of the YSIZE and ZSIZE. 
	There are two tables of longs that are written. Each consists of tablen longs of data. 
	The first table has the file offsets to the RLE data for each scanline in the image. In 
	a file with more than 1 channel (ZSIZE > 1) this table first has all the offsets for the 
	scanlines in the first channel, followed be offsets for the scanlines in the second 
	channel, etc. The second table has the RLE data length for each scanline in the image. 
	In a file with more than 1 channel (ZSIZE > 1) this table first has all the RLE data 
	lengths for the scanlines in the first channel, followed be RLE data lengths for the 
	scanlines in the second channel, etc. 

	Return a row of data, expanding RLE compression if necessary
*/
void CImageLoaderRGB::readRGBrow(u8 *buf, int y, int z, io::IReadFile* file, rgbStruct* rgb) const
{
	u8 *iPtr, *oPtr;
	u16 pixel;
	int count;
	bool done = false;
	u16 *tempShort;

	if (rgb->header.Storage  != 1)
	{
		// stored VERBATIM

		file->seek(512+(y*rgb->header.Xsize * rgb->header.BPC)+(z* rgb->header.Xsize * rgb->header.Ysize * rgb->header.BPC));
		file->read((char*)buf, rgb->header.Xsize * rgb->header.BPC);

#ifndef __BIG_ENDIAN__
		if (rgb->header.BPC != 1)
			convertShort( (u16 *)(buf), rgb->header.Xsize);
#endif
		return;

	}
		

	// the file is stored as Run Length Encoding (RLE)
	// each sequence is stored as 0x80  NumRepeats ByteToRepeat

	// get the file offset from StartTable and SEEK
	// then read the data

	file->seek((long) rgb->StartTable[y+z * rgb->header.Ysize]);
	file->read((char*) rgb->tmp, (unsigned int)rgb->LengthTable[y+z * rgb->header.Ysize]);

	// rgb->tmp has the data

	iPtr = rgb->tmp;
	oPtr = buf;
	while (!done)
        {
		// if BPC = 1, then one byte per pixel
		if (rgb->header.BPC == 1)
		{
			pixel = *iPtr++;
		}
		else
		{
			// BPC = 2, so two bytes per pixel
			tempShort = (u16 *)  iPtr;
			pixel = *tempShort;
			tempShort++;
			iPtr = (u8 *) tempShort;
		}

#ifndef __BIG_ENDIAN__
		if (rgb->header.BPC != 1)
			convertShort(&pixel, 1);
#endif
		
		count = (int)(pixel & 0x7F);
            
		// limit the count value to the remiaing row size
		if (oPtr + count*rgb->header.BPC > buf + rgb->header.Xsize * rgb->header.BPC)
		{
			count = ( (buf + rgb->header.Xsize * rgb->header.BPC) - oPtr ) / rgb->header.BPC;
		}
                
		if (count<=0)
		{
			done = true;
			return;
		}

		if (pixel & 0x80)
		{
			// repeat the byte pointed to by iPtr, count times
			while (count--)
			{
				if(rgb->header.BPC == 1)
				{
					*oPtr++ = *iPtr++;
				}
				else
				{
					tempShort = (u16 *) (iPtr);
					pixel = *tempShort;
					tempShort++;
					iPtr = (u8 *) (tempShort);
#ifndef __BIG_ENDIAN__
					convertShort(&pixel, 1);
#endif
					tempShort = (u16 *) (oPtr);
					*tempShort = pixel;
					tempShort++;
					oPtr = (u8 *) (tempShort);
				}
			}
		}
		else
		{
			if (rgb->header.BPC == 1)
			{
				pixel = *iPtr++;
			}
			else
			{
				tempShort = (u16 *) (iPtr);
				pixel = *tempShort;
				tempShort++;
				iPtr = (u8 *) (tempShort);
			}

#ifndef __BIG_ENDIAN__
			if (rgb->header.BPC != 1)
				convertShort(&pixel, 1);
#endif
			
			while (count--)
			{
				if(rgb->header.BPC == 1)
				{
					*oPtr++ = (char) pixel;
				}
				else
				{
					tempShort  = (u16 *) (oPtr);
					*tempShort = pixel;
					tempShort++;
					oPtr = (u8 *) (tempShort);
				}
			}

		} // else if (pixel & 0x80)
	} // while (!done)

}




#ifndef __BIG_ENDIAN__
//todo: replace with os::byteswap

/*
	In the following description a notation like bits[7..0] is used to denote a 
	range of bits in a binary value. Bit 0 is the lowest order bit in the value. 

	All short values are represented by 2 bytes. The first byte stores the 
	high order 8 bits of the value: bits[15..8]. The second byte stores the 
	low order 8 bits of the value: bits[7..0]. 
*/

void CImageLoaderRGB::convertShort(u16 *array, long length) const
{
	unsigned long b1, b2;
	unsigned char *ptr;

	ptr = (unsigned char *)array;
	while (length--)
	{
		b1 = *ptr++;
		b2 = *ptr++;
		*array++ = (u16) ((b1 << 8) | (b2));
	}
}



/*
	All long values are represented by 4 bytes. The first byte stores the 
	high order 8 bits of the value: bits[31..24]. The second byte stores 
	bits[23..16]. The third byte stores bits[15..8]. The forth byte stores 
	the low order 8 bits of the value: bits[7..0]. 

	And this function will read a long value from the file: 
*/
void CImageLoaderRGB::convertLong(u32 *array, long length) const
{
	unsigned long b1, b2, b3, b4;
	unsigned char *ptr;

	ptr = (unsigned char *)array;
	while (length--)
	{
		b1 = *ptr++;
		b2 = *ptr++;
		b3 = *ptr++;
		b4 = *ptr++;
		*array++ = (b1 << 24) | (b2 << 16) | (b3 << 8) | (b4);
	}
}

#endif

// we have 1 byte per COLOR VALUE, eg 24bpp and 1 alpha channel
// calculate the color values based on the alpha values
// color values are stored as R,G,B,A

// todo: replace with CColorConverter method

void CImageLoaderRGB::converttoARGB(u8* in, rgbStruct *rgb) const
{
	u32 cnt=0;
	u8 tmp;
	//			(image color ◊ alpha) + (background color ◊ (100% - alpha)).

	for ( int y=0; y < rgb->header.Ysize; y++)
	{
		for ( int x=0; x < rgb->header.Xsize; x++)
		{
			tmp = in[cnt+3];
			in[cnt+3] = in[cnt+2];
			in[cnt+2] = in[cnt+1];
			in[cnt+1] = in[cnt];
			in[cnt]   = tmp;
			in +=4;
		}
	}
}



//! creates a loader which is able to load windows bitmaps
IImageLoader* createImageLoaderRGB()
{
	return new CImageLoaderRGB;
}


} // end namespace video
} // end namespace irr


#endif

