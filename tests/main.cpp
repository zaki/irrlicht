// This is the entry point for the Irrlicht test suite.
#define _CRT_SECURE_NO_WARNINGS

#include "testUtils.h"
#include <stdio.h>
#include <time.h>
#include <assert.h>
#include <vector>

// This is an MSVC pragma to link against the Irrlicht library.
// Other builds must link against it in the project files.
#if defined(_MSC_VER)
#pragma comment(lib, "Irrlicht.lib")
#endif // _MSC_VER

typedef struct _STestDefinition
{
	bool(*testSignature)(void);
	const char * testName;
} STestDefinition;

//! This is the main entry point for the Irrlicht test suite.
/** \return The number of test that failed, i.e. 0 is success. */
int main(int argumentCount, char * arguments[])
{
	bool logFileOpened = openTestLog(1 == argumentCount);
	assert(logFileOpened);

	if(argumentCount > 3)
	{
		logTestString("\nUsage: %s [testNumber] [totalFails]\n");
		closeTestLog();
		return 9999;
	}


	#define TEST(x)\
	{\
		extern bool x(void);\
		STestDefinition newTest;\
		newTest.testSignature = x;\
		newTest.testName = #x;\
		tests.push_back(newTest);\
	}

	std::vector<STestDefinition> tests;

	// Note that to interactively debug a test, you will generally want to move it
	// (temporarily) to the beginning of the list, since each test runs in its own
	// process.
	TEST(disambiguateTextures); // Normally you should run this first, since it validates the working directory.
	TEST(sceneNodeAnimator);
	TEST(sceneCollisionManager);
	TEST(collisionResponseAnimator);
	TEST(exports);
	TEST(testVector3d);
	TEST(testVector2d);
	TEST(planeMatrix);
	TEST(fast_atof);
	TEST(line2dIntersectWith);
	TEST(testDimension2d);
	TEST(drawPixel);
	TEST(md2Animation);
	TEST(guiDisabledMenu);
	TEST(softwareDevice);
	TEST(b3dAnimation);

	const unsigned int numberOfTests = tests.size();

	unsigned int testToRun = 0;
	unsigned int fails = 0;

	if(argumentCount > 1)
	{
		testToRun = (unsigned int)atoi(arguments[1]);
		if(testToRun >= numberOfTests)
		{
			logTestString("\nError: invalid test %d (maximum %d)\n",
						testToRun, numberOfTests - 1);
			closeTestLog();
			return 9999;
		}
	}

	if(argumentCount > 2)
		fails = (unsigned int)atoi(arguments[2]);

	logTestString("\nStarting test %d, '%s'\n",
				testToRun, tests[testToRun].testName);

	bool success = tests[testToRun].testSignature();

	if(!success)
	{
		logTestString("\n\n\n******** Test failure ********\nTest %d '%s' failed\n"\
		"******** Test failure ********\n",
						testToRun, tests[testToRun].testName);
		fails++;
	}

	testToRun++;
	if(testToRun < numberOfTests)
	{
		closeTestLog();
		char runNextTest[256];
		(void)sprintf(runNextTest, "%s %d %d", arguments[0], testToRun, fails);
		fails = system(runNextTest);
	}

	if(1 == testToRun)
	{
		(void)openTestLog(false);
		const int passed = numberOfTests - fails;

		logTestString("\nTests finished. %d test%s of %d passed.\n",
			passed, 1 == passed ? "" : "s", numberOfTests);

		if(0 == fails)
		{
			time_t rawtime;
			struct tm * timeinfo;
			(void)time(&rawtime);
			timeinfo = gmtime(&rawtime);
			(void)printf("\nTest suite pass at GMT %s\n", asctime(timeinfo));
			FILE * testsLastPassedAtFile = fopen("tests-last-passed-at.txt", "w");
			if(testsLastPassedAtFile)
			{
				(void)fprintf(testsLastPassedAtFile, "Test suite pass at GMT %s\n", asctime(timeinfo));
				(void)fclose(testsLastPassedAtFile);
			}
		}
		closeTestLog();
	}

	return fails;
}

