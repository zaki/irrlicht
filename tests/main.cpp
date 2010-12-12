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
#include <vector>

struct STestDefinition
{
	//! The test entry point function
	bool(*testSignature)(void);

	//! A descriptive name for the test
	const char * testName;
};

//! This is the main entry point for the Irrlicht test suite.
/** \return The number of test that failed, i.e. 0 is success. */
int main(int argumentCount, char * arguments[])
{
	if(argumentCount > 3)
	{
		logTestString("\nUsage: %s [testNumber] [testCount]\n");
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
	// Now the simple tests without device
	TEST(testIrrArray);
	TEST(testIrrMap);
	TEST(testIrrList);
	TEST(exports);
	TEST(irrCoreEquals);
	TEST(testIrrString);
	TEST(line2dIntersectWith);
	TEST(matrixOps);
	TEST(testDimension2d);
	TEST(testVector2d);
	TEST(testVector3d);
	TEST(testQuaternion);
	TEST(testS3DVertex);
	TEST(testaabbox3d);
    TEST(color);
	// TODO: Needs to be fixed first
//	TEST(testTriangle3d);
	TEST(vectorPositionDimension2d);
	// file system checks (with null driver)
	TEST(filesystem);
	TEST(archiveReader); 
	TEST(testXML);
	TEST(serializeAttributes);
	// null driver
	TEST(fast_atof);
	TEST(loadTextures);
	TEST(collisionResponseAnimator);
	TEST(enumerateImageManipulators);
	TEST(removeCustomAnimator);
	TEST(sceneCollisionManager);
	TEST(sceneNodeAnimator);
	TEST(meshLoaders);
	TEST(testTimer);
	// software drivers only
	TEST(softwareDevice);
	TEST(b3dAnimation);
	TEST(burningsVideo);
	TEST(cursorSetVisible);
	TEST(flyCircleAnimator);
	TEST(md2Animation);
	TEST(testGeometryCreator); 
	TEST(writeImageToFile);
	TEST(meshTransform);
	TEST(createImage);
	// all driver checks
	TEST(videoDriver);
	TEST(drawPixel);
	TEST(drawRectOutline);
	TEST(guiDisabledMenu);
	TEST(makeColorKeyTexture);
	TEST(renderTargetTexture);
	TEST(textureFeatures);
	TEST(textureRenderStates);
	TEST(transparentMaterials);
	TEST(antiAliasing);
	TEST(draw2DImage);
	TEST(lights);
	TEST(twodmaterial);
	TEST(viewPort);
	TEST(mrt);
	TEST(projectionMatrix);
	// large scenes/long rendering
	// shadows are slow
//	TEST(orthoCam);
//	TEST(stencilShadow);
	// q3 maps are slow
	TEST(planeMatrix);
	TEST(terrainSceneNode);
	TEST(lightMaps); 

	unsigned int numberOfTests = tests.size();
	unsigned int testToRun = 0;
	unsigned int fails = 0;

	bool firstRun=true;
	const bool spawn=false;
	// args: [testNumber] [testCount]
	if(argumentCount > 1)
	{
		if (!strcmp(arguments[1],"--list"))
		{
			for (unsigned int i=0; i<tests.size(); ++i)
			{
				printf("%3d: %s\n", i, tests[i].testName);
			}
			printf("\n");
			return 0;
		}

		int tmp = atoi(arguments[1]);
		firstRun = (tmp>=0);
		testToRun=abs(tmp);
		if (!firstRun)
			testToRun -= 1;

		if(argumentCount > 2)
		{
			numberOfTests = testToRun + abs(atoi(arguments[2]));
			if (numberOfTests>=tests.size())
				numberOfTests=tests.size();
		}
	}

	if(testToRun >= numberOfTests)
	{
		logTestString("\nError: invalid test %d (maximum %d)\n",
					testToRun, numberOfTests-testToRun);
		return 9999;
	}

	const unsigned int testCount = numberOfTests-testToRun;
	const bool logFileOpened = openTestLog(firstRun);
	assert(logFileOpened);

	if (firstRun)
	{
		if (numberOfTests)
		{
			for (unsigned int i=testToRun; i<numberOfTests; ++i)
			{
				logTestString("\nStarting test %d, '%s'\n",
						i, tests[i].testName);
				if (spawn)
				{
					closeTestLog();
					char runNextTest[256];
					(void)sprintf(runNextTest, "\"%s\" -%d 1", arguments[0], i+1);
					// Spawn the next test in a new process.
					if (system(runNextTest))
					{
						(void)openTestLog(false);
						logTestString("\n******** Test failure ********\n"\
								"Test %d '%s' failed\n"\
								"******** Test failure ********\n",
								i, tests[i].testName);
						++fails;
					}
					else
						(void)openTestLog(false);
				}
				else
				{
					if (!tests[i].testSignature())
					{
						logTestString("\n******** Test failure ********\n"\
								"Test %d '%s' failed\n"\
								"******** Test failure ********\n",
								i, tests[i].testName);
						++fails;
					}
				}
			}
		}
		const int passed = testCount - fails;

		logTestString("\nTests finished. %d test%s of %d passed.\n\n",
			passed, 1 == passed ? "" : "s", testCount);

		if(0 == fails && testCount==tests.size())
		{
			time_t rawtime;
			struct tm * timeinfo;
			(void)time(&rawtime);
			timeinfo = gmtime(&rawtime);
			(void)printf("\nTest suite pass at GMT %s\n", asctime(timeinfo));
			FILE * testsLastPassedAtFile = fopen("tests-last-passed-at.txt", "w");
			if(testsLastPassedAtFile)
			{
				(void)fprintf(testsLastPassedAtFile, "Tests finished. %d test%s of %d passed.\n",
			passed, 1 == passed ? "" : "s", numberOfTests);
#ifdef _DEBUG
				(void)fprintf(testsLastPassedAtFile, "Compiled as DEBUG\n");
#else
				(void)fprintf(testsLastPassedAtFile, "Compiled as RELEASE\n");
#endif
				(void)fprintf(testsLastPassedAtFile, "Test suite pass at GMT %s\n", asctime(timeinfo));
				(void)fclose(testsLastPassedAtFile);
			}
		}
		closeTestLog();
#ifdef _IRR_WINDOWS_
		(void)system("tests.log");
#else
		(void)system("$PAGER tests.log");
#endif
		return fails;
	}
	else
	{
		const bool res = tests[testToRun].testSignature();
		closeTestLog();
		return res?0:1;
	}
}

