program Graphics2D_pas; 

{$APPTYPE CONSOLE} 

{%DelphiDotNetAssemblyCompiler '$(SystemRoot)\microsoft.net\framework\v1.1.4322\System.dll'} {%DelphiDotNetAssemblyCompiler '$(SystemRoot)\microsoft.net\framework\v1.1.4322\System.Data.dll'} {%DelphiDotNetAssemblyCompiler '$(SystemRoot)\microsoft.net\framework\v1.1.4322\System.Windows.Forms.dll'} {%DelphiDotNetAssemblyCompiler '$(SystemRoot)\microsoft.net\framework\v1.1.4322\System.Drawing.dll'} {%DelphiDotNetAssemblyCompiler '..\..\bin\win32-visualstudio\Irrlicht.NET.dll'} {%TogetherDiagram 'ModelSupport\default.txaPackage'} 


uses 

 System.Windows.Forms,
 Irrlicht,
 Irrlicht.Video,
 Irrlicht.Core,
 Irrlicht.Scene,
 Irrlicht.GUI;
var 

 device : IrrlichtDevice;
 texLogo : ITexture;
 fps : integer;
 s : char;
 driver : IVideoDriver;
 smgr :ISceneManager;
 images : ITexture;
 font, font2 : IGUIFont;
 imp1, imp2 : rect;
 time : Cardinal;
 m : Position2D;
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
 device.WindowCaption := 'Irrlicht.NET via Delphi.NET example 06 - - 2D Graphics Demo [ '+ device.VideoDriver.Name + ' ] fps : ' + fps.ToString;
 driver := device.VideoDriver;
 smgr := device.SceneManager;
 images := driver.GetTexture( '../../media/2ddemo.bmp' );
 driver.MakeColorKeyTexture(images, position2d.Create( 0, 0 ) );
 font := device.GUIEnvironment.BuiltInFont;
 font2 := device.GUIEnvironment.GetFont( '../../media/fonthaettenschweiler.bmp' );
 imp1 := rect.Create(349,15,385,78);
 imp2 := rect.Create(387,15,423,78);
 // The logo
 texLogo := driver.GetTexture('../../media/irrlichtlogoaligned.jpg');
 // make cursor invisible
 device.CursorControl.Visible := false;
 // start drawing loop
 fps := 0;
 while( device.Run ) do
 begin
   if (device.WindowActive) then
   begin
     //time := device.GetTimer().getTime();
     driver.BeginScene( true, true, Color.Create( 0, 120, 102, 136 ) );
     // draw fire & dragons background world
     driver.draw2DImage(images,
       position2d.Create(50,50),
       rect.Create(0,0,342,224),
       Irrlicht.Video.Color.Create(255,255,255,255), true);
     // draw flying imp
     if ((time div 500) mod 2) = 0 then
       driver.draw2DImage(images, position2d.Create(164,125),
        imp1,
        Irrlicht.Video.Color.Create(255,255,255,255), true)
     else
       driver.draw2DImage(images, position2d.Create(164,125),
        imp2, Irrlicht.Video.Color.Create(255,255,255,255), true);
     // draw second flying imp with colorcylce
     if ((time div 500) mod 2) = 0 then
       driver.draw2DImage(images, position2d.Create(270,105),
       imp1,
       Irrlicht.Video.Color.Create(255, time mod 255,255,255), true)
     else
     driver.draw2DImage(images, position2d.Create(270,105),
       imp2, 
       Irrlicht.Video.Color.Create(255, time mod 255,255,255), true);
     // draw some text
     if (font <> nil )then
       font.draw('This is some text.',
         rect.Create(130,10,300,50),
         Irrlicht.Video.Color.Create(255,255,255,255), false, false );
     // draw some other text
     if (font2 <> nil) then
        font2.draw('This is some other text.',
            rect.Create(130,20,300,60),
            Irrlicht.Video.Color.Create(255,time mod 255,time mod 255,255), false, false );
     // draw the logo
     driver.Draw2DImage(
       texLogo, Position2D.Create( 10, 10 ),
       Rect.Create( 0,0,88,31 ),
       Rect.Create( Position2D.Create( 0, 0 ),device.VideoDriver.ScreenSize),
       Color.Create($ffffff), false);
     // draw transparent rect under cursor
     m := device.CursorControl.Position;
     driver.draw2DRectangle(Irrlicht.Video.Color.Create(100,255,255,255),
           rect.Create(m.X-20, m.Y-20, m.X+20, m.Y+20));
     driver.EndScene();
     if (fps <> driver.FPS) then
     begin
       fps := driver.FPS;
       device.WindowCaption := 'Irrlicht.NET via Delphi.NET example 06 - - 2D Graphics Demo [ '+ device.VideoDriver.Name + ' ] fps : ' + fps.ToString;
     end;
   end;
 end; // end drawing-loop
end. 

Retrieved from "http://www.irrforge.org/index.php/Delphi.NET_version_of_Graphics2D"

This page has been accessed 258 times. This page was last modified 07:01, 20 Jan 2006. 

