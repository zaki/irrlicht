// Copyright (C) 2002-2009 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CZipReader.h"

#ifdef __IRR_COMPILE_WITH_ZIP_ARCHIVE_LOADER_

#include "CFileList.h"
#include "CReadFile.h"
#include "os.h"
#include "coreutil.h"

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_ZLIB_
	#ifndef _IRR_USE_NON_SYSTEM_ZLIB_
	#include <zlib.h> // use system lib
	#else // _IRR_USE_NON_SYSTEM_ZLIB_
	#include "zlib/zlib.h"
	#endif // _IRR_USE_NON_SYSTEM_ZLIB_
#endif // _IRR_COMPILE_WITH_ZLIB_
#include "aesGladman/fileenc.h"
namespace irr
{
namespace io
{


// -----------------------------------------------------------------------------
// zip loader
// -----------------------------------------------------------------------------

//! Constructor
CArchiveLoaderZIP::CArchiveLoaderZIP(io::IFileSystem* fs)
: FileSystem(fs)
{
	#ifdef _DEBUG
	setDebugName("CArchiveLoaderZIP");
	#endif
}

//! returns true if the file maybe is able to be loaded by this class
bool CArchiveLoaderZIP::isALoadableFileFormat(const io::path& filename) const
{
	return core::hasFileExtension(filename, "zip", "pk3") ||
	       core::hasFileExtension(filename, "gz", "tgz");
}

//! Check to see if the loader can create archives of this type.
bool CArchiveLoaderZIP::isALoadableFileFormat(E_FILE_ARCHIVE_TYPE fileType) const
{
	return (fileType == EFAT_ZIP || fileType == EFAT_GZIP);
}


//! Creates an archive from the filename
/** \param file File handle to check.
\return Pointer to newly created archive, or 0 upon error. */
IFileArchive* CArchiveLoaderZIP::createArchive(const io::path& filename, bool ignoreCase, bool ignorePaths) const
{
	IFileArchive *archive = 0;
	io::IReadFile* file = FileSystem->createAndOpenFile(filename);

	if (file)
	{
		archive = createArchive(file, ignoreCase, ignorePaths);
		file->drop();
	}

	return archive;
}

//! creates/loads an archive from the file.
//! \return Pointer to the created archive. Returns 0 if loading failed.
IFileArchive* CArchiveLoaderZIP::createArchive(io::IReadFile* file, bool ignoreCase, bool ignorePaths) const
{
	IFileArchive *archive = 0;
	if (file)
	{
		file->seek(0);

		u16 sig;
		file->read(&sig, 2);

#ifdef __BIG_ENDIAN__
		os::Byteswap::byteswap(sig);
#endif

		file->seek(0);

		bool isGZip = (sig == 0x8b1f);

		archive = new CZipReader(file, ignoreCase, ignorePaths, isGZip);
	}
	return archive;
}

//! Check if the file might be loaded by this class
/** Check might look into the file.
\param file File handle to check.
\return True if file seems to be loadable. */
bool CArchiveLoaderZIP::isALoadableFileFormat(io::IReadFile* file) const
{
	SZIPFileHeader header;

	file->read( &header.Sig, 4 );
#ifdef __BIG_ENDIAN__
	os::Byteswap::byteswap(header.Sig);
#endif

	return header.Sig == 0x04034b50 || // ZIP
		   (header.Sig&0xffff) == 0x8b1f; // gzip
}

// -----------------------------------------------------------------------------
// zip archive
// -----------------------------------------------------------------------------

CZipReader::CZipReader(IReadFile* file, bool ignoreCase, bool ignorePaths, bool isGZip)
 : CFileList(file ? file->getFileName() : "", ignoreCase, ignorePaths), File(file), IsGZip(isGZip)
{
	#ifdef _DEBUG
	setDebugName("CZipReader");
	#endif

	if (File)
	{
		File->grab();

		// load file entries
		if (IsGZip)
			while (scanGZipHeader()) { }
		else
			while (scanZipHeader()) { }

		sort();
	}
}

CZipReader::~CZipReader()
{
	if (File)
		File->drop();
}


//! get the archive type
E_FILE_ARCHIVE_TYPE CZipReader::getType() const
{
	return IsGZip ? EFAT_GZIP : EFAT_ZIP;
}

const IFileList* CZipReader::getFileList() const
{
	return this;
}

#if 0
#include <windows.h>

const c8 *sigName( u32 sig )
{
	switch ( sig )
	{
		case 0x04034b50: return "PK0304";
		case 0x02014b50: return "PK0102";
		case 0x06054b50: return "PK0506";
	}
	return "unknown";
}

bool CZipReader::scanLocalHeader2()
{
	c8 buf [ 128 ];
	c8 *c;

	File->read( &temp.header.Sig, 4 );

#ifdef __BIG_ENDIAN__
	os::Byteswap::byteswap(temp.header.Sig);
#endif

	sprintf ( buf, "sig: %08x,%s,", temp.header.Sig, sigName ( temp.header.Sig ) );
	OutputDebugStringA ( buf );

	// Local File Header
	if ( temp.header.Sig == 0x04034b50 )
	{
		File->read( &temp.header.VersionToExtract, sizeof( temp.header ) - 4 );

		temp.zipFileName.reserve( temp.header.FilenameLength+2);
		c = (c8*) temp.zipFileName.c_str();
		File->read( c, temp.header.FilenameLength);
		c [ temp.header.FilenameLength ] = 0;
		temp.zipFileName.verify();

		sprintf ( buf, "%d,'%s'\n", temp.header.CompressionMethod, c );
		OutputDebugStringA ( buf );

		if (temp.header.ExtraFieldLength)
		{
			File->seek( temp.header.ExtraFieldLength, true);
		}

		if (temp.header.GeneralBitFlag & ZIP_INFO_IN_DATA_DESCRIPTOR)
		{
			// read data descriptor
			File->seek(sizeof(SZIPFileDataDescriptor), true );
		}

		// compressed data
		temp.fileDataPosition = File->getPos();
		File->seek( temp.header.DataDescriptor.CompressedSize, true);
		FileList.push_back( temp );
		return true;
	}

	// Central directory structure
	if ( temp.header.Sig == 0x04034b50 )
	{
		//SZIPFileCentralDirFileHeader h;
		//File->read( &h, sizeof( h ) - 4 );
		return true;
	}

	// End of central dir
	if ( temp.header.Sig == 0x06054b50 )
	{
		return true;
	}

	// eof
	if ( temp.header.Sig == 0x02014b50 )
	{
		return false;
	}

	return false;
}

#endif

//! scans for a local header, returns false if there is no more local file header.
//! The gzip file format seems to think that there can be multiple files in a gzip file
//! but none
bool CZipReader::scanGZipHeader()
{
	SZipFileEntry entry;
	entry.Offset = 0;
	memset(&entry.header, 0, sizeof(SZIPFileHeader));

	// read header
	SGZIPMemberHeader header;
	if (File->read(&header, sizeof(SGZIPMemberHeader)) == sizeof(SGZIPMemberHeader))
	{

#ifdef __BIG_ENDIAN__
		os::Byteswap::byteswap(header.sig);
		os::Byteswap::byteswap(header.time);
#endif

		// check header value
		if (header.sig != 0x8b1f)
			return false;

		// now get the file info
		if (header.flags & EGZF_EXTRA_FIELDS)
		{
			// read lenth of extra data
			u16 dataLen;

			File->read(&dataLen, 2);

#ifdef __BIG_ENDIAN__
			os::Byteswap::byteswap(dataLen);
#endif

			// skip it
			File->seek(dataLen, true);
		}

		io::path ZipFileName = "";

		if (header.flags & EGZF_FILE_NAME)
		{
			c8 c;
			File->read(&c, 1);
			while (c)
			{
				ZipFileName.append(c);
				File->read(&c, 1);
			}
		}
		else
		{
			// no file name?
			ZipFileName = Path;
			core::deletePathFromFilename(ZipFileName);

			// rename tgz to tar or remove gz extension
			if (core::hasFileExtension(ZipFileName, "tgz"))
			{
				ZipFileName[ ZipFileName.size() - 2] = 'a';
				ZipFileName[ ZipFileName.size() - 1] = 'r';
			}
			else if (core::hasFileExtension(ZipFileName, "gz"))
			{
				ZipFileName[ ZipFileName.size() - 3] = 0;
				ZipFileName.validate();
			}
		}

		if (header.flags & EGZF_COMMENT)
		{
			c8 c='a';
			while (c)
				File->read(&c, 1);
		}

		if (header.flags & EGZF_CRC16)
			File->seek(2, true);

		// we are now at the start of the data blocks
		entry.Offset = File->getPos();

		entry.header.FilenameLength = ZipFileName.size();

		entry.header.CompressionMethod = header.compressionMethod;
		entry.header.DataDescriptor.CompressedSize = (File->getSize() - 8) - File->getPos();

		// seek to file end
		File->seek(entry.header.DataDescriptor.CompressedSize, true);

		// read CRC
		File->read(&entry.header.DataDescriptor.CRC32, 4);
		// read uncompressed size
		File->read(&entry.header.DataDescriptor.UncompressedSize, 4);

#ifdef __BIG_ENDIAN__
		os::Byteswap::byteswap(entry.header.DataDescriptor.CRC32);
		os::Byteswap::byteswap(entry.header.DataDescriptor.UncompressedSize);
#endif

		//! now we've filled all the fields, this is just a standard deflate block
		addItem(ZipFileName, entry.header.DataDescriptor.UncompressedSize, false, 0);
		FileInfo.push_back(entry);
	}

	// there's only one block of data in a gzip file
	return false;
}

//! scans for a local header, returns false if there is no more local file header.
bool CZipReader::scanZipHeader()
{
	io::path ZipFileName = "";
	SZipFileEntry entry;
	entry.Offset = 0;
	memset(&entry.header, 0, sizeof(SZIPFileHeader));

	File->read(&entry.header, sizeof(SZIPFileHeader));

#ifdef __BIG_ENDIAN__
		entry.header.Sig = os::Byteswap::byteswap(entry.header.Sig);
		entry.header.VersionToExtract = os::Byteswap::byteswap(entry.header.VersionToExtract);
		entry.header.GeneralBitFlag = os::Byteswap::byteswap(entry.header.GeneralBitFlag);
		entry.header.CompressionMethod = os::Byteswap::byteswap(entry.header.CompressionMethod);
		entry.header.LastModFileTime = os::Byteswap::byteswap(entry.header.LastModFileTime);
		entry.header.LastModFileDate = os::Byteswap::byteswap(entry.header.LastModFileDate);
		entry.header.DataDescriptor.CRC32 = os::Byteswap::byteswap(entry.header.DataDescriptor.CRC32);
		entry.header.DataDescriptor.CompressedSize = os::Byteswap::byteswap(entry.header.DataDescriptor.CompressedSize);
		entry.header.DataDescriptor.UncompressedSize = os::Byteswap::byteswap(entry.header.DataDescriptor.UncompressedSize);
		entry.header.FilenameLength = os::Byteswap::byteswap(entry.header.FilenameLength);
		entry.header.ExtraFieldLength = os::Byteswap::byteswap(entry.header.ExtraFieldLength);
#endif

	if (entry.header.Sig != 0x04034b50)
		return false; // local file headers end here.

	// read filename
	{
		c8 *tmp = new c8 [ entry.header.FilenameLength + 2 ];
		File->read(tmp, entry.header.FilenameLength);
		tmp[entry.header.FilenameLength] = 0;
		ZipFileName = tmp;
		delete [] tmp;
	}

#ifdef _IRR_COMPILE_WITH_ZIP_ENCRYPTION_
	// AES encryption
	if ((entry.header.GeneralBitFlag & ZIP_FILE_ENCRYPTED) && (entry.header.CompressionMethod == 99))
	{
		s16 restSize = entry.header.ExtraFieldLength;
		SZipFileExtraHeader extraHeader;
		while (restSize)
		{
			File->read(&extraHeader, sizeof(extraHeader));
#ifdef __BIG_ENDIAN__
			extraHeader.ID = os::Byteswap::byteswap(extraHeader.ID);
			extraHeader.Size = os::Byteswap::byteswap(extraHeader.Size);
#endif
			restSize -= sizeof(extraHeader);
			if (extraHeader.ID==(s16)0x9901)
			{
				SZipFileAESExtraData data;
				File->read(&data, sizeof(data));
#ifdef __BIG_ENDIAN__
				data.Version = os::Byteswap::byteswap(data.Version);
				data.CompressionMode = os::Byteswap::byteswap(data.CompressionMode);
#endif
				restSize -= sizeof(data);
				if (data.Vendor[0]=='A' && data.Vendor[1]=='E')
				{
					// encode values into Sig
					// AE-Version | Strength | ActualMode
					entry.header.Sig =
						((data.Version & 0xff) << 24) |
						(data.EncryptionStrength << 16) |
						(data.CompressionMode);
					File->seek(restSize, true);
					break;
				}
			}
		}
	}
	// move forward length of extra field.
	else
#endif
	if (entry.header.ExtraFieldLength)
		File->seek(entry.header.ExtraFieldLength, true);

	// if bit 3 was set, read DataDescriptor, following after the compressed data
	if (entry.header.GeneralBitFlag & ZIP_INFO_IN_DATA_DESCRIPTOR)
	{
		// read data descriptor
		File->read(&entry.header.DataDescriptor, sizeof(entry.header.DataDescriptor));
#ifdef __BIG_ENDIAN__
		entry.header.DataDescriptor.CRC32 = os::Byteswap::byteswap(entry.header.DataDescriptor.CRC32);
		entry.header.DataDescriptor.CompressedSize = os::Byteswap::byteswap(entry.header.DataDescriptor.CompressedSize);
		entry.header.DataDescriptor.UncompressedSize = os::Byteswap::byteswap(entry.header.DataDescriptor.UncompressedSize);
#endif
	}

	// store position in file
	entry.Offset = File->getPos();
	// move forward length of data
	File->seek(entry.header.DataDescriptor.CompressedSize, true);

	#ifdef _DEBUG
	//os::Debuginfo::print("added file from archive", ZipFileName.c_str());
	#endif

	addItem(ZipFileName, entry.header.DataDescriptor.UncompressedSize, false, FileInfo.size());
	FileInfo.push_back(entry);

	return true;
}


//! opens a file by file name
IReadFile* CZipReader::createAndOpenFile(const io::path& filename)
{
	s32 index = findFile(filename, false);

	if (index != -1)
		return createAndOpenFile(index);

	return 0;
}


//! opens a file by index
IReadFile* CZipReader::createAndOpenFile(u32 index)
{

	//0 - The file is stored (no compression)
	//1 - The file is Shrunk
	//2 - The file is Reduced with compression factor 1
	//3 - The file is Reduced with compression factor 2
	//4 - The file is Reduced with compression factor 3
	//5 - The file is Reduced with compression factor 4
	//6 - The file is Imploded
	//7 - Reserved for Tokenizing compression algorithm
	//8 - The file is Deflated
	//9 - Reserved for enhanced Deflating
	//10 - PKWARE Date Compression Library Imploding

	const SZipFileEntry &e = FileInfo[Files[index].ID];
	wchar_t buf[64];
	s16 actualCompressionMethod=e.header.CompressionMethod;
	IReadFile* decrypted=0;
	u8* decryptedBuf=0;
	u32 decryptedSize=e.header.DataDescriptor.CompressedSize;
#ifdef _IRR_COMPILE_WITH_ZIP_ENCRYPTION_
	if ((e.header.GeneralBitFlag & ZIP_FILE_ENCRYPTED) && (e.header.CompressionMethod == 99))
	{
		os::Printer::log("Reading encrypted file.");
		u8 salt[16]={0};
		const u16 saltSize = (((e.header.Sig & 0x00ff0000) >>16)+1)*4;
		File->seek(e.Offset);
		File->read(salt, saltSize);
		char pwVerification[2];
		char pwVerificationFile[2];
		File->read(pwVerification, 2);
		fcrypt_ctx zctx; // the encryption context
		int rc = fcrypt_init(
			(e.header.Sig & 0x00ff0000) >>16,
			(const unsigned char*)Password.c_str(), // the password
			Password.size(), // number of bytes in password
			salt, // the salt
			(unsigned char*)pwVerificationFile, // on return contains password verifier
			&zctx); // encryption context
		if (strncmp(pwVerificationFile, pwVerification, 2))
		{
			os::Printer::log("Wrong password");
			return 0;
		}
		decryptedSize= e.header.DataDescriptor.CompressedSize-saltSize-12;
		decryptedBuf= new u8[decryptedSize];
		u32 c = 0;
		while ((c+32768)<=decryptedSize)
		{
			File->read(decryptedBuf+c, 32768);
			fcrypt_decrypt(
				decryptedBuf+c, // pointer to the data to decrypt
				32768,   // how many bytes to decrypt
				&zctx); // decryption context
			c+=32768;
		}
		File->read(decryptedBuf+c, decryptedSize-c);
		fcrypt_decrypt(
			decryptedBuf+c, // pointer to the data to decrypt
			decryptedSize-c,   // how many bytes to decrypt
			&zctx); // decryption context

		char fileMAC[10];
		char resMAC[10];
		rc = fcrypt_end(
			(unsigned char*)resMAC, // on return contains the authentication code
			&zctx); // encryption context
		if (rc != 10)
		{
			os::Printer::log("Error on encryption closing");
			delete [] decryptedBuf;
			return 0;
		}
		File->read(fileMAC, 10);
		if (strncmp(fileMAC, resMAC, 10))
		{
			os::Printer::log("Error on encryption check");
			delete [] decryptedBuf;
			return 0;
		}
		decrypted = io::createMemoryReadFile(decryptedBuf, decryptedSize, Files[index].FullName, true);
		actualCompressionMethod = (e.header.Sig & 0xffff);
#if 0
		if ((e.header.Sig & 0xff000000)==0x01000000)
		{
		}
		else if ((e.header.Sig & 0xff000000)==0x02000000)
		{
		}
		else
		{
			os::Printer::log("Unknown encryption method");
			return 0;
		}
#endif
	}
#endif
	switch(actualCompressionMethod)
	{
	case 0: // no compression
		{
			if (decrypted)
				return decrypted;
			else
				return createLimitReadFile(Files[index].FullName, File, e.Offset, decryptedSize);
		}
	case 8:
		{
  			#ifdef _IRR_COMPILE_WITH_ZLIB_

			const u32 uncompressedSize = e.header.DataDescriptor.UncompressedSize;
			const u32 compressedSize   = e.header.DataDescriptor.CompressedSize;			
			c8* pBuf = new c8[ uncompressedSize ];
			if (!pBuf)
			{
				swprintf ( buf, 64, L"Not enough memory for decompressing %s", Files[index].FullName.c_str() );
				os::Printer::log( buf, ELL_ERROR);
				if (decrypted)
					decrypted->drop();
				return 0;
			}

			u8 *pcData = decryptedBuf;
			if (!pcData)
			{
				pcData = new u8[compressedSize];
				if (!pcData)
				{
					swprintf ( buf, 64, L"Not enough memory for decompressing %s", Files[index].FullName.c_str() );
					os::Printer::log( buf, ELL_ERROR);
					delete [] pBuf;
					return 0;
				}

				//memset(pcData, 0, decryptedSize);
				File->seek(e.Offset);
				File->read(pcData, compressedSize);
			}

			// Setup the inflate stream.
			z_stream stream;
			s32 err;

			stream.next_in = (Bytef*)pcData;
			stream.avail_in = (uInt)compressedSize;
			stream.next_out = (Bytef*)pBuf;
			stream.avail_out = uncompressedSize;
			stream.zalloc = (alloc_func)0;
			stream.zfree = (free_func)0;

			// Perform inflation. wbits < 0 indicates no zlib header inside the data.
			err = inflateInit2(&stream, -MAX_WBITS);
			if (err == Z_OK)
			{
				err = inflate(&stream, Z_FINISH);
				inflateEnd(&stream);
				if (err == Z_STREAM_END)
					err = Z_OK;
				err = Z_OK;
				inflateEnd(&stream);
			}

			if (decrypted)
				decrypted->drop();
			else
				delete[] pcData;

			if (err != Z_OK)
			{
				swprintf ( buf, 64, L"Error decompressing %s", Files[index].FullName.c_str() );
				os::Printer::log( buf, ELL_ERROR);
				delete [] pBuf;
				return 0;
			}
			else
				return io::createMemoryReadFile(pBuf, uncompressedSize, Files[index].FullName, true);

			#else
			return 0; // zlib not compiled, we cannot decompress the data.
			#endif
		}
	default:
		swprintf ( buf, 64, L"file has unsupported compression method. %s", Files[index].FullName.c_str() );
		os::Printer::log( buf, ELL_ERROR);
		return 0;
	};

}

} // end namespace io
} // end namespace irr

#endif // __IRR_COMPILE_WITH_ZIP_ARCHIVE_LOADER_
