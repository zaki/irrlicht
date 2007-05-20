' This code has been directly CONVERTED from C# into CV
'
'* This tutorial covers creating a mesh viewer.
'*
'* It is similar to the native tutorial, but different.
'* Since much of the gui and xml stuff is not in the
'* .NET wrapper yet, I decided to use winforms and
'* the .NET xml mush for those portions of the tutorial.
'*
'* Some of this tutorial is based on the work Braneloc did
'* on Tutorial 14.
'*
'* The tutorial also includes some other interesting stuff
'* that I decided to play with.
'*
'* For instance, you can change the driver in the middle of
'* looking at a mesh.
'* Since I used .net 2.0 & VS2005, you can also move the
'* toolbar around the form.  (A new freebe in .net 2.0)
'*
'* DISCLAIMERS:  This will not work with .net 1.1.
'* The reason is that some of the controls I used in this
'* tutorial are not available in .net 1.1.
'* Most of the controls that I know have changed are the
'* menu and toolbar stuff.
'* The new stuff now has names like "MenuStrip",
'* "ToolStrip", and "ToolStripMenuItem", etc.
'* Seems kinda silly, I know, but I'm sure someone over
'* in Redmond really thought they had a great idea.
'*
'* Also, MS changed their xml api in .net 2.0.
'* The xml part of the tutorial might port to .net 1.1
'* just fine, but I'm not sure and not ambitious enough
'* to check it out myself.
'*
'* BUGS:  Yeah, I'm sure there's a few in there.
'* I notice when switching drivers, the app will
'* sometimes lock.  I haven't quite figured out why yet,
'* but it seems to have something to do with Irrlicht
'* having a null device.EventReceiver.  Also, the
'* debugger will sometimes point to a problem with fonts
'* in this case.
'*
'* The winforms event responses are very doggy.
'* I tried a couple of Application.DoEvents() in the render loop,
'* but they didn't seem to help much.  I may
'* look at this in the future.
'*
'* FINAL NOTE:  If you happen to find any fixes for any of the
'* bugs listed above or any new ones you find, share them with
'* the community.  Post about them on the forum, and update
'* the Wiki.
'*
'* Getting it working: 
'* Create project as windows form, and add the 2 Irrlicht DLL's to it.
'* (This file is the only one needed in the project)
'* Add open.bmp and help.bmp from the Irrlicht media directory to the
'* resources.
'* You'll probably need to rebind the images to the toolstrip, hint,
'* open is the one on the left..
'
Imports System
Imports System.Collections.Generic
Imports System.Drawing
Imports System.Windows.Forms
Imports System.Text
Imports System.IO
Imports System.Xml
Imports Irrlicht
Imports core = Irrlicht.Core
Imports scene = Irrlicht.Scene
Imports video = Irrlicht.Video
Imports gui = Irrlicht.GUI
Namespace _09.WinFormMeshViewer
	 <summary>
	 Description of MainForm.
	 </summary>
	Public Class MainForm
	Inherits System.Windows.Forms.Form
		' Path from tutorial binary to standard SDK media folder.
		Private path As String = "../../../../media/"
		Private messageText As String = String.Empty
		Private caption As String = String.Empty
		Private captBase As String = "Irrlicht.NET with WinForms - MeshViewer"
		Private configFile As String = "config.xml"
		Private startupModelFile As String = String.Empty
		Private currentModelFile As String = String.Empty
		Private quitting As Boolean = False
		Private paused As Boolean = False
		Private device As IrrlichtDevice
		Private model As scene.IAnimatedMeshSceneNode
		Private skyBox As scene.ISceneNode
		Private irrLogo As video.ITexture
		Private startDelay As Timer
		Private pnlIrrlicht As Panel
		Private panelMemory As Rectangle = Rectangle.Empty
		' Remembers the size & location of the panel
		Private mnuFile As ToolStripMenuItem
		Private mnuFileOpen As ToolStripMenuItem
		Private toolStripMenuItem1 As ToolStripSeparator
		Private mnuFileExit As ToolStripMenuItem
		Private mnuView As ToolStripMenuItem
		Private mnuHelp As ToolStripMenuItem
		Private toolStripContainer1 As ToolStripContainer
		Private toolStrip1 As ToolStrip
		Private btnOpen As ToolStripButton
		Private btnHelp As ToolStripButton
		Private mnuViewSkyBox As ToolStripMenuItem
		Private mnuViewDebug As ToolStripMenuItem
		Private mnuViewMaterial As ToolStripMenuItem
		Private mnuViewMatSolid As ToolStripMenuItem
		Private mnuViewMatTransp As ToolStripMenuItem
		Private mnuViewMatReflect As ToolStripMenuItem
		Private mnuHelpAbout As ToolStripMenuItem
		Private lblScaleZ As Label
		Private numScaleZ As NumericUpDown
		Private lblScaleY As Label
		Private numScaleY As NumericUpDown
		Private lblScaleX As Label
		Private numScaleX As NumericUpDown
		Private chkDebug As CheckBox
		Private chkSkyBox As CheckBox
		Private rbSoft1 As RadioButton
		Private rbOGL As RadioButton
		Private rbD3D8 As RadioButton
		Private rbD3D9 As RadioButton
		Private rbNull As RadioButton
		Private rbSoft2 As RadioButton
		Private gbDrivers As GroupBox
		Private txtFPS As ToolStripTextBox
		Private mnuMain As MenuStrip
		Public Sub New()
			'
			' The InitializeComponent() call is required for Windows Forms designer support.
			'
			InitializeComponent()
			'
			' Add constructor code after the InitializeComponent() call.
			'
			startDelay = New Timer()
			startDelay.Interval = 1000
			startDelay.Enabled = False
			AddHandler startDelay.Tick, AddressOf startDelay_Tick
			CreatePanel()
		End Sub
		 <summary>
		 The main entry point for the application.
		 </summary>
		<STAThread()> _
		Private Shared Sub Main()
			Try
				' Create a new instance of our application
				Dim viewer As MainForm = New MainForm()
				'Show the window
				viewer.Show()
				' Initialize items
				viewer.LoadIrrConfig()
				If viewer.startupModelFile = String.Empty Then
					viewer.startupModelFile = "dwarf.x"
				End If
				viewer.currentModelFile = viewer.startupModelFile
				viewer.SetDevice(viewer.pnlIrrlicht, viewer.PickDriver())
				viewer.LoadModel(viewer.path + viewer.currentModelFile)
				viewer.LoadSkyBox()
				' Run the viewer
				System.Windows.Forms.Application.Run(viewer)
			Catch ex As Exception
				MessageBox.Show(ex.Message)
				Application.[Exit]()
				GC.Collect()
			End Try
		End Sub
		 <summary>
		 Required designer variable.
		 </summary>
		Private components As System.ComponentModel.IContainer = Nothing
		 <summary>
		 Clean up any resources being used.
		 </summary>
		 <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
		Protected Overloads Overrides Sub Dispose(ByVal disposing As Boolean)
			If disposing AndAlso (components IsNot Nothing) Then
				components.Dispose()
			End If
			MyBase.Dispose(disposing)
		End Sub
		#Region "Windows Forms Designer generated code"
		 <summary>
		 This method is required for Windows Forms designer support.
		 Do not change the method contents inside the source code editor. The Forms designer might
		 not be able to load this method if it was changed manually.
		 </summary>
		Private Sub InitializeComponent()
			Dim resources As System.ComponentModel.ComponentResourceManager = New System.ComponentModel.ComponentResourceManager(GetType(MainForm))
			Me.mnuMain = New System.Windows.Forms.MenuStrip()
			Me.mnuFile = New System.Windows.Forms.ToolStripMenuItem()
			Me.mnuFileOpen = New System.Windows.Forms.ToolStripMenuItem()
			Me.toolStripMenuItem1 = New System.Windows.Forms.ToolStripSeparator()
			Me.mnuFileExit = New System.Windows.Forms.ToolStripMenuItem()
			Me.mnuView = New System.Windows.Forms.ToolStripMenuItem()
			Me.mnuViewSkyBox = New System.Windows.Forms.ToolStripMenuItem()
			Me.mnuViewDebug = New System.Windows.Forms.ToolStripMenuItem()
			Me.mnuViewMaterial = New System.Windows.Forms.ToolStripMenuItem()
			Me.mnuViewMatSolid = New System.Windows.Forms.ToolStripMenuItem()
			Me.mnuViewMatTransp = New System.Windows.Forms.ToolStripMenuItem()
			Me.mnuViewMatReflect = New System.Windows.Forms.ToolStripMenuItem()
			Me.mnuHelp = New System.Windows.Forms.ToolStripMenuItem()
			Me.mnuHelpAbout = New System.Windows.Forms.ToolStripMenuItem()
			Me.toolStripContainer1 = New System.Windows.Forms.ToolStripContainer()
			Me.gbDrivers = New System.Windows.Forms.GroupBox()
			Me.rbNull = New System.Windows.Forms.RadioButton()
			Me.rbD3D9 = New System.Windows.Forms.RadioButton()
			Me.rbSoft2 = New System.Windows.Forms.RadioButton()
			Me.rbD3D8 = New System.Windows.Forms.RadioButton()
			Me.rbSoft1 = New System.Windows.Forms.RadioButton()
			Me.rbOGL = New System.Windows.Forms.RadioButton()
			Me.chkDebug = New System.Windows.Forms.CheckBox()
			Me.chkSkyBox = New System.Windows.Forms.CheckBox()
			Me.lblScaleZ = New System.Windows.Forms.Label()
			Me.numScaleZ = New System.Windows.Forms.NumericUpDown()
			Me.lblScaleY = New System.Windows.Forms.Label()
			Me.numScaleY = New System.Windows.Forms.NumericUpDown()
			Me.lblScaleX = New System.Windows.Forms.Label()
			Me.numScaleX = New System.Windows.Forms.NumericUpDown()
			Me.toolStrip1 = New System.Windows.Forms.ToolStrip()
			Me.btnOpen = New System.Windows.Forms.ToolStripButton()
			Me.btnHelp = New System.Windows.Forms.ToolStripButton()
			Me.txtFPS = New System.Windows.Forms.ToolStripTextBox()
			Me.mnuMain.SuspendLayout()
			Me.toolStripContainer1.ContentPanel.SuspendLayout()
			Me.toolStripContainer1.TopToolStripPanel.SuspendLayout()
			Me.toolStripContainer1.SuspendLayout()
			Me.gbDrivers.SuspendLayout()
			(DirectCast((Me.numScaleZ), System.ComponentModel.ISupportInitialize)).BeginInit()
			(DirectCast((Me.numScaleY), System.ComponentModel.ISupportInitialize)).BeginInit()
			(DirectCast((Me.numScaleX), System.ComponentModel.ISupportInitialize)).BeginInit()
			Me.toolStrip1.SuspendLayout()
			Me.SuspendLayout()
			' 
			' mnuMain
			' 
			Me.mnuMain.Items.AddRange(New System.Windows.Forms.ToolStripItem() {Me.mnuFile, Me.mnuView, Me.mnuHelp})
			Me.mnuMain.Location = New System.Drawing.Point(0, 0)
			Me.mnuMain.Name = "mnuMain"
			Me.mnuMain.Size = New System.Drawing.Size(632, 24)
			Me.mnuMain.TabIndex = 0
			Me.mnuMain.Text = "menuStrip1"
			' 
			' mnuFile
			' 
			Me.mnuFile.DropDownItems.AddRange(New System.Windows.Forms.ToolStripItem() {Me.mnuFileOpen, Me.toolStripMenuItem1, Me.mnuFileExit})
			Me.mnuFile.Name = "mnuFile"
			Me.mnuFile.Size = New System.Drawing.Size(35, 20)
			Me.mnuFile.Text = "&File"
			' 
			' mnuFileOpen
			' 
			Me.mnuFileOpen.Name = "mnuFileOpen"
			Me.mnuFileOpen.Size = New System.Drawing.Size(139, 22)
			Me.mnuFileOpen.Text = "&Open Mesh"
			AddHandler Me.mnuFileOpen.Click, AddressOf open_Click
			' 
			' toolStripMenuItem1
			' 
			Me.toolStripMenuItem1.Name = "toolStripMenuItem1"
			Me.toolStripMenuItem1.Size = New System.Drawing.Size(136, 6)
			' 
			' mnuFileExit
			' 
			Me.mnuFileExit.Name = "mnuFileExit"
			Me.mnuFileExit.Size = New System.Drawing.Size(139, 22)
			Me.mnuFileExit.Text = "E&xit"
			AddHandler Me.mnuFileExit.Click, AddressOf Exit_Click
			' 
			' mnuView
			' 
			Me.mnuView.DropDownItems.AddRange(New System.Windows.Forms.ToolStripItem() {Me.mnuViewSkyBox, Me.mnuViewDebug, Me.mnuViewMaterial})
			Me.mnuView.Name = "mnuView"
			Me.mnuView.Size = New System.Drawing.Size(41, 20)
			Me.mnuView.Text = "&View"
			' 
			' mnuViewSkyBox
			' 
			Me.mnuViewSkyBox.Checked = True
			Me.mnuViewSkyBox.CheckOnClick = True
			Me.mnuViewSkyBox.CheckState = System.Windows.Forms.CheckState.Checked
			Me.mnuViewSkyBox.Name = "mnuViewSkyBox"
			Me.mnuViewSkyBox.Size = New System.Drawing.Size(170, 22)
			Me.mnuViewSkyBox.Text = "&SkyBox Visibility"
			AddHandler Me.mnuViewSkyBox.CheckedChanged, AddressOf SkyBox_CheckedChanged
			' 
			' mnuViewDebug
			' 
			Me.mnuViewDebug.Checked = True
			Me.mnuViewDebug.CheckOnClick = True
			Me.mnuViewDebug.CheckState = System.Windows.Forms.CheckState.Checked
			Me.mnuViewDebug.Name = "mnuViewDebug"
			Me.mnuViewDebug.Size = New System.Drawing.Size(170, 22)
			Me.mnuViewDebug.Text = "Model &Debug Info"
			AddHandler Me.mnuViewDebug.CheckedChanged, AddressOf Debug_CheckedChanged
			' 
			' mnuViewMaterial
			' 
			Me.mnuViewMaterial.DropDownItems.AddRange(New System.Windows.Forms.ToolStripItem() {Me.mnuViewMatSolid, Me.mnuViewMatTransp, Me.mnuViewMatReflect})
			Me.mnuViewMaterial.Name = "mnuViewMaterial"
			Me.mnuViewMaterial.Size = New System.Drawing.Size(170, 22)
			Me.mnuViewMaterial.Text = "&Model Material"
			' 
			' mnuViewMatSolid
			' 
			Me.mnuViewMatSolid.Checked = True
			Me.mnuViewMatSolid.CheckState = System.Windows.Forms.CheckState.Checked
			Me.mnuViewMatSolid.Name = "mnuViewMatSolid"
			Me.mnuViewMatSolid.Size = New System.Drawing.Size(144, 22)
			Me.mnuViewMatSolid.Text = "S&olid"
			AddHandler Me.mnuViewMatSolid.Click, AddressOf mnuViewMatSolid_Click
			' 
			' mnuViewMatTransp
			' 
			Me.mnuViewMatTransp.Name = "mnuViewMatTransp"
			Me.mnuViewMatTransp.Size = New System.Drawing.Size(144, 22)
			Me.mnuViewMatTransp.Text = "&Transparent"
			AddHandler Me.mnuViewMatTransp.Click, AddressOf mnuViewMatTransp_Click
			' 
			' mnuViewMatReflect
			' 
			Me.mnuViewMatReflect.Name = "mnuViewMatReflect"
			Me.mnuViewMatReflect.Size = New System.Drawing.Size(144, 22)
			Me.mnuViewMatReflect.Text = "Reflection"
			AddHandler Me.mnuViewMatReflect.Click, AddressOf mnuViewMatReflect_Click
			' 
			' mnuHelp
			' 
			Me.mnuHelp.DropDownItems.AddRange(New System.Windows.Forms.ToolStripItem() {Me.mnuHelpAbout})
			Me.mnuHelp.Name = "mnuHelp"
			Me.mnuHelp.Size = New System.Drawing.Size(40, 20)
			Me.mnuHelp.Text = "&Help"
			' 
			' mnuHelpAbout
			' 
			Me.mnuHelpAbout.Name = "mnuHelpAbout"
			Me.mnuHelpAbout.Size = New System.Drawing.Size(114, 22)
			Me.mnuHelpAbout.Text = "&About"
			AddHandler Me.mnuHelpAbout.Click, AddressOf Help_Click
			' 
			' toolStripContainer1
			' 
			' 
			' toolStripContainer1.ContentPanel
			' 
			Me.toolStripContainer1.ContentPanel.Controls.Add(Me.gbDrivers)
			Me.toolStripContainer1.ContentPanel.Controls.Add(Me.chkDebug)
			Me.toolStripContainer1.ContentPanel.Controls.Add(Me.chkSkyBox)
			Me.toolStripContainer1.ContentPanel.Controls.Add(Me.lblScaleZ)
			Me.toolStripContainer1.ContentPanel.Controls.Add(Me.numScaleZ)
			Me.toolStripContainer1.ContentPanel.Controls.Add(Me.lblScaleY)
			Me.toolStripContainer1.ContentPanel.Controls.Add(Me.numScaleY)
			Me.toolStripContainer1.ContentPanel.Controls.Add(Me.lblScaleX)
			Me.toolStripContainer1.ContentPanel.Controls.Add(Me.numScaleX)
			Me.toolStripContainer1.ContentPanel.Size = New System.Drawing.Size(632, 397)
			Me.toolStripContainer1.Dock = System.Windows.Forms.DockStyle.Fill
			Me.toolStripContainer1.Location = New System.Drawing.Point(0, 24)
			Me.toolStripContainer1.Name = "toolStripContainer1"
			Me.toolStripContainer1.Size = New System.Drawing.Size(632, 422)
			Me.toolStripContainer1.TabIndex = 1
			Me.toolStripContainer1.Text = "toolStripContainer1"
			' 
			' toolStripContainer1.TopToolStripPanel
			' 
			Me.toolStripContainer1.TopToolStripPanel.Controls.Add(Me.toolStrip1)
			' 
			' gbDrivers
			' 
			Me.gbDrivers.Anchor = (DirectCast(((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right)), System.Windows.Forms.AnchorStyles))
			Me.gbDrivers.Controls.Add(Me.rbNull)
			Me.gbDrivers.Controls.Add(Me.rbD3D9)
			Me.gbDrivers.Controls.Add(Me.rbSoft2)
			Me.gbDrivers.Controls.Add(Me.rbD3D8)
			Me.gbDrivers.Controls.Add(Me.rbSoft1)
			Me.gbDrivers.Controls.Add(Me.rbOGL)
			Me.gbDrivers.Location = New System.Drawing.Point(510, 223)
			Me.gbDrivers.Name = "gbDrivers"
			Me.gbDrivers.Size = New System.Drawing.Size(104, 162)
			Me.gbDrivers.TabIndex = 15
			Me.gbDrivers.TabStop = False
			Me.gbDrivers.Text = "Driver Selection:"
			' 
			' rbNull
			' 
			Me.rbNull.AutoSize = True
			Me.rbNull.Location = New System.Drawing.Point(6, 134)
			Me.rbNull.Name = "rbNull"
			Me.rbNull.Size = New System.Drawing.Size(72, 17)
			Me.rbNull.TabIndex = 14
			Me.rbNull.TabStop = True
			Me.rbNull.Text = "null Driver"
			Me.rbNull.UseVisualStyleBackColor = True
			AddHandler Me.rbNull.CheckedChanged, AddressOf rb_CheckedChanged
			' 
			' rbD3D9
			' 
			Me.rbD3D9.AutoSize = True
			Me.rbD3D9.Checked = True
			Me.rbD3D9.Location = New System.Drawing.Point(6, 19)
			Me.rbD3D9.Name = "rbD3D9"
			Me.rbD3D9.Size = New System.Drawing.Size(91, 17)
			Me.rbD3D9.TabIndex = 9
			Me.rbD3D9.TabStop = True
			Me.rbD3D9.Text = "Direct3D 9.0c"
			Me.rbD3D9.UseVisualStyleBackColor = True
			AddHandler Me.rbD3D9.CheckedChanged, AddressOf rb_CheckedChanged
			' 
			' rbSoft2
			' 
			Me.rbSoft2.AutoSize = True
			Me.rbSoft2.Location = New System.Drawing.Point(6, 111)
			Me.rbSoft2.Name = "rbSoft2"
			Me.rbSoft2.Size = New System.Drawing.Size(75, 17)
			Me.rbSoft2.TabIndex = 13
			Me.rbSoft2.TabStop = True
			Me.rbSoft2.Text = "Apfelbaum"
			Me.rbSoft2.UseVisualStyleBackColor = True
			AddHandler Me.rbSoft2.CheckedChanged, AddressOf rb_CheckedChanged
			' 
			' rbD3D8
			' 
			Me.rbD3D8.AutoSize = True
			Me.rbD3D8.Location = New System.Drawing.Point(6, 42)
			Me.rbD3D8.Name = "rbD3D8"
			Me.rbD3D8.Size = New System.Drawing.Size(85, 17)
			Me.rbD3D8.TabIndex = 10
			Me.rbD3D8.TabStop = True
			Me.rbD3D8.Text = "Direct3D 8.1"
			Me.rbD3D8.UseVisualStyleBackColor = True
			AddHandler Me.rbD3D8.CheckedChanged, AddressOf rb_CheckedChanged
			' 
			' rbSoft1
			' 
			Me.rbSoft1.AutoSize = True
			Me.rbSoft1.Location = New System.Drawing.Point(6, 88)
			Me.rbSoft1.Name = "rbSoft1"
			Me.rbSoft1.Size = New System.Drawing.Size(67, 17)
			Me.rbSoft1.TabIndex = 12
			Me.rbSoft1.TabStop = True
			Me.rbSoft1.Text = "Software"
			Me.rbSoft1.UseVisualStyleBackColor = True
			AddHandler Me.rbSoft1.CheckedChanged, AddressOf rb_CheckedChanged
			' 
			' rbOGL
			' 
			Me.rbOGL.AutoSize = True
			Me.rbOGL.Location = New System.Drawing.Point(6, 65)
			Me.rbOGL.Name = "rbOGL"
			Me.rbOGL.Size = New System.Drawing.Size(83, 17)
			Me.rbOGL.TabIndex = 11
			Me.rbOGL.TabStop = True
			Me.rbOGL.Text = "OpenGL 1.5"
			Me.rbOGL.UseVisualStyleBackColor = True
			AddHandler Me.rbOGL.CheckedChanged, AddressOf rb_CheckedChanged
			' 
			' chkDebug
			' 
			Me.chkDebug.Anchor = (DirectCast(((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right)), System.Windows.Forms.AnchorStyles))
			Me.chkDebug.AutoSize = True
			Me.chkDebug.Checked = True
			Me.chkDebug.CheckState = System.Windows.Forms.CheckState.Checked
			Me.chkDebug.Location = New System.Drawing.Point(510, 193)
			Me.chkDebug.Name = "chkDebug"
			Me.chkDebug.Size = New System.Drawing.Size(113, 17)
			Me.chkDebug.TabIndex = 8
			Me.chkDebug.Text = "Debug Information"
			Me.chkDebug.UseVisualStyleBackColor = True
			AddHandler Me.chkDebug.CheckedChanged, AddressOf Debug_CheckedChanged
			' 
			' chkSkyBox
			' 
			Me.chkSkyBox.Anchor = (DirectCast(((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right)), System.Windows.Forms.AnchorStyles))
			Me.chkSkyBox.AutoSize = True
			Me.chkSkyBox.Checked = True
			Me.chkSkyBox.CheckState = System.Windows.Forms.CheckState.Checked
			Me.chkSkyBox.Location = New System.Drawing.Point(510, 170)
			Me.chkSkyBox.Name = "chkSkyBox"
			Me.chkSkyBox.Size = New System.Drawing.Size(101, 17)
			Me.chkSkyBox.TabIndex = 7
			Me.chkSkyBox.Text = "SkyBox Visibility"
			Me.chkSkyBox.UseVisualStyleBackColor = True
			AddHandler Me.chkSkyBox.CheckedChanged, AddressOf SkyBox_CheckedChanged
			' 
			' lblScaleZ
			' 
			Me.lblScaleZ.Anchor = (DirectCast(((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right)), System.Windows.Forms.AnchorStyles))
			Me.lblScaleZ.AutoSize = True
			Me.lblScaleZ.Location = New System.Drawing.Point(510, 116)
			Me.lblScaleZ.Name = "lblScaleZ"
			Me.lblScaleZ.Size = New System.Drawing.Size(55, 13)
			Me.lblScaleZ.TabIndex = 6
			Me.lblScaleZ.Text = "Z Scaling:"
			' 
			' numScaleZ
			' 
			Me.numScaleZ.Anchor = (DirectCast(((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right)), System.Windows.Forms.AnchorStyles))
			Me.numScaleZ.DecimalPlaces = 1
			Me.numScaleZ.Increment = New Decimal(New Integer() {1, 0, 0, 65536})
			Me.numScaleZ.Location = New System.Drawing.Point(510, 135)
			Me.numScaleZ.Name = "numScaleZ"
			Me.numScaleZ.Size = New System.Drawing.Size(61, 20)
			Me.numScaleZ.TabIndex = 5
			Me.numScaleZ.Value = New Decimal(New Integer() {10, 0, 0, 65536})
			AddHandler Me.numScaleZ.ValueChanged, AddressOf Scale_ValueChanged
			' 
			' lblScaleY
			' 
			Me.lblScaleY.Anchor = (DirectCast(((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right)), System.Windows.Forms.AnchorStyles))
			Me.lblScaleY.AutoSize = True
			Me.lblScaleY.Location = New System.Drawing.Point(510, 63)
			Me.lblScaleY.Name = "lblScaleY"
			Me.lblScaleY.Size = New System.Drawing.Size(55, 13)
			Me.lblScaleY.TabIndex = 4
			Me.lblScaleY.Text = "Y Scaling:"
			' 
			' numScaleY
			' 
			Me.numScaleY.Anchor = (DirectCast(((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right)), System.Windows.Forms.AnchorStyles))
			Me.numScaleY.DecimalPlaces = 1
			Me.numScaleY.Increment = New Decimal(New Integer() {1, 0, 0, 65536})
			Me.numScaleY.Location = New System.Drawing.Point(510, 82)
			Me.numScaleY.Name = "numScaleY"
			Me.numScaleY.Size = New System.Drawing.Size(61, 20)
			Me.numScaleY.TabIndex = 3
			Me.numScaleY.Value = New Decimal(New Integer() {10, 0, 0, 65536})
			AddHandler Me.numScaleY.ValueChanged, AddressOf Scale_ValueChanged
			' 
			' lblScaleX
			' 
			Me.lblScaleX.Anchor = (DirectCast(((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right)), System.Windows.Forms.AnchorStyles))
			Me.lblScaleX.AutoSize = True
			Me.lblScaleX.Location = New System.Drawing.Point(510, 12)
			Me.lblScaleX.Name = "lblScaleX"
			Me.lblScaleX.Size = New System.Drawing.Size(55, 13)
			Me.lblScaleX.TabIndex = 2
			Me.lblScaleX.Text = "X Scaling:"
			' 
			' numScaleX
			' 
			Me.numScaleX.Anchor = (DirectCast(((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right)), System.Windows.Forms.AnchorStyles))
			Me.numScaleX.DecimalPlaces = 1
			Me.numScaleX.Increment = New Decimal(New Integer() {1, 0, 0, 65536})
			Me.numScaleX.Location = New System.Drawing.Point(510, 31)
			Me.numScaleX.Name = "numScaleX"
			Me.numScaleX.Size = New System.Drawing.Size(61, 20)
			Me.numScaleX.TabIndex = 1
			Me.numScaleX.Value = New Decimal(New Integer() {10, 0, 0, 65536})
			AddHandler Me.numScaleX.ValueChanged, AddressOf Scale_ValueChanged
			' 
			' toolStrip1
			' 
			Me.toolStrip1.Dock = System.Windows.Forms.DockStyle.None
			Me.toolStrip1.Items.AddRange(New System.Windows.Forms.ToolStripItem() {Me.btnOpen, Me.btnHelp, Me.txtFPS})
			Me.toolStrip1.Location = New System.Drawing.Point(3, 0)
			Me.toolStrip1.Name = "toolStrip1"
			Me.toolStrip1.Size = New System.Drawing.Size(160, 25)
			Me.toolStrip1.TabIndex = 0
			' 
			' btnOpen
			' 
			Me.btnOpen.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
			Me.btnOpen.Image = (DirectCast((resources.GetObject("btnOpen.Image")), System.Drawing.Image))
			Me.btnOpen.ImageTransparentColor = System.Drawing.Color.Magenta
			Me.btnOpen.Name = "btnOpen"
			Me.btnOpen.Size = New System.Drawing.Size(23, 22)
			Me.btnOpen.Text = "toolStripButton1"
			AddHandler Me.btnOpen.Click, AddressOf open_Click
			' 
			' btnHelp
			' 
			Me.btnHelp.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
			Me.btnHelp.Image = (DirectCast((resources.GetObject("btnHelp.Image")), System.Drawing.Image))
			Me.btnHelp.ImageTransparentColor = System.Drawing.Color.Magenta
			Me.btnHelp.Name = "btnHelp"
			Me.btnHelp.Size = New System.Drawing.Size(23, 22)
			Me.btnHelp.Text = "btnHelp"
			' 
			' txtFPS
			' 
			Me.txtFPS.Name = "txtFPS"
			Me.txtFPS.Size = New System.Drawing.Size(100, 25)
			' 
			' MainForm
			' 
			Me.AutoScaleDimensions = New System.Drawing.SizeF(6F, 13F)
			Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
			Me.ClientSize = New System.Drawing.Size(632, 446)
			Me.Controls.Add(Me.toolStripContainer1)
			Me.Controls.Add(Me.mnuMain)
			Me.Icon = (DirectCast((resources.GetObject("$this.Icon")), System.Drawing.Icon))
			Me.MainMenuStrip = Me.mnuMain
			Me.MinimumSize = New System.Drawing.Size(640, 480)
			Me.Name = "MainForm"
			Me.Text = "Irrlicht with WinForms MeshViewer"
			AddHandler Me.FormClosing, AddressOf MainForm_FormClosing
			AddHandler Me.Load, AddressOf MainForm_Load
			Me.mnuMain.ResumeLayout(False)
			Me.mnuMain.PerformLayout()
			Me.toolStripContainer1.ContentPanel.ResumeLayout(False)
			Me.toolStripContainer1.ContentPanel.PerformLayout()
			Me.toolStripContainer1.TopToolStripPanel.ResumeLayout(False)
			Me.toolStripContainer1.TopToolStripPanel.PerformLayout()
			Me.toolStripContainer1.ResumeLayout(False)
			Me.toolStripContainer1.PerformLayout()
			Me.gbDrivers.ResumeLayout(False)
			Me.gbDrivers.PerformLayout()
			(DirectCast((Me.numScaleZ), System.ComponentModel.ISupportInitialize)).EndInit()
			(DirectCast((Me.numScaleY), System.ComponentModel.ISupportInitialize)).EndInit()
			(DirectCast((Me.numScaleX), System.ComponentModel.ISupportInitialize)).EndInit()
			Me.toolStrip1.ResumeLayout(False)
			Me.toolStrip1.PerformLayout()
			Me.ResumeLayout(False)
			Me.PerformLayout()
		End Sub
		#End Region
		#Region "- Winforms Events -"
		Private Sub MainForm_Load(ByVal sender As Object, ByVal e As EventArgs)
			startDelay.Enabled = True
		End Sub
		Private Sub startDelay_Tick(ByVal sender As Object, ByVal e As EventArgs)
			startDelay.Enabled = False
			RunDevice(pnlIrrlicht)
		End Sub
		Private Sub open_Click(ByVal sender As Object, ByVal e As EventArgs)
			Dim filter As StringBuilder = New StringBuilder()
			filter.Append("3D Studio Mesh(*.3ds)|*.3ds")
			filter.Append("|Alias Wavefront Maya(*.obj)|*.obj")
			filter.Append("|Cartography Shop 4(*.csm)|*.csm")
			filter.Append("|COLLADA(*.xml;*.dae)|*.xml;*.dae")
			filter.Append("|DeleD(*.dmf)|*.dmf")
			filter.Append("|FSRad oct(*.oct)|*.oct")
			filter.Append("|Microsoft DirectX(*.x)|*.x")
			filter.Append("|Milkshape (*.ms3d)|*.ms3d")
			filter.Append("|My3DTools 3(*my3D)|*.my3D")
			filter.Append("|Pulsar LMTools(*.lmts)|*.lmts")
			filter.Append("|Quake 3 levels(*.bsp)|*.bsp")
			filter.Append("|Quake 2 models(*.md2)|*.md2")
			filter.Append("|Packed models(*.pk3)|*.pk3")
			Dim ofd As OpenFileDialog = New OpenFileDialog()
			ofd.Filter = filter.ToString()
			ofd.FilterIndex = 7
			If ofd.ShowDialog() = DialogResult.OK Then
				LoadModel(ofd.FileName)
			End If
		End Sub
		Private Sub Exit_Click(ByVal sender As Object, ByVal e As EventArgs)
			Me.Close()
		End Sub
		Private Sub MainForm_FormClosing(ByVal sender As Object, ByVal e As FormClosingEventArgs)
			quitting = True
		End Sub
		Private Sub Help_Click(ByVal sender As Object, ByVal e As EventArgs)
			caption = "Irrlicht.NET engine mesh viewer - "
			If device IsNot Nothing AndAlso device.VideoDriver IsNot Nothing Then
				caption += device.VideoDriver.Name
			End If
			MessageBox.Show(messageText, caption)
		End Sub
		Private Sub rb_CheckedChanged(ByVal sender As Object, ByVal e As EventArgs)
			Try
				paused = True
				SetDevice(pnlIrrlicht, PickDriver())
				LoadModel(path + currentModelFile)
				LoadSkyBox()
				paused = False
				RunDevice(pnlIrrlicht)
			Catch ex As Exception
				MessageBox.Show(ex.Message, caption)
			End Try
		End Sub
		Private Sub SkyBox_CheckedChanged(ByVal sender As Object, ByVal e As EventArgs)
			If sender.[GetType]() = GetType(CheckBox) Then
				mnuViewSkyBox.Checked = chkSkyBox.Checked
			Else
				chkSkyBox.Checked = mnuViewSkyBox.Checked
			End If
			If skyBox IsNot Nothing Then
				skyBox.Visible = chkSkyBox.Checked
			End If
		End Sub
		Private Sub Debug_CheckedChanged(ByVal sender As Object, ByVal e As EventArgs)
			If sender.[GetType]() = GetType(CheckBox) Then
				mnuViewDebug.Checked = chkDebug.Checked
			Else
				chkDebug.Checked = mnuViewDebug.Checked
			End If
			If model IsNot Nothing Then
				model.DebugDataVisible = chkDebug.Checked
			End If
		End Sub
		Private Sub mnuViewMatSolid_Click(ByVal sender As Object, ByVal e As EventArgs)
			mnuViewMatSolid.Checked = True
			mnuViewMatTransp.Checked = False
			mnuViewMatReflect.Checked = False
			If model IsNot Nothing Then
				model.SetMaterialType(video.MaterialType.SOLID)
			End If
		End Sub
		Private Sub mnuViewMatTransp_Click(ByVal sender As Object, ByVal e As EventArgs)
			mnuViewMatSolid.Checked = False
			mnuViewMatTransp.Checked = True
			mnuViewMatReflect.Checked = False
			If model IsNot Nothing Then
				model.SetMaterialType(video.MaterialType.TRANSPARENT_ADD_COLOR)
			End If
		End Sub
		Private Sub mnuViewMatReflect_Click(ByVal sender As Object, ByVal e As EventArgs)
			mnuViewMatSolid.Checked = False
			mnuViewMatTransp.Checked = False
			mnuViewMatReflect.Checked = True
			If model IsNot Nothing Then
				model.SetMaterialType(video.MaterialType.SPHERE_MAP)
			End If
		End Sub
		Private Sub Scale_ValueChanged(ByVal sender As Object, ByVal e As EventArgs)
			Dim scale As core.Vector3D
			scale.X = DirectCast(numScaleX.Value, Single)
			scale.Y = DirectCast(numScaleY.Value, Single)
			scale.Z = DirectCast(numScaleZ.Value, Single)
			If model IsNot Nothing Then
				model.Scale = scale
			End If
		End Sub
		#End Region
		#Region "- Resource Loading -"
		Private Sub LoadIrrConfig()
			If File.Exists(path + configFile) Then
				Dim xtr As XmlTextReader = New XmlTextReader(path + configFile)
				Try
					While xtr.Read()
						If (xtr.NodeType = XmlNodeType.Element) Then
							If xtr.LocalName = "startUpModel" Then
								xtr.MoveToFirstAttribute()
								startupModelFile = xtr.Value
								' Clean the leading stuff from the filename:
								Dim lastSlash As Integer = startupModelFile.LastIndexOf("/")
								startupModelFile = startupModelFile.Substring(lastSlash + 1)
								xtr.MoveToElement()
