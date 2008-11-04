/** Example 009 Mesh Viewer

This tutorial show how to create a more complex application with the engine.
We construct a simple mesh viewer using the user interface API and the
scene management of Irrlicht. The tutorial show how to create and use Buttons,
Windows, Toolbars, Menus, ComboBoxes, Tabcontrols, Editboxes, Images,
MessageBoxes, SkyBoxes, and how to parse XML files with the integrated XML
reader of the engine.

We start like in most other tutorials: Include all nesessary header files, add
a comment to let the engine be linked with the right .lib file in Visual
Studio, and declare some global variables. We also add two 'using namespace'
statements, so we do not need to write the whole names of all classes. In this
tutorial, we use a lot stuff from the gui namespace.
*/
#include <irrlicht.h>
#include <iostream>


using namespace irr;
using namespace gui;

#pragma comment(lib, "Irrlicht.lib")


/*
Some global variables used later on
*/
IrrlichtDevice *Device = 0;
core::stringc StartUpModelFile;
core::stringw MessageText;
core::stringw Caption;
scene::IAnimatedMeshSceneNode* Model = 0;
scene::ISceneNode* SkyBox = 0;

scene::ICameraSceneNode* Camera[2] = { 0, 0};

/*
Toggle between various cameras
*/
void setActiveCamera ( scene::ICameraSceneNode* newActive )
{
	if ( 0 == Device )
		return;

	scene::ICameraSceneNode* active = Device->getSceneManager()->getActiveCamera ();

	newActive->setInputReceiverEnabled ( true );
	Device->getSceneManager()->setActiveCamera ( newActive );
}

/*
The three following functions do several stuff used by the mesh viewer. The
first function showAboutText() simply displays a messagebox with a caption and
a message text. The texts will be stored in the MessageText and Caption
variables at startup.
*/
void showAboutText()
{
	// create modal message box with the text
	// loaded from the xml file.
	Device->getGUIEnvironment()->addMessageBox(
		Caption.c_str(), MessageText.c_str());
}


/*
The second function loadModel() loads a model and displays it using an
addAnimatedMeshSceneNode and the scene manager. Nothing difficult. It also
displays a short message box, if the model could not be loaded.
*/
void loadModel(const c8* fn)
{
	// modify the name if it a .pk3 file

	core::stringc filename ( fn );

	core::stringc extension;
	core::getFileNameExtension ( extension, filename );
	extension.make_lower();

	// if a texture is loaded apply it to the current model..
	if (	extension == ".jpg" ||
			extension == ".pcx" ||
			extension == ".png" ||
			extension == ".ppm" ||
			extension == ".pgm" ||
			extension == ".pbm" ||
			extension == ".psd" ||
			extension == ".tga" ||
			extension == ".bmp"
		)
	{
		video::ITexture * texture =
			Device->getVideoDriver()->getTexture( filename.c_str() );
		if ( texture && Model )
		{
			// always reload texture
			Device->getVideoDriver()->removeTexture ( texture );
			texture = Device->getVideoDriver()->getTexture( filename.c_str() );

			Model->setMaterialTexture ( 0, texture );
		}
		return;
	}

	// if a archive is loaded add it to the FileSystems..
	if (	extension == ".pk3" ||
			extension == ".zip"
		)
	{
		Device->getFileSystem()->addZipFileArchive( filename.c_str () );
		return;
	}

	// load a model into the engine

	if (Model)
		Model->remove();

	Model = 0;

	scene::IAnimatedMesh* m = Device->getSceneManager()->getMesh( filename.c_str() );

	if (!m)
	{
		// model could not be loaded

		if (StartUpModelFile != filename)
			Device->getGUIEnvironment()->addMessageBox(
			Caption.c_str(), L"The model could not be loaded. " \
			L"Maybe it is not a supported file format.");
		return;
	}

	// set default material properties

	Model = Device->getSceneManager()->addAnimatedMeshSceneNode(m);
	Model->setMaterialFlag(video::EMF_LIGHTING, false);
//	Model->setMaterialFlag(video::EMF_BACK_FACE_CULLING, false);
	Model->setDebugDataVisible(scene::EDS_OFF);
	Model->setAnimationSpeed(30);
}


