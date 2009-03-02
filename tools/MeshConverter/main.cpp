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

void usage(const char* name)
{
	std::cerr << "Usage: " << name << " [options] <srcFile> <destFile>" << std::endl;
	std::cerr << "  where options are" << std::endl;
	std::cerr << " --createTangents: convert to tangents mesh is possible." << std::endl;
	std::cerr << " --format=[irrmesh|collada|stl|obj|ply]: Choose target format" << std::endl;
}

int main(int argc, char* argv[])
{
	if ((argc < 3) ||
			((argc==3) && (argv[1][0]=='-')))
	{
		usage(argv[0]);
		return 1;
	}

	IrrlichtDevice *device = createDevice( video::EDT_NULL,
			dimension2d<u32>(800, 600), 32, false, false, false, 0);

	device->setWindowCaption(L"Mesh Converter");

	scene::EMESH_WRITER_TYPE type = EMWT_IRR_MESH;
	u32 i=1;
	bool createTangents=false;
	while (argv[i][0]=='-')
	{
		core::stringc format = argv[i];
		if (format.size() > 3)
		{
			if (format.equalsn("--format=",9))
			{
				format = format.subString(9,format.size());
				if (format=="collada")
					type = EMWT_COLLADA;
				else if (format=="stl")
					type = EMWT_STL;
				else if (format=="obj")
					type = EMWT_OBJ;
				else if (format=="ply")
					type = EMWT_PLY;
				else
					type = EMWT_IRR_MESH;
			}
			else
			if (format =="--createTangents")
				createTangents=true;
		}
		else
		if (format=="--")
		{
			++i;
			break;
		}
		++i;
	}

	const s32 srcmesh = i;
	const s32 destmesh = i+1;

	--argc;
	if ((argc<srcmesh) || (argc<destmesh))
	{
		std::cerr << "Not enough files given." << std::endl;
		usage(argv[0]);
		return 1;
	}

	createTangents = createTangents && (type==EMWT_IRR_MESH);
	std::cout << "Converting " << argv[srcmesh] << " to " << argv[destmesh] << std::endl;
	IMesh* mesh = device->getSceneManager()->getMesh(argv[srcmesh])->getMesh(0);
	if (!mesh)
	{
		std::cerr << "Could not load " << argv[srcmesh] << std::endl;
		return 1;
	}
	if (createTangents)
	{
		IMesh* tmp = device->getSceneManager()->getMeshManipulator()->createMeshWithTangents(mesh);
		mesh->drop();
		mesh=tmp;
	}
	IMeshWriter* mw = device->getSceneManager()->createMeshWriter(type);
	IWriteFile* file = device->getFileSystem()->createAndWriteFile(argv[destmesh]);
	mw->writeMesh(file, mesh);
	
	file->drop();
	mw->drop();
	device->drop();

	return 0;
}

