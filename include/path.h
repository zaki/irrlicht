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
	SNamedPath(const path& p) : Path(p), Name( PathToName(p) )
	{
	}

	//! Is smaller comparator
	bool operator <(const SNamedPath& other) const
	{
		return Name < other.Name;
	}

	//! Set the path. As the name depends on the path the name will also be changed.
	void setPath(const path& p)
	{
		Path = p;
		Name = PathToName(p);
	}

	//! Get the path. This is the original, unprocessed string passed to SNamedPath.
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
		// Note: memory over speed here because of the typical use-cases.
		return PathToName(Path) != Name;
	}

	//! Get the name which is used to identify the file.
	//! This string is similar to the names and filenames used before Irrlicht 1.7
	const path& getName() const
	{
		return Name;
	}

	//! Returns the string which should be used in serialization.
	const path& getSerializationName() const
	{
		if ( isRenamed() )
			return getName();
		return Path;
	}

protected:
	// convert the given path string to a name string.
	path PathToName(const path& p) const
	{
		path name(p);
		name.replace( '\\', '/' );
		name.make_lower();
		return name;
	}

private:
	path Path;
	path Name;
};

} // io
} // irr

#endif // __IRR_PATH_H_INCLUDED__
