// Copyright (C) 2002-2005 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h


		// please note: This program is just a program to test out the features
		// of the irrlicht.net port. If you are looking for examples using Irrlicht.NET
		// please take a look into the examples.net directory.


using System;
using Irrlicht;
using Irrlicht.Video;
using Irrlicht.Core;
using Irrlicht.Scene;
using Irrlicht.GUI;

enum TestScenarios
{
	NONE = 0,
	TERRAIN_TEST,
	INDOOR_TEST,
	WINDOWS_FORMS_TEST,
	SHADER_TEST
};

	/// <summary>
	/// Example application using Irrlicht.NET 
	/// </summary>
class ExampleApp : IEventReceiver, IShaderConstantSetCallBack
{
	/// <summary>
	/// The main entry point for the application.
	/// </summary>
	[STAThread]
	static void Main(string[] args)
	{
		ExampleApp ea = new ExampleApp();

		ea.displayMainMenu();

		switch(ea.ClickedButton)
		{
			case TestScenarios.TERRAIN_TEST:
				ea.runTerrainTest();
				break;
			case TestScenarios.INDOOR_TEST:
				ea.runIndoorTest();
				break;
			case TestScenarios.WINDOWS_FORMS_TEST:
				ea.runIrrlichtInWindowsFormTest();				
				break;
			case TestScenarios.SHADER_TEST:
				ea.runShaderTest();
				break;
		}
	}

	/// <summary>
	/// Receives events (like input from mouse and keyboard) from the Irrlicht Engine
	/// </summary>
	public bool OnEvent(Event e)
	{
		if (e.Type == EventType.GUIEvent)
		{
			// a user interface event

			if ( e.GUIEventType == GUIEvent.LISTBOX_CHANGED ||
				 e.GUIEventType == GUIEvent.LISTBOX_SELECTED_AGAIN )
			{
				int selected = ((IGUIListBox)e.GUIEventCaller).Selected;
				SelectedDriverType = (DriverType)(selected+1);
			}
			else
				if (e.GUIEventType == GUIEvent.BUTTON_CLICKED)
				ClickedButton = (TestScenarios)e.GUIEventCaller.ID;
		}
		else
			if (e.Type == EventType.KeyInput)
		{
			// a key has been pressed

			if (!e.KeyPressedDown && e.Key == KeyCode.KEY_KEY_S && Shadow != null)
			{
				// toggle shadow visibility
				Shadow.Visible = !Shadow.Visible; 
				return true;
			}			
		}


		return false;
	}


	public void displayMainMenu()
	{
		device = new IrrlichtDevice(DriverType.SOFTWARE,
			new Dimension2D(512, 384), 16, false, false, false);

		// set event receiver
		device.EventReceiver = this;

		// set text

		device.WindowCaption = "Irrlicht .NET test demos - main menu";

		// load font

		IGUIFont font = device.GUIEnvironment.GetFont("../../media/fonthaettenschweiler.bmp");
		if (font != null)
			device.GUIEnvironment.Skin.Font = font;

		// add images

		device.GUIEnvironment.AddImage( device.VideoDriver.GetTexture("../../media/dotnetback.jpg"),
			new Position2D(0,0), false, null, -1, "");

		// add list box

		Rect pos = new Rect(150,60,350,135);

		IGUIListBox box = 
			device.GUIEnvironment.AddListBox( pos, null, -1, true );

		box.AddItem("Irrlicht Software Renderer 1.0");
		box.AddItem("Apfelbaum Software Renderer 1.0");
		box.AddItem("Direct3D 8.1");
		box.AddItem("Direct3D 9.0c");			
		box.AddItem("OpenGL 1.5");				
		box.Selected = ((int)SelectedDriverType)-1;

		// add terrain button

		pos.LowerRightCorner.Y += 50;
		pos.UpperLeftCorner.Y += 100;		
		device.GUIEnvironment.AddButton( pos, null, 
			(int)TestScenarios.TERRAIN_TEST, "Start Terrain Test" );

		// add indoor button

		pos.LowerRightCorner.Y += 30;
		pos.UpperLeftCorner.Y += 30;		
		device.GUIEnvironment.AddButton( pos, null, 
			(int)TestScenarios.INDOOR_TEST, "Start Indoor Test" );

		// add windows forms button

		pos.LowerRightCorner.Y += 30;
		pos.UpperLeftCorner.Y += 30;		
		device.GUIEnvironment.AddButton( pos, null,
			(int)TestScenarios.WINDOWS_FORMS_TEST, "Start Windows.Forms Test" );

		// add shader test button

		pos.LowerRightCorner.Y += 30;
		pos.UpperLeftCorner.Y += 30;		
		device.GUIEnvironment.AddButton( pos, null,
			(int)TestScenarios.SHADER_TEST, "Start Shader Test" );

		// add copyright stuff

		IGUIStaticText text = device.GUIEnvironment.AddStaticText( 
			"Background 3D scene created by Alvaro F. Celis, rendered using Irrlicht",
			new Rect(2,368,500,384), false, false, null, -1);

		text.OverrideColor = new Color(100,150,150,150);

		// draw everything

		while(device.Run() && ClickedButton == TestScenarios.NONE)
			if (device.WindowActive)
			{
				device.VideoDriver.BeginScene(true, true, new Color(255,0,0,50));

				device.SceneManager.DrawAll();
				device.GUIEnvironment.DrawAll();

				device.VideoDriver.EndScene();
			}

		device.CloseDevice();
		return;
	}


