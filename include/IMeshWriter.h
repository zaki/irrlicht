#ifndef __IRR_I_MESH_WRITER_H_INCLUDED__
#define __IRR_I_MESH_WRITER_H_INCLUDED__

#include "IReferenceCounted.h"
#include "IWriteFile.h"
#include "irrTypes.h"

namespace irr
{
	class IrrlichtDevice;

namespace scene
{
	class IMesh;

	//! An enumeration for all supported types of built-in mesh writers
	/** A scene mesh writers is represented by a four character code
	such as 'irrm' or 'coll' instead of simple numbers, to avoid
	name clashes with external mesh writers.*/
	enum EMESH_WRITER_TYPE
	{
		//! Irrlicht Native mesh writer, for static .irrmesh files.
		EMWT_IRR_MESH     = MAKE_IRR_ID('i','r','r','m'),

		//! COLLADA mesh writer for .dae and .xml files
		EMWT_COLLADA      = MAKE_IRR_ID('c','o','l','l'),
	};


	//! flags configuring mesh writing
	enum E_MESH_WRITER_FLAGS
	{
		//! no writer flags
		EMWF_NONE = 0,

		//! write lightmap textures out if possible
		EMWF_WRITE_LIGHTMAPS = 0x1
	};



	// interface for writing meshes
	class IMeshWriter : public virtual irr::IReferenceCounted
	{
	public:

		virtual ~IMeshWriter() {};

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

