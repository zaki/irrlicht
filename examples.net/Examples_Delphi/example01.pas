program HelloWorld_pas; 

{$APPTYPE CONSOLE} 

{%DelphiDotNetAssemblyCompiler '$(SystemRoot)\microsoft.net\framework\v1.1.4322\System.dll'} {%DelphiDotNetAssemblyCompiler '$(SystemRoot)\microsoft.net\framework\v1.1.4322\System.Data.dll'} {%DelphiDotNetAssemblyCompiler '$(SystemRoot)\microsoft.net\framework\v1.1.4322\System.Windows.Forms.dll'} {%DelphiDotNetAssemblyCompiler '..\..\bin\visualstudio\Irrlicht.NET.dll'} 


uses 

 System.Windows.Forms,
 Irrlicht,
 Irrlicht.Video,
 Irrlicht.Core,
 Irrlicht.Scene;
var 

 device : IrrlichtDevice;
 texSydney, texWall, texLogo : ITexture;
 mesh : Irrlicht.Scene.IAnimatedMesh;
 cam : ICameraSceneNode;
 node : ISceneNode;
 fps : integer;
 s : char;
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
 device.WindowCaption := 'Irrlicht.NET via Delphi.NET example 01 - Hello World [ '+ device.VideoDriver.Name + ' ] fps : ' + fps.ToString;
 // load some textures 
 texSydney := device.VideoDriver.GetTexture('..\..\media\sydney.bmp');
 texWall := device.VideoDriver.GetTexture('..\..\media\wall.bmp');
 texLogo := device.VideoDriver.GetTexture('..\..\media\irrlichtlogoaligned.jpg');
 // load the animated mesh of sydney
 mesh := device.SceneManager.GetMesh('..\..\media\sydney.md2');
 if (mesh = nil) then
 begin
   System.Windows.Forms.MessageBox.Show(
     'Could not load mesh ..\..\media\sydney.md2, exiting.',
     'Problem starting program');
   exit;
 end;
 // add a camera, a test scene node and the animated mesh to the scene	
 cam := device.SceneManager.AddCameraSceneNodeFPS(nil, 100, 100, -1);
 cam.Position := Vector3D.Create(20,0,-50);



 node := device.SceneManager.AddTestSceneNode(15, nil, -1, Vector3D.Create(30,-15,0));
 node.SetMaterialTexture(0, texWall);
 node := device.SceneManager.AddAnimatedMeshSceneNode(mesh, nil, -1);
 node.SetMaterialTexture(0, texSydney);
 node.SetMaterialFlag(MaterialFlag.LIGHTING, false);
 // make cursor invisible
 device.CursorControl.Visible := false;
 // start drawing loop
 fps := 0;
 while( device.Run ) do
 begin
   if (device.WindowActive) then
   begin
     device.VideoDriver.BeginScene( true, true, Color.Create( 0, 100, 100, 100 ) );
     device.SceneManager.DrawAll();
     // draw the logo
     device.VideoDriver.Draw2DImage(
       texLogo, Position2D.Create( 10, 10 ),
       Rect.Create( 0,0,88,31 ),
       Rect.Create( Position2D.Create( 0, 0 ),device.VideoDriver.ScreenSize),
       Color.Create($ffffff), false);
     device.VideoDriver.EndScene();
     if (fps <> device.VideoDriver.FPS) then
     begin
       fps := device.VideoDriver.FPS;
       device.WindowCaption := 'Irrlicht.NET via Delphi.NET example 01 - Hello World [ '+ device.VideoDriver.Name + ' ] fps : ' + fps.ToString;
     end;
   end;
 end; // end drawing-loop
end. 