	public void runTerrainTest()
	{
		device = new IrrlichtDevice(SelectedDriverType,
			new Dimension2D(800,600), 16, false, false, false);

		device.EventReceiver = this;
		device.ResizeAble = true;
		device.WindowCaption = "Irrlicht.NET terrain test";

		// create a camera

		ICameraSceneNode cam = 
			device.SceneManager.AddCameraSceneNodeFPS(null, 100, 1200, -1);

		cam.Position = new Vector3D(1900*2, 255*2, 3700*2);
		cam.Target = new Vector3D(2397*2, 343*2, 2700*2);
		cam.FarValue = 12000.0f;

		// create the terrain

		ITerrainSceneNode terrain = device.SceneManager.AddTerrainSceneNode( 
			"../../media/terrain-heightmap.bmp",
			null, -1, new Vector3D(), new Vector3D(40, 4.4f, 40), new Color(255,255,255,255));

		terrain.SetMaterialFlag(MaterialFlag.LIGHTING, false);
		terrain.SetMaterialType(MaterialType.DETAIL_MAP);
		terrain.SetMaterialTexture(0, device.VideoDriver.GetTexture("../../media/terrain-texture.jpg"));
		terrain.SetMaterialTexture(1, device.VideoDriver.GetTexture("../../media/detailmap3.jpg"));

		terrain.ScaleTexture( 1.0f, 20.0f );

		// create terrain triangle selector for collision

		ITriangleSelector selector = device.SceneManager.CreateTerrainTriangleSelector( terrain, 0 );

		// create collision animator and add it to the camera

		ISceneNodeAnimator collAnim = device.SceneManager.CreateCollisionResponseAnimator(
			selector, cam, 
			new Vector3D(30,50,30),  // size of ellipsoid around camera
			new Vector3D(0, 0, 0),  // gravity
			new Vector3D(0, 50, 0),  // translation
			0.0005f);                // sliding value

		cam.AddAnimator(collAnim);

		// add sky box

		device.SceneManager.AddSkyBoxSceneNode( 
			device.VideoDriver.GetTexture("../../media/irrlicht2_up.jpg"),
			device.VideoDriver.GetTexture("../../media/irrlicht2_dn.jpg"),
			device.VideoDriver.GetTexture("../../media/irrlicht2_lf.jpg"),
			device.VideoDriver.GetTexture("../../media/irrlicht2_rt.jpg"),
			device.VideoDriver.GetTexture("../../media/irrlicht2_ft.jpg"),
			device.VideoDriver.GetTexture("../../media/irrlicht2_bk.jpg"), 
			null, -1);

		// make cursor invisible
		device.CursorControl.Visible = false;

		// draw everything

		device.Run(); // fix for a temporary bug where quit messages are not be removed in the queue
		while(device.Run())
		{
			if (device.WindowActive)
			{
				device.VideoDriver.BeginScene(true, true, new Color(255,0,0,50));

				device.SceneManager.DrawAll();
				device.GUIEnvironment.DrawAll();

				device.VideoDriver.EndScene();
			}
		}
	}


