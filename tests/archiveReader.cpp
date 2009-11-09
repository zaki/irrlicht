#include "testUtils.h"

using namespace irr;
using namespace core;
using namespace io;

bool testArchive(IFileSystem* fs, const io::path& archiveName)
{
	if ( !fs->addFileArchive(archiveName, /*bool ignoreCase=*/true, /*bool ignorePaths=*/false) )
		return false;

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
		logTestString("existFile with deep path failed");
		return false;
	}

	filename="test/test.txt";
	if (!fs->existFile(filename))
	{
		logTestString("existFile failed");
		return false;
	}

	IReadFile* readFile = fs->createAndOpenFile(filename);
	if ( !readFile )
	{
		logTestString("createAndOpenFilefailed");
		return false;
	}

	char tmp[13] = {'\0'};
	readFile->read(tmp, 12);
	if (strncmp(tmp, "Hello world!", 12))
	{
		logTestString("Read bad data from archive: %s", tmp);
		return false;
	}
	if (!fs->removeFileArchive(fs->getFileArchiveCount()-1))
	{
		logTestString("Couldn't remove archive.\n");
		return false;
	}
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
	logTestString("Testing zip files.\n");
	ret &= testArchive(fs, "media/file_with_path.zip");
	logTestString("Testing pak files.\n");
	ret &= testArchive(fs, "media/sample_pakfile.pak");
	logTestString("Testing npk files.\n");
	ret &= testArchive(fs, "media/file_with_path.npk");
	return ret;
}

