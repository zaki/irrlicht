/** Example 024 Cursor Control

Show how to modify cursors and offer some useful tool-functions for creating cursors.
It can also be used for experiments with the mouse in general.
*/

#include <irrlicht.h>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

#ifdef _IRR_WINDOWS_
#pragma comment(lib, "Irrlicht.lib")
#endif

const int DELAY_TIME = 3000;

enum ETimerAction
{
	ETA_MOUSE_VISIBLE,
	ETA_MOUSE_INVISIBLE,
};

struct TimerAction
{
	u32 TargetTime;
	ETimerAction Action;
};

struct SAppContext
{
	SAppContext()
	: Device(0), InfoStatic(0), EventBox(0), CursorBox(0), SpriteBox(0)
	, ButtonSetVisible(0), ButtonSetInvisible(0), ButtonSimulateBadFps(0)
	, ButtonChangeIcon(0)
	, SimulateBadFps(false)
	{
	}

	void update()
	{
		if (!Device)
			return;
		u32 timeNow = Device->getTimer()->getTime();
		for ( irr::u32 i=0; i < TimerActions.size(); ++i )
		{
			if ( timeNow >= TimerActions[i].TargetTime )
			{
				runTimerAction(TimerActions[i]);
				TimerActions.erase(i);
			}
			else
			{
				++i;
			}
		}
	}

	void runTimerAction(const TimerAction& action)
	{
		if (ETA_MOUSE_VISIBLE == action.Action)
		{
			Device->getCursorControl()->setVisible(true);
			ButtonSetVisible->setEnabled(true);
		}
		else if ( ETA_MOUSE_INVISIBLE == action.Action)
		{
			Device->getCursorControl()->setVisible(false);
			ButtonSetInvisible->setEnabled(true);
		}
	}

	IrrlichtDevice * Device;
	irr::gui::IGUIStaticText * InfoStatic;
	irr::gui::IGUIListBox * EventBox;
	irr::gui::IGUIListBox * CursorBox;
	irr::gui::IGUIListBox * SpriteBox;
	irr::gui::IGUIButton * ButtonSetVisible;
	irr::gui::IGUIButton * ButtonSetInvisible;
	irr::gui::IGUIButton * ButtonSimulateBadFps;
	irr::gui::IGUIButton * ButtonChangeIcon;
	irr::core::array<TimerAction> TimerActions;
	bool SimulateBadFps;
	irr::core::array<SCursorSprite> Sprites;
};

void PrintMouseEventName(const SEvent& event, irr::core::stringw &result)
{
	switch ( event.MouseInput.Event )
	{
		case EMIE_LMOUSE_PRESSED_DOWN: 	result += stringw(L"EMIE_LMOUSE_PRESSED_DOWN"); break;
		case EMIE_RMOUSE_PRESSED_DOWN: 	result += stringw(L"EMIE_RMOUSE_PRESSED_DOWN"); break;
		case EMIE_MMOUSE_PRESSED_DOWN: 	result += stringw(L"EMIE_MMOUSE_PRESSED_DOWN"); break;
		case EMIE_LMOUSE_LEFT_UP: 		result += stringw(L"EMIE_LMOUSE_LEFT_UP"); break;
		case EMIE_RMOUSE_LEFT_UP: 		result += stringw(L"EMIE_RMOUSE_LEFT_UP"); break;
		case EMIE_MMOUSE_LEFT_UP: 		result += stringw(L"EMIE_MMOUSE_LEFT_UP"); break;
		case EMIE_MOUSE_MOVED: 			result += stringw(L"EMIE_MOUSE_MOVED"); break;
		case EMIE_MOUSE_WHEEL: 			result += stringw(L"EMIE_MOUSE_WHEEL"); break;
		case EMIE_LMOUSE_DOUBLE_CLICK: 	result += stringw(L"EMIE_LMOUSE_DOUBLE_CLICK"); break;
		case EMIE_RMOUSE_DOUBLE_CLICK: 	result += stringw(L"EMIE_RMOUSE_DOUBLE_CLICK"); break;
		case EMIE_MMOUSE_DOUBLE_CLICK: 	result += stringw(L"EMIE_MMOUSE_DOUBLE_CLICK"); break;
		case EMIE_LMOUSE_TRIPLE_CLICK: 	result += stringw(L"EMIE_LMOUSE_TRIPLE_CLICK"); break;
		case EMIE_RMOUSE_TRIPLE_CLICK: 	result += stringw(L"EMIE_RMOUSE_TRIPLE_CLICK"); break;
		case EMIE_MMOUSE_TRIPLE_CLICK: 	result += stringw(L"EMIE_MMOUSE_TRIPLE_CLICK"); break;
		default:
		break;
	}
}

