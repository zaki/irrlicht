/*
This Tutorial shows how to move and animate SceneNodes. The
basic concept of SceneNodeAnimators is shown as well as manual
movement of nodes using the keyboard.
*/
using System;
using System.Text;
using System.IO;
using Irrlicht;
using Irrlicht.Video;
using Irrlicht.Core;
using Irrlicht.Scene;
namespace _04.Movement
{
	/*
	In this tutorial, one of our goals is to move a scene node using some
	keys on the keyboard. We store a pointer to the scene node we want to
	move with the keys here.
	The other pointer is a pointer to the Irrlicht Device, which we need
	int the EventReceiver to manipulate the scene node and to get the
	active camera.
	*/
	/*
	To get events like mouse and keyboard input, or GUI events like
	"the OK button has been clicked", we need an object wich is derived from the
	IEventReceiver object. There is only one method to override: OnEvent.
	This method will be called by the engine when an event happened.
	We will use this input to move the scene node with the keys W and S.
	*/
	class Program : IEventReceiver
	{
		// Use this line for the default path to the Irrlicht standard media.
		//string path = "../../../../media/";
		// the_bob just puts the media in the executable folder, so I'm using this:
		string path = string.Empty;
		ISceneNode node = null;
		IrrlichtDevice device = null;
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
		public bool OnEvent(Event p_e)
		{
			if (node != null && p_e.Type == EventType.KeyInput &&
				!p_e.KeyPressedDown) 
			{
				switch (p_e.Key) 
				{
					case KeyCode.KEY_ESCAPE:
						device.CloseDevice();
						break;
					case KeyCode.KEY_KEY_W:
					case KeyCode.KEY_KEY_S: 
					{
						Vector3D v = node.Position;
						v.Y += p_e.Key == KeyCode.KEY_KEY_W ? 2.0f : -2.0f;
						node.Position = v;
					}
						return true;
				}
			}
			return false;
		}
		public void run()
		{
			/* Like in the HelloWorld example, we create an IrrlichtDevice with
			new(). The difference now is that we ask the user to select
			which hardware accelerated driver to use. The Software device would be
			too slow to draw a huge Quake 3 map, but just for the fun of it, we make
			this decision possible too.
			*/
			// ask user for driver
			DriverType driverType;
			// Ask user to select driver:
			StringBuilder sb = new StringBuilder();
			sb.Append("Please select the driver you want for this example:\n");
			sb.Append("\n(a) Direct3D 9.0c\n(b) Direct3D 8.1\n(c) OpenGL 1.5");
			sb.Append("\n(d) Software Renderer\n(e)Apfelbaum Software Renderer");
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
			device = new IrrlichtDevice(driverType, new Dimension2D(1024, 768), 32, true, true, true);   // Bob changed this.
			if (device == null) 
			{
				tOut.Write("Device creation failed.");
				return;
			}
			/* set this as event receiver*/
			device.EventReceiver = this;
			/*
			Get a reference to the video driver and the SceneManager so that
			we do not always have to write device.VideoDriver and
			device.SceneManager.
			*/
			ISceneManager smgr = device.SceneManager;
			IVideoDriver driver = device.VideoDriver;
			/*Create the node for moving it with the 'W' and 'S' key. We create a
			 'test node', which is a cube built in into the engine for testing purposes.
			 We place the node a (0,0,30) and we assign a texture to it to let it look a
			 little bit more interesting.*/
			node = smgr.AddCubeSceneNode(10, null, 0, new Vector3D(0, 0, 30), new Vector3D(), new Vector3D(1, 1, 1));
			node.SetMaterialTexture(0, driver.GetTexture(path + "wall.bmp"));
			node.SetMaterialFlag(MaterialFlag.LIGHTING, false);
			/* Now we create another node, moving using a scene node animator. Scene
			   node animators modify scene nodes and can be attached to any scene node
			   like mesh scene nodes, billboards, lights and even camera scene nodes.
			   Scene node animators are not only able to modify the position of a scene
			   node, they can also animate the textures of an object for example. We create
			   a test scene node again an attach a 'fly circle' scene node to it, letting
			   this node fly around our first test scene node.*/
			ISceneNode n = smgr.AddTestSceneNode(10, null, 0, new Vector3D(), new Vector3D(), new Vector3D(1, 1, 1));
			n.SetMaterialTexture(0, driver.GetTexture(path + "t351sml.jpg"));
			ISceneNodeAnimator anim = smgr.CreateFlyCircleAnimator(new Vector3D(0, 0, 30), 20, 0.001f);
			n.AddAnimator(anim);
			//is this really necessary?
			anim.__dtor();
			/*The last scene node we add to show possibilities of scene node animators
			  is a md2 model, which uses a 'fly straight' animator to run between two
			  points.*/
			IAnimatedMeshSceneNode anms = smgr.AddAnimatedMeshSceneNode(
				smgr.GetMesh(path + "sydney.md2"), null, 0);
			if (anms != null) 
			{
				anim = smgr.CreateFlyStraightAnimator(new Vector3D(100, 0, 60), new Vector3D(-100, 0, 60), 10000, true);
				anms.AddAnimator(anim);
				anim.__dtor();
				/*To make to model look better, we disable lighting (we have created no lights,
				  and so the model would be black), set the frames between which the animation
				  should loop, rotate the model around 180 degrees, and adjust the animation
				  speed and the texture.
				  To set the right animation (frames and speed), we would also be able to just
				  call "anms->setMD2Animation(scene::EMAT_RUN)" for the 'run' animation
				  instead of "setFrameLoop" and "setAnimationSpeed", but this only works with
				  MD2 animations, and so you know how to start other animations.*/
				anms.Position = new Vector3D(0, 0, 40);
				anms.SetMaterialFlag(MaterialFlag.LIGHTING, false);
				anms.SetFrameLoop(320, 360);
				anms.AnimationSpeed = 30;
				anms.Rotation = new Vector3D(0, 180, 0);
				anms.SetMaterialTexture(0, driver.GetTexture(path + "sydney.BMP"));
			}
			/*To be able to look at and move around in this scene, we create a first person
			 * shooter style camera and make the mouse cursor invisible.*/
			ICameraSceneNode camera = smgr.AddCameraSceneNodeFPS(null, 100, 100, 0);
			camera.Position = new Vector3D(0, 0, 0);
			device.CursorControl.Visible = false;
			/*
			We have done everything, so lets draw it. We also write the current
			frames per second and the drawn primitives to the caption of the
			window.
			*/
			int lastFPS = -1;
			while (device.Run()) 
			{
				if (device.WindowActive) 
				{
					device.VideoDriver.BeginScene(true, true, new Color(0, 200, 200, 200));
					device.SceneManager.DrawAll();
					device.VideoDriver.EndScene();
					int fps = device.VideoDriver.FPS;
					if (lastFPS != fps) 
					{
						device.WindowCaption = "Irrlicht Engine - Movement example [" +
							device.VideoDriver.Name + "] FPS:" + fps.ToString();
						lastFPS = fps;
					}
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

// Retrieved from "http://www.irrforge.org/index.php/CS_Tutorial_4"
// This page has been accessed 243 times. This page was last modified 06:03, 20 Jan 2006. 

