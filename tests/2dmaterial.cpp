#include "testUtils.h"

using namespace irr;

static bool addBlend2d(video::E_DRIVER_TYPE type)
{
	SIrrlichtCreationParameters params;
	params.AntiAlias = 0;
	params.Bits = 16;
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
		gui::IGUIElement* root = env->getRootGUIElement();

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
		material2D.TextureLayer[n].LODBias = 8;
	}
	material2D.AntiAliasing=video::EAAM_FULL_BASIC;

	driver->beginScene(true, true, video::SColor(255,100,101,140));
	smgr->drawAll();
	driver->enableMaterial2D();
	env->drawAll();
	driver->enableMaterial2D(false);
	driver->endScene();

	bool result = takeScreenshotAndCompareAgainstReference(driver, "-addBlend2D.png", 99.8f);

	device->drop();
    return result;
} 

bool twodmaterial()
{
	bool result = addBlend2d(video::EDT_OPENGL);
	result &= addBlend2d(video::EDT_DIRECT3D9);
	result &= addBlend2d(video::EDT_DIRECT3D8);
	result &= addBlend2d(video::EDT_BURNINGSVIDEO);
	return result;
}
