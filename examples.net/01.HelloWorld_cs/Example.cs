using System;
using Irrlicht; // compiler error here? Look at the next comment
using Irrlicht.Video;
using Irrlicht.Core;
using Irrlicht.Scene;

// PROBLEMS with this source? 
// Some solutions wich might help:
//
// - If your compiler complains that 'Irrlicht' cannot be found,
//  add the Irrlicht.NET.dll file (in bin\win32-VisualStudio) as Reference
//  in the solution browser.
// 
// - It compiles, but your program throws an exception like this:
//  "An unhandled exception of type 'System.IO.FileNotFoundException' occurred in Unknown Module.
//  Additional information: File or assembly name Irrlicht.NET, or one of its dependencies,
//  was not found."
//  Solution: You are missing the file Irrlicht.DLL. Copy it where Visual studio can find it.
//  For example where this program is.

namespace _01.HelloWorld
{
	/// <summary>
	/// Example application using Irrlicht.NET 
	/// A little bit more complicated than the C++ hello world example, 
	/// but hey, its the only one yet. Give me some time.
	/// </summary>
	class Example
	{
		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		static void Main(string[] args)
		{
			// start up the engine

			IrrlichtDevice device = new IrrlichtDevice(DriverType.OPENGL);

			device.ResizeAble = true;
			device.WindowCaption = "Irrlicht.NET C# example 01 - Hello World";

			// load some textures 

			ITexture texSydney = device.VideoDriver.GetTexture(@"..\..\media\sydney.bmp");
			ITexture texWall = device.VideoDriver.GetTexture(@"..\..\media\wall.bmp");
			ITexture texLogo = device.VideoDriver.GetTexture(@"..\..\media\irrlichtlogoaligned.jpg");

			// load the animated mesh of sydney

			Irrlicht.Scene.IAnimatedMesh mesh = 
				device.SceneManager.GetMesh(@"..\..\media\sydney.md2");

			if (mesh == null)
			{
				System.Windows.Forms.MessageBox.Show(
					@"Could not load mesh ..\..\media\sydney.md2, exiting.",
					"Problem starting program");
				return;
			}

			// add a camera, a test scene node and the animated mesh to the scene	

			ICameraSceneNode cam = 
				device.SceneManager.AddCameraSceneNodeFPS(null, 100, 100, -1);
			cam.Position = new Vector3D(20,0,-50);


			ISceneNode node = device.SceneManager.AddCubeSceneNode(15,
				null, -1, new Vector3D(30,-15,0));
			node.SetMaterialTexture(0, texWall);

			node = device.SceneManager.AddAnimatedMeshSceneNode(mesh, null, -1);
			node.SetMaterialTexture(0, texSydney);
			node.SetMaterialFlag(MaterialFlag.LIGHTING, false);

			// make cursor invisible
			device.CursorControl.Visible = false;

			// start drawing loop

			int fps = 0;

			while(device.Run())
			{
				if (device.WindowActive)
				{
					device.VideoDriver.BeginScene(true, true, new Color(0,100,100,100));

					device.SceneManager.DrawAll();
				
					// draw the logo
					device.VideoDriver.Draw2DImage(
						texLogo, new Position2D(10,10),
						new Rect(0,0,88,31), 
						new Rect(new Position2D(0,0),device.VideoDriver.ScreenSize),
						new Color(0xffffff), false);

					device.VideoDriver.EndScene();

					if (fps != device.VideoDriver.FPS)
					{
						fps = device.VideoDriver.FPS;
						device.WindowCaption = "Irrlicht.NET C# example 01 - Hello World ["+ 
							device.VideoDriver.Name + "] fps:" + fps;
					}
				}
			} // end drawing-loop


		} // end main()

	} // end class

} // end namespace
