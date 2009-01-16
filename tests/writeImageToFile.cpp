// Copyright (C) 2009 Colin MacDonald
// No rights reserved: this software is in the public domain.

#if defined(_MSC_VER)
#define _CRT_SECURE_NO_WARNINGS 1
#endif // _MSC_VER

#include "irrlicht.h"
#include "testUtils.h"
#include <assert.h>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

//! Tests IVideoDriver::writeImageToFile() using IWriteFile
bool writeImageToFile(void)
{
	IrrlichtDevice *device = createDevice( EDT_BURNINGSVIDEO, dimension2d<s32>(160, 120), 32);
	if (!device)
		return true; // Treat a failure to create a driver as benign; this saves a lot of #ifdefs

	IVideoDriver* driver = device->getVideoDriver();
	ISceneManager * smgr = device->getSceneManager();

	// Draw a cube background so that we can check that the pixels' alpha is working.
	ISceneNode * cube = smgr->addCubeSceneNode(50.f, 0, -1, vector3df(0, 0, 60));
	cube->setMaterialTexture(0, driver->getTexture("../media/wall.bmp"));
	cube->setMaterialFlag(video::EMF_LIGHTING, false);
	(void)smgr->addCameraSceneNode();

	driver->beginScene(true, true, SColor(255,100,101,140));
	smgr->drawAll();

	// Test for benign handling of offscreen pixel values as well as onscreen ones.
	for(s32 x = -10; x < 170; ++x)
	{
		s32 y = 120 * x / 160;
		driver->drawPixel((u32)x, (u32)y, SColor(255, 255 * x / 640, 255 * (640 - x) / 640, 0));
		y = 120 - y;
		driver->drawPixel((u32)x, (u32)y, SColor(255 * x / 640, 0, 255, 255));
	}

	driver->endScene();

	bool result = false;
	IWriteFile * writtenFile = 0;
	IWriteFile * memoryFile = 0;
	const char * writtenFilename = 0;
	const u32 BUFFER_SIZE = 160 * 120 * 4;
	c8 * buffer = 0;
	const char * referenceFilename = 0;

	irr::video::IImage * screenshot = driver->createScreenShot();
	if(!screenshot)
	{
		logTestString("Failed to take screenshot\n");
		assert(false);
		goto cleanup;
	}

	const video::ECOLOR_FORMAT format = screenshot->getColorFormat();
	if(format != video::ECF_R8G8B8)
	{
		irr::video::IImage * fixedScreenshot = driver->createImage(video::ECF_R8G8B8, screenshot);
		screenshot->drop();

		if(!fixedScreenshot)
		{
			logTestString("Failed to convert screenshot to ECF_A8R8G8B8\n");
			assert(false);
			goto cleanup;
		}

		screenshot = fixedScreenshot;
	}

	buffer = new c8[BUFFER_SIZE];
	writtenFilename = "results" DIR_SEP_STRING "burnings video 0.39b-drawPixel.png";
	memoryFile = device->getFileSystem()->createMemoryWriteFile(buffer, BUFFER_SIZE, writtenFilename, false);
	if(!driver->writeImageToFile(screenshot, memoryFile))
	{
		logTestString("Failed to write png to memory file\n");
		assert(false);
		goto cleanup;
	}

	writtenFile = device->getFileSystem()->createAndWriteFile(memoryFile->getFileName());
	if(!writtenFile)
	{
		logTestString("Can't open %s for writing.\n", writtenFilename);
		assert(false);
		goto cleanup;
	}

	if(memoryFile->getPos() != writtenFile->write(buffer, memoryFile->getPos()))
	{
		logTestString("Error while writing to %s.\n", writtenFilename);
		assert(false);
		goto cleanup;
	}

	writtenFile->drop();
	writtenFile = 0;

	referenceFilename = "media" DIR_SEP_STRING "burnings video 0.39b-drawPixel.png";
	if(!binaryCompareFiles(writtenFilename, referenceFilename))
	{
		logTestString("File written from memory is not the same as the reference file.\n");
		assert(false);
		goto cleanup;
	}

	result = true;

cleanup:
	if(writtenFile)
		writtenFile->drop();

	if(memoryFile)
		memoryFile->drop();

	delete [] buffer;

	device->drop();

	return result;
}

