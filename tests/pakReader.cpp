#include "testUtils.h"

using namespace irr;
using namespace core;
using namespace io;

bool pakReader(void)
{
	IrrlichtDevice * device = irr::createDevice(video::EDT_NULL, dimension2d<u32>(1, 1));
	assert(device);
	if(!device)
		return false;
	
	io::IFileSystem * fs = device->getFileSystem ();
	if ( !fs )
		return false;
	
	if ( !fs->addFileArchive(io::path("media/sample_pakfile.pak"), /*bool ignoreCase=*/true, /*bool ignorePaths=*/false) )
		return false;
	
	// log what we got
	io::IFileArchive* archive = fs->getFileArchive(fs->getFileArchiveCount()-1);
	const io::IFileList* fileList = archive->getFileList();
	for ( u32 f=0; f < fileList->getFileCount(); ++f)
	{
		logTestString("File name: %s\n", fileList->getFileName(f).c_str());
		logTestString("Full path: %s\n", fileList->getFullFileName(f).c_str());
	}
	
	bool result = true;
	io::path filename("test/test.txt");
	result &= fs->existFile(filename);
	if (!result )
	{
		logTestString("existFile failed");
	}
	IReadFile* readFile = fs->createAndOpenFile(filename);
	if ( !readFile )
	{
		result = false;
		logTestString("createAndOpenFilefailed");
	}

	char tmp[123] = {'\0'};
	readFile->read(tmp, sizeof(tmp));
	if (strncmp(tmp, "Hello world!", sizeof(tmp)))
	{
		result = false;
		logTestString("Read bad data from pak file.\n");
	}
	return result;
}
