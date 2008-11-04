/** Example 005 User Interface

This tutorial shows how to use the built in User Interface of
the Irrlicht Engine. It will give a brief overview and show
how to create and use windows, buttons, scroll bars, static
texts, and list boxes.

As always, we include the header files, and use the irrlicht
namespaces. We also store a pointer to the Irrlicht device,
a counter variable for changing the creation position of a window,
and a pointer to a listbox.
*/
#include <irrlicht.h>
#include <iostream>

using namespace irr;

using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

#ifdef _IRR_WINDOWS_
#pragma comment(lib, "Irrlicht.lib")
#endif


IrrlichtDevice *device = 0;
s32 cnt = 0;
IGUIListBox* listbox = 0;


/*
The Event Receiver is not only capable of getting keyboard and
mouse input events, but also events of the graphical user interface
(gui). There are events for almost everything: Button click,
Listbox selection change, events that say that a element was hovered
and so on. To be able to react to some of these events, we create
an event receiver.
We only react to gui events, and if it's such an event, we get the
id of the caller (the gui element which caused the event) and get
the pointer to the gui environment.
*/
class MyEventReceiver : public IEventReceiver
{
public:
	virtual bool OnEvent(const SEvent& event)
	{
		if (event.EventType == EET_GUI_EVENT)
		{
			s32 id = event.GUIEvent.Caller->getID();
			IGUIEnvironment* env = device->getGUIEnvironment();

			switch(event.GUIEvent.EventType)
			{

			/*
			If a scrollbar changed its scroll position, and it is
			'our' scrollbar (the one with id 104), then we change
			the transparency of all gui elements. This is a very
			easy task: There is a skin object, in which all color
			settings are stored. We simply go through all colors
			stored in the skin and change their alpha value.
			*/
			case EGET_SCROLL_BAR_CHANGED:
				if (id == 104)
				{
					s32 pos = ((IGUIScrollBar*)event.GUIEvent.Caller)->getPos();
					
					for (u32 i=0; i<EGDC_COUNT ; ++i)
					{
						SColor col = env->getSkin()->getColor((EGUI_DEFAULT_COLOR)i);
						col.setAlpha(pos);
						env->getSkin()->setColor((EGUI_DEFAULT_COLOR)i, col);
					}
					
				}
				break;

			/*
			If a button was clicked, it could be one of 'our'
			three buttons. If it is the first, we shut down the engine.
			If it is the second, we create a little window with some
			text on it. We also add a string to the list box to log
			what happened. And if it is the third button, we create
			a file open dialog, and add also this as string to the list box.
			That's all for the event receiver.
			*/
			case EGET_BUTTON_CLICKED:

				if (id == 101)
				{
					device->closeDevice();
					return true;
				}

				if (id == 102)
				{
					listbox->addItem(L"Window created");
					cnt += 30;
					if (cnt > 200)
						cnt = 0;

					IGUIWindow* window = env->addWindow(
						rect<s32>(100 + cnt, 100 + cnt, 300 + cnt, 200 + cnt),
						false, // modal?
						L"Test window");

					env->addStaticText(L"Please close me",
						rect<s32>(35,35,140,50),
						true, // border?
						false, // wordwrap?
						window);

					return true;
				}

				if (id == 103)
				{
					listbox->addItem(L"File open");
					env->addFileOpenDialog(L"Please choose a file.");
					return true;
				}

				break;
			default:
				break;
			}
		}

		return false;
	}
};


/*
Ok, now for the more interesting part. First, create the Irrlicht device. As in
some examples before, we ask the user which driver he wants to use for this
example:
*/
int main()
{
	// ask user for driver
	
	video::E_DRIVER_TYPE driverType;

	printf("Please select the driver you want for this example:\n"\
		" (a) Direct3D 9.0c\n (b) Direct3D 8.1\n (c) OpenGL 1.5\n"\
		" (d) Software Renderer\n (e) Burning's Software Renderer\n"\
		" (f) NullDevice\n (otherKey) exit\n\n");

	char i;
	std::cin >> i;

	switch(i)
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

	device = createDevice(driverType, core::dimension2d<s32>(640, 480));

	if (device == 0)
		return 1; // could not create selected driver.

	/* The creation was successful, now we set the event receiver and
		store pointers to the driver and to the gui environment. */

	MyEventReceiver receiver;
	device->setEventReceiver(&receiver);
	device->setWindowCaption(L"Irrlicht Engine - User Interface Demo");

	video::IVideoDriver* driver = device->getVideoDriver();
	IGUIEnvironment* env = device->getGUIEnvironment();

	/*
	To make the font a little bit nicer, we load an external font
	and set it as the new default font in the skin.
	To keep the standard font for tool tip text, we set it to
	the built-in font.
	*/

	IGUISkin* skin = env->getSkin();
	IGUIFont* font = env->getFont("../../media/fonthaettenschweiler.bmp");
	if (font)
		skin->setFont(font);

	skin->setFont(env->getBuiltInFont(), EGDF_TOOLTIP);

	/*
	We add three buttons. The first one closes the engine. The second
	creates a window and the third opens a file open dialog. The third
	parameter is the id of the button, with which we can easily identify
	the button in the event receiver.
	*/	

	env->addButton(rect<s32>(10,240,110,240 + 32), 0, 101,
			L"Quit", L"Exits Program");
	env->addButton(rect<s32>(10,280,110,280 + 32), 0, 102,
			L"New Window", L"Launches a new Window");
	env->addButton(rect<s32>(10,320,110,320 + 32), 0, 103,
			L"File Open", L"Opens a file");

	/*
	Now, we add a static text and a scrollbar, which modifies the
	transparency of all gui elements. We set the maximum value of
	the scrollbar to 255, because that's the maximal value for
	a color value.
	Then we create an other static text and a list box.
	*/

	env->addStaticText(L"Transparent Control:", rect<s32>(150,20,350,40), true);
	IGUIScrollBar* scrollbar = env->addScrollBar(true,
			rect<s32>(150, 45, 350, 60), 0, 104);
	scrollbar->setMax(255);

	// set scrollbar position to alpha value of an arbitrary element
	scrollbar->setPos(env->getSkin()->getColor(EGDC_WINDOW).getAlpha());

	env->addStaticText(L"Logging ListBox:", rect<s32>(50,110,250,130), true);
	listbox = env->addListBox(rect<s32>(50, 140, 250, 210));
	env->addEditBox(L"Editable Text", rect<s32>(350, 80, 550, 100));

	/*
	And at last, we create a nice Irrlicht Engine logo in the top left corner. 
	*/
	env->addImage(driver->getTexture("../../media/irrlichtlogo2.png"),
			position2d<int>(10,10));


	/*
	That's all, we only have to draw everything.
	*/

	while(device->run() && driver)
	if (device->isWindowActive())
	{
		driver->beginScene(true, true, SColor(0,200,200,200));

		env->drawAll();
	
		driver->endScene();
	}

	device->drop();

	return 0;
}

/*
**/
