// Copyright (C) 2002-2009 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __IRR_CORE_UTIL_H_INCLUDED__
#define __IRR_CORE_UTIL_H_INCLUDED__

#include "irrString.h"

namespace irr
{
namespace core
{

/*! \file irrxml.h
	\brief File containing useful basic utility functions
*/

// ----------- some basic quite often used string functions -----------------

//! search if a filename has a proper extension
inline s32 isFileExtension (	const core::string<c16>& filename,
								const core::string<c16>& ext0,
								const core::string<c16>& ext1,
								const core::string<c16>& ext2
							)
{
	s32 extPos = filename.findLast ( '.' );
	if ( extPos < 0 )
		return 0;

	extPos += 1;
	if ( filename.equals_substring_ignore_case ( ext0, extPos ) ) return 1;
	if ( filename.equals_substring_ignore_case ( ext1, extPos ) ) return 2;
	if ( filename.equals_substring_ignore_case ( ext2, extPos ) ) return 3;
	return 0;
}

//! search if a filename has a proper extension
inline bool hasFileExtension (	const core::string<c16>& filename,
								const core::string<c16>& ext0,
								const core::string<c16>& ext1 = "",
								const core::string<c16>& ext2 = ""
							)
{
	return isFileExtension ( filename, ext0, ext1, ext2 ) > 0;
}

//! cut the filename extension from a source string and stores in the dest string
inline stringc& cutFilenameExtension ( stringc &dest, const stringc &source )
{
	s32 endPos = source.findLast ( '.' );
	dest = source.subString ( 0, endPos < 0 ? source.size () : endPos );
	return dest;
}

//! cut the filename extension from a source string and stores in the dest string
inline stringw& cutFilenameExtension ( stringw &dest, const stringw &source )
{
	s32 endPos = source.findLast ( '.' );
	dest = source.subString ( 0, endPos < 0 ? source.size () : endPos );
	return dest;
}

//! get the filename extension from a string
inline stringc& getFileNameExtension ( stringc &dest, const stringc &source )
{
	s32 endPos = source.findLast ( '.' );
	if ( endPos < 0 )
		dest = "";
	else
		dest = source.subString ( endPos, source.size () );
	return dest;
}

//! delete path from filename
inline core::stringw& deletePathFromFilename(core::stringw& filename)
{
	// delete path from filename
	const wchar_t *s = filename.c_str();
	const wchar_t* p = s + filename.size();

	// search for path separator or beginning
	while ( *p != '/' && *p != '\\' && p != s )
		p--;

	if ( p != s )
	{
		++p;
		filename = p;
	}
	return filename;
}

//! delete path from filename
inline core::stringc& deletePathFromFilename(core::stringc& filename)
{
	// delete path from filename
	const c8 *s = filename.c_str();
	const c8* p = s + filename.size();

	// search for path separator or beginning
	while ( *p != '/' && *p != '\\' && p != s )
		p--;

	if ( p != s )
	{
		++p;
		filename = p;
	}
	return filename;
}

//! trim paths
inline core::string<c16>& deletePathFromPath(core::string<c16>& filename, s32 pathCount)
{
	// delete path from filename
	s32 i = filename.size();

	// search for path separator or beginning
	while ( i )
	{
		if ( filename[i] == '/' || filename[i] == '\\' )
		{
			if ( --pathCount <= 0 )
				break;
		}
		i -= 1;
	}

	if ( i )
	{
		filename [ i + 1 ] = 0;
		filename.validate();
	}
	return filename;
}

//! gets the last char of a string or null
inline c16 lastChar( const core::string<c16>& s)
{
	return s.size() ? s [ s.size() - 1 ] : 0;
}

//! looks if file is in the same directory of path. returns offset of directory.
//! 0 means in same directory. 1 means file is direct child of path
inline s32 isInSameDirectory ( const core::string<c16>& path, const core::string<c16>& file )
{
	s32 subA = 0;
	s32 subB = 0;
	s32 pos;

	if ( path.size() && !path.equalsn ( file, path.size() ) )
		return -1;

	pos = 0;
	while ( (pos = path.findNext ( '/', pos )) >= 0 )
	{
		subA += 1;
		pos += 1;
	}

	pos = 0;
	while ( (pos = file.findNext ( '/', pos )) >= 0 )
	{
		subB += 1;
		pos += 1;
	}

	return subB - subA;
}


//! some standard function ( to remove dependencies )
#undef isdigit
#undef isspace
#undef isupper
inline s32 isdigit(s32 c) { return c >= '0' && c <= '9'; }
inline s32 isspace(s32 c) { return c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' || c == '\v'; }
inline s32 isupper(s32 c) { return c >= 'A' && c <= 'Z'; }


} // end namespace core
} // end namespace irr

#endif

