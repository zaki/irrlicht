#include "irrlicht.h"
#include "testUtils.h"

using namespace irr;
using namespace core;
using namespace io;

bool zipReader(void)
{
	IrrlichtDevice * device = irr::createDevice(video::EDT_NULL, dimension2d<u32>(1, 1));
	assert(device);
	if(!device)
		return false;
	
	io::IFileSystem * fs = device->getFileSystem ();
	if ( !fs )
		return false;
	
	if ( !fs->registerFileArchive(core::string<c16>("media/file_with_path.zip"), /*bool ignoreCase=*/true, /*bool ignorePaths=*/false) )
		return false;
	
	// log what we got
	io::IFileArchive* archive = fs->getFileArchive(fs->getFileArchiveCount()-1);
	for ( u32 f=0; f < archive->getFileCount(); ++f)
	{
		const io::IFileArchiveEntry* entry = archive->getFileInfo(f);
		logTestString("simpleFileName: %s\n", entry->simpleFileName.c_str());
		logTestString("path: %s\n", entry->path.c_str());
	}
	
	bool result = true;
	core::string<c16> filename("mypath/myfile.txt");
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
	return result;
}
