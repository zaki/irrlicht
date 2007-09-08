#include "CIrrMeshWriter.h"
#include "os.h"
#include "IXMLWriter.h"
#include "IMesh.h"
#include "IAttributes.h"

namespace irr
{
namespace scene
{


CIrrMeshWriter::CIrrMeshWriter(irr::video::IVideoDriver* driver,
				irr::io::IFileSystem* fs)
	: FileSystem(fs), VideoDriver(driver), Writer(0)
{
	if (VideoDriver)
		VideoDriver->grab();

	if (FileSystem)
		FileSystem->grab();
}


CIrrMeshWriter::~CIrrMeshWriter()
{
	if (VideoDriver)
		VideoDriver->drop();

	if (FileSystem)
		FileSystem->drop();
}


//! Returns the type of the mesh writer
EMESH_WRITER_TYPE CIrrMeshWriter::getType() const
{
	return EMWT_IRR_MESH;
}


//! writes a mesh
bool CIrrMeshWriter::writeMesh(io::IWriteFile* file, scene::IMesh* mesh, s32 flags)
{
	if (!file)
		return false;

	Writer = FileSystem->createXMLWriter(file);

	if (!Writer)
	{
		os::Printer::log("Could not write file", file->getFileName());
		return false;
	}

	os::Printer::log("Writing mesh", file->getFileName());

	// write IRR MESH header

	Writer->writeXMLHeader();

	Writer->writeElement(L"irrmesh", false,
		L"xmlns", L"http://irrlicht.sourceforge.net/IRRMESH_08_2007",
		L"version", L"1.0");
	Writer->writeLineBreak();

	// write mesh

	// TODO: implement

	Writer->drop();
	return true;
}



} // end namespace
} // end namespace