/*
Finally, the third function creates a toolbox window. In this simple mesh
viewer, this toolbox only contains a tab control with three edit boxes for
changing the scale of the displayed model.
*/
void createToolBox()
{
	// remove tool box if already there
	IGUIEnvironment* env = Device->getGUIEnvironment();
	IGUIElement* root = env->getRootGUIElement();
	IGUIElement* e = root->getElementFromId(5000, true);
	if (e) e->remove();

	// create the toolbox window
	IGUIWindow* wnd = env->addWindow(core::rect<s32>(600,25,800,480),
		false, L"Toolset", 0, 5000);

	// create tab control and tabs
	IGUITabControl* tab = env->addTabControl(
		core::rect<s32>(2,20,800-602,480-7), wnd, true, true);

	IGUITab* t1 = tab->addTab(L"Scale");

	// add some edit boxes and a button to tab one
	env->addEditBox(L"1.0", core::rect<s32>(40,50,130,70), true, t1, 901);
	env->addEditBox(L"1.0", core::rect<s32>(40,80,130,100), true, t1, 902);
	env->addEditBox(L"1.0", core::rect<s32>(40,110,130,130), true, t1, 903);

	env->addButton(core::rect<s32>(10,150,100,190), t1, 1101, L"set");

	// add senseless checkbox
	env->addCheckBox(true, core::rect<s32>(10,220,200,240), t1, -1,
			L"Senseless Checkbox");

	// add undocumented transparent control
	env->addStaticText(L"Transparent Control:",
			core::rect<s32>(10,240,150,260), true, false, t1);
	IGUIScrollBar* scrollbar = env->addScrollBar(true,
			core::rect<s32>(10,260,150,275), t1, 104);
	scrollbar->setMax(255);
	scrollbar->setPos(255);

	// bring irrlicht engine logo to front, because it
	// now may be below the newly created toolbox
	root->bringToFront(root->getElementFromId(666, true));
}


