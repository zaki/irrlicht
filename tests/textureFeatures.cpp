// Copyright (C) 2008-2012 Christian Stehno, Colin MacDonald
// No rights reserved: this software is in the public domain.

#include "testUtils.h"

using namespace irr;
using namespace core;

namespace
{

//! check miplevels by visual test
bool renderMipLevels(video::E_DRIVER_TYPE driverType)
{
	IrrlichtDevice *device = createDevice( driverType, dimension2d<u32>(160, 120), 32);
	if (!device)
		return true; // Treat a failure to create a driver as benign; this saves a lot of #ifdefs

	video::IVideoDriver* driver = device->getVideoDriver();
	scene::ISceneManager * smgr = device->getSceneManager();
	if (!driver->queryFeature(video::EVDF_MIP_MAP))
	{
		device->closeDevice();
		device->run();
		device->drop();
		return true;
	}

	stabilizeScreenBackground(driver);

	logTestString("Testing driver %ls\n", driver->getName());

	scene::ISceneNode* n = smgr->addCubeSceneNode();
	scene::ISceneNode* n2 = smgr->addCubeSceneNode(10, 0, -1, vector3df(20,0,30), vector3df(0,45,0));

	// we use a main texture with blue on top and red below
	// and mipmap with pink on top and cyan below
	if (n && n2)
	{
		// create the texture and miplevels with distinct colors
		u32 texData[16*16];
		for (u32 i=0; i<16*16; ++i)
			texData[i]=(i<8*16?0xff0000ff:0xffff0000);
		video::IImage* image = driver->createImageFromData(video::ECF_A8R8G8B8, dimension2du(16,16), texData, false);
		u32 mipdata[8*16];
		u32 index=0;
		for (u32 j=8; j>0; j/=2)
		{
			for (u32 i=0; i<j; ++i)
			{
				u32 val=(i<j/2?0xffff00ff:0xff00ffff);
				for (u32 k=0; k<j; ++k)
					mipdata[index++]=val;
			}
		}

		image->setMipMapsData(mipdata, false, true);
		video::ITexture* tex = driver->addTexture("miptest", image);
		if (!tex)
			// is probably an error in the mipdata handling
			return false;
		else
		{
			n->setMaterialFlag(video::EMF_LIGHTING, false);
			n->setMaterialTexture(0, tex);
			n2->setMaterialFlag(video::EMF_LIGHTING, false);
			n2->setMaterialTexture(0, tex);
		}
		image->drop();
	}

	(void)smgr->addCameraSceneNode(0, vector3df(10,0,-30));

	driver->beginScene(video::ECBF_COLOR | video::ECBF_DEPTH, video::SColor(255,100,101,140));
	smgr->drawAll();
	driver->endScene();

	bool result = takeScreenshotAndCompareAgainstReference(driver, "-renderMipmap.png");

	if (!result)
		logTestString("mipmap render failed.\n", driver->getName());
	else
		logTestString("Passed\n");

	device->closeDevice();
	device->run();
	device->drop();

	return result;
}

//! Tests locking
bool lockTexture(video::E_DRIVER_TYPE driverType)
{
	IrrlichtDevice *device = createDevice( driverType, dimension2d<u32>(160, 120), 32);
	if (!device)
		return true; // Treat a failure to create a driver as benign; this saves a lot of #ifdefs

	video::IVideoDriver* driver = device->getVideoDriver();
	scene::ISceneManager * smgr = device->getSceneManager();

	if (!driver->queryFeature(video::EVDF_MIP_MAP))
	{
		device->closeDevice();
		device->run();
		device->drop();
		return true;
	}

	bool testCubemap = driver->queryFeature(video::EVDF_TEXTURE_CUBEMAP);

	stabilizeScreenBackground(driver);

	logTestString("Testing driver %ls\n", driver->getName());

	scene::ISceneNode* n = smgr->addCubeSceneNode();
	scene::ISceneNode* n2 = smgr->addCubeSceneNode(10, 0, -1, vector3df(20, 0, 30), vector3df(0, 45, 0));

	if (n && n2)
	{
		u32 texData[16*16];

		for (u32 i=0; i<16*16; ++i)
			texData[i]=0xff0000ff-i;

		// texture 2d

		video::IImage* image = driver->createImageFromData(video::ECF_A8R8G8B8, dimension2du(16,16), texData, false);

		video::ITexture* tex = driver->addTexture("tex2d", image);

		if (!tex)
			return false;
		else
			n->setMaterialTexture(0, tex);

		// cubemap

		if (testCubemap)
		{
			video::ITexture* texCube = driver->addTextureCubemap("texcube", image, image, image, image, image, image);

			if (!texCube)
				testCubemap = false;
			else
				n2->setMaterialTexture(0, texCube);
		}

		image->drop();
	}

	smgr->addCameraSceneNode(0, vector3df(10, 0, -30));

	driver->beginScene(video::ECBF_COLOR | video::ECBF_DEPTH, video::SColor(255,100,101,140));
	smgr->drawAll();
	driver->endScene();

	video::ITexture* tex = driver->findTexture("tex2d");

	video::SColor* bits = (video::SColor*)tex->lock(video::ETLM_READ_WRITE);
	bits[0]=0xff00ff00;
	bits[1]=0xff00ff00;
	tex->unlock();

	bits = (video::SColor*)tex->lock(video::ETLM_READ_ONLY, 0);
	bool result = ((bits[0].color==0xff00ff00)&&(bits[2].color==0xff0000fd));
	tex->unlock();

	if (!result)
		logTestString("texture 2d lock with driver %ls failed.\n", driver->getName());
	else
		logTestString("Passed\n");

	if (testCubemap)
	{
		tex = driver->findTexture("texcube");

		for (u32 i = 0; i < 6; ++i)
		{
			bits = (video::SColor*)tex->lock(video::ETLM_READ_WRITE, i);
			bits[0] = 0xff00ff00;
			bits[1] = 0xff00ff00;
			tex->unlock();
		}

		for (u32 i = 0; i < 6; ++i)
		{
			bits = (video::SColor*)tex->lock(video::ETLM_READ_ONLY, i);
			u32 b0 = bits[0].color;
			u32 b2 = bits[2].color;

			result &= ((bits[0].color == 0xff00ff00) && (bits[2].color == 0xff0000fd));
			tex->unlock();
		}

		if (!result)
			logTestString("texture cubemap lock with driver %ls failed.\n", driver->getName());
		else
			logTestString("Passed\n");
	}

	device->closeDevice();
	device->run();
	device->drop();

	return result;
}

}

bool textureFeatures(void)
{
	bool result = true;

	TestWithAllDrivers(renderMipLevels);
	TestWithAllDrivers(lockTexture);

	return result;
}
