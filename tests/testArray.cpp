// Copyright (C) 2008-2009 Colin MacDonald
// No rights reserved: this software is in the public domain.

#include "testUtils.h"
#include <irrlicht.h>
#include <assert.h>

using namespace irr;
using namespace core;

struct VarArray
{
	core::array < int, core::irrAllocatorFast<int> > MyArray;
};

// this will (did once) simply crash when wrong, so no return value
void crashTestFastAlloc()
{
	core::array < VarArray, core::irrAllocatorFast<VarArray> > ArrayArray;
	ArrayArray.setAllocStrategy(core::ALLOC_STRATEGY_SAFE); // force more re-allocations
	VarArray var;
	var.MyArray.setAllocStrategy(core::ALLOC_STRATEGY_SAFE); // force more re-allocations
	var.MyArray.push_back( 0 );

	for ( int i=0; i< 100; ++i )
	{
		ArrayArray.push_back(var);
		ArrayArray.push_back(var);
	}
}

// Test the functionality of core::array
bool testArray(void)
{
	bool allExpected = true;
	
	logTestString("crashTestFastAlloc\n");
	crashTestFastAlloc();

	if(allExpected)
		logTestString("\nAll tests passed\n");
	else
		logTestString("\nFAIL!\n");

	return allExpected;
}