	public void runIndoorTest()
	{
		device = new IrrlichtDevice(SelectedDriverType,
			new Dimension2D(800,600), 16, false, true, false);

		device.EventReceiver = this;
		device.ResizeAble = true;
		device.WindowCaption = "Irrlicht.NET indoor test";

		// load some textures and meshes

		ITexture texSydney = device.VideoDriver.GetTexture(@"..\..\media\sydney.bmp");
		ITexture texWall = device.VideoDriver.GetTexture(@"..\..\media\wall.jpg");
		ITexture texLogo = device.VideoDriver.GetTexture(@"..\..\media\irrlichtlogoaligned.jpg");
		Irrlicht.Scene.IAnimatedMesh mesh = 
			device.SceneManager.GetMesh(@"..\..\media\sydney.md2");

		if (mesh == null)
		{
			System.Windows.Forms.MessageBox.Show(
				@"Could not load mesh ..\..\media\sydney.md2, exiting.",
				"Problem starting program");
			return;
		}

		// add a cube to the scene

		ISceneNode node = device.SceneManager.AddCubeSceneNode(15,
			null, -1, new Vector3D(30,-15,0));
		node.SetMaterialTexture(0, texWall);
		node.SetMaterialFlag(Irrlicht.Video.MaterialFlag.LIGHTING, false);

		// add an animator to the cube to make it rotate

		ISceneNodeAnimator anim = device.SceneManager.CreateRotationAnimator(new Vector3D(0.2f, 0.2f, 0));
		node.AddAnimator(anim);

		// add animated mesh 

		IAnimatedMeshSceneNode anode = device.SceneManager.AddAnimatedMeshSceneNode(mesh, null, -1);
		anode.SetMaterialTexture(0, texSydney);
		anode.SetMaterialFlag(MaterialFlag.LIGHTING, false);

		anode.Scale = new Vector3D(2,2,2);
		anode.Position = new Vector3D(0, -20, 0);

		// add a shadow

		Shadow = anode.AddShadowVolumeSceneNode();
		if (Shadow != null)
			Shadow.Visible = false;

		// where no light there no shadow
		device.SceneManager.AddLightSceneNode(null, new Vector3D(20,100,-50),
			new Colorf(255,0,0), 200, -1);

		// add quake 3 level

		device.FileSystem.AddZipFileArchive("../../media/map-20kdm2.pk3");

		IAnimatedMesh q3levelmesh = device.SceneManager.GetMesh("20kdm2.bsp");
		ISceneNode q3node = device.SceneManager.AddOctTreeSceneNode(q3levelmesh, null, -1);
		q3node.Position = new Vector3D(-1370,-130,-1400);

		// create octtree triangle selector for q3 mesh

		ITriangleSelector selector = device.SceneManager.CreateOctTreeTriangleSelector(
			q3levelmesh.GetMesh(0), q3node, 128);

		// add billboard

		IBillboardSceneNode bill = device.SceneManager.AddBillboardSceneNode(null, 
			new Dimension2Df(20,20), new Vector3D(0,0,0), -1);

		bill.SetMaterialType(MaterialType.TRANSPARENT_ADD_COLOR);
		bill.SetMaterialTexture(0, device.VideoDriver.GetTexture("../../media/particle.bmp"));
		bill.SetMaterialFlag(MaterialFlag.LIGHTING, false);
		bill.SetMaterialFlag(MaterialFlag.ZBUFFER, false);
		
		// create camera

		ICameraSceneNode cam = 
			device.SceneManager.AddCameraSceneNodeFPS(null, 100, 300, -1);
		cam.Position = new Vector3D(20,300,-50);

		// make cursor invisible
		device.CursorControl.Visible = false;

		// create collision animator and add it to the camera

		ISceneNodeAnimator collAnim = device.SceneManager.CreateCollisionResponseAnimator(
			selector, cam, 
			new Vector3D(30,50,30),  // size of ellipsoid around camera
			new Vector3D(0, -3, 0),  // gravity
			new Vector3D(0, 50, 0),  // translation
			0.0005f);                // sliding value

		cam.AddAnimator(collAnim);

		// load some font and set it into the skin

		IGUIFont font = device.GUIEnvironment.GetFont("../../media/fonthaettenschweiler.bmp");
		device.GUIEnvironment.Skin.Font = font;

		// add some gui stuff
		
		device.GUIEnvironment.AddMessageBox("Hello World", 
			"I'm a Irrlicht.NET MessageBox. Please press SPACE to close me.", true, 
			MessageBoxFlag.OK | MessageBoxFlag.CANCEL, null, -1);
		
		// start drawing loop

		int fps = 0;

		device.Run(); // fix for a temporary bug where quit messages are not be removed in the queue
		while(device.Run())
			if (device.WindowActive)
			{
				device.VideoDriver.BeginScene(true, true, new Color(255,0,0,50));

				// draw scene

				device.SceneManager.DrawAll();
				device.GUIEnvironment.DrawAll();

				// do some collision testing

				Line3D line = new Line3D();
				line.start = cam.Position;
				line.end = ((cam.Target - line.start).Normalize() * 1000.0f ) + line.start;

				Vector3D intersection = new Vector3D();
				Triangle3D tri = new Triangle3D();

				if (device.SceneManager.SceneCollisionManager.GetCollisionPoint(
					line, selector, out intersection, out tri))
				{
					bill.Position = intersection;
				
					Material mat = new Material();
					mat.Lighting = false;

					device.VideoDriver.SetTransform(TransformationState.WORLD, new Matrix4());
					device.VideoDriver.SetMaterial(mat);
					device.VideoDriver.Draw3DTriangle(tri, new Color(0,255,0,0));
				}

				// draw 2d logo
					
				device.VideoDriver.Draw2DImage(
					texLogo, new Position2D(10,10),
					new Rect(0,0,88,31), 
					new Rect(new Position2D(0,0),device.VideoDriver.ScreenSize),
					new Color(0xffffff), false);

				// draw some text 
				font.Draw("Press 'S' to toggle the visibility of the realtime shadow.",
					new Position2D(120,20), new Color(100,150,200,200));

				device.VideoDriver.EndScene();

				if (fps != device.VideoDriver.FPS)
				{
					fps = device.VideoDriver.FPS;
					device.WindowCaption = "Irrlicht.NET test (primitives:" + 
						device.VideoDriver.PrimitiveCountDrawn + ") fps:" + fps;
				}
			}
	}


