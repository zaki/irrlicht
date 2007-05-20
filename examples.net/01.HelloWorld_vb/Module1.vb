Imports Irrlicht ' compiler error here? Look at the next comment
Imports Irrlicht.Video
Imports Irrlicht.Core
Imports Irrlicht.Scene

' PROBLEMS with this source? 
' Some solutions wich might help:
'
' - If your compiler complains that 'Irrlicht' cannot be found,
'  add the Irrlicht.NET.dll file (in bin\Win32-VisualStudio) as Reference
'  in the solution browser.
' 
' - It compiles, but your program throws an exception like this:
'  "An unhandled exception of type 'System.IO.FileNotFoundException' occurred in Unknown Module.
'  Additional information: File or assembly name Irrlicht.NET, or one of its dependencies,
'  was not found."
'  Solution: You are missing the file Irrlicht.DLL. Copy it where Visual studio can find it.
'  For example where this program is.


' Example application using Irrlicht.NET 
' A little bit more complicated than the C++ hello world example, 
' but hey, its the only one yet. Give me some time.
Module Module1

    ' The main entry point for the application.
    Sub Main()

        ' start up the engine

        Dim device As New IrrlichtDevice(Irrlicht.Video.DriverType.OPENGL)

        device.ResizeAble = True
        device.WindowCaption = "Irrlicht.NET VisualBasic example 01 - Hello World"

        ' load some textures 

        Dim texSydney As ITexture = device.VideoDriver.GetTexture("..\..\media\sydney.bmp")
        Dim texWall As ITexture = device.VideoDriver.GetTexture("..\..\media\wall.bmp")
        Dim texLogo As ITexture = device.VideoDriver.GetTexture("..\..\media\irrlichtlogoaligned.jpg")

        ' load the animated mesh of sydney

        Dim mesh As Irrlicht.Scene.IAnimatedMesh = device.SceneManager.GetMesh("..\..\media\sydney.md2")

        If mesh Is Nothing Then
            System.Windows.Forms.MessageBox.Show("Could not load mesh ..\..\media\sydney.md2, exiting.", "Problem starting program")
            Return
        End If

        ' add a camera, a test scene node and the animated mesh to the scene	

        Dim cam As ICameraSceneNode = device.SceneManager.AddCameraSceneNodeFPS(Nothing, 100, 100, -1)
        cam.Position = New Vector3D(20, 0, -50)

        Dim node As ISceneNode = device.SceneManager.AddCubeSceneNode(15, Nothing, -1, New Vector3D(30, -15, 0))
        node.SetMaterialTexture(0, texWall)

        node = device.SceneManager.AddAnimatedMeshSceneNode(mesh, Nothing, -1)
        node.SetMaterialTexture(0, texSydney)
        node.SetMaterialFlag(MaterialFlag.LIGHTING, False)

        ' disable mouse cursor
        device.CursorControl.Visible = False

        ' start drawing loop

        Dim fps As Integer = 0

        While device.Run() = True
            If device.WindowActive Then

                device.VideoDriver.BeginScene(True, True, New Color(0, 100, 100, 100))

                device.SceneManager.DrawAll()

                ' draw the logo

                device.VideoDriver.Draw2DImage(texLogo, New Position2D(10, 10), New Rect(0, 0, 88, 31), New Rect(New Position2D(0, 0), device.VideoDriver.ScreenSize), New Color(255, 255, 255, 255), False)
                device.VideoDriver.EndScene()

                If (fps <> device.VideoDriver.FPS) Then
                    fps = device.VideoDriver.FPS
                    device.WindowCaption = "Irrlicht.NET Visual Basic example 01 - Hello World [" + device.VideoDriver.Name + "] fps:" + fps.ToString
                End If

            End If
        End While

    End Sub

End Module