/*
To get all the events sent by the GUI Elements, we need to create an event
receiver. This one is really simple. If an event occurs, it checks the id of
the caller and the event type, and starts an action based on these values. For
example, if a menu item with id 100 was selected, if opens a file-open-dialog.
*/
class MyEventReceiver : public IEventReceiver
{
public:
	virtual bool OnEvent(const SEvent& event)
	{
		// Escape swaps Camera Input
		if (event.EventType == EET_KEY_INPUT_EVENT &&
			event.KeyInput.Key == irr::KEY_ESCAPE &&
			event.KeyInput.PressedDown == false)
		{
			if ( Device )
			{
				scene::ICameraSceneNode * camera =
					Device->getSceneManager()->getActiveCamera();
				if ( camera )
				{
					camera->setInputReceiverEnabled ( !camera->isInputReceiverEnabled() );
				}
				return true;
			}
		}

		if (event.EventType == EET_GUI_EVENT)
		{
			s32 id = event.GUIEvent.Caller->getID();
			IGUIEnvironment* env = Device->getGUIEnvironment();

			switch(event.GUIEvent.EventType)
			{
			case EGET_MENU_ITEM_SELECTED:
				{
					// a menu item was clicked

					IGUIContextMenu* menu = (IGUIContextMenu*)event.GUIEvent.Caller;
					s32 id = menu->getItemCommandId(menu->getSelectedItem());

					switch(id)
					{
					case 100: // File -> Open Model
						env->addFileOpenDialog(L"Please select a model file to open");
						break;
					case 101: // File -> Set Model Archive
						env->addFileOpenDialog(L"Please select your game archive/directory");
						break;
					case 200: // File -> Quit
						Device->closeDevice();
						break;
					case 300: // View -> Skybox
						SkyBox->setVisible(!SkyBox->isVisible());
						break;
					case 400: // View -> Debug Information
						if (Model)
							Model->setDebugDataVisible(scene::EDS_OFF);
						break;
					case 410: // View -> Debug Information
						if (Model)
							Model->setDebugDataVisible((scene::E_DEBUG_SCENE_TYPE)(Model->isDebugDataVisible()^scene::EDS_BBOX));
						break;
					case 420: // View -> Debug Information
						if (Model)
							Model->setDebugDataVisible((scene::E_DEBUG_SCENE_TYPE)(Model->isDebugDataVisible()^scene::EDS_NORMALS));
						break;
					case 430: // View -> Debug Information
						if (Model)
							Model->setDebugDataVisible((scene::E_DEBUG_SCENE_TYPE)(Model->isDebugDataVisible()^scene::EDS_SKELETON));
						break;
					case 440: // View -> Debug Information
						if (Model)
							Model->setDebugDataVisible((scene::E_DEBUG_SCENE_TYPE)(Model->isDebugDataVisible()^scene::EDS_MESH_WIRE_OVERLAY));
						break;
					case 450: // View -> Debug Information
						if (Model)
							Model->setDebugDataVisible((scene::E_DEBUG_SCENE_TYPE)(Model->isDebugDataVisible()^scene::EDS_HALF_TRANSPARENCY));
						break;
					case 460: // View -> Debug Information
						if (Model)
							Model->setDebugDataVisible((scene::E_DEBUG_SCENE_TYPE)(Model->isDebugDataVisible()^scene::EDS_BBOX_BUFFERS));
						break;
					case 499: // View -> Debug Information
						if (Model)
							Model->setDebugDataVisible(scene::EDS_FULL);
						break;
					case 500: // Help->About
						showAboutText();
						break;
					case 610: // View -> Material -> Solid
						if (Model)
							Model->setMaterialType(video::EMT_SOLID);
						break;
					case 620: // View -> Material -> Transparent
						if (Model)
							Model->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
						break;
					case 630: // View -> Material -> Reflection
						if (Model)
							Model->setMaterialType(video::EMT_SPHERE_MAP);
						break;

					case 1000:
						setActiveCamera ( Camera[0] );
						break;
					case 1100:
						setActiveCamera ( Camera[1] );
						break;

					}
				break;
				}

			case EGET_FILE_SELECTED:
				{
					// load the model file, selected in the file open dialog
					IGUIFileOpenDialog* dialog =
						(IGUIFileOpenDialog*)event.GUIEvent.Caller;
					loadModel(core::stringc(dialog->getFileName()).c_str());
				}

			case EGET_SCROLL_BAR_CHANGED:

				// control skin transparency
				if (id == 104)
				{
					s32 pos = ((IGUIScrollBar*)event.GUIEvent.Caller)->getPos();
					for (s32 i=0; i<irr::gui::EGDC_COUNT ; ++i)
					{
						video::SColor col = env->getSkin()->getColor((EGUI_DEFAULT_COLOR)i);
						col.setAlpha(pos);
						env->getSkin()->setColor((EGUI_DEFAULT_COLOR)i, col);
					}
				}
				break;

			case EGET_COMBO_BOX_CHANGED:

				// control anti-aliasing/filtering
				if (id == 108)
				{
					s32 pos = ((IGUIComboBox*)event.GUIEvent.Caller)->getSelected();
					switch (pos)
					{
						case 0:
						if (Model)
						{
							Model->setMaterialFlag(video::EMF_BILINEAR_FILTER, false);
							Model->setMaterialFlag(video::EMF_TRILINEAR_FILTER, false);
							Model->setMaterialFlag(video::EMF_ANISOTROPIC_FILTER, false);
						}
						break;
						case 1:
						if (Model)
						{
							Model->setMaterialFlag(video::EMF_BILINEAR_FILTER, true);
							Model->setMaterialFlag(video::EMF_TRILINEAR_FILTER, false);
						}
						break;
						case 2:
						if (Model)
						{
							Model->setMaterialFlag(video::EMF_BILINEAR_FILTER, false);
							Model->setMaterialFlag(video::EMF_TRILINEAR_FILTER, true);
						}
						break;
						case 3:
						if (Model)
						{
							Model->setMaterialFlag(video::EMF_ANISOTROPIC_FILTER, true);
						}
						break;
						case 4:
						if (Model)
						{
							Model->setMaterialFlag(video::EMF_ANISOTROPIC_FILTER, false);
						}
						break;
					}
				}
				break;

			case EGET_BUTTON_CLICKED:

				switch(id)
				{
				case 1101:
					{
						// set scale
						gui::IGUIElement* root = env->getRootGUIElement();
						core::vector3df scale;
						core::stringc s;

						s = root->getElementFromId(901, true)->getText();
						scale.X = (f32)atof(s.c_str());
						s = root->getElementFromId(902, true)->getText();
						scale.Y = (f32)atof(s.c_str());
						s = root->getElementFromId(903, true)->getText();
						scale.Z = (f32)atof(s.c_str());

						if (Model)
							Model->setScale(scale);
					}
					break;
				case 1102:
					env->addFileOpenDialog(L"Please select a model file to open");
					break;
				case 1103:
					showAboutText();
					break;
				case 1104:
					createToolBox();
					break;
				case 1105:
					env->addFileOpenDialog(L"Please select your game archive/directory");
					break;
				}

				break;
			}
		}

		return false;
	}
};