	void runIrrlichtInWindowsFormTest()
	{
		// testing irrlicht running embedded in a windows form
		
		System.Windows.Forms.Form f = new System.Windows.Forms.Form();
		f.Text = "Irrlicht running embedded in Windows.Form";

		device = new IrrlichtDevice(SelectedDriverType,
			new Dimension2D(800,600), 16, false, false, false, true, f.Handle);

		f.Show();

		// set up a simple scene

		ICameraSceneNode cam = 
			device.SceneManager.AddCameraSceneNode( null, new Vector3D(), new Vector3D(), -1);

		ISceneNodeAnimator anim = device.SceneManager.CreateFlyCircleAnimator(
			new Vector3D(0,10,0), 30.0f, 0.003f);
		cam.AddAnimator(anim);

		ISceneNode cube = device.SceneManager.AddCubeSceneNode(25, null, -1, new Vector3D());
		cube.SetMaterialTexture(0, device.VideoDriver.GetTexture("../../media/rockwall.bmp"));
		cube.SetMaterialFlag(Irrlicht.Video.MaterialFlag.LIGHTING, false);

		// draw everything

		device.Run(); // fix for a temporary bug where quit messages are not be removed in the queue
		while(device.Run() && f.Visible)
			if (device.WindowActive)
			{
				device.VideoDriver.BeginScene(true, true, new Color(255,0,0,50));

				device.SceneManager.DrawAll();
				device.GUIEnvironment.DrawAll();

				device.VideoDriver.EndScene();
			}
	}

