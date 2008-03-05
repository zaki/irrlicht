/*
This tutorial shows how to use the built in User Interface of
the Irrlicht Engine. It will give a brief overview and show
how to create and use windows, buttons, scroll bars, static 
texts and list boxes.

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

gui::IGUIButton *buttonMain;
gui::IGUIWindow *windowMain;
bool isWindowMinimized = false;

class : public IEventReceiver
{
   virtual bool OnEvent(const SEvent& event)
   {
      static s32 windowHeight;

      if (event.EventType==EET_GUI_EVENT)
      {
         if (event.GUIEvent.EventType==gui::EGET_BUTTON_CLICKED)
         {
            if (event.GUIEvent.Caller==buttonMain)
            {
               isWindowMinimized = !isWindowMinimized;

               rect<s32> pos = windowMain->getRelativePosition();
               if (isWindowMinimized)
               {
                  windowHeight = pos.LowerRightCorner.Y - pos.UpperLeftCorner.Y;
                  pos.LowerRightCorner.Y = pos.UpperLeftCorner.Y + 22;
               } else {
                  pos.LowerRightCorner.Y = pos.UpperLeftCorner.Y + windowHeight;
               }

               windowMain->setRelativePosition(pos);

               return true;
            }
         }
      }

      return false;
   }
} eventReceiver;

int main()
{
   // irr device and pointers
   IrrlichtDevice *irrDevice = createDevice(video::EDT_DIRECT3D9);
   video::IVideoDriver *irrVideo = irrDevice->getVideoDriver();
   gui::IGUIEnvironment *irrGUI = irrDevice->getGUIEnvironment();

   irrDevice->setEventReceiver(&eventReceiver);

   // gui mass
   
   buttonMain = irrGUI->addButton(rect<s32>(4,4,100,40), 0, -1, L"CLICK ME");
   windowMain = irrGUI->addWindow(rect<s32>(40,40,400,400), false, L"Main Window");

   irrGUI->addButton(rect<s32>(4,24,100,40), windowMain, -1, L"button");
   irrGUI->addCheckBox(true, rect<s32>(4,40,100,60), windowMain);
   irrGUI->addComboBox(rect<s32>(4,60,100,80), windowMain)->addItem(L"item");
   irrGUI->addEditBox(L"test", rect<s32>(4,80,100,100), true, windowMain);
   irrGUI->addImage(rect<s32>(4,100,100,120), windowMain);
   irrGUI->addListBox(rect<s32>(4,120,100,160), windowMain)->addItem(L"item");
   gui::IGUIContextMenu *m = irrGUI->addMenu(windowMain);
   m->setMinSize(dimension2di(100,40));
   m->addItem(L"menuitem1");
   m->addItem(L"menuitem2");
   irrGUI->addMeshViewer(rect<s32>(4,160,100,180), windowMain);
   irrGUI->addScrollBar(true, rect<s32>(4,180,100,200), windowMain);
   irrGUI->addSpinBox(L"spinbox", rect<s32>(4,200,100,220), windowMain);
   irrGUI->addStaticText(L"statictext", rect<s32>(4,220,100,240), false, true, windowMain);
   irrGUI->addTabControl(rect<s32>(120,24,220,64), windowMain);
//   irrGUI->addTable(rect<s32>(120,80,220,120), windowMain)->addColumn(L"column1");
   gui::IGUIToolBar *t = irrGUI->addToolBar(windowMain);
   t->setMinSize(dimension2di(100,80));
   t->addButton(-1, L"toolbarButton1");
   t->addButton(-1, L"toolbarButton2");
   irrGUI->addWindow(rect<s32>(120,150,250,300), false, L"testWindow", windowMain);

   // show time!
   while(irrDevice->run())
   if (irrDevice->isWindowActive())
   {
      irrVideo->beginScene(true, true, video::SColor(0x204060));
      irrGUI->drawAll();
      irrVideo->endScene();
   }

   // drop time
   irrDevice->drop();

   return 0;
}