/*
Most of the hard work is done. We only need to create the Irrlicht Engine
device and all the buttons, menus and toolbars. We start up the engine as
usual, using createDevice(). To make our application catch events, we set our
eventreceiver as parameter. The #ifdef WIN32 preprocessor commands are not
necessary, but I included them to make the tutorial use DirectX on Windows and
OpenGL on all other platforms like Linux. As you can see, there is also a
unusual call to IrrlichtDevice::setResizeAble(). This makes the render window
resizeable, which is quite useful for a mesh viewer.
*/
int main(int argc, char* argv[])
{
	// ask user for driver

	video::E_DRIVER_TYPE driverType = video::EDT_DIRECT3D8;

	printf("Please select the driver you want for this example:\n"\
		" (a) Direct3D 9.0c\n (b) Direct3D 8.1\n (c) OpenGL 1.5\n"\
		" (d) Software Renderer\n (e) Burning's Software Renderer\n"\
		" (f) NullDevice\n (otherKey) exit\n\n");

	char key;
	std::cin >> key;

	switch(key)
	{
		case 'a': driverType = video::EDT_DIRECT3D9;break;
		case 'b': driverType = video::EDT_DIRECT3D8;break;
		case 'c': driverType = video::EDT_OPENGL;   break;
		case 'd': driverType = video::EDT_SOFTWARE; break;
		case 'e': driverType = video::EDT_BURNINGSVIDEO;break;
		case 'f': driverType = video::EDT_NULL;     break;
		default: return 1;
	}

	// create device and exit if creation failed

	MyEventReceiver receiver;
	Device = createDevice(driverType, core::dimension2d<s32>(800, 600),
		16, false, false, false, &receiver);

	if (Device == 0)
		return 1; // could not create selected driver.

	Device->setResizeAble(true);

	Device->setWindowCaption(L"Irrlicht Engine - Loading...");

	video::IVideoDriver* driver = Device->getVideoDriver();
	IGUIEnvironment* env = Device->getGUIEnvironment();
	scene::ISceneManager* smgr = Device->getSceneManager();
	smgr->getParameters()->setAttribute(scene::COLLADA_CREATE_SCENE_INSTANCES, true);

	driver->setTextureCreationFlag(video::ETCF_ALWAYS_32_BIT, true);

	smgr->addLightSceneNode();
	smgr->addLightSceneNode(0, core::vector3df(50,-50,100),
			video::SColorf(1.0f,1.0f,1.0f),20000);
	// add our media directory as "search path"
	Device->getFileSystem()->addFolderFileArchive ( "../../media/" );

	/*
	The next step is to read the configuration file. It is stored in the xml
	format and looks a little bit like this:

	@verbatim
	<?xml version="1.0"?>
	<config>
		<startUpModel file="some filename" />
		<messageText caption="Irrlicht Engine Mesh Viewer">
			Hello!
		</messageText>
	</config>
	@endverbatim

	We need the data stored in there to be written into the global variables
	StartUpModelFile, MessageText and Caption. This is now done using the
	Irrlicht Engine integrated XML parser:
	*/

	// read configuration from xml file

	io::IXMLReader* xml = Device->getFileSystem()->createXMLReader("config.xml");

	while(xml && xml->read())
	{
		switch(xml->getNodeType())
		{
		case io::EXN_TEXT:
			// in this xml file, the only text which occurs is the
			// messageText
			MessageText = xml->getNodeData();
			break;
		case io::EXN_ELEMENT:
			{
				if (core::stringw("startUpModel") == xml->getNodeName())
					StartUpModelFile = xml->getAttributeValue(L"file");
				else
				if (core::stringw("messageText") == xml->getNodeName())
					Caption = xml->getAttributeValue(L"caption");
			}
			break;
		}
	}

	if (xml)
		xml->drop(); // don't forget to delete the xml reader

	if (argc > 1)
		StartUpModelFile = argv[1];

	/*
	That wasn't difficult. Now we'll set a nicer font and create the Menu.
	It is possible to create submenus for every menu item. The call
	menu->addItem(L"File", -1, true, true); for example adds a new menu
	Item with the name "File" and the id -1. The following parameter says
	that the menu item should be enabled, and the last one says, that there
	should be a submenu. The submenu can now be accessed with
	menu->getSubMenu(0), because the "File" entry is the menu item with
	index 0.
	*/

	// set a nicer font

	IGUISkin* skin = env->getSkin();
	IGUIFont* font = env->getFont("fonthaettenschweiler.bmp");
	if (font)
		skin->setFont(font);

	// create menu
	gui::IGUIContextMenu* menu = env->addMenu();
	menu->addItem(L"File", -1, true, true);
	menu->addItem(L"View", -1, true, true);
	menu->addItem(L"Camera", -1, true, true);
	menu->addItem(L"Help", -1, true, true);

	gui::IGUIContextMenu* submenu;
	submenu = menu->getSubMenu(0);
	submenu->addItem(L"Open Model File & Texture...", 100);
	submenu->addItem(L"Set Model Archive...", 101);
	submenu->addSeparator();
	submenu->addItem(L"Quit", 200);

	submenu = menu->getSubMenu(1);
	submenu->addItem(L"toggle sky box visibility", 300);
	submenu->addItem(L"toggle model debug information", -1, true, true);
	submenu->addItem(L"model material", -1, true, true );

	submenu = submenu->getSubMenu(1);
	submenu->addItem(L"Off", 400);
	submenu->addItem(L"Bounding Box", 410);
	submenu->addItem(L"Normals", 420);
	submenu->addItem(L"Skeleton", 430);
	submenu->addItem(L"Wire overlay", 440);
	submenu->addItem(L"Half-Transparent", 450);
	submenu->addItem(L"Buffers bounding boxes", 460);
	submenu->addItem(L"All", 499);

	submenu = menu->getSubMenu(1)->getSubMenu(2);
	submenu->addItem(L"Solid", 610);
	submenu->addItem(L"Transparent", 620);
	submenu->addItem(L"Reflection", 630);

	submenu = menu->getSubMenu(2);
	submenu->addItem(L"Maya Style", 1000);
	submenu->addItem(L"First Person", 1100);

	submenu = menu->getSubMenu(3);
	submenu->addItem(L"About", 500);

	/*
	Below the menu we want a toolbar, onto which we can place colored
	buttons and important looking stuff like a senseless combobox.
	*/

	// create toolbar

	gui::IGUIToolBar* bar = env->addToolBar();

	video::ITexture* image = driver->getTexture("open.png");
	bar->addButton(1102, 0, L"Open a model",image, 0, false, true);

	image = driver->getTexture("tools.png");
	bar->addButton(1104, 0, L"Open Toolset",image, 0, false, true);

	image = driver->getTexture("zip.png");
	bar->addButton(1105, 0, L"Set Model Archive",image, 0, false, true);

	image = driver->getTexture("help.png");
	bar->addButton(1103, 0, L"Open Help", image, 0, false, true);

	// create a combobox with some senseless texts

	gui::IGUIComboBox* box = env->addComboBox(core::rect<s32>(250,4,350,23), bar, 108);
	box->addItem(L"No filtering");
	box->addItem(L"Bilinear");
	box->addItem(L"Trilinear");
	box->addItem(L"Anisotropic");
	box->addItem(L"Isotropic");

	/*
	To make the editor look a little bit better, we disable transparent gui
	elements, and add an Irrlicht Engine logo. In addition, a text showing
	the current frames per second value is created and the window caption is
	changed.
	*/

	// disable alpha

	for (s32 i=0; i<gui::EGDC_COUNT ; ++i)
	{
		video::SColor col = env->getSkin()->getColor((gui::EGUI_DEFAULT_COLOR)i);
		col.setAlpha(255);
		env->getSkin()->setColor((gui::EGUI_DEFAULT_COLOR)i, col);
	}

	// add a tabcontrol

	createToolBox();

	// create fps text

	IGUIStaticText* fpstext = env->addStaticText(L"",
			core::rect<s32>(400,4,570,23), true, false, bar);

	// set window caption

	Caption += " - [";
	Caption += driver->getName();
	Caption += "]";
	Device->setWindowCaption(Caption.c_str());

	/*
	That's nearly the whole application. We simply show the about message
	box at start up, and load the first model. To make everything look
	better, a skybox is created and a user controled camera, to make the
	application a little bit more interactive. Finally, everything is drawn
	in a standard drawing loop.
	*/

	// show about message box and load default model
	if (argc==1)
	showAboutText();
	loadModel(StartUpModelFile.c_str());

	// add skybox

	SkyBox = smgr->addSkyBoxSceneNode(
		driver->getTexture("irrlicht2_up.jpg"),
		driver->getTexture("irrlicht2_dn.jpg"),
		driver->getTexture("irrlicht2_lf.jpg"),
		driver->getTexture("irrlicht2_rt.jpg"),
		driver->getTexture("irrlicht2_ft.jpg"),
		driver->getTexture("irrlicht2_bk.jpg"));

	// add a camera scene node
	Camera[0] = smgr->addCameraSceneNodeMaya();
	Camera[0]->setFarValue(20000.f);
	Camera[1] = smgr->addCameraSceneNodeFPS();
	Camera[1]->setFarValue(20000.f);

	setActiveCamera ( Camera[0] );

	// load the irrlicht engine logo
	IGUIImage *img =
		env->addImage(driver->getTexture("irrlichtlogo2.png"),
			core::position2d<s32>(10, driver->getScreenSize().Height - 128));

	// lock the logo's edges to the bottom left corner of the screen
	img->setAlignment(EGUIA_UPPERLEFT, EGUIA_UPPERLEFT,
			EGUIA_LOWERRIGHT, EGUIA_LOWERRIGHT);

	// draw everything

	while(Device->run() && driver)
	{
		if (Device->isWindowActive())
		{
			driver->beginScene(true, true, video::SColor(150,50,50,50));

			smgr->drawAll();
			env->drawAll();

			driver->endScene();

			core::stringw str(L"FPS: ");
			str.append(core::stringw(driver->getFPS()));
			str += L" Tris: ";
			str.append(core::stringw(driver->getPrimitiveCountDrawn()));
			fpstext->setText(str.c_str());
		}
		else
			Device->yield();
	}

	Device->drop();
	return 0;
}

/*
**/