	public void runShaderTest()
	{
		// testing irrlicht with shaders

		device = new IrrlichtDevice(SelectedDriverType,
			new Dimension2D(800,600), 16, false, true, false);

		device.EventReceiver = this;
		device.ResizeAble = true;
		device.WindowCaption = "Irrlicht.NET shader test";

		String vsFileName = "";
		String psFileName = "";
		UseHighLevelShaders = false;

		switch(SelectedDriverType)
		{
			case DriverType.DIRECT3D8: 
				psFileName = "../../media/d3d8.psh";
				vsFileName = "../../media/d3d8.vsh";
				break;
			case DriverType.DIRECT3D9:
				if (UseHighLevelShaders)
				{
					psFileName = "../../media/d3d9.hlsl";
					vsFileName = psFileName; // both shaders are in the same file
				}
				else
				{
					psFileName = "../../media/d3d9.psh";
					vsFileName = "../../media/d3d9.vsh";
				}
				break;
			case DriverType.OPENGL:
				if (UseHighLevelShaders)
				{
					psFileName = "../../media/opengl.frag";
					vsFileName = "../../media/opengl.vert";
				}
				else
				{
					psFileName = "../../media/opengl.psh";
					vsFileName = "../../media/opengl.vsh";
				}
				break;
		}

		// create materials
		int newMaterialType1 = 0;
		int newMaterialType2 = 0;

		if ( device.VideoDriver.GPUProgrammingServices != null )
		{
			IShaderConstantSetCallBack callBack = this;

			if (UseHighLevelShaders)
			{
				newMaterialType1 = device.VideoDriver.GPUProgrammingServices.AddHighLevelShaderMaterialFromFiles(
					vsFileName, "vertexMain", VertexShaderType.VST_VS_1_1,
					psFileName, "pixelMain", PixelShaderType.PST_PS_1_1,
					callBack, MaterialType.SOLID);

				newMaterialType2 = device.VideoDriver.GPUProgrammingServices.AddHighLevelShaderMaterialFromFiles(
					vsFileName, "vertexMain", VertexShaderType.VST_VS_1_1,
					psFileName, "pixelMain", PixelShaderType.PST_PS_1_1,
					callBack, MaterialType.TRANSPARENT_ADD_COLOR);
			}
			else
			{
				newMaterialType1 = device.VideoDriver.GPUProgrammingServices.AddShaderMaterialFromFiles(
					vsFileName, psFileName, callBack, MaterialType.SOLID );

				newMaterialType2 = device.VideoDriver.GPUProgrammingServices.AddShaderMaterialFromFiles(
					vsFileName, psFileName, callBack, MaterialType.TRANSPARENT_ADD_COLOR );
			}
		}

		// create test scene node 1, with the new created material type 1

		ISceneNode node = device.SceneManager.AddCubeSceneNode(50, null, -1, new Vector3D(0,0,0));
		node.SetMaterialTexture(0, device.VideoDriver.GetTexture("../../media/wall.bmp"));
		node.SetMaterialType((MaterialType)newMaterialType1);
		node.SetMaterialFlag(Irrlicht.Video.MaterialFlag.LIGHTING, false);

		//device.SceneManager.AddTextSceneNode( device.GUIEnvironment.BuiltInFont, 
		//	"PS & VS & EMT_SOLID", new Color(255,255,255,255), node, new Vector3D(0,0,0), -1);

		ISceneNodeAnimator anim = device.SceneManager.CreateRotationAnimator(
			new Vector3D(0,0.3f,0) );
		node.AddAnimator(anim);

		// create test scene node 1, with the new created material type 1

		node = device.SceneManager.AddCubeSceneNode(50, null, -1, new Vector3D(0,-10,50));
		node.SetMaterialTexture(0, device.VideoDriver.GetTexture("../../media/wall.bmp"));
		node.SetMaterialType((MaterialType)newMaterialType2);
		node.SetMaterialFlag(Irrlicht.Video.MaterialFlag.LIGHTING, false);

		//device.SceneManager.AddTextSceneNode( device.GUIEnvironment.BuiltInFont, 
		//	"PS & VS & EMT_TRANSPARENT", new Color(255,255,255,255), node, new Vector3D(0,0,0), -1);

		anim = device.SceneManager.CreateRotationAnimator(
			new Vector3D(0,0.3f,0) );
		node.AddAnimator(anim);

		//Then we add a third cube without a shader on it, to be able to compare the
		//cubes.

		node = device.SceneManager.AddCubeSceneNode(50, null, -1, new Vector3D(0,50,25));
		node.SetMaterialTexture(0, device.VideoDriver.GetTexture("../../media/wall.bmp"));
		node.SetMaterialFlag(Irrlicht.Video.MaterialFlag.LIGHTING, false);

		//device.SceneManager.AddTextSceneNode( device.GUIEnvironment.BuiltInFont, 
		//	"NO SHADER", new Color(255,255,255,255), node, new Vector3D(0,0,0), -1);

		// And last, we add a skybox and a user controlled camera to the scene.
	
		device.SceneManager.AddSkyBoxSceneNode(
			device.VideoDriver.GetTexture("../../media/irrlicht2_up.jpg"), 
			device.VideoDriver.GetTexture("../../media/irrlicht2_dn.jpg"),
			device.VideoDriver.GetTexture("../../media/irrlicht2_lf.jpg"),
			device.VideoDriver.GetTexture("../../media/irrlicht2_rt.jpg"),
			device.VideoDriver.GetTexture("../../media/irrlicht2_ft.jpg"),
			device.VideoDriver.GetTexture("../../media/irrlicht2_bk.jpg"), null, -1 );

		ICameraSceneNode cam = device.SceneManager.AddCameraSceneNodeFPS();
		cam.Target = new Vector3D(0,0,0);
		cam.Position = new Vector3D(-100,50,100);

		device.CursorControl.Visible = false;

		device.Run(); // fix for a temporary bug where quit messages are not be removed in the queue
		while(device.Run())
			if (device.WindowActive)
			{
				device.VideoDriver.BeginScene(true, true, new Color(255,0,0,50));

				device.SceneManager.DrawAll();
				device.GUIEnvironment.DrawAll();

				device.VideoDriver.EndScene();
			}
	}

