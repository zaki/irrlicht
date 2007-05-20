// Copyright (C) 2002-2007 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_OS_OPERATOR_H_INCLUDED__
#define __C_OS_OPERATOR_H_INCLUDED__

#include "IOSOperator.h"
#include "irrString.h"

namespace irr
{

//! The Operating system operator provides operation system specific methods and informations.
class COSOperator : public IOSOperator
{
public:

	// constructor
	COSOperator(const c8* osversion);

	//! destructor
	virtual ~COSOperator() {};

	//! returns the current operation system version as string.
	virtual const wchar_t* getOperationSystemVersion();

	//! copies text to the clipboard
	virtual void copyToClipboard(const c8* text);

	//! gets text from the clipboard
	//! \return Returns 0 if no string is in there.
	virtual c8* getTextFromClipboard();

	//! gets the processor speed in megahertz
	//! \param Mhz:
	//! \return Returns true if successful, false if not
	virtual bool getProcessorSpeedMHz(irr::u32* MHz);

	//! gets the total and available system RAM in kB
	//! \param Total: will contain the total system memory
	//! \param Avail: will contain the available memory
	//! \return Returns true if successful, false if not
	virtual bool getSystemMemory(irr::u32* Total, irr::u32* Avail);

private:

	core::stringw OperatingSystem;
};

} // end namespace

#endif

