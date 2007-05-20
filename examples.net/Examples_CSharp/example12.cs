/* 
Lets start like the HelloWorld example: We include 
the irrlicht header files and an additional file to be able 
to ask the user for a driver type using the console. 
*/ 
using System; 
using System.Text; 
using System.IO; 

using Irrlicht; 
using Irrlicht.Video; 
using Irrlicht.Core; 
using Irrlicht.Scene; 
using Irrlicht.GUI; 

namespace _12.TerrainRendering 
{ 
	class Program: IEventReceiver 
	{ 
		string path="../../../../media/"; 
		ITerrainSceneNode terrain; 
		bool isWireframe=false; 

		//ISceneNode node=null; 
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

		/* In the beginning there is nothing special. We include the needed header files 
			  and create an event listener to listen if the user presses the 'W' key so we 
			  can switch to wireframe mode and if he presses 'D' we toggle to material 
			  between solid and detail mapped. 
		*/ 
		public bool OnEvent(Event p_event) 
		{ 
			// check if user presses the key 'W' or 'D' 
			if (p_event.Type == EventType.KeyInput && 
				!p_event.KeyPressedDown) 
			{ 
				switch(p_event.Key) 
				{ 
					case KeyCode.KEY_KEY_W: 
						isWireframe=!isWireframe; 
						terrain.SetMaterialFlag(MaterialFlag.WIREFRAME,isWireframe); 
						break; 
					case KeyCode.KEY_KEY_D: 
						terrain.SetMaterialType( 
							terrain.GetMaterial(0).Type==MaterialType.SOLID? 
							MaterialType.DETAIL_MAP : MaterialType.SOLID); 
						return true; 
				} 
			} 
			return false; 
		} 

		public void run() 
		{ 
			/* The start of the main function starts like in most other example. 
			   We ask the user for the desired renderer and start it up. 
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
			IrrlichtDevice device = new IrrlichtDevice( 
				driverType, new Dimension2D(640, 480), 32, false, true, true); 
			if (device == null) 
			{ 
				tOut.Write("Device creation failed."); 
				return; 
			} 

			/* set this as event receiver*/ 
			device.EventReceiver=this; 
			/*************************************************/ 
			/* First, we add standard stuff to the scene: A nice irrlicht engine logo, 
			   a small help text, a user controlled camera, and we disable the mouse 
			   cursor.*/          
			ISceneManager smgr=device.SceneManager; 
			IVideoDriver driver=device.VideoDriver; 
			IGUIEnvironment env= device.GUIEnvironment; 

			driver.SetTextureCreationFlag(TextureCreationFlag.ALWAYS_32_BIT,true); 

			// add irrlicht logo 
			env.AddImage(driver.GetTexture(path+"irrlichtlogoalpha.tga"), 
				new Position2D(10,10),true,null,0,""); 

			// add some help text 
			IGUIStaticText text = env.AddStaticText( 
				"Press 'W' to change wireframe mode\nPress 'D' to toggle detail map", 
				new Rect(10,453,200,475), true, true, null, -1); 

			// add camera 
			ICameraSceneNode camera = 
				smgr.AddCameraSceneNodeFPS(null,100.0f,1200.0f,-1); 
			camera.Position=new Vector3D(1900*2,255*2,3700*2); 
			camera.Target= new Vector3D(2397*2,343*2,2700*2); 
			camera.FarValue=12000.0f; 

			// disable mouse cursor 
			device.CursorControl.Visible=false; 
			/* Here comes the terrain renderer scene node: We add it just like any other scene 
			   node to the scene using ISceneManager::addTerrainSceneNode(). The only parameter 
			   we use is a file name to the heightmap we use. A heightmap is simply a gray 
			   scale texture. The terrain renderer loads it and creates the 3D terrain 
			   from it. 
			   To make the terrain look more big, we change the scale factor of it to 
			   (40, 4.4, 40). Because we don't have any dynamic lights in the scene, we 
			   switch off the lighting, and we set the file terrain-texture.jpg as texture 
			   for the terrain and detailmap3.jpg as second texture, called detail map. 
			   At last, we set the scale values for the texture: The first texture will be 
			   repeated only one time over the whole terrain, and the second one (detail map) 
			   20 times. 
			 */ 
			// add terrain scene node 
			terrain = smgr.AddTerrainSceneNode( 
				path+"terrain-heightmap.bmp",null,-1, 
				new Vector3D(),new Vector3D(40, 4.4f, 40), new Color(255,255,255,255)); 

			terrain.SetMaterialFlag(MaterialFlag.LIGHTING, false); 
			terrain.SetMaterialType(MaterialType.DETAIL_MAP); 
			terrain.SetMaterialTexture(0, driver.GetTexture(path+"terrain-texture.jpg")); 
			terrain.SetMaterialTexture(1, driver.GetTexture(path+"detailmap3.jpg")); 
             

			terrain.ScaleTexture(1.0f, 20.0f); 

			/* To be able to do collision with the terrain, we create a triangle selector. 
			   If you want to know what triangle selectors do, just take a look into the 
			   collision tutorial. The terrain triangle selector works together with the 
			   terrain. To demonstrate this, we create a collision response animator and 
			   attach it to the camera, so that the camera will not be able to fly through 
			   the terrain.*/ 
			// create triangle selector for the terrain    
			ITriangleSelector selector = 
				smgr.CreateTerrainTriangleSelector(terrain, 0); 

			// create collision response animator and attach it to the camera 
			ISceneNodeAnimator anim = smgr.CreateCollisionResponseAnimator( 
				selector, camera, new Vector3D(60,100,60), 
				new Vector3D(0,0,0), 
				new Vector3D(0,50,0),0.0005f); 
			camera.AddAnimator(anim); 

			//we add the skybox which we already used in lots of Irrlicht examples. 
			driver.SetTextureCreationFlag(TextureCreationFlag.CREATE_MIP_MAPS, false); 

			smgr.AddSkyBoxSceneNode( 
				driver.GetTexture(path+"irrlicht2_up.jpg"), 
				driver.GetTexture(path+"irrlicht2_dn.jpg"), 
				driver.GetTexture(path+"irrlicht2_lf.jpg"), 
				driver.GetTexture(path+"irrlicht2_rt.jpg"), 
				driver.GetTexture(path+"irrlicht2_ft.jpg"), 
				driver.GetTexture(path+"irrlicht2_bk.jpg"),null,0); 

			driver.SetTextureCreationFlag(TextureCreationFlag.CREATE_MIP_MAPS, true); 










			/* That's it, draw everything. Now you know how to use terrain 
			   in Irrlicht. 
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
						device.WindowCaption = "Irrlicht Engine - Terrain example [" + 
							device.VideoDriver.Name + "] FPS:" + fps.ToString(); 
						lastFPS = fps; 
					} 
				} 
			} 


			/* 
			In the end, delete the Irrlicht device. 
			*/ 
			// Instead of device.drop, we'll use: 
			GC.Collect(); 
            
		} 
	} 
}
// Retrieved from "http://www.irrforge.org/index.php/CS_Tutorial_12"
// This page has been accessed 228 times. This page was last modified 06:58, 20 Jan 2006. 

