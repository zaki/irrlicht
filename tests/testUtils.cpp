// Copyright (C) 2008 Colin MacDonald
// No rights reserved: this software is in the public domain.

#define _CRT_SECURE_NO_WARNINGS
#include "testUtils.h"
#include <memory.h>
#include <stdio.h>
#include <assert.h>
#include <stdarg.h>

#if defined(_MSC_VER) && !defined(NDEBUG)
#include <windows.h>
#endif // #if defined(_MSC_VER) && !defined(NDEBUG)

using namespace irr;

bool binaryCompareFiles(const char * fileName1, const char * fileName2)
{
	assert(fileName1);
	assert(fileName2);
	if(!fileName1 || !fileName2)
		return false;

	FILE * file1 = fopen(fileName1, "rb");
	if(!file1)
	{
		logTestString("binaryCompareFiles: File '%s' cannot be opened\n", fileName1);
		assert(file1);
		return false;
	}

	FILE * file2 = fopen(fileName2, "rb");
	if(!file2)
	{
		logTestString("binaryCompareFiles: File '%s' cannot be opened\n", fileName2);
		(void)fclose(file1);
		assert(file2);
		return false;
	}


	(void)fseek(file1, 0, SEEK_END);
	(void)fseek(file2, 0, SEEK_END);
	if(ftell(file1) != ftell(file2))
	{
		logTestString("binaryCompareFiles: Files are different sizes\n");
		(void)fclose(file1);
		(void)fclose(file2);
		return false;
	}

	(void)fseek(file1, 0, SEEK_SET);
	(void)fseek(file2, 0, SEEK_SET);

	char file1Buffer[8196];
	char file2Buffer[8196];

	while(!feof(file1))
	{
		if(feof(file2)
			||(fread(file1Buffer, sizeof(file1Buffer), 1, file1) !=
			fread(file2Buffer, sizeof(file2Buffer), 1, file2)))
		{
			logTestString("binaryCompareFiles: Error during file reading\n");
			break;
		}

		if(memcmp(file1Buffer, file2Buffer, sizeof(file1Buffer)))
		{
			logTestString("binaryCompareFiles: files are different\n");
			break;
		}
	}

	bool filesAreIdentical = feof(file1) && feof(file2);
	(void)fclose(file1);
	(void)fclose(file2);

	return filesAreIdentical;
}


//! Compare two images, returning the degree to which they match.
/** \param image1 The first image to compare.
	\param image2 The second image to compare.
	\return The match, from 0.f to 100.f */
static float fuzzyCompareImages(irr::video::IImage * image1,
								irr::video::IImage * image2)
{
	assert(image1);
	assert(image2);
	if(!image1 || !image2)
		return 0.f;

	if(image1->getDimension() != image2->getDimension())
	{
		logTestString("fuzzyCompareImages: images are different sizes\n");
		return 0.f;
	}

	video::ECOLOR_FORMAT format1 = image1->getColorFormat();
	if(video::ECF_A8R8G8B8 != format1 && video::ECF_R8G8B8 != format1)
	{
		logTestString("fuzzyCompareImages: image 1 must be ECF_A8R8G8B8 or ECF_R8G8B8\n");
		return 0.f;
	}

	video::ECOLOR_FORMAT format2 = image2->getColorFormat();
	if(video::ECF_A8R8G8B8 != format2 && video::ECF_R8G8B8 != format2)
	{
		logTestString("fuzzyCompareImages: image 2 must be ECF_AR8G8B8 or ECF_R8G8B8\n");
		return 0.f;
	}

	u8 * image1Data = (u8*)image1->lock();
	u8 * image2Data = (u8*)image2->lock();

	const u32 pixels = (image1->getPitch() * image1->getDimension().Height) / 4;
	u32 mismatchedPixels = 0;
	for(u32 pixel = 0; pixel < pixels; ++pixel)
	{
		if(video::ECF_A8R8G8B8 == format1)
			image1Data++;

		const u8 r1 = *(image1Data++);
		const u8 g1 = *(image1Data++);
		const u8 b1 = *(image1Data++);

		if(video::ECF_A8R8G8B8 == format2)
			image2Data++;

		const u8 r2 = *(image2Data++);
		const u8 g2 = *(image2Data++);
		const u8 b2 = *(image2Data++);

		// Empirically, some OpenGL screenshots have up to 2 shades difference per pixel.
		if(abs(r1 - r2) > 2 || abs(g1 - g2) > 2 || abs(b1 - b2) > 2)
			++mismatchedPixels;
	}

	image1->unlock();
	image2->unlock();
	
	return 100.f * (pixels - mismatchedPixels) / pixels;
}


bool takeScreenshotAndCompareAgainstReference(irr::video::IVideoDriver * driver, const char * fileName)
{
	irr::video::IImage * screenshot = driver->createScreenShot();
	if(!screenshot)
	{
		logTestString("Failed to take screenshot\n");
		return false;
	}

	irr::core::stringc driverName = driver->getName();
	// For OpenGL (only), chop the version number out. Other drivers have more stable version numbers.
	if(driverName.find("OpenGL") > -1)
		driverName = "OpenGL";

	irr::core::stringc referenceFilename = "media/";
	referenceFilename += driverName;
	referenceFilename += fileName;
	irr::video::IImage * reference = driver->createImageFromFile(referenceFilename.c_str());
	if(!reference)
	{
		logTestString("\n*** Failed to load reference image '%s'\n*** Creating from screenshot - please check this image.\n\n",
			referenceFilename.c_str());
		(void)driver->writeImageToFile(screenshot, referenceFilename.c_str());
		screenshot->drop();
		return false;
	}

	float match = fuzzyCompareImages(screenshot, reference);

	screenshot->drop();
	reference->drop();

	logTestString("Image match: %f%%\n", match);

	return (match > 99.f); // Require a very confident match.
}

static FILE * logFile = 0;

bool openTestLog(const char * filename)
{
	closeTestLog();

	logFile = fopen(filename, "w");
	assert(logFile);
	if(!logFile)
		logTestString("\nWARNING: unable to open the test log file %s\n", filename);
	
	return (logFile != 0);
}

void closeTestLog(void)
{
	if(logFile)
		(void)fclose(logFile);
}


void logTestString(const char * format, ...)
{
	char logString[1024];

	va_list arguments;
	va_start(arguments, format);
	vsprintf(logString, format, arguments);
	va_end(arguments);

	(void)printf(logString);
	if(logFile)
	{
		(void)fprintf(logFile, logString);
		(void)fflush(logFile);
	}

#if defined(_MSC_VER) && !defined(NDEBUG)
	OutputDebugStringA(logString);
#endif // #if defined(_MSC_VER) && !defined(NDEBUG)
}
