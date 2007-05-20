// Copyright (C) 2002-2007 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "COSOperator.h"
#include "IrrCompileConfig.h"

#ifdef _IRR_WINDOWS_API_
#include <windows.h>
#else
#include <string.h>
#include <unistd.h>
#ifdef MACOSX
#include "OSXClipboard.h"
#include <sys/types.h>
#include <sys/sysctl.h>
#endif
#endif

namespace irr
{


// constructor
COSOperator::COSOperator(const c8* osVersion)
{
	OperatingSystem = osVersion;
}


//! returns the current operating system version as string.
const wchar_t* COSOperator::getOperationSystemVersion()
{
	return OperatingSystem.c_str();
}


//! copies text to the clipboard
void COSOperator::copyToClipboard(const c8* text)
{
	if (strlen(text)==0)
		return;

// Windows version
#if defined(_IRR_WINDOWS_API_)
	if (!OpenClipboard(0) || text == 0)
		return;

	EmptyClipboard();

	HGLOBAL clipbuffer;
	char * buffer;

	clipbuffer = GlobalAlloc(GMEM_DDESHARE, strlen(text)+1);
	buffer = (char*)GlobalLock(clipbuffer);

	strcpy(buffer, text);

	GlobalUnlock(clipbuffer);
	SetClipboardData(CF_TEXT, clipbuffer);
	CloseClipboard();

// MacOSX version
#elif defined(MACOSX)

	OSXCopyToClipboard(text);

// todo: Linux version
#endif
}


//! gets text from the clipboard
//! \return Returns 0 if no string is in there.
c8* COSOperator::getTextFromClipboard()
{
#if defined(_IRR_WINDOWS_API_)
	if (!OpenClipboard(NULL))
		return 0;
	
	char * buffer = 0;

	HANDLE hData = GetClipboardData( CF_TEXT );
	buffer = (char*)GlobalLock( hData );
	GlobalUnlock( hData );
	CloseClipboard();
	return buffer;

#elif defined(MACOSX)
	return (OSXCopyFromClipboard());
#else

// todo: Linux version

	return 0;
#endif
}


bool COSOperator::getProcessorSpeedMHz(irr::u32* MHz)
{
#if defined(_IRR_WINDOWS_API_)
	LONG Error;
	
	HKEY Key;
	Error = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
			"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",
			0, KEY_READ, &Key);

	if(Error != ERROR_SUCCESS)
		return false;

	DWORD Speed = 0;
	DWORD Size = sizeof(Speed);
	Error = RegQueryValueEx(Key, "~MHz", NULL, NULL, (LPBYTE)&Speed, &Size);

	RegCloseKey(Key);

	if (Error != ERROR_SUCCESS)
		return false;
	else if (MHz)
		*MHz = Speed;
	_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
	return true;

#elif defined(MACOSX)
	struct clockinfo CpuClock;
	size_t Size = sizeof(clockinfo);

	if (!sysctlbyname("kern.clockrate", &CpuClock, &Size, NULL, 0))
		return false;
	else if (MHz)
		*MHz = CpuClock.hz;
	return true;
#else
	// could probably be read from "/proc/cpuinfo" or "/proc/cpufreq"

	return false;
#endif
}

bool COSOperator::getSystemMemory(irr::u32* Total, irr::u32* Avail)
{
#if defined(_IRR_WINDOWS_API_)
	MEMORYSTATUS MemoryStatus;
	MemoryStatus.dwLength = sizeof(MEMORYSTATUS);

	// cannot fail
	GlobalMemoryStatus(&MemoryStatus);

	if (Total)
		*Total = (irr::u32)(MemoryStatus.dwTotalPhys>>10);
	if (Avail)
		*Avail = (irr::u32)(MemoryStatus.dwAvailPhys>>10);
	
	_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
	return true;

#elif defined(_IRR_POSIX_API_) // || defined(MACOSX)
        long ps = sysconf(_SC_PAGESIZE);
        long pp = sysconf(_SC_PHYS_PAGES);
        long ap = sysconf(_SC_AVPHYS_PAGES);
 
	if ((ps==-1)||(pp==-1)||(ap==-1))
		return false;

	if (Total)
		*Total = ((ps*(long long)pp)>>10);
	if (Avail)
		*Avail = ((ps*(long long)ap)>>10);
	return true;
#endif
	// TODO: implement for OSX 
	return false;
}


} // end namespace

