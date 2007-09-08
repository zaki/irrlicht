#ifndef __IRR_IRR_MESH_WRITER_H_INCLUDED__
#define __IRR_IRR_MESH_WRITER_H_INCLUDED__

#include "IMeshWriter.h"
#include "S3DVertex.h"
#include "IVideoDriver.h"
#include "IFileSystem.h"

namespace irr
{
namespace io
{
	class IXMLWriter;
}
namespace scene
{

//! class to write meshes, implementing a IrrMesh (.irrmesh, .xml) writer
/** This writer implementation has been originally developed for irrEdit and then
merged out to the Irrlicht Engine */
class CIrrMeshWriter : public IMeshWriter
{
public:

	CIrrMeshWriter(irr::video::IVideoDriver* driver, irr::io::IFileSystem* fs);
	virtual ~CIrrMeshWriter();

	//! Returns the type of the mesh writer
	virtual EMESH_WRITER_TYPE getType() const;

	//! writes a mesh
	virtual bool writeMesh(io::IWriteFile* file, scene::IMesh* mesh, s32 flags=EMWF_NONE);

protected:

	io::IFileSystem* FileSystem;
	video::IVideoDriver* VideoDriver;
	io::IXMLWriter* Writer;
};


} // end namespace
} // end namespace

#endif

