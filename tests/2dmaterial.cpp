#include "testUtils.h"

using namespace irr;

// This test renders a 3d scene and a gui on top of it. The GUI is
// filtered via 2dmaterial (blurred).
// TODO: Works only for OpenGL right now
static bool addBlend2d(video::E_DRIVER_TYPE type)
{
	SIrrlichtCreationParameters params;
	params.AntiAlias = 0;
	params.Bits = 32;
	params.WindowSize = core::dimension2d<u32>(160, 120);
	params.DriverType = type;

	IrrlichtDevice *device = createDeviceEx(params);

	if (!device)
		return true; // in case the driver type does not exist

	video::IVideoDriver* driver = device->getVideoDriver();
	scene::ISceneManager* smgr = device->getSceneManager();

	scene::IAnimatedMesh* mesh = smgr->getMesh("../media/sydney.md2");
	if (!mesh)
	{
		device->closeDevice();
		device->run();
		device->drop();
		return false;
	}
	scene::IAnimatedMeshSceneNode* node = smgr->addAnimatedMeshSceneNode( mesh );

	if (node)
	{
		node->setMaterialFlag(video::EMF_LIGHTING, false);
		node->setMD2Animation(scene::EMAT_STAND);
		node->setMaterialTexture( 0, driver->getTexture("../media/sydney.bmp") );
	}

	smgr->addCameraSceneNode(0, core::vector3df(0,30,-40), core::vector3df(0,5,0));

	gui::IGUIEnvironment* env = device->getGUIEnvironment();
	{
		// create the toolbox window
		gui::IGUIWindow* wnd = env->addWindow(core::rect<s32>(0,0,800,480),
			false, L"Toolset", 0, 100);

		// create tab control and tabs
		gui::IGUITabControl* tab = env->addTabControl(
			core::rect<s32>(2,20,800-602,480-7), wnd, true, true);

		gui::IGUITab* t1 = tab->addTab(L"Config");

		// add some edit boxes and a button to tab one
		env->addImage(driver->getTexture("../media/tools.png"), core::vector2d<s32>(10,20), true, t1);
		env->addStaticText(L"X:", core::rect<s32>(22,48,40,66), false, false, t1);
		env->addEditBox(L"1.0", core::rect<s32>(40,46,130,66), true, t1, 201);

		// quick scale buttons
		env->addButton(core::rect<s32>(65,20,95,40), t1, 102, L"* 10");
		env->addButton(core::rect<s32>(100,20,130,40), t1, 103, L"* 0.1");
	}

	video::SMaterial& material2D = driver->getMaterial2D();
	material2D.setFlag(video::EMF_ANTI_ALIASING, true);
	for (unsigned int n=0; n<video::MATERIAL_MAX_TEXTURES; n++)
	{
		material2D.TextureLayer[n].BilinearFilter = true;
		material2D.TextureLayer[n].TextureWrapU = video::ETC_CLAMP_TO_EDGE;
		material2D.TextureLayer[n].TextureWrapV = video::ETC_CLAMP_TO_EDGE;
	}
	material2D.AntiAliasing=video::EAAM_FULL_BASIC;

	driver->beginScene(true, true, video::SColor(255,100,101,140));
	smgr->drawAll();
	driver->enableMaterial2D();
	env->drawAll();
	driver->enableMaterial2D(false);
	driver->endScene();

	bool result = takeScreenshotAndCompareAgainstReference(driver, "-addBlend2D.png", 98.2f);

	device->closeDevice();
	device->run();
	device->drop();
    return result;
} 

// This test renders 4 times the same image. Two via IGUIImage, two via draw2DImage
// 3 of the 4 images are filtered via 2dmaterial and bilinear filter, only the one
// at the bottom left is not.
static bool moreFilterTests(video::E_DRIVER_TYPE type)
{
	IrrlichtDevice* device = irr::createDevice(type, core::dimension2du(160,120));
	if (!device)
		return true;

    video::IVideoDriver* driver = device->getVideoDriver();
    gui::IGUIEnvironment* gui = device->getGUIEnvironment();

	driver->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, false);
	video::ITexture* tex = driver->getTexture("../media/irrlichtlogo.jpg");
    gui::IGUIImage* image = gui->addImage(core::recti(0,0,64,64));
    image->setScaleImage(true);
    image->setImage(tex);
    image->setUseAlphaChannel(true);
	driver->getMaterial2D().TextureLayer[0].BilinearFilter=true;

    {
        driver->beginScene(true, true, irr::video::SColor(255,255,255,255));

		// all three logos should be with bilinear filtering
        driver->enableMaterial2D();

        driver->getMaterial2D().setTexture(0, tex);
        driver->draw2DImage(tex, irr::core::rect<irr::s32>(64, 64, 128, 128), irr::core::rect<irr::s32>(0, 0, 88, 31));

		driver->getMaterial2D().setTexture(0, 0);
        driver->draw2DImage(tex, irr::core::rect<irr::s32>(64, 0, 128, 64), irr::core::rect<irr::s32>(0, 0, 88, 31));

        gui->drawAll();

		// the next gui image should be without filter
        driver->enableMaterial2D(false);
		image->setRelativePosition(core::recti(0,64,64,128));
        gui->drawAll();

		driver->endScene();
    }

	bool result = takeScreenshotAndCompareAgainstReference(driver, "-2dmatFilter.png");

	device->closeDevice();
	device->run();
	device->drop();
    return result;
} 

bool twodmaterial()
{
	bool result = addBlend2d(video::EDT_OPENGL);
	result &= addBlend2d(video::EDT_DIRECT3D9);
	result &= addBlend2d(video::EDT_DIRECT3D8);
	result &= addBlend2d(video::EDT_BURNINGSVIDEO);

	result &= moreFilterTests(video::EDT_OPENGL);
	result &= moreFilterTests(video::EDT_DIRECT3D9);
	result &= moreFilterTests(video::EDT_DIRECT3D8);
	result &= moreFilterTests(video::EDT_BURNINGSVIDEO);

	return result;
}
