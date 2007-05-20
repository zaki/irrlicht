/* This tutorial shows how to use the built in User Interface
of the Irrlicht Engine. It will give a brief overview and show
how to create and use windows, buttons, scroll bars, static texts
and list boxes.
Lets start like the HelloWorld example: We include
the irrlicht header files and an additional file to be able
to ask the user for a driver type using the console.
*/
using System;
using System.Text;
using System.IO;
using Irrlicht;
using Irrlicht.Video;
using Irrlicht.GUI;
using Irrlicht.Core;
using Irrlicht.Scene;
namespace _05.UserInterface
{
	class Program : IEventReceiver
	{
		// path to standard Irrlicht media folder
		//string path = "../../../../media/";
		// empty path for executable folder:
		string path = string.Empty;
		IrrlichtDevice device;
		int cnt = 0;
		IGUIListBox listbox;
		/// <summary>
		/// Main entry point for the program.
		/// </summary>
		/// <param name="args">Arguments to pass the software.</param>
		[STAThread]
		static void Main(string[] args)
		{
			Program prog = new Program();
			prog.run();
		}
		/*The Event Receiver is not only capable of getting keyboard and mouse input events,
		  but also events of the graphical user interface (gui). There are events for almost
		  everything: Button click, Listbox selection change, events that say that a element
		  was hovered and so on. To be able to react to some of these events, we create an
		  event receiver. We only react to gui events, and if it's such an event, we get the
		  id of the caller (the gui element which caused the event) and get the pointer to the
		  gui environment.*/
		public bool OnEvent(Event p_e)
		{
			if (p_e.Type == EventType.GUIEvent) 
			{
				int id = p_e.GUIEventCaller.ID;
				IGUIEnvironment env = device.GUIEnvironment;
				switch (p_e.GUIEventType) 
				{
					case (GUIEvent.SCROLL_BAR_CHANGED):
						if (id == 104) 
						{
							// I think this is still not implemented
							//int pos=p_e.GUIEventCaller.Position;
						}
						break;
						/*If a button was clicked, it could be one of 'our' three buttons.
						  If it is the first, we shut down the engine. If it is the second,
						  we create a little window with some text on it. We also add a string
						  to the list box to log what happened. And if it is the third button,
						  we create a file open dialog, and add also this as string to the list
						  box. That's all for the event receiver.*/
					case (GUIEvent.BUTTON_CLICKED):
						if (id == 101) 
						{
							device.CloseDevice();
							return true;
						}
						if (id == 102) 
						{
							listbox.AddItem("Window Created");
							cnt += 30;
							if (cnt > 200)
								cnt = 0;
							IGUIElement window = env.AddWindow(
								new Rect(100 + cnt, 100 + cnt, 300 + cnt, 200 + cnt),
								false,   //modal?
								"Test Window",
								null,   //parent
								0);      // id
							env.AddStaticText("Please close me",
								new Rect(35, 35, 140, 50),
								true,   //border
								false,   //wordwrap
								window,   //parent
								0);      //id
							return true;
						}
						if (id == 103) 
						{
							listbox.AddItem("File open");
							env.AddFileOpenDialog("Please choose a file", false, null, 0);
							return true;
						}
						break;
				}
			}
			return false;
		}
		public void run()
		{
			// ask user for driver
			DriverType driverType;
			// Ask user to select driver:
			StringBuilder sb = new StringBuilder();
			sb.Append("Please select the driver you want for this example:\n");
			sb.Append("\n(a) Direct3D 9.0c\n(b) Direct3D 8.1\n(c) OpenGL 1.5");
			sb.Append("\n(d) Software Renderer\n(e) Apfelbaum Software Renderer");
			sb.Append("\n(f) Null Device\n(otherKey) exit\n\n");
			// Get the user's input:
			TextReader tIn = Console.In;
			TextWriter tOut = Console.Out;
			tOut.Write(sb.ToString());
			string input = tIn.ReadLine();
			// Select device based on user's input:
			switch (input) 
			{
				case "a":
					driverType = DriverType.DIRECT3D9;
					break;
				case "b":
					driverType = DriverType.DIRECT3D8;
					break;
				case "c":
					driverType = DriverType.OPENGL;
					break;
				case "d":
					driverType = DriverType.SOFTWARE;
					break;
				case "e":
					driverType = DriverType.SOFTWARE2;
					break;
				case "f":
					driverType = DriverType.NULL_DRIVER;
					break;
				default:
					return;
			}
			// Create device and exit if creation fails:
			device = new IrrlichtDevice(driverType, new Dimension2D(1024, 768), 32, false, true, true);
			if (device == null) 
			{
				tOut.Write("Device creation failed.");
				return;
			}
			/* set this as event receiver*/
			device.EventReceiver = this;
			/*
			Get a pointer to the video driver and the SceneManager so that
			we do not always have to write device.VideoDriver and
			device.SceneManager and device.GUIEnvironment.
			*/
			ISceneManager smgr = device.SceneManager;
			IVideoDriver driver = device.VideoDriver;
			IGUIEnvironment env = device.GUIEnvironment;
			/*We add three buttons. The first one closes the engine. The second creates
			  a window and the third opens a file open dialog. The third parameter is
			  the id of the button, with which we can easily identify the button in the
			  event receiver.*/
			env.AddButton(new Rect(10, 210, 100, 240), null, 101, "Quit");
			env.AddButton(new Rect(10, 250, 100, 290), null, 102, "New Window");
			env.AddButton(new Rect(10, 300, 100, 340), null, 103, "File Open");
			/*Now, we add a static text and a scrollbar, which modifies the transparency
			  of all gui elements. We set the maximum value of the scrollbar to 255,
			  because that's the maximal value for a color value. Then we create an other
			  static text and a list box.*/
			env.AddStaticText("Transparent Control:", new Rect(150, 20, 350, 40), true, false, null, 0);
			IGUIElement scrollbar = env.AddScrollBar(true, new Rect(150, 45, 350, 60), null, 104);
			//nopt implemented yet
			//scrollbar.Max=255;
			env.AddStaticText("Logging Listbox:", new Rect(50, 80, 250, 100), true, false, null, 0);
			listbox = env.AddListBox(new Rect(50, 110, 250, 180), null, 0, true);
			/*To make the font a little bit nicer, we load an external font and set it as
			  new font in the skin. An at last, we create a nice Irrlicht Engine logo in the
			  top left corner. */
			IGUISkin skin = env.Skin;
			IGUIFont font = env.GetFont(path + "fonthaettenschweiler.bmp");
			if (font != null) 
			{
				skin.Font = font;
			}
			IGUIElement img = env.AddImage(driver.GetTexture(path + "irrlichtlogoalpha.tga"),
				new Position2D(10, 10),
				false,   //UseAlphaChannel
				null,   //Parent
				0,      //ID
				"");   //Text
			/*
			We have done everything, so lets draw it.
			*/
			while (device.Run()) 
			{
				if (device.WindowActive) 
				{
					device.VideoDriver.BeginScene(true, true, new Color(0, 122, 65, 171));
					device.SceneManager.DrawAll();
					device.GUIEnvironment.DrawAll();
					device.VideoDriver.EndScene();
				}
			}
			/*
			In the end, delete the Irrlicht device.
			*/
			// Instead of device->drop, we'll use:
			GC.Collect();
		}
	}
}

// Retrieved from "http://www.irrforge.org/index.php/CS_Tutorial_5"
// This page has been accessed 221 times. This page was last modified 06:10, 20 Jan 2006. 

