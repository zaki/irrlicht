// Copyright (C) 2008-2009 Christian Stehno, Colin MacDonald
// No rights reserved: this software is in the public domain.

#include "testUtils.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

//! Tests rendering RTTs with draw2DImage
/** This test is very special in its setup, problematic situation was found by stefbuet. */
static bool testWith2DImage(E_DRIVER_TYPE driverType)
{
	IrrlichtDevice *device = createDevice (driverType, core::dimension2d < u32 > (128, 128));
	if (!device)
		return true; // No error if device does not exist

	device->setWindowCaption (L"Irrlicht - RTT Bug report");
	IVideoDriver *driver = device->getVideoDriver ();
	ISceneManager *smgr = device->getSceneManager ();

	ITexture *image = driver->getTexture ("../media/irrlichtlogo2.png");
	ITexture *RTT_texture = driver->addRenderTargetTexture (core::dimension2d < u32 > (128, 128));

	smgr->addCameraSceneNode (0, core::vector3df (100, 100, 100),
			      core::vector3df (0, 0, 0));

	/*to reproduce the bug :
	-draw the image : it's normal
	-apply an RTT texture to a model
	-remove the model
	-draw the image again : it's flipped
	*/

	SColor colors[4]={0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff};
	//draw the image :
	driver->beginScene (true, true, video::SColor (255, 200, 200, 200));
	driver->draw2DImage (image,
		       core::rect < s32 >
		       (64 - image->getSize ().Width / 2,
			64 - image->getSize ().Height / 2,
			64 + image->getSize ().Width / 2,
			64 + image->getSize ().Height / 2),
		       core::rect < s32 > (0, 0, image->getSize ().Width,
					   image->getSize ().Height), 0, colors,
		       true);
	driver->endScene ();

	//then create a model and apply to it the RTT Texture
	//rendering the model is important, if not rendered 1 time, bug won't appear.
	//after the render, we remove the node : important, if not done, bug won't appear too.
	IMesh *modelMesh = smgr->getMesh ("../media/earth.x");
	ISceneNode *modelNode = smgr->addMeshSceneNode(modelMesh);
	modelNode->setMaterialTexture (0, RTT_texture);
	driver->beginScene (true, true, video::SColor (255, 200, 200, 200));
	smgr->drawAll ();
	driver->endScene ();

	modelNode->remove ();

	//then we render the image normaly
	//it's now fliped...
	for (u32 i=0; i<10; ++i)
	{
		driver->beginScene (true, true, video::SColor (255, 200, 200, 200));

		//draw img
		driver->draw2DImage (image,
				   core::rect < s32 >
				   (64 - image->getSize ().Width / 2,
				    64 - image->getSize ().Height / 2,
				    64 + image->getSize ().Width / 2,
				    64 + image->getSize ().Height / 2),
				   core::rect < s32 > (0, 0, image->getSize ().Width,
						       image->getSize ().Height), 0,
				   colors, true);

		//call this is important :
		//if not called, the bug won't appear
		smgr->drawAll ();

		driver->endScene ();
	}

	bool result = takeScreenshotAndCompareAgainstReference(driver, "-rttWith2DImage.png", 99.9f);

	device->closeDevice();
	device->run();
	device->drop();

	return result;
}


