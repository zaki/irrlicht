
#ifndef _TEST_UTILS_H_
#define _TEST_UTILS_H_ 1

#include "irrlicht.h"

//! Compare two files
/** \param fileName1 The first file for comparison.
	\param fileName1 The second file for comparison.
	\return true if the files are identical, false on any error or difference. */
extern bool binaryCompareFiles(const char * fileName1, const char * fileName2);

//! Take a screenshot and compare it against a reference screenshot in the tests/media subdirectory
/** \param driver The Irrlicht video driver.
	\fileName The unique filename suffix that will be appended to the name of the video driver.
	\return true if the screenshot was taken and is identical to the reference image of the same name
	in the tests/media directory, false on any error or difference. */
bool takeScreenshotAndCompareAgainstReference(irr::video::IVideoDriver * driver, const char * fileName);


#endif // _TEST_UTILS_H_
