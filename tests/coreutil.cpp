#include "testUtils.h"

using namespace irr;
using namespace core;

bool testMergeFilename()
{
	// path mergeFilename(const path& path, const path& filename, const path& extension = "");
	if ( mergeFilename(_IRR_TEXT(""), _IRR_TEXT(""), _IRR_TEXT("") ) != io::path(_IRR_TEXT("")) )
		return false;
	if ( mergeFilename(_IRR_TEXT("folder"), _IRR_TEXT(""), _IRR_TEXT("") ) != io::path(_IRR_TEXT("folder/")) )
		return false;
	if ( mergeFilename(_IRR_TEXT("folder/"), _IRR_TEXT(""), _IRR_TEXT("") ) != io::path(_IRR_TEXT("folder/")) )
		return false;
	if ( mergeFilename(_IRR_TEXT("folder"), _IRR_TEXT("file"), _IRR_TEXT("") ) != io::path(_IRR_TEXT("folder/file")) )
		return false;
	if ( mergeFilename(_IRR_TEXT("folder/"), _IRR_TEXT("file"), _IRR_TEXT("") ) != io::path(_IRR_TEXT("folder/file")) )
		return false;
	if ( mergeFilename(_IRR_TEXT("folder\\"), _IRR_TEXT("file"), _IRR_TEXT("") ) != io::path(_IRR_TEXT("folder\\file")) )
		return false;
	if ( mergeFilename(_IRR_TEXT(""), _IRR_TEXT("file"), _IRR_TEXT("") ) != io::path(_IRR_TEXT("file")) )
		return false;
	if ( mergeFilename(_IRR_TEXT("."), _IRR_TEXT("file"), _IRR_TEXT("") ) != io::path(_IRR_TEXT("./file")) )
		return false;
	if ( mergeFilename(_IRR_TEXT("folder"), _IRR_TEXT(""), _IRR_TEXT(".bmp") ) != io::path(_IRR_TEXT("folder/.bmp")) )
		return false;
	if ( mergeFilename(_IRR_TEXT("folder/"), _IRR_TEXT(""), _IRR_TEXT(".bmp") ) != io::path(_IRR_TEXT("folder/.bmp")) )
		return false;
	if ( mergeFilename(_IRR_TEXT("folder"), _IRR_TEXT(""), _IRR_TEXT("bmp") ) != io::path(_IRR_TEXT("folder/.bmp")) )
		return false;
	if ( mergeFilename(_IRR_TEXT("."), _IRR_TEXT(""), _IRR_TEXT("bmp") ) != io::path(_IRR_TEXT("./.bmp")) )
		return false;
	if ( mergeFilename(_IRR_TEXT("folder"), _IRR_TEXT("file"), _IRR_TEXT("bmp") ) != io::path(_IRR_TEXT("folder/file.bmp")) )
		return false;
	if ( mergeFilename(_IRR_TEXT("folder/"), _IRR_TEXT("file"), _IRR_TEXT("bmp") ) != io::path(_IRR_TEXT("folder/file.bmp")) )
		return false;
	if ( mergeFilename(_IRR_TEXT("folder/"), _IRR_TEXT("file"), _IRR_TEXT(".bmp") ) != io::path(_IRR_TEXT("folder/file.bmp")) )
		return false;
	if ( mergeFilename(_IRR_TEXT("folder"), _IRR_TEXT("file.bmp"), _IRR_TEXT("") ) != io::path(_IRR_TEXT("folder/file.bmp")) )
		return false;
	if ( mergeFilename(_IRR_TEXT(""), _IRR_TEXT("file"), _IRR_TEXT("bmp") ) != io::path(_IRR_TEXT("file.bmp")) )
		return false;
	if ( mergeFilename(_IRR_TEXT(""), _IRR_TEXT("file"), _IRR_TEXT(".bmp") ) != io::path(_IRR_TEXT("file.bmp")) )
		return false;
	if ( mergeFilename(_IRR_TEXT("c:"), _IRR_TEXT("file"), _IRR_TEXT(".bmp") ) != io::path(_IRR_TEXT("c:/file.bmp")) ) // TODO: is this actually correct on windows?
		return false; 
	
	return true;
}

// Test the functionality of the Irrlicht timer
bool testCoreutil(void)
{
	bool ok = true;
	
	ok &= testMergeFilename();
	
	return ok;
}