bool rttAndZBuffer(video::E_DRIVER_TYPE driverType)
{
	SIrrlichtCreationParameters cp;
	cp.WindowSize.set(160,120);
	cp.Bits = 32;
	cp.AntiAlias = 4;
	cp.DriverType = driverType;

	IrrlichtDevice* nullDevice = createDevice(EDT_NULL);
	cp.WindowSize = nullDevice->getVideoModeList()->getDesktopResolution();
	nullDevice->drop();

	cp.WindowSize -= dimension2d<u32>(100, 100);

	IrrlichtDevice* device = createDeviceEx(cp);
	if (!device)
		return true;

	IVideoDriver* vd = device->getVideoDriver();
	ISceneManager* sm = device->getSceneManager();

	if	(!vd->queryFeature(video::EVDF_RENDER_TO_TARGET))
		return true;

	ITexture* rt = vd->addRenderTargetTexture(cp.WindowSize, "rt", ECF_A32B32G32R32F);
	S3DVertex vertices[4];
	vertices[0].Pos.Z = vertices[1].Pos.Z = vertices[2].Pos.Z = vertices[3].Pos.Z = 1.0f;
	vertices[0].Pos.Y = vertices[1].Pos.Y = 1.0f;
	vertices[2].Pos.Y = vertices[3].Pos.Y = -1.0f;
	vertices[0].Pos.X = vertices[3].Pos.X = -1.0f;
	vertices[1].Pos.X = vertices[2].Pos.X = 1.0f;
	vertices[0].TCoords.Y = vertices[1].TCoords.Y = 0.0f;
	vertices[2].TCoords.Y = vertices[3].TCoords.Y = 1.0f;
	vertices[0].TCoords.X = vertices[3].TCoords.X = 1.0f;
	vertices[1].TCoords.X = vertices[2].TCoords.X = 0.0f;

	u16 indices[6] = {0, 1, 3, 1, 2, 3};

	SMaterial rtMat;
	rtMat.BackfaceCulling = false;
	rtMat.Lighting = false;
	rtMat.TextureLayer[0].TextureWrapU =
		rtMat.TextureLayer[0].TextureWrapV = video::ETC_CLAMP_TO_EDGE;

	ILightSceneNode* light = sm->addLightSceneNode(NULL, vector3df(0, 50, 0),
		SColorf(1, 1, 1), 100);
	SLight& lightData = light->getLightData();

	ICameraSceneNode* cam = sm->addCameraSceneNode(NULL, vector3df(0, 10, 0));

	const IGeometryCreator* geom = sm->getGeometryCreator();
	IMeshManipulator* manip = sm->getMeshManipulator();
	IMesh* mesh;
	ISceneNode* node;
   
	mesh = geom->createCubeMesh(vector3df(10, 10, 10));
	manip->setVertexColors(mesh, SColor(255, 0, 0, 255));
	node = sm->addMeshSceneNode(mesh, NULL, -1, vector3df(0, 0, 30));
	node->getMaterial(0).EmissiveColor = SColor(255, 0, 0, 30);
	mesh->drop();

	mesh = geom->createSphereMesh(5.0f, 32, 32);
	node = sm->addMeshSceneNode(mesh, NULL, -1, vector3df(0, 0, 50));
	node->getMaterial(0).EmissiveColor = SColor(255, 30, 30, 30);
	mesh->drop();

	mesh = geom->createConeMesh(5.0f, 10.0f, 32, SColor(255, 255, 0, 0), SColor(255, 255, 0, 0));
	node = sm->addMeshSceneNode(mesh, NULL, -1, vector3df(0, 0, 70));
	node->getMaterial(0).EmissiveColor = SColor(255, 30, 0, 0);
	mesh->drop();

	{
		vd->beginScene(true, true, SColor(255, 0, 0, 0));
		vd->setRenderTarget(rt);
		sm->drawAll();
		vd->setRenderTarget(NULL);
		vd->setTransform(ETS_WORLD, core::IdentityMatrix);
		vd->setTransform(ETS_VIEW, core::IdentityMatrix);
		vd->setTransform(ETS_PROJECTION, core::IdentityMatrix);
		rtMat.setTexture(0, rt);
		vd->setMaterial(rtMat);
		vd->drawIndexedTriangleList(vertices, 4, indices, 2);
		vd->endScene();
	}
	bool result = takeScreenshotAndCompareAgainstReference(vd, "-rttAndZBuffer.png");

	device->closeDevice();
	device->run();
	device->drop();

	return result;
}