ElseIf xtr.LocalName = "messageText" Then
								xtr.MoveToFirstAttribute()
								captBase = xtr.Value
								xtr.MoveToElement()
								messageText = xtr.ReadElementString()
							End If
						End If
					End While
				Catch ex As Exception
					MessageBox.Show(ex.Message)
				End Try
				xtr.Close()
			End If
		End Sub
		 <summary>
		 This method loads a model and displays it using an
		 addAnimatedMeshSceneNode and the scene manager. Nothing difficult. It also
		 displays a short message box, if the model could not be loaded.
		 </summary>
		 <param name="fileName">File name of the model to be loaded.</param>
		Private Sub LoadModel(ByVal fileName As String)
			' Check if we have a valid device to use.
			If device Is Nothing Then
				MessageBox.Show("Can't load model because the device is not created.", caption)
				Return
			End If
			Dim foundBSP As Boolean = False
			' modify the name if it a .pk3 file
			If fileName.EndsWith(".pk3") Then
				device.FileSystem.AddZipFileArchive(fileName)
				foundBSP = True
			End If
			' Clear the model if there was something loaded before.
			If model IsNot Nothing Then
				model.Remove()
			End If
			' Load a mesh from file.
			Dim m As scene.IAnimatedMesh = device.SceneManager.GetMesh(fileName)
			If m Is Nothing Then
				' Model could not be loaded
				If startupModelFile <> fileName Then
					Dim [error] As String = "The model could not be loaded." & Chr(10) & "Maybe it is not a supported file format."
					MessageBox.Show([error], caption)
				End If
				Return
			End If
			' load a model into the engine from the mesh
			model = device.SceneManager.AddAnimatedMeshSceneNode(m, Nothing, 0)
			' set default material properties
			If Not foundBSP Then
				If mnuViewMatSolid.Checked Then
					model.SetMaterialType(video.MaterialType.SOLID)
