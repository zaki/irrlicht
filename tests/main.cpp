// This is the entry point for the Irrlicht test suite.
#define _CRT_SECURE_NO_WARNINGS

#include "testUtils.h"
#include <stdio.h>
#include <time.h>
#include <assert.h>

// This is an MSVC pragma to link against the Irrlicht library.
// Other builds must link against it in the project files.
#if defined(_MSC_VER)
#pragma comment(lib, "Irrlicht.lib")
#endif // _MSC_VER

/* Each test must have the same signature.  Test should (but are not
 * required to) live in a .cpp file of the same name.  There is no
 * need to #include anything since the test entry points can be
 * declared as extern before calling them.
 */
#define RUN_TEST(testEntryPoint)\
	extern bool testEntryPoint(void);\
	logTestString("\nStarting test '" #testEntryPoint "'\n");\
	if(!testEntryPoint()) \
	{\
		(void)printf("\n\n\n******** Test failure ********\nTest '" #testEntryPoint "' failed\n"\
		"******** Test failure ********\n\nPress return to continue\n");\
		(void)getc(stdin);\
		fails++;\
	}

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

	extern bool disambiguateTextures(void);
	extern bool softwareDevice(void);
	extern bool exports(void);
	extern bool testVector3d(void);
	extern bool testVector2d(void);
	extern bool planeMatrix(void);
	extern bool fast_atof(void);
	extern bool line2dIntersectWith(void);
	extern bool drawPixel(void);
	extern bool md2Animation(void);
	extern bool b3dAnimation(void);
	extern bool guiDisabledMenu(void);
	extern bool collisionResponseAnimator(void);
	extern bool sceneCollisionManager(void);

	typedef struct _STest
	{
		bool(*testSignature)(void);
		const char * testName;
	} STest;

	#define TEST(x) { x, #x }

	static const STest tests[] =
	{
		// Note that to interactively debug a test, you will generally want to move it
		// (temporarily) to the beginning of the list, since each test runs in its own
		// process.
		TEST(disambiguateTextures), // Normally you should run this first, since it validates the working directory.
		TEST(sceneCollisionManager),
		TEST(collisionResponseAnimator),
		TEST(exports),
		TEST(testVector3d),
		TEST(testVector2d),
		TEST(planeMatrix),
		TEST(fast_atof),
		TEST(line2dIntersectWith),
		TEST(drawPixel),
		TEST(md2Animation),
		TEST(guiDisabledMenu),
		TEST(softwareDevice),
		TEST(b3dAnimation)
	};
	static const unsigned int numberOfTests = sizeof tests / sizeof tests[0];

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

	if(testToRun == numberOfTests)
	{
		logTestString("\nTests finished. %d test%s failed.\n", fails, 1 == fails ? "" : "s");
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
	else
	{
		closeTestLog();
		char runNextTest[256];
		(void)sprintf(runNextTest, "%s %d %d", arguments[0], testToRun, fails);
		fails = system(runNextTest);
	}

	return fails;
}

