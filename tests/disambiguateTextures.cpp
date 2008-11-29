#include <irrlicht.h>
#include <assert.h>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

bool disambiguateTextures(void)
{
	IrrlichtDevice *device =
		createDevice( video::EDT_SOFTWARE, dimension2d<s32>(640, 480), 16,
			false, false, false, 0);

	if (!device)
		return false;

	IVideoDriver * driver = device->getVideoDriver();

	ITexture * tex1 = driver->getTexture("../media/tools.png");
	ITexture * tex2 = driver->getTexture("../media/tools.png");

	IReadFile * readFile = device->getFileSystem()->createAndOpenFile("../media/tools.png");
	assert(readFile);

	ITexture * tex3 = driver->getTexture(readFile);
	readFile->drop();

	// Expects an empty tmp/tmp directory under this app's wd and
	// a media directory under this apps' directory with tools.png in it.
	stringc wd = device->getFileSystem()->getWorkingDirectory();
	stringc newWd = wd + "/empty/empty";
	bool changed = device->getFileSystem()->changeWorkingDirectoryTo(newWd.c_str());
	assert(changed);
	ITexture * tex4 = driver->getTexture("../../media/tools.png");
	assert(tex4);
	assert(tex1 == tex2 && tex1 == tex3 && tex1 != tex4);

	changed &= device->getFileSystem()->changeWorkingDirectoryTo(wd.c_str());

	device->drop();

	return (changed && tex1 == tex2 && tex1 == tex3 && tex1 != tex4) ? true : false;
}
 	  	 
