// Copyright (C) 2008-2009 Colin MacDonald
// No rights reserved: this software is in the public domain.

#include "testUtils.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;

/** Tests that the geometry creator does what it says it does. */
bool testGeometryCreator(void)
{
    IrrlichtDevice *device = createDevice(video::EDT_BURNINGSVIDEO,
										core::dimension2du(160,120), 32);
    if (!device)
        return false;

    IVideoDriver* driver = device->getVideoDriver();
    ISceneManager* smgr = device->getSceneManager();
	(void)smgr->addCameraSceneNode(0, vector3df(0, 0, -50));

	const IGeometryCreator * geom = smgr->getGeometryCreator();

	ITexture * wall = driver->getTexture("../media/wall.bmp");

	SMaterial material;
	material.Lighting = false;
	material.TextureLayer[0].Texture = wall;

	irr::scene::IMesh * meshHill = geom->createHillPlaneMesh(dimension2df(10, 5), dimension2du(5, 5), 
									&material, 10, dimension2df(2, 2), dimension2df(3, 3) );
	IMeshSceneNode * node = smgr->addMeshSceneNode(meshHill, 0, -1,
									vector3df(0, 10, 0), vector3df(-60, 0, 0));
	meshHill->drop();

	irr::scene::IMesh * meshArrow = geom->createArrowMesh(4, 8, 10, 6, 3, 6,
								SColor(255, 255, 0, 0), SColor(255, 0, 255, 0));
	node = smgr->addMeshSceneNode(meshArrow, 0, -1, vector3df(-10, -20, 0));
	node->setMaterialFlag(video::EMF_LIGHTING, false);
	meshArrow->drop();

	irr::scene::IMesh * meshCone = geom->createConeMesh(5.f, 10.f, 16);
	node = smgr->addMeshSceneNode(meshCone, 0, -1, vector3df(-35, -20, 0));
	node->setMaterialFlag(video::EMF_LIGHTING, false);
	node->setMaterialTexture(0, wall);
	meshCone->drop();

	irr::scene::IMesh * meshCube = geom->createCubeMesh();
	node = smgr->addMeshSceneNode(meshCube, 0, -1, vector3df(-20, -20, 0));
	node->setMaterialFlag(video::EMF_LIGHTING, false);
	node->setMaterialTexture(0, wall);
	meshCube->drop();

	irr::scene::IMesh * meshCylinder = geom->createCylinderMesh(3, 10, 16);
	node = smgr->addMeshSceneNode(meshCylinder, 0, -1, vector3df(0, -20, 10), core::vector3df(45,0,0));
	node->setMaterialFlag(video::EMF_LIGHTING, false);
	node->setMaterialTexture(0, wall);
	meshCylinder->drop();

	irr::scene::IMesh * meshSphere = geom->createSphereMesh();
	node = smgr->addMeshSceneNode(meshSphere, 0, -1, vector3df(10, -15, 0));
	node->setMaterialFlag(video::EMF_LIGHTING, false);
	node->setMaterialTexture(0, wall);
	meshSphere->drop();

	irr::scene::IMesh * meshVolumeLight = geom->createVolumeLightMesh();
	node = smgr->addMeshSceneNode(meshVolumeLight, 0, -1, vector3df(20, -20, -10));
	node->setMaterialFlag(video::EMF_LIGHTING, false);
	node->setMaterialTexture(0, wall);
	node->setScale(core::vector3df(4.f,4.f,4.f));
	meshVolumeLight->drop();

	bool result = false;
	device->run();
	if (driver->beginScene(true, true, video::SColor(0, 80, 80, 80)))
	{
		smgr->drawAll();
		driver->endScene();
		result = takeScreenshotAndCompareAgainstReference(driver, "-testGeometryCreator.png", 99.999f);
	}

    device->drop();

    return result;
}
