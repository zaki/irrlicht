#include "testUtils.h"

using namespace irr;
using namespace core;
using namespace io;

bool testArchive(IFileSystem* fs, const io::path& archiveName)
{
	// make sure there is no archive mounted
	if ( fs->getFileArchiveCount() )
	{
		logTestString("Already mounted archives found\n");
		return false;
	}

	if ( !fs->addFileArchive(archiveName, /*bool ignoreCase=*/true, /*bool ignorePaths=*/false) )
	{
		logTestString("Mounting archive failed\n");
		return false;
	}

	// make sure there is an archive mounted
	if ( !fs->getFileArchiveCount() )
	{
		logTestString("Mounted archive not in list\n");
		return false;
	}

	// mount again
	if ( !fs->addFileArchive(archiveName, /*bool ignoreCase=*/true, /*bool ignorePaths=*/false) )
	{
		logTestString("Mounting a second time failed\n");
		fs->removeFileArchive(fs->getFileArchiveCount()-1);
		return false;
	}

	// make sure there is exactly one archive mounted
	if ( fs->getFileArchiveCount() != 1 )
	{
		logTestString("Duplicate mount not recognized\n");
		while (fs->getFileArchiveCount())
			fs->removeFileArchive(fs->getFileArchiveCount()-1);
		return false;
	}
	if (fs->getFileArchive(0)->getType()==io::EFAT_FOLDER)
	{
		// mount again with different path end symbol (either with slash or without)
		core::stringc newArchiveName=archiveName;
		if (archiveName.lastChar()=='/')
			newArchiveName.erase(newArchiveName.size()-1);
		else
			newArchiveName.append('/');
		if ( !fs->addFileArchive(newArchiveName, /*bool ignoreCase=*/true, /*bool ignorePaths=*/false) )
		{
			logTestString("Mounting a second time with different name failed\n");
			fs->removeFileArchive(fs->getFileArchiveCount()-1);
			return false;
		}

		// make sure there is exactly one archive mounted
		if ( fs->getFileArchiveCount() != 1 )
		{
			logTestString("Duplicate mount with different filename not recognized\n");
			while (fs->getFileArchiveCount())
				fs->removeFileArchive(fs->getFileArchiveCount()-1);
			return false;
		}
	}

	// log what we got
	io::IFileArchive* archive = fs->getFileArchive(fs->getFileArchiveCount()-1);
	const io::IFileList* fileList = archive->getFileList();
	for ( u32 f=0; f < fileList->getFileCount(); ++f)
	{
		logTestString("File name: %s\n", fileList->getFileName(f).c_str());
		logTestString("Full path: %s\n", fileList->getFullFileName(f).c_str());
	}
	
	io::path filename("mypath/mypath/myfile.txt");
	if (!fs->existFile(filename))
	{
		logTestString("existFile with deep path failed\n");
		while (fs->getFileArchiveCount())
			fs->removeFileArchive(fs->getFileArchiveCount()-1);
		return false;
	}

	filename="test/test.txt";
	if (!fs->existFile(filename))
	{
		logTestString("existFile failed\n");
		while (fs->getFileArchiveCount())
			fs->removeFileArchive(fs->getFileArchiveCount()-1);
		return false;
	}

	IReadFile* readFile = fs->createAndOpenFile(filename);
	if ( !readFile )
	{
		logTestString("createAndOpenFile failed\n");
		while (fs->getFileArchiveCount())
			fs->removeFileArchive(fs->getFileArchiveCount()-1);
		return false;
	}

	char tmp[13] = {'\0'};
	readFile->read(tmp, 12);
	if (strncmp(tmp, "Hello world!", 12))
	{
		logTestString("Read bad data from archive: %s\n", tmp);
		while (fs->getFileArchiveCount())
			fs->removeFileArchive(fs->getFileArchiveCount()-1);
		return false;
	}
	if (!fs->removeFileArchive(fs->getFileArchiveCount()-1))
	{
		logTestString("Couldn't remove archive.\n");
		return false;
	}

	// make sure there is no archive mounted
	if ( fs->getFileArchiveCount() )
		return false;

	readFile->drop();

	return true;
}

bool testEncryptedZip(IFileSystem* fs)
{
	// make sure there is no archive mounted
	if ( fs->getFileArchiveCount() )
	{
		logTestString("Already mounted archives found\n");
		return false;
	}

	const char* archiveName = "media/enc.zip";
	if ( !fs->addFileArchive(archiveName, /*bool ignoreCase=*/true, /*bool ignorePaths=*/false) )
	{
		logTestString("Mounting archive failed\n");
		return false;
	}

	// make sure there is an archive mounted
	if ( !fs->getFileArchiveCount() )
	{
		logTestString("Mounted archive not in list\n");
		return false;
	}

	// mount again
	if ( !fs->addFileArchive(archiveName, /*bool ignoreCase=*/true, /*bool ignorePaths=*/false) )
	{
		logTestString("Mounting a second time failed\n");
		return false;
	}

	// make sure there is exactly one archive mounted
	if ( fs->getFileArchiveCount() != 1 )
	{
		logTestString("Duplicate mount not recognized\n");
		return false;
	}

	// log what we got
	io::IFileArchive* archive = fs->getFileArchive(fs->getFileArchiveCount()-1);
	const io::IFileList* fileList = archive->getFileList();
	for ( u32 f=0; f < fileList->getFileCount(); ++f)
	{
		logTestString("File name: %s\n", fileList->getFileName(f).c_str());
		logTestString("Full path: %s\n", fileList->getFullFileName(f).c_str());
	}
	
	io::path filename("doc/readme.txt");
	if (!fs->existFile(filename))
	{
		logTestString("existFile failed\n");
		return false;
	}

	IReadFile* readFile = fs->createAndOpenFile(filename);
	if ( readFile )
	{
		logTestString("createAndOpenFile succeeded, even though no password was set.\n");
		return false;
	}

	archive->Password="33445";
	readFile = fs->createAndOpenFile(filename);
#ifdef _IRR_COMPILE_WITH_ZIP_ENCRYPTION_
	if ( !readFile )
	{
		logTestString("createAndOpenFile failed\n");
		return false;
	}

	char tmp[13] = {'\0'};
	readFile->read(tmp, 12);
	if (strncmp(tmp, "Linux Users:", 12))
	{
		logTestString("Read bad data from archive: %s\n", tmp);
		return false;
	}
#endif

	if (!fs->removeFileArchive(fs->getFileArchiveCount()-1))
	{
		logTestString("Couldn't remove archive.\n");
		return false;
	}

	// make sure there is no archive mounted
	if ( fs->getFileArchiveCount() )
		return false;

	readFile->drop();

	return true;
}

bool archiveReader()
{
	IrrlichtDevice * device = irr::createDevice(video::EDT_NULL, dimension2d<u32>(1, 1));
	assert(device);
	if(!device)
		return false;
	
	io::IFileSystem * fs = device->getFileSystem ();
	if ( !fs )
		return false;
	
	bool ret = true;
	logTestString("Testing mount file.\n");
	ret &= testArchive(fs, "media/file_with_path");
	logTestString("Testing mount file.\n");
	ret &= testArchive(fs, "media/file_with_path/");
	logTestString("Testing zip files.\n");
	ret &= testArchive(fs, "media/file_with_path.zip");
	logTestString("Testing pak files.\n");
	ret &= testArchive(fs, "media/sample_pakfile.pak");
	logTestString("Testing npk files.\n");
	ret &= testArchive(fs, "media/file_with_path.npk");
	logTestString("Testing encrypted zip files.\n");
	ret &= testEncryptedZip(fs);

	device->drop();

	return ret;
}

