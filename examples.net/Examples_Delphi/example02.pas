program Quake3Map_pas; 

{$APPTYPE CONSOLE} 

{%DelphiDotNetAssemblyCompiler '$(SystemRoot)\microsoft.net\framework\v1.1.4322\System.dll'} {%DelphiDotNetAssemblyCompiler '$(SystemRoot)\microsoft.net\framework\v1.1.4322\System.Data.dll'} {%DelphiDotNetAssemblyCompiler '$(SystemRoot)\microsoft.net\framework\v1.1.4322\System.Windows.Forms.dll'} {%DelphiDotNetAssemblyCompiler '$(SystemRoot)\microsoft.net\framework\v1.1.4322\System.Drawing.dll'} {%DelphiDotNetAssemblyCompiler '..\..\bin\win32-visualstudio\Irrlicht.NET.dll'} 


uses 

 System.Windows.Forms,
 Irrlicht,
 Irrlicht.Video,
 Irrlicht.Core,
 Irrlicht.Scene;
var 

 device : IrrlichtDevice;
 texLogo : ITexture;
 mesh : Irrlicht.Scene.IAnimatedMesh;
 node : ISceneNode;
 fps : integer;
 s : char;
 driver : IVideoDriver;
 smgr :ISceneManager;
begin 

 WriteLn( 'Please select the driver you want for this example :' );
 WriteLn( ' (a) Direct3D 9.0c' );
 WriteLn( ' (b) Direct3D 8.1' );
 WriteLn( ' (c) OpenGL 1.2' );
 WriteLn( ' (d) Software Renderer' );
 WriteLn( ' (e) NullDevice' );
 WriteLn( ' (otherKey) exit' );
 WriteLn(  );
 ReadLn( s );
 // start up the engine
 case s of
   'a' : device := IrrlichtDevice.Create(DriverType.DIRECTX9);
   'b' : device := IrrlichtDevice.Create(DriverType.DIRECTX8);
   'c' : device := IrrlichtDevice.Create(DriverType.OPENGL);
   'd' : device := IrrlichtDevice.Create(DriverType.SOFTWARE);
   'e' : device := IrrlichtDevice.Create(DriverType.NULL_DRIVER);
 else
   exit;
 end;
 device.ResizeAble := true;
 device.WindowCaption := 'Irrlicht.NET via Delphi.NET example 02 - Quake3Map [ '+ device.VideoDriver.Name + ' ] fps : ' + fps.ToString;
 driver := device.VideoDriver;
 smgr := device.SceneManager;
 {*
To display the Quake 3 map, we first need to load it. Quake 3 maps are packed into .pk3 files wich are nothing other than .zip files. So we add the .pk3 file to our FileSystem. After it was added, we are able to read from the files in that archive as they would directly be stored on disk. *} 

 device.FileSystem.AddZipFileArchive('../../media/map-20kdm2.pk3');
 {*
Now we can load the mesh by calling getMesh(). We get a pointer returned to a IAnimatedMesh. As you know, Quake 3 maps are not really animated, they are only a huge chunk of static geometry with some materials attached. Hence the IAnimated mesh consists of only one frame, so we get the "first frame" of the "animation", which is our quake level and create an OctTree scene node with it, using addOctTreeSceneNode(). The OctTree optimizes the scene a little bit, trying to draw only geometry which is currently visible. An alternative to the OctTree would be a AnimatedMeshSceneNode, which would draw always the complete geometry of the mesh, without optimization. Try it out: Write addAnimatedMeshSceneNode instead of addOctTreeSceneNode and compare the primitives drawed by the video driver. (There is a getPrimitiveCountDrawed() method in the IVideoDriver class). Note that this optimization with the Octree is only useful when drawing huge meshes consiting of lots of geometry. *} 

  // load the animated mesh of sydney
 mesh := smgr.GetMesh('20kdm2.bsp');
 node := nil;
 if (mesh <> nil) then
 begin
   node := smgr.addOctTreeSceneNode( mesh.GetMesh( 0 ), nil, 0 );
 end;
 {*
Because the level was modelled not around the origin (0,0,0), we translate the whole level a little bit. *} if (node <> nil) then node.Position := Vector3D.Create( -1300,-144,-1249 ); 

 {*
Now we only need a Camera to look at the Quake 3 map. And we want to create a user controlled camera. There are some different cameras available in the Irrlicht engine. For example the Maya Camera which can be controlled compareable to the camera in Maya: Rotate with left mouse button pressed, Zoom with both buttons pressed, translate with right mouse button pressed. This could be created with addCameraSceneNodeMaya(). But for this example, we want to create a camera which behaves like the ones in first person shooter games (FPS). *} smgr.AddCameraSceneNodeFPS(); 

 texLogo := driver.GetTexture('../../media/irrlichtlogoaligned.jpg');



 // make cursor invisible
 device.CursorControl.Visible := false;
 // start drawing loop
 fps := 0;
 while( device.Run ) do
 begin
   if (device.WindowActive) then
   begin
     driver.BeginScene( true, true, Color.Create( 0, 200, 200, 200 ) );
     smgr.DrawAll();
     // draw the logo
     driver.Draw2DImage(
       texLogo, Position2D.Create( 10, 10 ),
       Rect.Create( 0,0,88,31 ),
       Rect.Create( Position2D.Create( 0, 0 ),device.VideoDriver.ScreenSize),
       Color.Create($ffffff), false);
     driver.EndScene();
     if (fps <> driver.FPS) then
     begin
       fps := driver.FPS;
       device.WindowCaption := 'Irrlicht.NET via Delphi.NET example 02 - Quake3Map [ '+ device.VideoDriver.Name + ' ] fps : ' + fps.ToString;
     end;
   end;
 end; // end drawing-loop
end. 

// Retrieved from "http://www.irrforge.org/index.php/Delphi.NET_version_of_Quake3Map"
// This page has been accessed 364 times. This page was last modified 07:00, 20 Jan 2006. 