	public void OnSetConstants( IMaterialRendererServices services )
	{
		Matrix4 invWorld = services.VideoDriver.GetTransform(TransformationState.WORLD);
		invWorld.MakeInverse();

		if (UseHighLevelShaders)
			services.SetVertexShaderConstant("mInvWorld", invWorld.GetFloats(), 16);
		else
			services.SetVertexShaderConstant(invWorld.GetFloats(), 0, 4);

		// set clip matrix

		Matrix4 worldViewProj;
		worldViewProj = services.VideoDriver.GetTransform(TransformationState.PROJECTION);			
		worldViewProj *= services.VideoDriver.GetTransform(TransformationState.VIEW);
		worldViewProj *= services.VideoDriver.GetTransform(TransformationState.WORLD);

		if (UseHighLevelShaders)
			services.SetVertexShaderConstant("mWorldViewProj", worldViewProj.GetFloats(), 16);
		else
			services.SetVertexShaderConstant(worldViewProj.GetFloats(), 4, 4);
		
		// set camera position

		Vector3D campos = device.SceneManager.ActiveCamera.Position;

		float[] pos = new float[4]; 
		pos[0] = campos.X;
		pos[1] = campos.Y;
		pos[2] = campos.Z;
		pos[3] = 0;

		if (UseHighLevelShaders)
			services.SetVertexShaderConstant("mLightPos", pos, 3);
		else
			services.SetVertexShaderConstant(pos, 8, 1);

		// set light color

		pos[0] = 0;
		pos[1] = 1;
		pos[2] = 1;
		pos[3] = 0;

		if (UseHighLevelShaders)
			services.SetVertexShaderConstant("mLightColor", pos, 4);
		else
			services.SetVertexShaderConstant(pos, 9, 1);

		// set transposed world matrix

		Matrix4 world = services.VideoDriver.GetTransform(TransformationState.WORLD);
		world = world.GetTransposed();

		if (UseHighLevelShaders)
			services.SetVertexShaderConstant("mTransWorld", world.GetFloats(), 16);
		else
			services.SetVertexShaderConstant(world.GetFloats(), 10, 4);

	}


	private DriverType SelectedDriverType = DriverType.DIRECT3D8;
	private TestScenarios ClickedButton = TestScenarios.NONE;
	private ISceneNode Shadow;
	private bool UseHighLevelShaders = false;
	private IrrlichtDevice device;
}