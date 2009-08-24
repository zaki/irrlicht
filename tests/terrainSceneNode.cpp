
#include "testUtils.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;

bool terrainSceneNode(void)
{
    IrrlichtDevice *device =
        createDevice(video::EDT_OPENGL, dimension2du(160, 120), 32);

    IVideoDriver* driver = device->getVideoDriver();
    ISceneManager* smgr = device->getSceneManager();

    ITerrainSceneNode* terrain = smgr->addTerrainSceneNode(
        "../media/terrain-heightmap.bmp");
    terrain->setScale(core::vector3df(40.f, .1f, 40.f));

    terrain->setMaterialFlag(video::EMF_LIGHTING, false);
    terrain->setMaterialTexture(0, driver->getTexture("../media/terrain-texture.jpg"));
    terrain->setDebugDataVisible(scene::EDS_FULL);

    ICameraSceneNode* camera = smgr->addCameraSceneNode();

    const core::vector3df center (terrain->getBoundingBox().getCenter());
    camera->setTarget (center);

    // yes, Y is intentionally being set to X here
    const core::vector3df above (center.X, center.X, center.Z);
    camera->setPosition (above);
	camera->setUpVector(vector3df(1.f, 0.f, 0.f));
    camera->setFarValue(above.Y);

    device->run();
    smgr->drawAll();


	// This shouldn't cause a recalc
	camera->setUpVector(vector3df(1.f, 0.f, .01f).normalize());
    device->run();
	driver->beginScene(true, true, video::SColor(255,100,101,140));
	smgr->drawAll();
	driver->endScene();

	// Note that this has to be a slightly fuzzier than usual compare to satisfy multiple OpenGL environments
	bool result = takeScreenshotAndCompareAgainstReference(driver, "-terrainSceneNode-1.png", 98.3f);
	if(!result)
	{
		logTestString("Small camera up rotation caused bad recalc.\n");
		assert(false);
	}


	// This is big enough to cause a recalc
	camera->setUpVector(vector3df(1.f, 0.f, .1f).normalize());
    device->run();
	driver->beginScene(true, true, video::SColor(255,100,101,140));
	smgr->drawAll();
	driver->endScene();

	result &= takeScreenshotAndCompareAgainstReference(driver, "-terrainSceneNode-2.png", 98.9f);
	if(!result)
	{
		logTestString("Large camera up rotation caused bad recalc.\n");
		assert(false);
	}

    device->drop();
    return result;
}
