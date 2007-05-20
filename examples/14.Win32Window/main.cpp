// this example only runs in windows and demonstrates that Irrlicht
// can run inside a win32 window.

#include <irrlicht.h>
#ifndef _IRR_WINDOWS_
#error Windows only example
#else
#include <windows.h> // this example only runs with windows

using namespace irr;

#pragma comment(lib, "irrlicht.lib")

HWND hOKButton;
HWND hWnd;

static LRESULT CALLBACK CustomWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
		{
			HWND hwndCtl = (HWND)lParam;
			int code = HIWORD(wParam);

			if (hwndCtl == hOKButton)
			{
				DestroyWindow(hWnd);
				PostQuitMessage(0);
				return 0;
			}
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}





int main()
//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hpre, LPSTR cmd, int cc)
{
	HINSTANCE hInstance = 0;
	// create dialog

	const char* Win32ClassName = "CIrrlichtWindowsTestDialog";

	WNDCLASSEX wcex;
	wcex.cbSize			= sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)CustomWndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= DLGWINDOWEXTRA;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW);
	wcex.lpszMenuName	= 0;
	wcex.lpszClassName	= Win32ClassName;
	wcex.hIconSm		= 0;

	RegisterClassEx(&wcex);

	DWORD style = WS_SYSMENU | WS_BORDER | WS_CAPTION |
		WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SIZEBOX;

	int windowWidth = 440;
	int windowHeight = 380;

	hWnd = CreateWindow( Win32ClassName, "Irrlicht Win32 window example",
		style, 100, 100, windowWidth, windowHeight,
		NULL, NULL, hInstance, NULL);

	RECT clientRect;
	GetClientRect(hWnd, &clientRect);
	windowWidth = clientRect.right;
	windowHeight = clientRect.bottom;

	// create ok button

	hOKButton = CreateWindow("BUTTON", "OK - Close", WS_CHILD | WS_VISIBLE | BS_TEXT,
		windowWidth - 160, windowHeight - 40, 150, 30, hWnd, NULL, hInstance, NULL);

	// create some text

	CreateWindow("STATIC", "This is Irrlicht running inside a standard Win32 window.\n"\
		"Also mixing with MFC and .NET Windows.Forms is possible.",
		WS_CHILD | WS_VISIBLE, 20, 20, 400, 40, hWnd, NULL, hInstance, NULL);

	// create window to put irrlicht in

	HWND hIrrlichtWindow = CreateWindow("BUTTON", "", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
			50, 80, 320, 220, hWnd, NULL, hInstance, NULL);

	// create irrlicht device in the button window

	irr::SIrrlichtCreationParameters param;
	param.WindowId = reinterpret_cast<void*>(hIrrlichtWindow); // hColorButton
	param.DriverType = video::EDT_OPENGL;

	irr::IrrlichtDevice* device = irr::createDeviceEx(param);

	// setup a simple 3d scene

	irr::scene::ISceneManager* smgr = device->getSceneManager();
	video::IVideoDriver* driver = device->getVideoDriver();

	scene::ICameraSceneNode* cam = smgr->addCameraSceneNode();
	cam->setTarget(core::vector3df(0,0,0));

	scene::ISceneNodeAnimator* anim = smgr->createFlyCircleAnimator(core::vector3df(0,15,0), 30.0f);
	cam->addAnimator(anim);
	anim->drop();

	scene::ISceneNode* cube = smgr->addCubeSceneNode(20);

	cube->setMaterialTexture(0, driver->getTexture("../../media/wall.bmp"));
	cube->setMaterialTexture(1, driver->getTexture("../../media/water.jpg"));
	cube->setMaterialFlag( video::EMF_LIGHTING, false );
	cube->setMaterialType( video::EMT_REFLECTION_2_LAYER );

	smgr->addSkyBoxSceneNode(
	driver->getTexture("../../media/irrlicht2_up.jpg"),
	driver->getTexture("../../media/irrlicht2_dn.jpg"),
	driver->getTexture("../../media/irrlicht2_lf.jpg"),
	driver->getTexture("../../media/irrlicht2_rt.jpg"),
	driver->getTexture("../../media/irrlicht2_ft.jpg"),
	driver->getTexture("../../media/irrlicht2_bk.jpg"));

	// show and execute dialog

	ShowWindow(hWnd , SW_SHOW);
	UpdateWindow(hWnd);

	// do message queue

	// Instead of this, you can also simply use your own message loop
	// using GetMessage, DispatchMessage and whatever. Calling
	// Device->run() will cause Irrlicht to dispatch messages internally too.
	// You need not call Device->run() if you want to do your own message
	// dispatching loop, but Irrlicht will not be able to fetch
	// user input then and you have to do it on your own using the window
	// messages, DirectInput, or whatever.

	while (device->run())
	{
		driver->beginScene(true, true, 0);
		smgr->drawAll();
		driver->endScene();
	}

	// the alternative, own message dispatching loop without Device->run() would
	// look like this:

	/*MSG msg;
	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
				break;
		}

		// advance virtual time
		device->getTimer()->tick();

		// draw engine picture
		driver->beginScene(true, true, 0);
		smgr->drawAll();
		driver->endScene();
	}*/

	device->closeDevice();
	device->drop();

	return 0;
}
#endif // if windows

