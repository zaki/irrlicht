/** Example 017 Helloworld mobile

This example show Hello World for Windows mobile
*/

#include <irrlicht.h>

#ifdef _IRR_USE_WINDOWS_CE_DEVICE_
#include <windows.h>
#endif

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

#pragma comment(lib, "Irrlicht.lib")

class MyEventReceiver : public IEventReceiver
{
private:
	IrrlichtDevice *Device;
public:
	MyEventReceiver ( IrrlichtDevice *device ): Device ( device ) {}

	virtual bool OnEvent(const SEvent& event)
	{
		if (event.EventType == EET_GUI_EVENT)
		{
			s32 id = event.GUIEvent.Caller->getID();

			switch(event.GUIEvent.EventType)
			{
				case EGET_BUTTON_CLICKED:
				if (id == 2)
				{
					Device->closeDevice();
					return true;
				} break;
			}
		}

		return false;
	}
};

class CSampleSceneNode : public ISceneNode
{
	aabbox3d<f32> Box;
	S3DVertex Vertices[4];
	SMaterial Material;
public:

	CSampleSceneNode(ISceneNode* parent, ISceneManager* mgr, s32 id)
		: ISceneNode(parent, mgr, id)
	{
		Material.Wireframe = false;
		Material.Lighting = false;

		Vertices[0] = S3DVertex(0,0,10, 1,1,0, SColor(255,0,255,255), 0, 1);
		Vertices[1] = S3DVertex(10,0,-10, 1,0,0, SColor(255,255,0,255), 1, 1);
		Vertices[2] = S3DVertex(0,20,0, 0,1,1, SColor(255,255,255,0), 1, 0);
		Vertices[3] = S3DVertex(-10,0,-10, 0,0,1, SColor(255,0,255,0), 0, 0);
		Box.reset(Vertices[0].Pos);
		for (s32 i=1; i<4; ++i)
			Box.addInternalPoint(Vertices[i].Pos);
	}
	virtual void OnRegisterSceneNode()
	{
		if (IsVisible)
			SceneManager->registerNodeForRendering(this);

		ISceneNode::OnRegisterSceneNode();
	}

	virtual void render()
	{
		u16 indices[] = {	0,2,3, 2,1,3, 1,0,3, 2,0,1	};
		IVideoDriver* driver = SceneManager->getVideoDriver();

		driver->setMaterial(Material);
		driver->setTransform(ETS_WORLD, AbsoluteTransformation);
		driver->drawIndexedTriangleList(&Vertices[0], 4, &indices[0], 4);
	}

	virtual const aabbox3d<f32>& getBoundingBox() const
	{
		return Box;
	}

	virtual u32 getMaterialCount()
	{
		return 1;
	}

	virtual SMaterial& getMaterial(u32 i)
	{
		return Material;
	}	
};


#if defined(_WIN32_WCE)
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine,
                        int nCmdShow )
#else
int main()
#endif
{
	E_DRIVER_TYPE driverType = EDT_BURNINGSVIDEO;

	// create device

	IrrlichtDevice *device =
		createDevice(driverType, dimension2d<s32>(240, 320), 16, true );
		
	if (device == 0)
		return 1; // could not create selected driver.

	// create engine and camera
	MyEventReceiver receiver(device);
	device->setEventReceiver(&receiver);
	device->setWindowCaption(L"Irrlicht CE Demo");

	IVideoDriver* driver = device->getVideoDriver();
	ISceneManager* smgr = device->getSceneManager();
	IGUIEnvironment* guienv = device->getGUIEnvironment();

	// set the filesystem relative to the executable
	{
		wchar_t buf[255];
		GetModuleFileNameW ( 0, buf, 255 );

		stringc base = buf;
		base = base.subString ( 0, base.findLast ( '\\' ) );
		device->getFileSystem()->addFolderFileArchive ( base.c_str() );
	}


	IGUIStaticText *text = 
	guienv->addStaticText(L"FPS: 25",
		rect<s32>(60,5,200,20), false );

	guienv->addButton(core::rect<int>(10,5,50,20), 0, 2, L"Quit");

	smgr->addCameraSceneNode(0, vector3df(0,-40,0), vector3df(0,0,0));

	CSampleSceneNode *myNode = 
		new CSampleSceneNode(smgr->getRootSceneNode(), smgr, 666);

	ISceneNodeAnimator* anim = 
		smgr->createRotationAnimator(vector3df(0.8f, 0, 0.8f));

	if(anim)
	{
		myNode->addAnimator(anim);
		anim->drop();
		anim = 0; // As I shouldn't refer to it again, ensure that I can't
	}

	myNode->drop();
	myNode = 0; // As I shouldn't refer to it again, ensure that I can't

	u32 frames=0;
	while(device->run())
	{
		driver->beginScene(true, true, SColor(0,100,100,100));
		smgr->drawAll();
		guienv->drawAll();
		driver->endScene();

		if (++frames==10)
		{
			stringw str = L"FPS: ";
			str += (s32)driver->getFPS();

			text->setText ( str.c_str() );
			frames=0;
		}
	}

	device->drop();
	
	return 0;
}

