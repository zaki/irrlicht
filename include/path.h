// Copyright (C) 2002-2009 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine" and the "irrXML" project.
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __IRR_PATH_H_INCLUDED__
#define __IRR_PATH_H_INCLUDED__

#include "irrString.h"

namespace irr
{
namespace io
{

//! Type used for all file system related strings.
/** This type will transparently handle different file system encodings. */
typedef core::string<fschar_t> path;

//! Used in places where we identify objects by a filename, but don't actually work with the real filename
/** Irrlicht is internally not case-sensitive when it comes to names.
    Also this class is a first step towards support for correctly serializing renamed objects.
*/
struct SNamedPath
{
	//! Constructor
	SNamedPath() {}

	//! Constructor
	SNamedPath(const path& p) : Path(p), Name(p)
	{
		Name.replace( '\\', '/' );
		Name.make_lower();
	}

	//! Is smaller comparator
	bool operator <(const SNamedPath& other) const
	{
		return Name < other.Name;
	}

	//! Set the path. As the name depends on the path it will also be changed.
	void setPath(const path& p)
	{
		Path = p;
		Name = p;
		Name.make_lower();
	}

	//! Get the path
	const path& getPath() const
	{
		return Path;
	};

	//! Give the file a new name which is used for identification.
	void rename(const path& name)
	{
		Name = name;
	};

	//! Has the file been given a new name?
	bool isRenamed() const
	{
		return !Path.equals_ignore_case(Name);
	}

	//! Get the name which is used to identify the file. By default a lower-case version of the filename.
	const path& getName() const
	{
		return Name;
	}

	//! Returns the name used in serialization
	/** When the name wasn't renamed the path is used otherwise the name is used.
		TODO: This is a workaround, as both strings should be serialized in the long run.
	*/
	const path& getSerializationName() const
	{
		if ( isRenamed() )
			return getName();
		return Path;
	}

private:
	path Path;
	path Name;
};

} // io
} // irr

#endif // __IRR_PATH_H_INCLUDED__