void PrintMouseState(const SEvent& event, irr::core::stringw &result)
{
	result += stringw(L"X: ");
	result += stringw(event.MouseInput.X);
	result += stringw(L"\n");

	result += stringw(L"Y: ");
	result += stringw(event.MouseInput.Y);
	result += stringw(L"\n");


	result += stringw(L"Wheel: ");
	result += stringw(event.MouseInput.Wheel);
	result += stringw(L"\n");

	result += stringw(L"Shift: ");
	if ( event.MouseInput.Shift )
		result += stringw(L"true\n");
	else
		result += stringw(L"false\n");

	result += stringw(L"Control: ");
	if ( event.MouseInput.Control )
		result += stringw(L"true\n");
	else
		result += stringw(L"false\n");

	result += stringw(L"ButtonStates: ");
	result += stringw(event.MouseInput.ButtonStates);
	result += stringw(L"\n");

	result += stringw(L"isLeftPressed: ");
	if ( event.MouseInput.isLeftPressed() )
		result += stringw(L"true\n");
	else
		result += stringw(L"false\n");

	result += stringw(L"isRightPressed: ");
	if ( event.MouseInput.isRightPressed() )
		result += stringw(L"true\n");
	else
		result += stringw(L"false\n");

	result += stringw(L"isMiddlePressed: ");
	if ( event.MouseInput.isMiddlePressed() )
		result += stringw(L"true\n");
	else
		result += stringw(L"false\n");

	result += stringw(L"Event: ");

	PrintMouseEventName(event, result);

	result += stringw(L"\n");
}

class MyEventReceiver : public IEventReceiver
{
public:
	MyEventReceiver(SAppContext & context) : Context(context) { }

	virtual bool OnEvent(const SEvent& event)
	{
		if (event.EventType == EET_GUI_EVENT )
		{
			switch ( event.GUIEvent.EventType )
			{
				case EGET_BUTTON_CLICKED:
				{
					u32 timeNow = Context.Device->getTimer()->getTime();
					TimerAction action;
					action.TargetTime = timeNow + DELAY_TIME;
					if ( event.GUIEvent.Caller == Context.ButtonSetVisible )
					{
						action.Action = ETA_MOUSE_VISIBLE;
						Context.TimerActions.push_back(action);
						Context.ButtonSetVisible->setEnabled(false);
					}
					else if ( event.GUIEvent.Caller == Context.ButtonSetInvisible )
					{
						action.Action = ETA_MOUSE_INVISIBLE;
						Context.TimerActions.push_back(action);
						Context.ButtonSetInvisible->setEnabled(false);
					}
					else if ( event.GUIEvent.Caller == Context.ButtonSimulateBadFps )
					{
						Context.SimulateBadFps = Context.ButtonSimulateBadFps->isPressed();
					}
					else if ( event.GUIEvent.Caller == Context.ButtonChangeIcon )
					{
						irr::s32 selectedCursor = Context.CursorBox->getSelected();
						irr::s32 selectedSprite = Context.SpriteBox->getSelected();
						if ( selectedCursor >= 0 && selectedSprite >= 0 )
						{
							Context.Device->getCursorControl()->changeIcon((ECURSOR_ICON)selectedCursor, Context.Sprites[selectedSprite] );
							Context.Device->getCursorControl()->setActiveIcon( ECURSOR_ICON(selectedCursor) );
						}
					}
				}
				break;
				case EGET_LISTBOX_CHANGED:
				case EGET_LISTBOX_SELECTED_AGAIN:
				{
					if ( event.GUIEvent.Caller == Context.CursorBox )
					{
						irr::s32 selected = Context.CursorBox->getSelected();
						if ( selected >= 0 )
							Context.Device->getCursorControl()->setActiveIcon( ECURSOR_ICON(selected) );
					}
				}
				break;
				default:
				break;
			}
		}

		if (event.EventType == EET_MOUSE_INPUT_EVENT)
		{
			irr::core::stringw infoText;
			PrintMouseState(event, infoText);
			Context.InfoStatic->setText(infoText.c_str());
			if ( event.MouseInput.Event != EMIE_MOUSE_MOVED && event.MouseInput.Event != EMIE_MOUSE_WHEEL ) // no spam
			{
				infoText = L"";
				PrintMouseEventName(event, infoText);
				Context.EventBox->insertItem(0, infoText.c_str(), -1);
			}
		}

		return false;
	}

private:
	SAppContext & Context;
};

