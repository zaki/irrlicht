namespace HelloWorld 
import System 
import Irrlicht 
import Irrlicht.Video 
import Irrlicht.Core 
import Irrlicht.Scene 

[Module] 
class Example: 
   [STAThread()] 
   static def Main(args as (string)): 
      device as IrrlichtDevice = IrrlichtDevice(DriverType.OPENGL) 
      device.WindowCaption = 'Irrlicht.NET Boo example 01 - Hello World' 
      texSydney as ITexture = device.VideoDriver.GetTexture('sydney.bmp') 
      mesh as Irrlicht.Scene.IAnimatedMesh = device.SceneManager.GetMesh('sydney.md2') 
      cam as ICameraSceneNode = device.SceneManager.AddCameraSceneNodeFPS(null, 100, 100, -1) 
      cam.Position = Vector3D(20, 0, -50) 
      node as ISceneNode = device.SceneManager.AddAnimatedMeshSceneNode(mesh, null, -1) 
      node.SetMaterialTexture(0, texSydney) 
      node.SetMaterialFlag(MaterialFlag.LIGHTING, false) 
      device.CursorControl.Visible = false 
      fps as int = 0 
      while device.Run(): 
         if device.WindowActive: 
            device.VideoDriver.BeginScene(true, true, Color(0, 100, 100, 100)) 
            device.SceneManager.DrawAll() 
            device.VideoDriver.EndScene() 
            if fps != device.VideoDriver.FPS: 
               fps = device.VideoDriver.FPS 
               device.WindowCaption = 'Irrlicht.NET Boo example 01 - Hello World [' + device.VideoDriver.Name + '] fps:' + fps

 	