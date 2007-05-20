using System; 
using System.Text; 
using System.IO; 

using Irrlicht; 
using Irrlicht.Video; 
using Irrlicht.GUI; 
using Irrlicht.Core; 
using Irrlicht.Scene; 

namespace _10._Shaders 
{ 
	/*Because we want to use some interesting shaders in this tutorials, we need to set 
	  some data for them to make them able to compute nice colors. In this example, 
	  we'll use a simple vertex shader which will calculate the color of the vertex based 
	  on the position of the camera. For this, the shader needs the following data: 
	  The inverted world matrix for transforming the normal, the clip matrix for 
	  transforming the position, the camera position and the world position of the object 
	  for the calculation of the angle of light, and the color of the light. To be able 
	  to tell the shader all this data every frame, we have to derive a class from the 
	  IShaderConstantSetCallBack interface and override its only method, namely 
	  OnSetConstants(). This method will be called every time the material is set. 
	  The method setVertexShaderConstant() of the IMaterialRendererServices interface is 
	  used to set the data the shader needs. If the user chose to use a High Level shader 
	  language like HLSL instead of Assembler in this example, you have to set the 
	  variable name as parameter instead of the register index.*/ 
	class Program: IShaderConstantSetCallBack 
	{ 
		string path="../../../../media/"; 
		IrrlichtDevice device;    
		bool UseHighLevelShaders = false; 

		public virtual void OnSetConstants(IMaterialRendererServices services) 
		{ 
			IVideoDriver driver = services.VideoDriver; 

			// set inverted world matrix 
			// if we are using highlevel shaders (the user can select this when 
			// starting the program), we must set the constants by name. 
			Matrix4 invWorld = driver.GetTransform(TransformationState.WORLD); 
			invWorld.MakeInverse(); 

			if (UseHighLevelShaders) 
				services.SetVertexShaderConstant("mInvWorld", invWorld.GetFloats(), 16); 
			else 
				services.SetVertexShaderConstant(invWorld.GetFloats(), 0, 4); 

			// set clip matrix 
			Matrix4 worldViewProj; 
			worldViewProj = driver.GetTransform(TransformationState.PROJECTION);          
			worldViewProj *= driver.GetTransform(TransformationState.VIEW); 
			worldViewProj *= driver.GetTransform(TransformationState.WORLD); 

			if (UseHighLevelShaders) 
				services.SetVertexShaderConstant("mWorldViewProj", worldViewProj.GetFloats(), 16); 
			else 
				services.SetVertexShaderConstant(worldViewProj.GetFloats(), 4, 4); 
       
			// set camera position 
			Vector3D pos = device.SceneManager.ActiveCamera.Position; 

			if (UseHighLevelShaders) 
				services.SetVertexShaderConstant("mLightPos",vectorToArray(pos), 3); 
			else 
				services.SetVertexShaderConstant(vectorToArray(pos), 8, 1); 

			// set light color 
			Colorf col= new Colorf(0.0f,1.0f,1.0f,0.0f); 

			if (UseHighLevelShaders) 
				services.SetVertexShaderConstant("mLightColor", colorToArray(col), 4); 
			else 
				services.SetVertexShaderConstant(colorToArray(col), 9, 1); 

			// set transposed world matrix 
			Matrix4 world = driver.GetTransform(TransformationState.WORLD); 
			world = world.GetTransposed(); 

			if (UseHighLevelShaders) 
				services.SetVertexShaderConstant("mTransWorld", world.GetFloats(), 16); 
			else 
				services.SetVertexShaderConstant(world.GetFloats(), 10, 4); 
		} 



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
			/* The next few lines start up the engine. Just like in most other tutorials 
			   before. But in addition, we ask the user if he wants this example to use 
			   high level shaders if he selected a driver which is capable of doing so. 
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
			// ask the user if we should use high level shaders for this example 
			if (driverType == DriverType.DIRECT3D9 || 
				driverType == DriverType.OPENGL) 
			{ 
				tOut.Write("Please press 'y' if you want to use high level shaders.\n"); 
				input=tIn.ReadLine(); 
				if (input.ToLower()=="y") 
					UseHighLevelShaders=true; 
			} 

			// create device 
			device = new IrrlichtDevice(driverType, new Dimension2D(1024, 768), 32, false, true, true); 
			if (device == null) 
			{ 
				tOut.Write("Device creation failed."); 
				return; 
			} 
          
			ISceneManager smgr=device.SceneManager; 
			IVideoDriver driver=device.VideoDriver; 
			IGUIEnvironment gui = device.GUIEnvironment; 

			/*Now for the more interesting parts. If we are using Direct3D, we want to 
			  load vertex and pixel shader programs, if we have OpenGL, we want to use ARB 
			  fragment and vertex programs. I wrote the corresponding programs down into the 
			  files d3d8.ps, d3d8.vs, d3d9.ps, d3d9.vs, opengl.ps and opengl.vs. We only 
			  need the right filenames now. This is done in the following switch. Note, 
			  that it is not necessary to write the shaders into text files, like in this 
			  example. You can even write the shaders directly as strings into the cpp source 
			  file, and use later addShaderMaterial() instead of addShaderMaterialFromFiles().*/ 
			string vsFileName="" ; 
			string psFileName="" ; 

