// Copyright (C) 2002-2007 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __IRR_TYPES_H_INCLUDED__
#define __IRR_TYPES_H_INCLUDED__

#include "IrrCompileConfig.h"

namespace irr
{

//! 8 bit unsigned variable.
/** This is a typedef for unsigned char, it ensures portability of the engine. */
typedef unsigned char		u8; 

//! 8 bit signed variable.
/** This is a typedef for signed char, it ensures portability of the engine. */
typedef signed char		s8; 

//! 8 bit character variable.
/** This is a typedef for char, it ensures portability of the engine. */
typedef char			c8; 



//! 16 bit unsigned variable.
/** This is a typedef for unsigned short, it ensures portability of the engine. */
typedef unsigned short		u16;

//! 16 bit signed variable.
/** This is a typedef for signed short, it ensures portability of the engine. */
typedef signed short		s16; 



//! 32 bit unsigned variable.
/** This is a typedef for unsigned int, it ensures portability of the engine. */
typedef unsigned int		u32;

//! 32 bit signed variable.
/** This is a typedef for signed int, it ensures portability of the engine. */
typedef signed int		s32; 



// 64 bit signed variable.
// This is a typedef for __int64, it ensures portability of the engine. 
// This type is currently not used by the engine and not supported by compilers
// other than Microsoft Compilers, so it is outcommented.
//typedef __int64				s64; 



//! 32 bit floating point variable.
/** This is a typedef for float, it ensures portability of the engine. */
typedef float				f32; 

//! 64 bit floating point variable.
/** This is a typedef for double, it ensures portability of the engine. */
typedef double				f64; 


} // end namespace irr


#include <wchar.h>
#ifdef _IRR_WINDOWS_API_
//! Defines for s{w,n}printf because these methods do not match the ISO C
//! standard on Windows platforms, but it does on all others.
#define swprintf _snwprintf
#define snprintf _snprintf

// define the wchar_t type if not already built in.
#ifdef _MSC_VER 
#ifndef _WCHAR_T_DEFINED
//! A 16 bit wide character type.
/**
	Defines the wchar_t-type.
	In VS6, its not possible to tell
	the standard compiler to treat wchar_t as a built-in type, and 
	sometimes we just don't want to include the huge stdlib.h or wchar.h,
	so we'll use this.
*/
typedef unsigned short wchar_t;
#define _WCHAR_T_DEFINED
#endif // wchar is not defined
#endif // microsoft compiler
#endif // _IRR_WINDOWS_API_

//! define a break macro for debugging.
#if defined(_DEBUG)
#if defined(_IRR_WINDOWS_API_) && defined(_MSC_VER)
#define _IRR_DEBUG_BREAK_IF( _CONDITION_ ) if (_CONDITION_) {_asm int 3}
#else 
#include "assert.h"
#define _IRR_DEBUG_BREAK_IF( _CONDITION_ ) assert( !(_CONDITION_) );
#endif
#else 
#define _IRR_DEBUG_BREAK_IF( _CONDITION_ )
#endif

//! Defines a small statement to work around a microsoft compiler bug.
/** The microsft compiler 7.0 - 7.1 has a bug:
When you call unmanaged code that returns a bool type value of false from managed code, 
the return value may appear as true. See 
http://support.microsoft.com/default.aspx?kbid=823071 for details. 
Compiler version defines: VC6.0 : 1200, VC7.0 : 1300, VC7.1 : 1310, VC8.0 : 1400*/
#if defined(_IRR_WINDOWS_API_) && defined(_MSC_VER) && (_MSC_VER > 1299) && (_MSC_VER < 1400)
#define _IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX __asm mov eax,100
#else
#define _IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX
#endif // _IRR_MANAGED_MARSHALLING_BUGFIX


// memory debugging
#if defined(_DEBUG) && defined(IRRLICHT_EXPORTS) && defined(_MSC_VER) && \
	(_MSC_VER > 1299) && !defined(_IRR_DONT_DO_MEMORY_DEBUGGING_HERE)
	
	#define CRTDBG_MAP_ALLOC
	#define _CRTDBG_MAP_ALLOC
	#define DEBUG_CLIENTBLOCK   new( _CLIENT_BLOCK, __FILE__, __LINE__)
	#include <stdlib.h>
	#include <crtdbg.h>
	#define new DEBUG_CLIENTBLOCK
#endif 

// disable truncated debug information warning in visual studio 6 by default
#if defined(_MSC_VER) && (_MSC_VER < 1300 )
#pragma warning( disable: 4786)
#endif // _MSC


//! ignore VC8 warning deprecated
/** The microsoft compiler */
#if defined(_IRR_WINDOWS_API_) && defined(_MSC_VER) && (_MSC_VER >= 1400)
	//#pragma warning( disable: 4996)
	//#define _CRT_SECURE_NO_DEPRECATE 1
	//#define _CRT_NONSTDC_NO_DEPRECATE 1
#endif


//! creates four CC codes used in Irrlicht for simple ids
/** some compilers can create those by directly writing the
code like 'code', but some generate warnings so we use this macro here */
#define MAKE_IRR_ID(c0, c1, c2, c3)                       \
                ((u32)(u8)(c0) | ((u32)(u8)(c1) << 8) |   \
                ((u32)(u8)(c2) << 16) | ((u32)(u8)(c3) << 24 ))


#endif // __IRR_TYPES_H_INCLUDED__

