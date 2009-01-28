// Copyright (C) 2008-2009 Colin MacDonald and Christian Stehno
// No rights reserved: this software is in the public domain.

// This is the entry point for the Irrlicht test suite.

// This is an MSVC pragma to link against the Irrlicht library.
// Other builds must link against it in the project files.
#if defined(_MSC_VER)
#pragma comment(lib, "Irrlicht.lib")
#define _CRT_SECURE_NO_WARNINGS 1
#endif // _MSC_VER

#include "testUtils.h"
#include <stdio.h>
#include <time.h>
#include <assert.h>
#include <vector>

typedef struct _STestDefinition
{
	//! The test entry point function
	bool(*testSignature)(void);

	//! A descriptive name for the test
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

	// Use an STL vector so that we don't rely on Irrlicht.
	std::vector<STestDefinition> tests;

	// Note that to interactively debug a test, you will generally want to move it
	// (temporarily) to the beginning of the list, since each test runs in its own
	// process.
	TEST(disambiguateTextures); // Normally you should run this first, since it validates the working directory.
	TEST(exports);
	TEST(sceneCollisionManager);
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
	TEST(textureRenderStates);
	TEST(terrainSceneNode);
	TEST(burningsVideo);
	TEST(makeColorKeyTexture);
	TEST(cursorSetVisible);
	TEST(transparentAlphaChannelRef);
	TEST(drawRectOutline);

	// Tests available on 1.6+
	TEST(collisionResponseAnimator);
	TEST(irrCoreEquals);
	TEST(makeColorKeyTexture);
	TEST(matrixOps);
	TEST(sceneNodeAnimator);
	TEST(vectorPositionDimension2d);
	TEST(writeImageToFile);
	TEST(flyCircleAnimator);
//	TEST(relativeTransformations);

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
				testToRun + 1, tests[testToRun].testName);

	bool success = tests[testToRun].testSignature();

	if(!success)
	{
		logTestString("\n******** Test failure ********\nTest %d '%s' failed\n"\
		"******** Test failure ********\n",
						testToRun + 1, tests[testToRun].testName);
		fails++;
	}

	testToRun++;
	if(testToRun < numberOfTests)
	{
		closeTestLog();
		char runNextTest[256];
		(void)sprintf(runNextTest, "\"%s\" %d %d", arguments[0], testToRun, fails);
		fails = system(runNextTest); // Spawn the next test in a new process.
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
		(void)system("tests.log");
	}

	return fails;
}