// Use several imagefiles as animation frames for a sprite which can be used as cursor icon.
// The images in those files all need to have the same size.
// Return sprite index on success or -1 on failure
irr::s32 AddAnimatedIcon( irr::gui::IGUISpriteBank * spriteBank, video::IVideoDriver* driver,  const irr::core::array< irr::io::path >& files, u32 frameTime )
{
	if ( !spriteBank || !driver || !files.size() )
		return -1;

	irr::video::ITexture * tex = driver->getTexture( files[0] );
	if ( tex )
	{
		core::array< core::rect<s32> >& spritePositions = spriteBank->getPositions();
		u32 idxRect = spritePositions.size();
		spritePositions.push_back( core::rect<s32>(0,0, tex->getSize().Width, tex->getSize().Height) );

		SGUISprite sprite;
		sprite.frameTime = frameTime;

		core::array< SGUISprite >& sprites = spriteBank->getSprites();
		u32 startIdx = spriteBank->getTextureCount();
		for ( u32 f=0; f < files.size(); ++f )
		{
			tex = driver->getTexture( files[f] );
			if ( tex )
			{
				spriteBank->addTexture( driver->getTexture(files[f]) );
				irr::gui::SGUISpriteFrame frame;
				frame.rectNumber = idxRect;
				frame.textureNumber = startIdx+f;
				sprite.Frames.push_back( frame );
			}
		}

		sprites.push_back( sprite );
		return sprites.size()-1;
	}

	return -1;
}

// Use several images within one imagefile as animation frames for a sprite which can be used as cursor icon
// The sizes of the icons within that file all need to have the same size
// Return sprite index on success or -1 on failure
irr::s32 AddAnimatedIcon( irr::gui::IGUISpriteBank * spriteBank, video::IVideoDriver* driver,  const irr::io::path& file, const irr::core::array< irr::core::rect<s32> >& rects, u32 frameTime )
{
	if ( !spriteBank || !driver || !rects.size() )
		return -1;

	irr::video::ITexture * tex = driver->getTexture( file );
	if ( tex )
	{
		core::array< core::rect<s32> >& spritePositions = spriteBank->getPositions();
		u32 idxRect = spritePositions.size();
		u32 idxTex = spriteBank->getTextureCount();
		spriteBank->addTexture( tex );

		SGUISprite sprite;
		sprite.frameTime = frameTime;

		core::array< SGUISprite >& sprites = spriteBank->getSprites();
		for ( u32 i=0; i < rects.size(); ++i )
		{
			spritePositions.push_back( rects[i] );

			irr::gui::SGUISpriteFrame frame;
			frame.rectNumber = idxRect+i;
			frame.textureNumber = idxTex;
			sprite.Frames.push_back( frame );
		}

		sprites.push_back( sprite );
		return sprites.size()-1;
	}

	return -1;
}


