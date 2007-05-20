/* This Tutorial shows how to do 2d graphics with the Irrlicht
Engine. It shows how to draw images, keycolor based sprites,
transparent rectangles and different fonts. You will may
consider this useful if you want to make a 2d game with the
engine, or if you want to draw a cool interface or head up
display for your 3d game. */
using System;
using System.Text;
using System.IO;
using Irrlicht;
using Irrlicht.Video;
using Irrlicht.GUI;
using Irrlicht.Core;
using Irrlicht.Scene;
namespace _06._2DGraphics
{
	class Program
	{
		// Path to default irrlicht media folder
		// string path = "../../../../media/";
		// Empty path - media in executable folder
		string path = string.Empty;
		IrrlichtDevice device;
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
		public void run()
		{
			/* At first, we let the user select the driver type,
			   then start up the engine, set a caption, and get a
			   pointer to the video driver.
			*/
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
			/*
			Get a pointer to the video driver and the SceneManager so that
			we do not always have to write device.VideoDriver()
			*/
			IVideoDriver driver = device.VideoDriver;
			/*All 2d graphics in this example are put together into one texture,
			  2ddemo.bmp. Because we want to draw colorkey based sprites, we need
			  to load this texture and tell the engine, which part of it should be
			  transparent based on a colorkey. In this example, we don't tell it
			  the color directly, we just say "Hey Irrlicht Engine, you'll find the
			  color I want at position (0,0) on the texture.". Instead, it would be
			  also possible to call driver.MakeColorKeyTexture(images, Color)
			  to make e.g. all black pixels transparent. Please note, that
			  makeColorKeyTexture just creates an alpha channel based on the color.*/
			ITexture images = driver.GetTexture(path + "2ddemo.bmp");
			driver.MakeColorKeyTexture(images, new Position2D(0, 0));
			/*
			 To be able to draw some text with two different fonts,
			 we load them. Ok, we load just one, as first font we just
			 use the default font which is built into the engine.
			 Also, we define two rectangles, which specify the position
			 of the images of the red imps (little flying creatures) in
			 the texture.
			 */
			IGUIFont font = device.GUIEnvironment.BuiltInFont;
			IGUIFont font2 = device.GUIEnvironment.GetFont(path + "fonthaettenschweiler.bmp");
			Rect imp1 = new Rect(349, 15, 385, 78);
			Rect imp2 = new Rect(387, 15, 423, 78);
			/*
			 Everything is prepared, now we can draw everything in the draw loop,
			 between the begin scene and end scene calls. In this example, we are just
			 doing 2d graphics, but it would be no problem to mix them with 3d graphics.
			 Just try it out, and draw some 3d vertices or set up a scene with the scene
			 manager and draw it.
			 */
			while (device.Run() && driver != null) 
			{
				if (device.WindowActive) 
				{
					uint time = device.Timer.Time;
					driver.BeginScene(true, true, new Color(0, 120, 102, 136));
					/*
					 First, we draw 3 sprites, using the alpha channel we created with
					 makeColorKeyTexture. The last parameter specifiys that the drawing
					 method should use thiw alpha channel. The parameter before the last
					 one specifies a color, with which the sprite should be colored.
					 (255,255,255,255) is full white, so the sprite will look like the
					 original. The third sprite is drawed colored based on the time.*/
					// draw fire & dragons background world
					driver.Draw2DImage(images, new Position2D(50, 50),
						new Rect(0, 0, 342, 224),
						new Color(255, 255, 255, 255), true);
					// draw flying imp
					driver.Draw2DImage(images, new Position2D(164, 125),
						(time / 500 % 2) == 0 ? imp1 : imp2,
						new Color(255, 255, 255, 255), true);
					// draw second flying imp with colorcylce
					driver.Draw2DImage(images, new Position2D(270, 105),
						(time / 500 % 2) == 0 ? imp1 : imp2,
						new Color(255, ((int)(time) % 255), 255, 255), true);
					// Drawing text is really simple. The code should be self explanatory.
					if (font != null) 
					{
						font.Draw("This is some text",
							new Rect(130, 10, 300, 50),
							new Color(255, 255, 255, 255), false, false);
					}
					if (font2 != null) 
					{
						font2.Draw("This is some text",
							new Rect(130, 20, 300, 60),
							new Color(255, (int)time % 255, (int)time % 255, 255),
							false, false);
					}
					/*At last, we draw the Irrlicht Engine logo (without using
					  a color or an alpha channel) and a transparent 2d Rectangle
					  at the position of the mouse cursor.*/
					// draw logo
					driver.Draw2DImage(images, new Position2D(10, 10),
						new Rect(354, 87, 442, 118), new Color(255, 255, 255, 255), false);
					// draw transparent rect under cursor
					Position2D m = device.CursorControl.Position;
					driver.Draw2DRectangle(new Color(100, 255, 255, 255),
						new Rect(m.X - 20, m.Y - 20, m.X + 20, m.Y + 20));
					driver.EndScene();
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

// Retrieved from "http://www.irrforge.org/index.php/CS_Tutorial_6"
// This page has been accessed 220 times. This page was last modified 06:10, 20 Jan 2006. 

