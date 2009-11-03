// Copyright (C) 2008-2009 Colin MacDonald
// No rights reserved: this software is in the public domain.

#include "testUtils.h"
#include <irrlicht.h>
#include <assert.h>

using namespace irr;
using namespace core;

// Test the functionality of irrString
/** Validation is done with asserts() against expected results. */
bool testIrrString(void)
{
	bool allExpected = true;

	logTestString("Test stringc\n");
	{
		// Check empty string
		core::stringc empty;
		assert(empty.size()==0);
		assert(empty[0]==0);
		assert(empty.c_str()!=0);
		assert(*(empty.c_str())==0);
		// Assign content
		empty = "Test";
		assert(empty.size()==4);
		assert(empty[0]=='T');
		assert(empty[3]=='t');
		assert(*(empty.c_str())=='T');
		//Assign empty string, should be same as in the beginning
		empty = "";
		assert(empty.size()==0);
		assert(empty[0]==0);
		assert(*(empty.c_str())==0);
	}
	logTestString("Test stringw\n");
	{
		core::stringw empty;
		assert(empty.size()==0);
		assert(empty[0]==0);
		assert(empty.c_str()!=0);
		assert(*(empty.c_str())==0);
		empty = L"Test";
		assert(empty.size()==4);
		assert(empty[0]==L'T');
		assert(empty[3]=='t');
		assert(*(empty.c_str())==L'T');
		empty = L"";
		assert(empty.size()==0);
		assert(empty[0]==0);
		assert(*(empty.c_str())==0);
	}

	logTestString("Test io::path\n");
	{
		// Only test that this type exists, it's one from above
		io::path myPath;
		myPath = "Some text"; // Only to avoid wrong optimizations
	}

	if(allExpected)
		logTestString("\nAll tests passed\n");
	else
		logTestString("\nFAIL!\n");

	return allExpected;
}