			switch (driverType) 
			{ 
				case DriverType.DIRECT3D8: 
					psFileName = path+"d3d8.psh"; 
					vsFileName = path+"d3d8.vsh"; 
					break; 
				case DriverType.DIRECT3D9: 
					if (UseHighLevelShaders) 
					{ 
						psFileName = path+"d3d9.hlsl"; 
						vsFileName = psFileName; // both shaders are in the same file 
					} 
					else 
					{ 
						psFileName = path+"d3d9.psh"; 
						vsFileName = path+"d3d9.vsh"; 
					} 
					break; 
				case DriverType.OPENGL: 
					if (UseHighLevelShaders) 
					{ 
						psFileName = path+"opengl.frag"; 
						vsFileName = path+"opengl.vert"; 
					} 
					else 
					{ 
						psFileName = path+"opengl.psh"; 
						vsFileName = path+"opengl.vsh"; 
					} 
					break; 
			} 

			/*In addition, we check if the hardware and the selected renderer is capable 
			  of executing the shaders we want. If not, we simply set the filename string 
			  to 0. This is not necessary, but useful in this example: For example, if the 
			  hardware is able to execute vertex shaders but not pixel shaders, we create a 
			  new material which only uses the vertex shader, and no pixel shader. Otherwise, 
			  if we would tell the engine to create this material and the engine sees that 
			  the hardware wouldn't be able to fullfill the request completely, it would not 
			  create any new material at all. So in this example you would see at least the 
			  vertex shader in action, without the pixel shader.*/ 
			if(!driver.QueryFeature(VideoDriverFeature.PIXEL_SHADER_1_1)&& 
				!driver.QueryFeature(VideoDriverFeature.ARB_FRAGMENT_PROGRAM_1)) 
			{ 
				// still unimplemented 
				//device.Logger.log("WARNING: Pixel shaders disabled \n"+ 
				//   "because of missing driver/hardware support."); 
				psFileName = null; 
			} 
			if(!driver.QueryFeature(VideoDriverFeature.VERTEX_SHADER_1_1)&& 
				!driver.QueryFeature(VideoDriverFeature.ARB_FRAGMENT_PROGRAM_1)) 
			{ 
				// still unimplemented 
				//device.Logger.log("WARNING: Vertex shaders disabled \n"+ 
				//   "because of missing driver/hardware support."); 
				vsFileName = null; 
			} 

			/*Now lets create the new materials. As you maybe know from previous examples, 
			  a material type in the Irrlicht engine is set by simply changing the 
			  MaterialType value in the SMaterial struct. And this value is just a simple 
			  32 bit value, like video::EMT_SOLID. So we only need the engine to create a 
			  new value for us which we can set there. To do this, we get a pointer to the 
			  IGPUProgrammingServices and call addShaderMaterialFromFiles(), which returns 
			  such a new 32 bit value. That's all. The parameters to this method are the 
			  following: First, the names of the files containing the code of the vertex 
			  and the pixel shader. If you would use addShaderMaterial() instead, you would 
			  not need file names, then you could write the code of the shader directly as 
			  string. The following parameter is a pointer to the IShaderConstantSetCallBack 
			  class we wrote at the beginning of this tutorial. If you don't want to set 
			  constants, set this to 0. The last paramter tells the engine which material 
			  it should use as base material. To demonstrate this, we create two materials 
			  with a different base material, one with EMT_SOLID and one with 
			  EMT_TRANSPARENT_ADD_COLOR.*/ 
			// create materials 

			IGPUProgrammingServices gpu = driver.GPUProgrammingServices; 

			int newMaterialType1 = 0; 
			int newMaterialType2 = 0; 

