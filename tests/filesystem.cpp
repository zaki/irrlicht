#include "testUtils.h"

using namespace irr;
using namespace core;
using namespace io;

static bool testFlattenFilename(io::IFileSystem* fs)
{
	bool result=true;
	io::path tmpString="../tmp";
	io::path refString="../tmp/";
	fs->flattenFilename(tmpString);
	if (tmpString != refString)
	{
		logTestString("flattening destroys path.\n%s!=%s\n", tmpString.c_str(),refString.c_str());
		result = false;
	}

	tmpString="tmp/tmp/../";
	refString="tmp/";
	fs->flattenFilename(tmpString);
	if (tmpString != refString)
	{
		logTestString("flattening destroys path.\n%s!=%s\n", tmpString.c_str(),refString.c_str());
		result = false;
	}

	tmpString="tmp/tmp/..";
	fs->flattenFilename(tmpString);
	if (tmpString != refString)
	{
		logTestString("flattening destroys path.\n%s!=%s\n", tmpString.c_str(),refString.c_str());
		result = false;
	}

	tmpString="tmp/next/../third";
	refString="tmp/third/";
	fs->flattenFilename(tmpString);
	if (tmpString != refString)
	{
		logTestString("flattening destroys path.\n%s!=%s\n", tmpString.c_str(),refString.c_str());
		result = false;
	}
	return result;
}

bool filesystem(void)
{
	IrrlichtDevice * device = irr::createDevice(video::EDT_NULL, dimension2d<u32>(1, 1));
	assert(device);
	if(!device)
		return false;
	
	io::IFileSystem * fs = device->getFileSystem ();
	if ( !fs )
		return false;
	
	bool result = true;
	
	io::path workingDir = device->getFileSystem()->getWorkingDirectory();
	
	io::path empty;
	if ( fs->existFile(empty) )
	{
		logTestString("Empty filename should not exist.\n");
		result = false;
	}
	
	io::path newWd = workingDir + "/media";
	bool changed = device->getFileSystem()->changeWorkingDirectoryTo(newWd);
	assert(changed);
	
	if ( fs->existFile(empty) )
	{
		logTestString("Empty filename should not exist even in another workingdirectory.\n");
		result = false;
	}
	
	// The working directory must be restored for the other tests to work.
	changed = device->getFileSystem()->changeWorkingDirectoryTo(workingDir.c_str());
	assert(changed);
	
	// adding  a folder archive which just should not really change anything
	device->getFileSystem()->addFolderFileArchive( "./" );
	
	if ( fs->existFile(empty) )
	{
		logTestString("Empty filename should not exist in folder file archive.\n");
		result = false;
	}
	
	// remove it again to not affect other tests
	device->getFileSystem()->removeFileArchive( device->getFileSystem()->getFileArchiveCount() );

	result |= testFlattenFilename(fs);
	return result;
}