// result should be two times the same blind text on the left side, and
// the fireball image (with a very small text inside) in the middle of the screen
// drivers that don't support image scaling will show a pink background instead
bool rttAndText(video::E_DRIVER_TYPE driverType)
{
	IrrlichtDevice* device = createDevice(driverType, dimension2d<u32>(160, 120));
	if (!device)
		return true;

	IVideoDriver* driver = device->getVideoDriver();
	IGUIEnvironment* guienv = device->getGUIEnvironment();

	//RTT
	ITexture* rt = driver->addRenderTargetTexture(core::dimension2d<u32>(256, 256), "rt");
	if (!rt)
		return false;

	driver->beginScene(true, true, SColor(255,255, 255, 255));
	driver->setRenderTarget(rt, true, true, video::SColor(255,255,0,255));
	driver->draw2DImage(driver->getTexture("../media/fireball.bmp"), recti(0, 0,rt->getSize().Width,rt->getSize().Height), recti(0,0,64,64));
	guienv->getBuiltInFont()->draw(L"OMGGG =!", rect<s32>(120, 100, 256, 256), SColor(255, 0, 0, 255));
	driver->setRenderTarget(0);
	driver->endScene();

	ISceneManager* smgr = device->getSceneManager();

	ISceneNode* cube = smgr->addCubeSceneNode(20);
	cube->setMaterialFlag(EMF_LIGHTING, false);
	cube->setMaterialTexture(0, rt); // set material of cube to render target

	smgr->addCameraSceneNode(0, vector3df(0, 0, -30));

	// create a long text to produce much difference in failing result pictures
	gui::IGUIStaticText* text = guienv->addStaticText(L"asdddddddoamgmoasmgom\nfoaomsodommogdd\nddddddddd", rect<s32>(10, 20, 100, 160));

	driver->beginScene(true, true, SColor(255,255, 255, 255));
	cube->setVisible(false);
	smgr->drawAll();
	guienv->drawAll();

	cube->setVisible(true);
	smgr->drawAll();
	SMaterial mat(cube->getMaterial(0));
	driver->setMaterial(mat);
	text->setRelativePosition(core::position2di(10,30));
	guienv->drawAll();

	driver->endScene();

	bool result = takeScreenshotAndCompareAgainstReference(driver, "-rttAndText.png");

	device->closeDevice();
	device->run();
	device->drop();

	return result;
}

static void Render(IrrlichtDevice* device, ITexture* rt, vector3df& pos1, 
	vector3df& pos2, IAnimatedMesh* sphereMesh, vector3df& pos3, vector3df& pos4)
{
	device->getVideoDriver()->setRenderTarget(rt);
	device->getSceneManager()->drawAll();

	video::SMaterial mat;
	mat.ColorMaterial=video::ECM_NONE;
	mat.AmbientColor.set(255, 80, 80, 80);
	mat.DiffuseColor.set(255, 120, 30, 210);
	mat.SpecularColor.set(255,80,80,80);
	mat.Shininess = 8.f;	

	core::matrix4 m;
	m.setTranslation(pos1);
	device->getVideoDriver()->setTransform(video::ETS_WORLD, m);
	device->getVideoDriver()->setMaterial(mat);
	device->getVideoDriver()->drawMeshBuffer(sphereMesh->getMeshBuffer(0));

	m.setTranslation(pos2);
	mat.Shininess=0.f;
	device->getVideoDriver()->setTransform(video::ETS_WORLD, m);
	device->getVideoDriver()->setMaterial(mat);		
	device->getVideoDriver()->drawMeshBuffer(sphereMesh->getMeshBuffer(0));

	m.setTranslation(pos3);
	mat.Shininess=8.f;
	device->getVideoDriver()->setTransform(video::ETS_WORLD, m);
	device->getVideoDriver()->setMaterial(mat);
	device->getVideoDriver()->drawMeshBuffer(sphereMesh->getMeshBuffer(0));

	m.setTranslation(pos4);
	mat.Shininess=0.f;
	device->getVideoDriver()->setTransform(video::ETS_WORLD, m);
	device->getVideoDriver()->setMaterial(mat);
	device->getVideoDriver()->drawMeshBuffer(sphereMesh->getMeshBuffer(0));
}