			if(gpu!=null) 
			{ 
				IShaderConstantSetCallBack callBack = this; 
				// create the shaders depending on if the user wanted high level 
				// or low level shaders: 
				if(UseHighLevelShaders) 
				{ 
					// create material from high level shaders (hlsl or glsl) 
					newMaterialType1=gpu.AddHighLevelShaderMaterialFromFiles( 
						vsFileName,"vertexMain",VertexShaderType.VST_VS_1_1, 
						psFileName,"pixelMain",PixelShaderType.PST_PS_1_1, 
						callBack,MaterialType.SOLID); 
					newMaterialType2=gpu.AddHighLevelShaderMaterialFromFiles( 
						vsFileName,"vertexMain",VertexShaderType.VST_VS_1_1, 
						psFileName,"pixelMain",PixelShaderType.PST_PS_1_1, 
						callBack,MaterialType.TRANSPARENT_ADD_COLOR); 
				} 
				else 
				{ 
					newMaterialType1=gpu.AddShaderMaterialFromFiles(vsFileName, 
						psFileName,callBack,MaterialType.SOLID); 
					newMaterialType2=gpu.AddShaderMaterialFromFiles(vsFileName, 
						psFileName,callBack,MaterialType.TRANSPARENT_ADD_COLOR); 
				} 
			} 

			/*Now its time for testing out the materials. We create a test cube and set the 
			  material we created. In addition, we add a text scene node to the cube and a 
			  rotatation animator, to make it look more interesting and important.*/ 

			// create test scene node 1, with the new created material type 1 
			ISceneNode node = smgr.AddTestSceneNode(50,null,0,new Vector3D(0,0,0)); 
			node.SetMaterialTexture(0,driver.GetTexture(path+"wall.bmp")); 
			node.SetMaterialType((MaterialType)newMaterialType1); 

			smgr.AddTextSceneNode(gui.BuiltInFont,"PS & VS & EMT_SOLID", 
				new Color(255,255,255,255),node,new Vector3D(),0); 

			ISceneNodeAnimator anim= smgr.CreateRotationAnimator( 
				new Vector3D(0,0.3f,0)); 
			node.AddAnimator(anim); 

			//Same for the second cube, but with the second material we created. 
			node = smgr.AddTestSceneNode(50,null,0,new Vector3D(0,-10,50)); 
			node.SetMaterialTexture(0,driver.GetTexture(path+"wall.bmp")); 
			node.SetMaterialType((MaterialType)newMaterialType2); 

			smgr.AddTextSceneNode(gui.BuiltInFont,"PS & VS & EMT_TRANSPARENT", 
				new Color(255,255,255,255),node,new Vector3D(),0); 

			anim= smgr.CreateRotationAnimator( 
				new Vector3D(0,0.3f,0)); 
			node.AddAnimator(anim); 

			// Then we add a third cube without a shader on it, to be able to compare the cubes. 
			node = smgr.AddTestSceneNode(50,null,0,new Vector3D(0,50,25)); 
			node.SetMaterialTexture(0,driver.GetTexture(path+"wall.bmp")); 
			smgr.AddTextSceneNode(gui.BuiltInFont,"NO SHADER", 
				new Color(255,255,255,255),node,new Vector3D(),0); 

			//And last, we add a skybox and a user controlled camera to the scene. For the 
			//skybox textures, we disable mipmap generation, because we don't need mipmaps on it. 
          
			// add a nice skybox 
			driver.SetTextureCreationFlag(TextureCreationFlag.CREATE_MIP_MAPS,false); 
			smgr.AddSkyBoxSceneNode( 
				driver.GetTexture(path+"irrlicht2_up.jpg"), 
				driver.GetTexture(path+"irrlicht2_dn.jpg"), 
				driver.GetTexture(path+"irrlicht2_lf.jpg"), 
				driver.GetTexture(path+"irrlicht2_rt.jpg"), 
				driver.GetTexture(path+"irrlicht2_ft.jpg"), 
				driver.GetTexture(path+"irrlicht2_bk.jpg"), 
				null,0); 
			driver.SetTextureCreationFlag(TextureCreationFlag.CREATE_MIP_MAPS,true); 

			// add a camera and disable the mouse cursor 
			ICameraSceneNode cam = smgr.AddCameraSceneNodeFPS(null,100,100,0); 
			cam.Position=new Vector3D(-100,50,100); 
			cam.Target=new Vector3D(); 
			device.CursorControl.Visible=false; 

			/*Finally we simply have to draw everything, that's all.*/ 
			int lastFPS=-1; 

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
						device.WindowCaption = "Irrlicht Engine - Quake 3 Map example [" + 
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

		float[] colorToArray (Colorf p_m) 
		{ 
			float[] t_a=new float[4]; 
			t_a[0]=p_m.r; 
			t_a[1]=p_m.g; 
			t_a[2]=p_m.b; 
			t_a[3]=p_m.a; 
			return t_a; 
		} 

		float[] vectorToArray (Vector3D p_m) 
		{ 
			float[] t_a=new float[4]; 
			t_a[0]=p_m.X; 
			t_a[1]=p_m.Y; 
			t_a[2]=p_m.Z; 
			t_a[3]=0; 
			return t_a; 
		} 
	} 
}

// Retrieved from "http://www.irrforge.org/index.php/CS_Tutorial_10"
// This page has been accessed 185 times. This page was last modified 06:57, 20 Jan 2006. 

