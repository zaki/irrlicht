#ifndef __IRR_I_MESH_WRITER_H_INCLUDED__
#define __IRR_I_MESH_WRITER_H_INCLUDED__

#include "IReferenceCounted.h"
#include "EMeshWriterEnums.h"

namespace irr
{
namespace io
{
	class IWriteFile;
} // end namespace io

namespace scene
{
	class IMesh;

	// interface for writing meshes
	class IMeshWriter : public virtual IReferenceCounted
	{
	public:

		virtual ~IMeshWriter() {}

		//! Returns the type of the mesh writer
		/** For own implementations, use  MAKE_IRR_ID as shown in the EMESH_WRITER_TYPE
		enumeration to return your own unique mesh type id.*/
		virtual EMESH_WRITER_TYPE getType() const = 0;

		//! writes a static mesh
		/** \return Returns true if sucessful */
		virtual bool writeMesh(io::IWriteFile* file, scene::IMesh* mesh,
							s32 flags=EMWF_NONE) = 0;

		// writes an animated mesh
		// for future use, no writer is able to write animated meshes currently
		/* \return Returns true if sucessful */
		//virtual bool writeAnimatedMesh(io::IWriteFile* file,
		// scene::IAnimatedMesh* mesh,
		// s32 flags=EMWF_NONE) = 0;
	};


} // end namespace
} // end namespace

#endif