bool rttAndAntiAliasing(video::E_DRIVER_TYPE driverType)
{
	SIrrlichtCreationParameters cp;
	cp.DriverType = driverType;
	cp.WindowSize = core::dimension2di(160, 120);
	cp.AntiAlias = 2;
	cp.Vsync = true;

	IrrlichtDevice* device = createDeviceEx(cp);
	if (!device)
		return true;

	IVideoDriver* driver = device->getVideoDriver();

	// sphere mesh
	scene::IAnimatedMesh* sphereMesh = device->getSceneManager()->addSphereMesh("atom", 1, 32, 32);

	// cam
	scene::ICameraSceneNode* cam = device->getSceneManager()->addCameraSceneNode(NULL, vector3df(0, 1, -5), vector3df(0, 0, 0));
	cam->setNearValue(0.01f);
	cam->setFarValue(100.f);
	cam->updateAbsolutePosition();
	device->getSceneManager()->setActiveCamera(cam);
	device->getSceneManager()->addLightSceneNode(0, core::vector3df(10,10,10));
	device->getSceneManager()->setAmbientLight(SColorf(0.3f,0.3f,0.3f));

	float radius = 3.f;
	vector3df pos1(-radius,0,0);
	vector3df pos2(radius,0,0);
	vector3df pos3(0,0,radius);
	vector3df pos4(0,0,-radius);
	matrix4 m;

	IGUIStaticText* st = device->getGUIEnvironment()->addStaticText(L"", recti(0,0,200,20), false, false);
	st->setOverrideColor(SColor(255,255,255,0));

	dimension2du dim_txt = dimension2du(160/2, 120/2);

	ITexture* rt0 = NULL;
	ITexture* rt1 = device->getVideoDriver()->addRenderTargetTexture(dim_txt, "rt1", device->getColorFormat());
	ITexture* rt2 = device->getVideoDriver()->addRenderTargetTexture(dim_txt, "rt2", device->getColorFormat());
	ITexture* rt3 = device->getVideoDriver()->addRenderTargetTexture(dim_txt, "rt3", video::ECF_A8R8G8B8);//device->getColorFormat());
	ITexture* rt4 = device->getVideoDriver()->addRenderTargetTexture(dim_txt, "rt4", device->getColorFormat());

	device->getSceneManager()->setActiveCamera(cam);
	device->getVideoDriver()->beginScene(); //true, true, SColor(0, 30, 40, 60));
#if 1
	st->setText(L"Texture Rendering");
	Render(device, rt1, pos1, pos2, sphereMesh, pos3, pos4);
	Render(device, rt2, pos1, pos2, sphereMesh, pos3, pos4);
	Render(device, rt3, pos1, pos2, sphereMesh, pos3, pos4);
	Render(device, rt4, pos1, pos2, sphereMesh, pos3, pos4);

	device->getVideoDriver()->setRenderTarget(0);
	device->getVideoDriver()->draw2DImage(rt1, core::position2di(0,0));
	device->getVideoDriver()->draw2DImage(rt2, core::position2di(80,0));
	device->getVideoDriver()->draw2DImage(rt3, core::position2di(0,60));
	device->getVideoDriver()->draw2DImage(rt4, core::position2di(80,60));
#else
	Render(device, rt0, pos1, pos2, sphereMesh, pos3, pos4);
#endif
	st->draw();
	device->getVideoDriver()->endScene();

	bool result = takeScreenshotAndCompareAgainstReference(driver, "-rttAndAntiAlias.png");

	device->closeDevice();
	device->run();
	device->drop();

	return result;
}

bool renderTargetTexture(void)
{
	bool passed = true;

	passed &= testWith2DImage(EDT_OPENGL);
	passed &= testWith2DImage(EDT_SOFTWARE);
	passed &= testWith2DImage(EDT_BURNINGSVIDEO);
	passed &= testWith2DImage(EDT_DIRECT3D9);
	passed &= testWith2DImage(EDT_DIRECT3D8);

#if 0
	passed &= rttAndZBuffer(EDT_OPENGL);
	passed &= rttAndZBuffer(EDT_SOFTWARE);
	passed &= rttAndZBuffer(EDT_BURNINGSVIDEO);
	passed &= rttAndZBuffer(EDT_DIRECT3D9);
	passed &= rttAndZBuffer(EDT_DIRECT3D8);
#endif

	passed &= rttAndAntiAliasing(EDT_OPENGL);
//	passed &= rttAndAntiAliasing(EDT_SOFTWARE);
	passed &= rttAndAntiAliasing(EDT_BURNINGSVIDEO);
	passed &= rttAndAntiAliasing(EDT_DIRECT3D9);
	passed &= rttAndAntiAliasing(EDT_DIRECT3D8);

	passed &= rttAndText(EDT_OPENGL);
	passed &= rttAndText(EDT_DIRECT3D9);
	passed &= rttAndText(EDT_DIRECT3D8);
	passed &= rttAndText(EDT_BURNINGSVIDEO);
	passed &= rttAndText(EDT_SOFTWARE);

	return passed;
}
