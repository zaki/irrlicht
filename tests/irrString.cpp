// Copyright (C) 2008-2011 Colin MacDonald
// No rights reserved: this software is in the public domain.

#include "testUtils.h"
#include <irrlicht.h>

using namespace irr;
using namespace core;

static bool testSelfAssignment()
{
	core::stringw myString(L"foo");
	myString = myString;
	return myString == core::stringw(L"foo");
}

static bool testSplit()
{
	logTestString("Test stringw::split()\n");
	core::stringw teststring(L"[b]this [/b] is a [color=0xff000000]test[/color].");
	core::list<core::stringw> parts1;
	teststring.split<core::list<core::stringw> >(parts1, L"[");
	core::list<core::stringw> parts2;
	teststring.split<core::list<core::stringw> >(parts2, L"[", 1, false, true);
	return (parts1.getSize()==4) && (parts2.getSize()==5);
}

static bool testFastAlloc()
{
	core::string<wchar_t, core::irrAllocatorFast<wchar_t> > FastString(L"abc");
	core::string<wchar_t, core::irrAllocatorFast<wchar_t> > FastStringLong(L"longer");

	FastString  = L"test";

	// cause a reallocation
	FastString = FastStringLong;

	// this test should either not compile or crash when the allocaters are messed up
	return true;
}


bool testAppendStringc()
{
	core::stringc str;
	// Test with character
	if (str != "")
		return false;
	str += 'W';
	if (str != "W")
		return false;
	str += 'i';
	if (str != "Wi")
		return false;
	str="";
	if (str != "")
		return false;

	// Test with C-style string
	str += "Another Test";
	if (str != "Another Test")
		return false;
	str="";
	str += 'A';
	str += "nother Test";
	if (str != "Another Test")
		return false;
	str="";

	// Test with int
	str += 10;
	if (str != "10")
		return false;
	str += 0;
	if (str != "100")
		return false;
	str="";
	str += "-32";
	if (str != "-32")
		return false;
	str="";

	// Test with unsigned int
	str += 21u;
	if (str != "21")
		return false;
	str += 0u;
	if (str != "210")
		return false;
	str="";

	// Test with long int
	str += 456l;
	if (str != "456")
		return false;
	str += 0l;
	if (str != "4560")
		return false;
	str="";
	str += -456l;
	if (str != "-456")
		return false;
	str="";

	// Test with unsigned long
	str += 994ul;
	if (str != "994")
		return false;
	str += 0ul;
	if (str != "9940")
		return false;
	str="";
	return true;
}


// Test the functionality of irrString
/** Validation is done with assert_log() against expected results. */
bool testIrrString(void)
{
	bool allExpected = true;

	logTestString("Test stringc\n");
	{
		// Check empty string
		core::stringc empty;
		assert_log(empty.size()==0);
		assert_log(empty[0]==0);
		assert_log(empty.c_str()!=0);
		assert_log(*(empty.c_str())==0);
		// Assign content
		empty = "Test";
		assert_log(empty.size()==4);
		assert_log(empty[0]=='T');
		assert_log(empty[3]=='t');
		assert_log(*(empty.c_str())=='T');
		//Assign empty string, should be same as in the beginning
		empty = "";
		assert_log(empty.size()==0);
		assert_log(empty[0]==0);
		assert_log(*(empty.c_str())==0);
	}
	logTestString("Test stringw\n");
	{
		core::stringw empty;
		assert_log(empty.size()==0);
		assert_log(empty[0]==0);
		assert_log(empty.c_str()!=0);
		assert_log(*(empty.c_str())==0);
		empty = L"Test";
		assert_log(empty.size()==4);
		assert_log(empty[0]==L'T');
		assert_log(empty[3]=='t');
		assert_log(*(empty.c_str())==L'T');
		empty = L"";
		assert_log(empty.size()==0);
		assert_log(empty[0]==0);
		assert_log(*(empty.c_str())==0);
		assert_log(allExpected &= testSplit());
	}
	allExpected &= testAppendStringc();

	logTestString("Test io::path\n");
	{
		// Only test that this type exists, it's one from above
		io::path myPath;
		myPath = "Some text"; // Only to avoid wrong optimizations
	}

	logTestString("Test self assignment\n");
	allExpected &= testSelfAssignment();

	logTestString("test fast alloc\n");
	allExpected &= testFastAlloc();

	if(allExpected)
		logTestString("\nAll tests passed\n");
	else
		logTestString("\nFAIL!\n");

	return allExpected;
}