int main()
{
	video::E_DRIVER_TYPE driverType = video::EDT_OPENGL;
	IrrlichtDevice * device = createDevice(driverType, core::dimension2d<u32>(640, 480));
	if (device == 0)
		return 1; // could not create selected driver.

	device->setWindowCaption(L"Cursor control - Irrlicht engine tutorial");
	video::IVideoDriver* driver = device->getVideoDriver();
	IGUIEnvironment* env = device->getGUIEnvironment();

	irr::gui::IGUISpriteBank * SpriteBankIcons;

	SAppContext context;
	context.Device = device;

	core::rect< s32 > rectInfoStatic(10,10, 200, 200);
	env->addStaticText (L"Cursor state information", rectInfoStatic, true, true);
	rectInfoStatic.UpperLeftCorner += core::dimension2di(0, 15);
	context.InfoStatic = env->addStaticText (L"", rectInfoStatic, true, true);
	core::rect< s32 > rectEventBox(10,210, 200, 400);
	env->addStaticText (L"click events (new on top)", rectEventBox, true, true);
	rectEventBox.UpperLeftCorner += core::dimension2di(0, 15);
	context.EventBox = env->addListBox(rectEventBox);
	core::rect< s32 > rectCursorBox(210,10, 400, 250);
	env->addStaticText (L"cursors, click to set the active one", rectCursorBox, true, true);
	rectCursorBox.UpperLeftCorner += core::dimension2di(0, 15);
	context.CursorBox = env->addListBox(rectCursorBox);
	core::rect< s32 > rectSpriteBox(210,260, 400, 400);
	env->addStaticText (L"sprites", rectSpriteBox, true, true);
	rectSpriteBox.UpperLeftCorner += core::dimension2di(0, 15);
	context.SpriteBox = env->addListBox(rectSpriteBox);

	context.ButtonSetVisible = env->addButton( rect<s32>( 410, 20, 560, 40 ), 0, -1, L"set visible (delayed)" );
	context.ButtonSetInvisible = env->addButton( rect<s32>( 410, 50, 560, 70 ), 0, -1, L"set invisible (delayed)" );
	context.ButtonSimulateBadFps = env->addButton( rect<s32>( 410, 80, 560, 100 ), 0, -1, L"simulate bad FPS" );
	context.ButtonSimulateBadFps->setIsPushButton(true);
	context.ButtonChangeIcon = env->addButton( rect<s32>( 410, 140, 560, 160 ), 0, -1, L"change cursor icon\n(cursor+sprite must be selected)" );

	// create sprites for cursor icons
	SpriteBankIcons = env->addEmptySpriteBank(io::path("cursor_icons"));
	context.SpriteBox->setSpriteBank(SpriteBankIcons);

	// create an animated icon from several files
	irr::core::array< irr::io::path > files;
	files.push_back( irr::io::path("../../media/icon_crosshairs16x16bw1.png") );
	files.push_back( irr::io::path("../../media/icon_crosshairs16x16bw2.png") );
	files.push_back( irr::io::path("../../media/icon_crosshairs16x16bw3.png") );
	files.push_back( irr::io::path("../../media/icon_crosshairs16x16bw3.png") );
	files.push_back( irr::io::path("../../media/icon_crosshairs16x16bw2.png") );
	irr::s32 SpriteIdxBw = AddAnimatedIcon( SpriteBankIcons, driver, files, 200 );
	SCursorSprite spriteBw( SpriteBankIcons, SpriteIdxBw, core::position2d<s32>(7,7) );
	context.SpriteBox->addItem(L"crosshair_bw", SpriteIdxBw);
	context.Sprites.push_back(spriteBw);

	// create an animated icon from one file
	irr::core::array< irr::core::rect<s32> > iconRects;
	iconRects.push_back( irr::core::rect<s32>(0,0, 16, 16) );
	iconRects.push_back( irr::core::rect<s32>(16,0, 32, 16) );
	iconRects.push_back( irr::core::rect<s32>(0,16, 16, 32) );
	iconRects.push_back( irr::core::rect<s32>(0,16, 16, 32) );
	iconRects.push_back( irr::core::rect<s32>(16,0, 32, 16) );
	irr::s32 SpriteIdxCol = AddAnimatedIcon( SpriteBankIcons, driver, irr::io::path("../../media/icon_crosshairs16x16col.png"), iconRects, 200 );
	SCursorSprite spriteCol( SpriteBankIcons, SpriteIdxCol, core::position2d<s32>(7,7) );
	context.SpriteBox->addItem(L"up_icon_colored", SpriteIdxCol);
	context.Sprites.push_back(spriteCol);

	for ( int i=0; i < (int)gui::ECI_COUNT; ++i )
	{
		context.CursorBox->addItem(core::stringw( GUICursorIconNames[i] ).c_str());
	}

	context.Device->getCursorControl()->addIcon(spriteBw);
	context.CursorBox->addItem(L"black-white");

	context.Device->getCursorControl()->addIcon(spriteCol);
	context.CursorBox->addItem(L"color");

	MyEventReceiver receiver(context);
	device->setEventReceiver(&receiver);


	while(device->run() && driver)
	{
		// if (device->isWindowActive())
		{
			u32 realTimeNow = device->getTimer()->getRealTime();

			context.update();

			driver->beginScene(true, true, SColor(0,200,200,200));

			env->drawAll();


			// draw custom sprite with normal functions for comparison and to show the difference also for the hardware cursor.
			if ( SpriteIdxCol >= 0 )
				SpriteBankIcons->draw2DSprite(u32(SpriteIdxCol), core::position2di(30, 140), 0, video::SColor(255, 255, 255, 255), 0, realTimeNow);
			if ( SpriteIdxBw >= 0 )
				SpriteBankIcons->draw2DSprite(u32(SpriteIdxBw), core::position2di(80, 140), 0, video::SColor(255, 255, 255, 255), 0, realTimeNow);


			driver->endScene();
		}

		// By simulating bad fps we can find out if hardware-support for cusors works or not. If it works the cursor will move as usual,while it otherwise will just update with 2 fps now.
		if ( context.SimulateBadFps )
		{
			device->sleep(500);	// 2 fps
		}
		else
		{
			device->sleep(10);
		}
	}

	device->drop();

	return 0;
}

/*
**/