ElseIf mnuViewMatTransp.Checked Then
					model.SetMaterialType(video.MaterialType.TRANSPARENT_ADD_COLOR)
ElseIf mnuViewMatReflect.Checked Then
					model.SetMaterialType(video.MaterialType.SPHERE_MAP)
				End If
			End If
			model.SetMaterialFlag(video.MaterialFlag.LIGHTING, False)
			model.DebugDataVisible = True
			model.AnimationSpeed = 1000
		End Sub
		 <summary>
		 Loads the skybox using the device's driver & scene manager.
		 </summary>
		Private Sub LoadSkyBox()
			If device Is Nothing Then
				MessageBox.Show("Couldn't load skybox because device was null.", caption)
				Return
			End If
			Dim driver As video.IVideoDriver = device.VideoDriver
			Dim smgr As scene.ISceneManager = device.SceneManager
			skyBox = smgr.AddSkyBoxSceneNode(driver.GetTexture(path + "irrlicht2_up.jpg"), driver.GetTexture(path + "irrlicht2_dn.jpg"), driver.GetTexture(path + "irrlicht2_lf.jpg"), driver.GetTexture(path + "irrlicht2_rt.jpg"), driver.GetTexture(path + "irrlicht2_ft.jpg"), driver.GetTexture(path + "irrlicht2_bk.jpg"), _ 
				Nothing, - 1)
			' null & -1 are the defaults in the native engine.
			If skyBox IsNot Nothing Then
				skyBox.Visible = chkSkyBox.Checked
			End If
		End Sub
		#End Region
		#Region "- Device selection & setup -"
		 <summary>
		 Gets the <see cref="Irrlicht.Video.DriverType"/> that the user selected.
		 </summary>
		 <returns><see cref="Irrlicht.Video.DriverType"/> the user selected.</returns>
		Private Function PickDriver() As video.DriverType
			If rbD3D9.Checked Then
				Return Irrlicht.Video.DriverType.DIRECT3D9
			End If
			If rbD3D8.Checked Then
				Return Irrlicht.Video.DriverType.DIRECT3D8
			End If
			If rbOGL.Checked Then
				Return Irrlicht.Video.DriverType.OPENGL
			End If
			If rbSoft1.Checked Then
				Return Irrlicht.Video.DriverType.SOFTWARE
			End If
			If rbSoft2.Checked Then
				Return Irrlicht.Video.DriverType.SOFTWARE2
			End If
			Return Irrlicht.Video.DriverType.NULL_DRIVER
		End Function
		 <summary>
		 Sets up the device to run in a <see cref="Control"/> with the specified <see cref="Irrlicht.Video.DriverType"/>.
		 </summary>
		 <param name="c">Winforms <see cref="Control"/> that Irrlicht is to render in.</param>
		 <param name="driver"><see cref="Irrlicht.Video.DriverType"/> to use when setting up the IrrlichtDevice.</param>
		 <returns>True if the setup went well, False otherwise.</returns>
		Private Function SetDevice(ByVal c As Control, ByVal driverType As video.DriverType) As Boolean
			If quitting Then
				Return False
			End If
			If device IsNot Nothing Then
				device.CloseDevice()
			End If
			CreatePanel()
			device = New IrrlichtDevice(driverType, New core.Dimension2D(c.ClientRectangle.Width, c.ClientRectangle.Height), 32, False, False, False, _ 
				True, c.Handle)
			If device Is Nothing Then
				Return False
			End If
			device.ResizeAble = True
			Dim env As gui.IGUIEnvironment = device.GUIEnvironment
			Dim driver As video.IVideoDriver = device.VideoDriver
			Dim smgr As scene.ISceneManager = device.SceneManager
			driver.SetTextureCreationFlag(video.TextureCreationFlag.ALWAYS_32_BIT, True)
			' This sets the gui font from a bmp file.
			Dim skin As gui.IGUISkin = env.Skin
			Dim font As gui.IGUIFont = env.GetFont(path + "fonthaettenschweiler.bmp")
			If font IsNot Nothing Then
				skin.Font = font
			End If
			' This is loaded with the default values for the native engine.
			smgr.AddCameraSceneNodeMaya(Nothing, - 1500, 500, 1500, - 1)
			irrLogo = driver.GetTexture(path + "irrlichtlogoaligned.jpg")
			caption = captBase + " (using " + driver.Name + " driver)"
			Me.Text = caption
			Return True
		End Function
		 <summary>
		 Since the panel gets destroyed with device.CloseDevice,
		 we are creating it separately from the standard
		 Visual Studio section.
		 </summary>
		Private Sub CreatePanel()
			If panelMemory Is Nothing OrElse panelMemory = Rectangle.Empty Then
				panelMemory = New Rectangle(12, 12, 481, 373)
			End If
			Me.pnlIrrlicht = New System.Windows.Forms.Panel()
			' 
			' pnlIrrlicht
			' 
			Me.pnlIrrlicht.Anchor = (DirectCast(((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) Or System.Windows.Forms.AnchorStyles.Left) Or System.Windows.Forms.AnchorStyles.Right)), System.Windows.Forms.AnchorStyles))
			Me.pnlIrrlicht.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
			Me.pnlIrrlicht.Location = panelMemory.Location
			Me.pnlIrrlicht.Name = "pnlIrrlicht"
			Me.pnlIrrlicht.Size = panelMemory.Size
			Me.pnlIrrlicht.TabIndex = 0
			AddHandler Me.pnlIrrlicht.Resize, AddressOf pnlIrrlicht_Resize
			AddHandler Me.pnlIrrlicht.Move, AddressOf pnlIrrlicht_Move
			Me.toolStripContainer1.ContentPanel.Controls.Add(Me.pnlIrrlicht)
			Me.toolStripContainer1.ContentPanel.ResumeLayout(False)
			Me.toolStripContainer1.ContentPanel.PerformLayout()
		End Sub
		 <summary>
		 We are using this to rember the location / size of the Irrlicht panel.
		 </summary>
		 <param name="sender">Event's sender.</param>
		 <param name="e">Event arguments.</param>
		Private Sub pnlIrrlicht_Move(ByVal sender As Object, ByVal e As EventArgs)
			panelMemory.Location = pnlIrrlicht.Location
			panelMemory.Size = pnlIrrlicht.Size
		End Sub
		 <summary>
		 We are using this to rember the location / size of the Irrlicht panel.
		 </summary>
		 <param name="sender">Event's sender.</param>
		 <param name="e">Event arguments.</param>
		Private Sub pnlIrrlicht_Resize(ByVal sender As Object, ByVal e As EventArgs)
			panelMemory.Location = pnlIrrlicht.Location
			panelMemory.Size = pnlIrrlicht.Size
		End Sub
		#End Region
		 <summary>
		 Starts the Irrlicht rendering loop.
		 </summary>
		 <param name="c">The <see cref="Control"/> that Irrlicht is running in.</param>
		Private Sub RunDevice(ByVal c As Control)
			If model Is Nothing Then
				LoadModel(path + startupModelFile)
			End If
			While Not paused AndAlso Not quitting AndAlso device.Run() AndAlso device.VideoDriver IsNot Nothing
				device.VideoDriver.BeginScene(True, True, New Irrlicht.Video.Color(150, 50, 50, 50))
				device.SceneManager.DrawAll()
				device.GUIEnvironment.DrawAll()
				Dim logoLocation As core.Position2D = New core.Position2D(c.ClientRectangle.Left + 20, c.ClientRectangle.Bottom - 40)
				device.VideoDriver.Draw2DImage(irrLogo, logoLocation)
				device.VideoDriver.EndScene()
				If Not quitting AndAlso Not paused Then
					txtFPS.Text = device.VideoDriver.FPS + " FPS"
				End If
			End While
		End Sub
	End Class
End Namespace

' Retrieved from "http://www.irrforge.org/index.php/VB_Tutorial_9"
' This page has been accessed 99 times. This page was last modified 06:24, 25 Feb 2006. 

