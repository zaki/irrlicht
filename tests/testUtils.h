
#ifndef _TEST_UTILS_H_
#define _TEST_UTILS_H_ 1

#include "irrlicht.h"
#include <assert.h>

#define TestWithAllDrivers(X) \
	logTestString("Running test " #X "\n"); \
	for (u32 i=1; i<video::EDT_COUNT; ++i) \
	result &= X(video::E_DRIVER_TYPE(i))
#define TestWithAllHWDrivers(X) \
	logTestString("Running test " #X "\n"); \
	for (u32 i=video::EDT_DIRECT3D8; i<video::EDT_COUNT; ++i) \
	result &= X(video::E_DRIVER_TYPE(i))

//! Compare two files
/** \param fileName1 The first file for comparison.
	\param fileName2 The second file for comparison.
	\return true if the files are identical, false on any error or difference. */
extern bool binaryCompareFiles(const char * fileName1, const char * fileName2);

//! Take a screenshot and compare it against a reference screenshot in the tests/media subdirectory
/** \param driver The Irrlicht video driver.
	\param fileName The unique filename suffix that will be appended to the name of the video driver.
	\param requiredMatch The degree to which the screenshot needs to match the reference image
	in order to be considered a match.
	\return true if the screenshot was taken and is identical to the reference image of the same name
	in the tests/media directory, false on any error or difference. */
extern bool takeScreenshotAndCompareAgainstReference(irr::video::IVideoDriver * driver,
													const char * fileName,
													irr::f32 requiredMatch = 99.f);


//! Opens a test log file, deleting any existing contents.
/** \param startNewLog true to create a new log file, false to append to an
						existing one.
	\param filename The filename to open
	\return true if the test log file was opened, false on error. */
extern bool openTestLog(bool startNewLog, const char * filename = "tests.log");

//! Close the test log file opened with openTestLog()
extern void closeTestLog();

//! Log a string to the console and the test log file created by openTestLog().
/** \param format The format string
	\... optional parameters */
extern void logTestString(const char * format, ...);

#endif // _TEST_UTILS_H_
