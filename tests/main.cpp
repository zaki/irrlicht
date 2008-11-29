// This is the entry point for the Irrlicht test suite.
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <time.h>

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
	if(!testEntryPoint()) \
	{\
		(void)printf("\n\n\n******** Test failure ********\nTest '" #testEntryPoint "' failed\n"\
		"******** Test failure ********\n\nPress return to continue\n\a\a\a");\
		(void)getc(stdin);\
		fails++;\
	}

//! This is the main entry point for the Irrlicht test suite.
/** \return The number of test that failed, i.e. 0 is success. */
int main()
{
	int fails = 0;

	RUN_TEST(testVector3d);
	RUN_TEST(testVector2d);
	RUN_TEST(planeMatrix);
	RUN_TEST(fast_atof);
	RUN_TEST(line2dIntersectWith);
	RUN_TEST(disambiguateTextures);
	RUN_TEST(drawPixel);
	RUN_TEST(md2Animation);

	(void)printf("\nTests finished. %d test%s failed.\n", fails, 1 == fails ? "" : "s");
	
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

	return fails;
}

