#include <irrlicht.h>
#include <iostream>

using namespace irr;

using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

#ifdef _IRR_WINDOWS_
#pragma comment(lib, "Irrlicht.lib")
#endif

int main(int argc, char* argv[])
{
	if ((argc < 3) ||
			((argc==3) && (argv[1][0]=='-')))
	{
		std::cout << "Usage: " << argv[0] << " [--format=irrmesh|collada|stl] <srcFile> <destFile>" << std::endl;
		return 1;
	}

	IrrlichtDevice *device = createDevice( video::EDT_NULL,
			dimension2d<s32>(800, 600), 32, false, false, false, 0);

	device->setWindowCaption(L"Image Converter");

	IVideoDriver* driver = device->getVideoDriver();

	scene::EMESH_WRITER_TYPE type = EMWT_IRR_MESH;
	u32 srcmesh = 1;
	u32 destmesh = 2;
	if (argv[1][0]=='-')
	{
		core::stringc format = argv[1];
		if ((format.size() > 3) && format.equalsn("--format=",9))
		{
			++srcmesh;
			++destmesh;
			format = format.subString(9,format.size());
			if (format=="collada")
				type = EMWT_COLLADA;
			else
			if (format=="stl")
				type = EMWT_STL;
			else
				type = EMWT_IRR_MESH;
		}
	}
	std::cout << "Converting " << argv[srcmesh] << " to " << argv[destmesh] << std::endl;
	IAnimatedMesh* mesh = device->getSceneManager()->getMesh(argv[srcmesh]);
	if (!mesh)
	{
		std::cerr << "Could not load " << argv[srcmesh] << std::endl;
		return 1;
	}
	IMeshWriter* mw = device->getSceneManager()->createMeshWriter(type);
	IWriteFile* file = device->getFileSystem()->createAndWriteFile(argv[destmesh]);
	mw->writeMesh(file, mesh->getMesh(0));
	file->drop();
	mw->drop();

	return 0;
}

