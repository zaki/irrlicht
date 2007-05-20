/* 

* This tutorial covers creating a mesh viewer.
*
* It is similar to the native tutorial, but different.
* Since much of the gui and xml stuff is not in the
* .NET wrapper yet, I decided to use winforms and
* the .NET xml mush for those portions of the tutorial.
*
* Some of this tutorial is based on the work Braneloc did
* on Tutorial 14.
*
* The tutorial also includes some other interesting stuff
* that I decided to play with.
*
* For instance, you can change the driver in the middle of
* looking at a mesh.
* Since I used .net 2.0 & VS2005, you can also move the
* toolbar around the form.  (A new freebe in .net 2.0)
*
* DISCLAIMERS:  This will not work with .net 1.1.
* The reason is that some of the controls I used in this
* tutorial are not available in .net 1.1.
* Most of the controls that I know have changed are the
* menu and toolbar stuff.
* The new stuff now has names like "MenuStrip",
* "ToolStrip", and "ToolStripMenuItem", etc.
* Seems kinda silly, I know, but I'm sure someone over
* in Redmond really thought they had a great idea.
*
* Also, MS changed their xml api in .net 2.0.
* The xml part of the tutorial might port to .net 1.1
* just fine, but I'm not sure and not ambitious enough
* to check it out myself.
*
* BUGS:  Yeah, I'm sure there's a few in there.
* I notice when switching drivers, the app will
* sometimes lock.  I haven't quite figured out why yet,
* but it seems to have something to do with Irrlicht
* having a null device.EventReceiver.  Also, the
* debugger will sometimes point to a problem with fonts
* in this case.
*
* The winforms event responses are very doggy.
* I tried a couple of Application.DoEvents() in the render loop,
* but they didn't seem to help much.  I may
* look at this in the future.
*
* FINAL NOTE:  If you happen to find any fixes for any of the
* bugs listed above or any new ones you find, share them with
* the community.  Post about them on the forum, and update
* the Wiki.
*
* Getting it working: 
* Create project as windows form, and add the 2 Irrlicht DLL's to it.
* (This file is the only one needed in the project)
* Add open.bmp and help.bmp from the Irrlicht media directory to the
* resources.
* You'll probably need to rebind the images to the toolstrip, hint,
* open is the one on the left..
*/
using System;
using System.Collections.Generic;
using System.Drawing;
using System.Windows.Forms;
using System.Text;
using System.IO;
using System.Xml;
using Irrlicht;
using core = Irrlicht.Core;
using scene = Irrlicht.Scene;
using video = Irrlicht.Video;
using gui = Irrlicht.GUI;
namespace _09.WinFormMeshViewer
{
	/// <summary>
	/// Description of MainForm.
	/// </summary>
	public class MainForm : System.Windows.Forms.Form
	{
		// Path from tutorial binary to standard SDK media folder.
		private string path = "../../../../media/";
		private string messageText = string.Empty;
		private string caption = string.Empty;
		private string captBase = "Irrlicht.NET with WinForms - MeshViewer";
		private string configFile = "config.xml";
		private string startupModelFile = string.Empty;
		private string currentModelFile = string.Empty;
		private bool quitting = false;
		private bool paused = false;
		private IrrlichtDevice device;
		private scene.IAnimatedMeshSceneNode model;
		private scene.ISceneNode skyBox;
		private video.ITexture irrLogo;
		private Timer startDelay;
		private Panel pnlIrrlicht;
		private Rectangle panelMemory = Rectangle.Empty;	// Remembers the size & location of the panel
       
		private ToolStripMenuItem mnuFile;
		private ToolStripMenuItem mnuFileOpen;
		private ToolStripSeparator toolStripMenuItem1;
		private ToolStripMenuItem mnuFileExit;
		private ToolStripMenuItem mnuView;
		private ToolStripMenuItem mnuHelp;
		private ToolStripContainer toolStripContainer1;
		private ToolStrip toolStrip1;
		private ToolStripButton btnOpen;
		private ToolStripButton btnHelp;
		private ToolStripMenuItem mnuViewSkyBox;
		private ToolStripMenuItem mnuViewDebug;
		private ToolStripMenuItem mnuViewMaterial;
		private ToolStripMenuItem mnuViewMatSolid;
		private ToolStripMenuItem mnuViewMatTransp;
		private ToolStripMenuItem mnuViewMatReflect;
		private ToolStripMenuItem mnuHelpAbout;
		private Label lblScaleZ;
		private NumericUpDown numScaleZ;
		private Label lblScaleY;
		private NumericUpDown numScaleY;
		private Label lblScaleX;
		private NumericUpDown numScaleX;
		private CheckBox chkDebug;
		private CheckBox chkSkyBox;
		private RadioButton rbSoft1;
		private RadioButton rbOGL;
		private RadioButton rbD3D8;
		private RadioButton rbD3D9;
		private RadioButton rbNull;
		private RadioButton rbSoft2;
		private GroupBox gbDrivers;
		private ToolStripTextBox txtFPS;
		private MenuStrip mnuMain;
       
		public MainForm()
		{
			//
			// The InitializeComponent() call is required for Windows Forms designer support.
			//
			InitializeComponent();
           
			//
			// Add constructor code after the InitializeComponent() call.
			//
			startDelay = new Timer();
			startDelay.Interval = 1000;
			startDelay.Enabled = false;
			startDelay.Tick += new EventHandler(startDelay_Tick);
			CreatePanel();
		}
       
		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		static void Main()
		{
			try 
			{
				// Create a new instance of our application
				MainForm viewer = new MainForm();
               
				//Show the window
				viewer.Show();
               
				// Initialize items
				viewer.LoadIrrConfig();
				if (viewer.startupModelFile == string.Empty) viewer.startupModelFile = "dwarf.x";
				viewer.currentModelFile = viewer.startupModelFile;
				viewer.SetDevice(viewer.pnlIrrlicht, viewer.PickDriver());
				viewer.LoadModel(viewer.path + viewer.currentModelFile);
				viewer.LoadSkyBox();
               
				// Run the viewer
				System.Windows.Forms.Application.Run(viewer);
			} 
			catch (Exception ex) 
			{
				MessageBox.Show(ex.Message);
				Application.Exit();
				GC.Collect();
			}
           
		}
       
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.IContainer components = null;
       
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		/// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
		protected override void Dispose(bool disposing)
		{
			if (disposing && (components != null)) 
			{
				components.Dispose();
			}
			base.Dispose(disposing);
		}
       
		#region Windows Forms Designer generated code
		/// <summary>
		/// This method is required for Windows Forms designer support.
		/// Do not change the method contents inside the source code editor. The Forms designer might
		/// not be able to load this method if it was changed manually.
		/// </summary>
		private void InitializeComponent()
		{
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MainForm));
			this.mnuMain = new System.Windows.Forms.MenuStrip();
			this.mnuFile = new System.Windows.Forms.ToolStripMenuItem();
			this.mnuFileOpen = new System.Windows.Forms.ToolStripMenuItem();
			this.toolStripMenuItem1 = new System.Windows.Forms.ToolStripSeparator();
			this.mnuFileExit = new System.Windows.Forms.ToolStripMenuItem();
			this.mnuView = new System.Windows.Forms.ToolStripMenuItem();
			this.mnuViewSkyBox = new System.Windows.Forms.ToolStripMenuItem();
			this.mnuViewDebug = new System.Windows.Forms.ToolStripMenuItem();
			this.mnuViewMaterial = new System.Windows.Forms.ToolStripMenuItem();
			this.mnuViewMatSolid = new System.Windows.Forms.ToolStripMenuItem();
			this.mnuViewMatTransp = new System.Windows.Forms.ToolStripMenuItem();
			this.mnuViewMatReflect = new System.Windows.Forms.ToolStripMenuItem();
			this.mnuHelp = new System.Windows.Forms.ToolStripMenuItem();
			this.mnuHelpAbout = new System.Windows.Forms.ToolStripMenuItem();
			this.toolStripContainer1 = new System.Windows.Forms.ToolStripContainer();
			this.gbDrivers = new System.Windows.Forms.GroupBox();
			this.rbNull = new System.Windows.Forms.RadioButton();
			this.rbD3D9 = new System.Windows.Forms.RadioButton();
			this.rbSoft2 = new System.Windows.Forms.RadioButton();
			this.rbD3D8 = new System.Windows.Forms.RadioButton();
			this.rbSoft1 = new System.Windows.Forms.RadioButton();
			this.rbOGL = new System.Windows.Forms.RadioButton();
			this.chkDebug = new System.Windows.Forms.CheckBox();
			this.chkSkyBox = new System.Windows.Forms.CheckBox();
			this.lblScaleZ = new System.Windows.Forms.Label();
			this.numScaleZ = new System.Windows.Forms.NumericUpDown();
			this.lblScaleY = new System.Windows.Forms.Label();
			this.numScaleY = new System.Windows.Forms.NumericUpDown();
			this.lblScaleX = new System.Windows.Forms.Label();
			this.numScaleX = new System.Windows.Forms.NumericUpDown();
			this.toolStrip1 = new System.Windows.Forms.ToolStrip();
			this.btnOpen = new System.Windows.Forms.ToolStripButton();
			this.btnHelp = new System.Windows.Forms.ToolStripButton();
			this.txtFPS = new System.Windows.Forms.ToolStripTextBox();
			this.mnuMain.SuspendLayout();
			this.toolStripContainer1.ContentPanel.SuspendLayout();
			this.toolStripContainer1.TopToolStripPanel.SuspendLayout();
			this.toolStripContainer1.SuspendLayout();
			this.gbDrivers.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.numScaleZ)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.numScaleY)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.numScaleX)).BeginInit();
			this.toolStrip1.SuspendLayout();
			this.SuspendLayout();
			// 
			// mnuMain
			// 
			this.mnuMain.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
																					 this.mnuFile,
																					 this.mnuView,
																					 this.mnuHelp});
			this.mnuMain.Location = new System.Drawing.Point(0, 0);
			this.mnuMain.Name = "mnuMain";
			this.mnuMain.Size = new System.Drawing.Size(632, 24);
			this.mnuMain.TabIndex = 0;
			this.mnuMain.Text = "menuStrip1";
			// 
			// mnuFile
			// 
			this.mnuFile.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
																							 this.mnuFileOpen,
																							 this.toolStripMenuItem1,
																							 this.mnuFileExit});
			this.mnuFile.Name = "mnuFile";
			this.mnuFile.Size = new System.Drawing.Size(35, 20);
			this.mnuFile.Text = "&File";
			// 
			// mnuFileOpen
			// 
			this.mnuFileOpen.Name = "mnuFileOpen";
			this.mnuFileOpen.Size = new System.Drawing.Size(139, 22);
			this.mnuFileOpen.Text = "&Open Mesh";
			this.mnuFileOpen.Click += new System.EventHandler(this.open_Click);
			// 
			// toolStripMenuItem1
			// 
			this.toolStripMenuItem1.Name = "toolStripMenuItem1";
			this.toolStripMenuItem1.Size = new System.Drawing.Size(136, 6);
			// 
			// mnuFileExit
			// 
			this.mnuFileExit.Name = "mnuFileExit";
			this.mnuFileExit.Size = new System.Drawing.Size(139, 22);
			this.mnuFileExit.Text = "E&xit";
			this.mnuFileExit.Click += new System.EventHandler(this.Exit_Click);
			// 
			// mnuView
			// 
			this.mnuView.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
																							 this.mnuViewSkyBox,
																							 this.mnuViewDebug,
																							 this.mnuViewMaterial});
			this.mnuView.Name = "mnuView";
			this.mnuView.Size = new System.Drawing.Size(41, 20);
			this.mnuView.Text = "&View";
			// 
			// mnuViewSkyBox
			// 
			this.mnuViewSkyBox.Checked = true;
			this.mnuViewSkyBox.CheckOnClick = true;
			this.mnuViewSkyBox.CheckState = System.Windows.Forms.CheckState.Checked;
			this.mnuViewSkyBox.Name = "mnuViewSkyBox";
			this.mnuViewSkyBox.Size = new System.Drawing.Size(170, 22);
			this.mnuViewSkyBox.Text = "&SkyBox Visibility";
			this.mnuViewSkyBox.CheckedChanged += new System.EventHandler(this.SkyBox_CheckedChanged);
			// 
			// mnuViewDebug
			// 
			this.mnuViewDebug.Checked = true;
			this.mnuViewDebug.CheckOnClick = true;
			this.mnuViewDebug.CheckState = System.Windows.Forms.CheckState.Checked;
			this.mnuViewDebug.Name = "mnuViewDebug";
			this.mnuViewDebug.Size = new System.Drawing.Size(170, 22);
			this.mnuViewDebug.Text = "Model &Debug Info";
			this.mnuViewDebug.CheckedChanged += new System.EventHandler(this.Debug_CheckedChanged);
			// 
			// mnuViewMaterial
			// 
			this.mnuViewMaterial.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
																									 this.mnuViewMatSolid,
																									 this.mnuViewMatTransp,
																									 this.mnuViewMatReflect});
			this.mnuViewMaterial.Name = "mnuViewMaterial";
			this.mnuViewMaterial.Size = new System.Drawing.Size(170, 22);
			this.mnuViewMaterial.Text = "&Model Material";
			// 
			// mnuViewMatSolid
			// 
			this.mnuViewMatSolid.Checked = true;
			this.mnuViewMatSolid.CheckState = System.Windows.Forms.CheckState.Checked;
			this.mnuViewMatSolid.Name = "mnuViewMatSolid";
			this.mnuViewMatSolid.Size = new System.Drawing.Size(144, 22);
			this.mnuViewMatSolid.Text = "S&olid";
			this.mnuViewMatSolid.Click += new System.EventHandler(this.mnuViewMatSolid_Click);
			// 
			// mnuViewMatTransp
			// 
			this.mnuViewMatTransp.Name = "mnuViewMatTransp";
			this.mnuViewMatTransp.Size = new System.Drawing.Size(144, 22);
			this.mnuViewMatTransp.Text = "&Transparent";
			this.mnuViewMatTransp.Click += new System.EventHandler(this.mnuViewMatTransp_Click);
			// 
			// mnuViewMatReflect
			// 
			this.mnuViewMatReflect.Name = "mnuViewMatReflect";
			this.mnuViewMatReflect.Size = new System.Drawing.Size(144, 22);
			this.mnuViewMatReflect.Text = "Reflection";
			this.mnuViewMatReflect.Click += new System.EventHandler(this.mnuViewMatReflect_Click);
			// 
			// mnuHelp
			// 
			this.mnuHelp.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
																							 this.mnuHelpAbout});
			this.mnuHelp.Name = "mnuHelp";
			this.mnuHelp.Size = new System.Drawing.Size(40, 20);
			this.mnuHelp.Text = "&Help";
			// 
			// mnuHelpAbout
			// 
			this.mnuHelpAbout.Name = "mnuHelpAbout";
			this.mnuHelpAbout.Size = new System.Drawing.Size(114, 22);
			this.mnuHelpAbout.Text = "&About";
			this.mnuHelpAbout.Click += new System.EventHandler(this.Help_Click);
			// 
			// toolStripContainer1
			// 
			// 
			// toolStripContainer1.ContentPanel
			// 
			this.toolStripContainer1.ContentPanel.Controls.Add(this.gbDrivers);
			this.toolStripContainer1.ContentPanel.Controls.Add(this.chkDebug);
			this.toolStripContainer1.ContentPanel.Controls.Add(this.chkSkyBox);
			this.toolStripContainer1.ContentPanel.Controls.Add(this.lblScaleZ);
			this.toolStripContainer1.ContentPanel.Controls.Add(this.numScaleZ);
			this.toolStripContainer1.ContentPanel.Controls.Add(this.lblScaleY);
			this.toolStripContainer1.ContentPanel.Controls.Add(this.numScaleY);
			this.toolStripContainer1.ContentPanel.Controls.Add(this.lblScaleX);
			this.toolStripContainer1.ContentPanel.Controls.Add(this.numScaleX);
			this.toolStripContainer1.ContentPanel.Size = new System.Drawing.Size(632, 397);
			this.toolStripContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
			this.toolStripContainer1.Location = new System.Drawing.Point(0, 24);
			this.toolStripContainer1.Name = "toolStripContainer1";
			this.toolStripContainer1.Size = new System.Drawing.Size(632, 422);
			this.toolStripContainer1.TabIndex = 1;
			this.toolStripContainer1.Text = "toolStripContainer1";
			// 
			// toolStripContainer1.TopToolStripPanel
			// 
			this.toolStripContainer1.TopToolStripPanel.Controls.Add(this.toolStrip1);
			// 
			// gbDrivers
			// 
			this.gbDrivers.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.gbDrivers.Controls.Add(this.rbNull);
			this.gbDrivers.Controls.Add(this.rbD3D9);
			this.gbDrivers.Controls.Add(this.rbSoft2);
			this.gbDrivers.Controls.Add(this.rbD3D8);
			this.gbDrivers.Controls.Add(this.rbSoft1);
			this.gbDrivers.Controls.Add(this.rbOGL);
			this.gbDrivers.Location = new System.Drawing.Point(510, 223);
			this.gbDrivers.Name = "gbDrivers";
			this.gbDrivers.Size = new System.Drawing.Size(104, 162);
			this.gbDrivers.TabIndex = 15;
			this.gbDrivers.TabStop = false;
			this.gbDrivers.Text = "Driver Selection:";
			// 
			// rbNull
			// 
			this.rbNull.AutoSize = true;
			this.rbNull.Location = new System.Drawing.Point(6, 134);
			this.rbNull.Name = "rbNull";
			this.rbNull.Size = new System.Drawing.Size(72, 17);
			this.rbNull.TabIndex = 14;
			this.rbNull.TabStop = true;
			this.rbNull.Text = "null Driver";
			this.rbNull.UseVisualStyleBackColor = true;
			this.rbNull.CheckedChanged += new System.EventHandler(this.rb_CheckedChanged);
			// 
			// rbD3D9
			// 
			this.rbD3D9.AutoSize = true;
			this.rbD3D9.Checked = true;
			this.rbD3D9.Location = new System.Drawing.Point(6, 19);
			this.rbD3D9.Name = "rbD3D9";
			this.rbD3D9.Size = new System.Drawing.Size(91, 17);
			this.rbD3D9.TabIndex = 9;
			this.rbD3D9.TabStop = true;
			this.rbD3D9.Text = "Direct3D 9.0c";
			this.rbD3D9.UseVisualStyleBackColor = true;
			this.rbD3D9.CheckedChanged += new System.EventHandler(this.rb_CheckedChanged);
			// 
			// rbSoft2
			// 
			this.rbSoft2.AutoSize = true;
			this.rbSoft2.Location = new System.Drawing.Point(6, 111);
			this.rbSoft2.Name = "rbSoft2";
			this.rbSoft2.Size = new System.Drawing.Size(75, 17);
			this.rbSoft2.TabIndex = 13;
			this.rbSoft2.TabStop = true;
			this.rbSoft2.Text = "Apfelbaum";
			this.rbSoft2.UseVisualStyleBackColor = true;
			this.rbSoft2.CheckedChanged += new System.EventHandler(this.rb_CheckedChanged);
			// 
			// rbD3D8
			// 
			this.rbD3D8.AutoSize = true;
			this.rbD3D8.Location = new System.Drawing.Point(6, 42);
			this.rbD3D8.Name = "rbD3D8";
			this.rbD3D8.Size = new System.Drawing.Size(85, 17);
			this.rbD3D8.TabIndex = 10;
			this.rbD3D8.TabStop = true;
			this.rbD3D8.Text = "Direct3D 8.1";
			this.rbD3D8.UseVisualStyleBackColor = true;
			this.rbD3D8.CheckedChanged += new System.EventHandler(this.rb_CheckedChanged);
			// 
			// rbSoft1
			// 
			this.rbSoft1.AutoSize = true;
			this.rbSoft1.Location = new System.Drawing.Point(6, 88);
			this.rbSoft1.Name = "rbSoft1";
			this.rbSoft1.Size = new System.Drawing.Size(67, 17);
			this.rbSoft1.TabIndex = 12;
			this.rbSoft1.TabStop = true;
			this.rbSoft1.Text = "Software";
			this.rbSoft1.UseVisualStyleBackColor = true;
			this.rbSoft1.CheckedChanged += new System.EventHandler(this.rb_CheckedChanged);
			// 
			// rbOGL
			// 
			this.rbOGL.AutoSize = true;
			this.rbOGL.Location = new System.Drawing.Point(6, 65);
			this.rbOGL.Name = "rbOGL";
			this.rbOGL.Size = new System.Drawing.Size(83, 17);
			this.rbOGL.TabIndex = 11;
			this.rbOGL.TabStop = true;
			this.rbOGL.Text = "OpenGL 1.5";
			this.rbOGL.UseVisualStyleBackColor = true;
			this.rbOGL.CheckedChanged += new System.EventHandler(this.rb_CheckedChanged);
			// 
			// chkDebug
			// 
			this.chkDebug.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.chkDebug.AutoSize = true;
			this.chkDebug.Checked = true;
			this.chkDebug.CheckState = System.Windows.Forms.CheckState.Checked;
			this.chkDebug.Location = new System.Drawing.Point(510, 193);
			this.chkDebug.Name = "chkDebug";
			this.chkDebug.Size = new System.Drawing.Size(113, 17);
			this.chkDebug.TabIndex = 8;
			this.chkDebug.Text = "Debug Information";
			this.chkDebug.UseVisualStyleBackColor = true;
			this.chkDebug.CheckedChanged += new System.EventHandler(this.Debug_CheckedChanged);
			// 
			// chkSkyBox
			// 
			this.chkSkyBox.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.chkSkyBox.AutoSize = true;
			this.chkSkyBox.Checked = true;
			this.chkSkyBox.CheckState = System.Windows.Forms.CheckState.Checked;
			this.chkSkyBox.Location = new System.Drawing.Point(510, 170);
			this.chkSkyBox.Name = "chkSkyBox";
			this.chkSkyBox.Size = new System.Drawing.Size(101, 17);
			this.chkSkyBox.TabIndex = 7;
			this.chkSkyBox.Text = "SkyBox Visibility";
			this.chkSkyBox.UseVisualStyleBackColor = true;
			this.chkSkyBox.CheckedChanged += new System.EventHandler(this.SkyBox_CheckedChanged);
			// 
			// lblScaleZ
			// 
			this.lblScaleZ.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.lblScaleZ.AutoSize = true;
			this.lblScaleZ.Location = new System.Drawing.Point(510, 116);
			this.lblScaleZ.Name = "lblScaleZ";
			this.lblScaleZ.Size = new System.Drawing.Size(55, 13);
			this.lblScaleZ.TabIndex = 6;
			this.lblScaleZ.Text = "Z Scaling:";
			// 
			// numScaleZ
			// 
			this.numScaleZ.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.numScaleZ.DecimalPlaces = 1;
			this.numScaleZ.Increment = new decimal(new int[] {
																 1,
																 0,
																 0,
																 65536});
			this.numScaleZ.Location = new System.Drawing.Point(510, 135);
			this.numScaleZ.Name = "numScaleZ";
			this.numScaleZ.Size = new System.Drawing.Size(61, 20);
			this.numScaleZ.TabIndex = 5;
			this.numScaleZ.Value = new decimal(new int[] {
															 10,
															 0,
															 0,
															 65536});
			this.numScaleZ.ValueChanged += new System.EventHandler(this.Scale_ValueChanged);
			// 
			// lblScaleY
			// 
			this.lblScaleY.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.lblScaleY.AutoSize = true;
			this.lblScaleY.Location = new System.Drawing.Point(510, 63);
			this.lblScaleY.Name = "lblScaleY";
			this.lblScaleY.Size = new System.Drawing.Size(55, 13);
			this.lblScaleY.TabIndex = 4;
			this.lblScaleY.Text = "Y Scaling:";
			// 
			// numScaleY
			// 
			this.numScaleY.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.numScaleY.DecimalPlaces = 1;
			this.numScaleY.Increment = new decimal(new int[] {
																 1,
																 0,
																 0,
																 65536});
			this.numScaleY.Location = new System.Drawing.Point(510, 82);
			this.numScaleY.Name = "numScaleY";
			this.numScaleY.Size = new System.Drawing.Size(61, 20);
			this.numScaleY.TabIndex = 3;
			this.numScaleY.Value = new decimal(new int[] {
															 10,
															 0,
															 0,
															 65536});
			this.numScaleY.ValueChanged += new System.EventHandler(this.Scale_ValueChanged);
			// 
			// lblScaleX
			// 
			this.lblScaleX.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.lblScaleX.AutoSize = true;
			this.lblScaleX.Location = new System.Drawing.Point(510, 12);
			this.lblScaleX.Name = "lblScaleX";
			this.lblScaleX.Size = new System.Drawing.Size(55, 13);
			this.lblScaleX.TabIndex = 2;
			this.lblScaleX.Text = "X Scaling:";
			// 
			// numScaleX
			// 
			this.numScaleX.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.numScaleX.DecimalPlaces = 1;
			this.numScaleX.Increment = new decimal(new int[] {
																 1,
																 0,
																 0,
																 65536});
			this.numScaleX.Location = new System.Drawing.Point(510, 31);
			this.numScaleX.Name = "numScaleX";
			this.numScaleX.Size = new System.Drawing.Size(61, 20);
			this.numScaleX.TabIndex = 1;
			this.numScaleX.Value = new decimal(new int[] {
															 10,
															 0,
															 0,
															 65536});
			this.numScaleX.ValueChanged += new System.EventHandler(this.Scale_ValueChanged);
			// 
			// toolStrip1
			// 
			this.toolStrip1.Dock = System.Windows.Forms.DockStyle.None;
			this.toolStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
																						this.btnOpen,
																						this.btnHelp,
																						this.txtFPS});
			this.toolStrip1.Location = new System.Drawing.Point(3, 0);
			this.toolStrip1.Name = "toolStrip1";
			this.toolStrip1.Size = new System.Drawing.Size(160, 25);
			this.toolStrip1.TabIndex = 0;
			// 
			// btnOpen
			// 
			this.btnOpen.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
			this.btnOpen.Image = ((System.Drawing.Image)(resources.GetObject("btnOpen.Image")));
			this.btnOpen.ImageTransparentColor = System.Drawing.Color.Magenta;
			this.btnOpen.Name = "btnOpen";
			this.btnOpen.Size = new System.Drawing.Size(23, 22);
			this.btnOpen.Text = "toolStripButton1";
			this.btnOpen.Click += new System.EventHandler(this.open_Click);
			// 
			// btnHelp
			// 
			this.btnHelp.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
			this.btnHelp.Image = ((System.Drawing.Image)(resources.GetObject("btnHelp.Image")));
			this.btnHelp.ImageTransparentColor = System.Drawing.Color.Magenta;
			this.btnHelp.Name = "btnHelp";
			this.btnHelp.Size = new System.Drawing.Size(23, 22);
			this.btnHelp.Text = "btnHelp";
			// 
			// txtFPS
			// 
			this.txtFPS.Name = "txtFPS";
			this.txtFPS.Size = new System.Drawing.Size(100, 25);
			// 
			// MainForm
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(632, 446);
			this.Controls.Add(this.toolStripContainer1);
			this.Controls.Add(this.mnuMain);
			this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
			this.MainMenuStrip = this.mnuMain;
			this.MinimumSize = new System.Drawing.Size(640, 480);
			this.Name = "MainForm";
			this.Text = "Irrlicht with WinForms MeshViewer";
			this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.MainForm_FormClosing);
			this.Load += new System.EventHandler(this.MainForm_Load);
			this.mnuMain.ResumeLayout(false);
			this.mnuMain.PerformLayout();
			this.toolStripContainer1.ContentPanel.ResumeLayout(false);
			this.toolStripContainer1.ContentPanel.PerformLayout();
			this.toolStripContainer1.TopToolStripPanel.ResumeLayout(false);
			this.toolStripContainer1.TopToolStripPanel.PerformLayout();
			this.toolStripContainer1.ResumeLayout(false);
			this.toolStripContainer1.PerformLayout();
			this.gbDrivers.ResumeLayout(false);
			this.gbDrivers.PerformLayout();
			((System.ComponentModel.ISupportInitialize)(this.numScaleZ)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.numScaleY)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.numScaleX)).EndInit();
			this.toolStrip1.ResumeLayout(false);
			this.toolStrip1.PerformLayout();
			this.ResumeLayout(false);
			this.PerformLayout();
           
		}
		#endregion
       
       
		#region - Winforms Events -
       
		private void MainForm_Load(object sender, EventArgs e)
		{
			startDelay.Enabled = true;
		}
       
		void startDelay_Tick(object sender, EventArgs e)
		{
			startDelay.Enabled = false;
			RunDevice(pnlIrrlicht);
		}
       
		private void open_Click(object sender, EventArgs e)
		{
			StringBuilder filter = new StringBuilder();
			filter.Append("3D Studio Mesh(*.3ds)|*.3ds");
			filter.Append("|Alias Wavefront Maya(*.obj)|*.obj");
			filter.Append("|Cartography Shop 4(*.csm)|*.csm");
			filter.Append("|COLLADA(*.xml;*.dae)|*.xml;*.dae");
			filter.Append("|DeleD(*.dmf)|*.dmf");
			filter.Append("|FSRad oct(*.oct)|*.oct");
			filter.Append("|Microsoft DirectX(*.x)|*.x");
			filter.Append("|Milkshape (*.ms3d)|*.ms3d");
			filter.Append("|My3DTools 3(*my3D)|*.my3D");
			filter.Append("|Pulsar LMTools(*.lmts)|*.lmts");
			filter.Append("|Quake 3 levels(*.bsp)|*.bsp");
			filter.Append("|Quake 2 models(*.md2)|*.md2");
			filter.Append("|Packed models(*.pk3)|*.pk3");
			OpenFileDialog ofd = new OpenFileDialog();
			ofd.Filter = filter.ToString();
			ofd.FilterIndex = 7;
           
			if (ofd.ShowDialog() == DialogResult.OK) 
			{
				LoadModel(ofd.FileName);
			}
		}
       
		private void Exit_Click(object sender, EventArgs e)
		{
			this.Close();
		}
       
		private void MainForm_FormClosing(object sender, FormClosingEventArgs e)
		{
			quitting = true;
		}
       
		private void Help_Click(object sender, EventArgs e)
		{
			caption = "Irrlicht.NET engine mesh viewer - ";
			if (device != null && device.VideoDriver != null) 
			{
				caption += device.VideoDriver.Name;
			}
			MessageBox.Show(messageText, caption);
		}
       
		private void rb_CheckedChanged(object sender, EventArgs e)
		{
			try 
			{
				paused = true;
				SetDevice(pnlIrrlicht, PickDriver());
				LoadModel(path + currentModelFile);
				LoadSkyBox();
				paused = false;
				RunDevice(pnlIrrlicht);
			} 
			catch (Exception ex) 
			{
				MessageBox.Show(ex.Message, caption);
			}
		}
       
		private void SkyBox_CheckedChanged(object sender, EventArgs e)
		{
			if (sender.GetType() == typeof(CheckBox))
				mnuViewSkyBox.Checked = chkSkyBox.Checked;
			else
				chkSkyBox.Checked = mnuViewSkyBox.Checked;
           
			if (skyBox != null) skyBox.Visible = chkSkyBox.Checked;
		}
       
		private void Debug_CheckedChanged(object sender, EventArgs e)
		{
			if (sender.GetType() == typeof(CheckBox))
				mnuViewDebug.Checked = chkDebug.Checked;
			else
				chkDebug.Checked = mnuViewDebug.Checked;
           
			if (model != null) model.DebugDataVisible = chkDebug.Checked;
		}
       
		private void mnuViewMatSolid_Click(object sender, EventArgs e)
		{
			mnuViewMatSolid.Checked = true;
			mnuViewMatTransp.Checked = false;
			mnuViewMatReflect.Checked = false;
           
			if (model != null) model.SetMaterialType(video.MaterialType.SOLID);
		}
       
		private void mnuViewMatTransp_Click(object sender, EventArgs e)
		{
			mnuViewMatSolid.Checked = false;
			mnuViewMatTransp.Checked = true;
			mnuViewMatReflect.Checked = false;
           
			if (model != null) model.SetMaterialType(video.MaterialType.TRANSPARENT_ADD_COLOR);
		}
       
		private void mnuViewMatReflect_Click(object sender, EventArgs e)
		{
			mnuViewMatSolid.Checked = false;
			mnuViewMatTransp.Checked = false;
			mnuViewMatReflect.Checked = true;
           
			if (model != null) model.SetMaterialType(video.MaterialType.SPHERE_MAP);
		}
       
		private void Scale_ValueChanged(object sender, EventArgs e)
		{
			core.Vector3D scale;
			scale.X = (float)numScaleX.Value;
			scale.Y = (float)numScaleY.Value;
			scale.Z = (float)numScaleZ.Value;
           
			if (model != null) model.Scale = scale;
		}
		#endregion
       
       
		#region - Resource Loading -
       
		private void LoadIrrConfig()
		{
			if (File.Exists(path + configFile)) 
			{
				XmlTextReader xtr = new XmlTextReader(path + configFile);
               
				try 
				{
					while (xtr.Read()) 
					{
						if ((xtr.NodeType == XmlNodeType.Element)) 
						{
							if (xtr.LocalName == "startUpModel") 
							{
								xtr.MoveToFirstAttribute();
								startupModelFile = xtr.Value;
								// Clean the leading stuff from the filename:
								int lastSlash = startupModelFile.LastIndexOf("/");
								startupModelFile = startupModelFile.Substring(lastSlash + 1);
								xtr.MoveToElement();
							} 
							else if (xtr.LocalName == "messageText") 
							{
								xtr.MoveToFirstAttribute();
								captBase = xtr.Value;
								xtr.MoveToElement();
								messageText = xtr.ReadElementString();
							}
						}
					}
				} 
				catch (Exception ex) 
				{
					MessageBox.Show(ex.Message);
				}
				xtr.Close();
			}
		}
       
		/// <summary>
		/// This method loads a model and displays it using an
		/// addAnimatedMeshSceneNode and the scene manager. Nothing difficult. It also
		/// displays a short message box, if the model could not be loaded.
		/// </summary>
		/// <param name="fileName">File name of the model to be loaded.</param>
		private void LoadModel(string fileName)
		{
			// Check if we have a valid device to use.
			if (device == null) 
			{
				MessageBox.Show("Can't load model because the device is not created.", caption);
				return;
			}
           
			bool foundBSP = false;
			// modify the name if it a .pk3 file
			if (fileName.EndsWith(".pk3")) 
			{
				device.FileSystem.AddZipFileArchive(fileName);
				foundBSP = true;
			}
           
			// Clear the model if there was something loaded before.
			if (model != null) model.Remove();
           
			// Load a mesh from file.
			scene.IAnimatedMesh m = device.SceneManager.GetMesh(fileName);
           
			if (m == null) 
			{
				// Model could not be loaded
				if (startupModelFile != fileName) 
				{
					string error = "The model could not be loaded.\nMaybe it is not a supported file format.";
					MessageBox.Show(error, caption);
				}
				return;
			}
           
			// load a model into the engine from the mesh
			model = device.SceneManager.AddAnimatedMeshSceneNode(m, null, 0);
           
			// set default material properties
			if (!foundBSP) 
			{
				if (mnuViewMatSolid.Checked)
					model.SetMaterialType(video.MaterialType.SOLID);
				else if (mnuViewMatTransp.Checked)
					model.SetMaterialType(video.MaterialType.TRANSPARENT_ADD_COLOR);
				else if (mnuViewMatReflect.Checked)
					model.SetMaterialType(video.MaterialType.SPHERE_MAP);
			}
           
			model.SetMaterialFlag(video.MaterialFlag.LIGHTING, false);
			model.DebugDataVisible = true;
			model.AnimationSpeed = 1000;
		}
       
		/// <summary>
		/// Loads the skybox using the device's driver & scene manager.
		/// </summary>
		private void LoadSkyBox()
		{
			if (device == null) 
			{
				MessageBox.Show("Couldn't load skybox because device was null.", caption);
				return;
			}
           
			video.IVideoDriver driver = device.VideoDriver;
			scene.ISceneManager smgr = device.SceneManager;
			skyBox = smgr.AddSkyBoxSceneNode(driver.GetTexture(path + "irrlicht2_up.jpg"),
				driver.GetTexture(path + "irrlicht2_dn.jpg"),
				driver.GetTexture(path + "irrlicht2_lf.jpg"),
				driver.GetTexture(path + "irrlicht2_rt.jpg"),
				driver.GetTexture(path + "irrlicht2_ft.jpg"),
				driver.GetTexture(path + "irrlicht2_bk.jpg"),
				null, -1);	// null & -1 are the defaults in the native engine.
           
			if (skyBox != null) skyBox.Visible = chkSkyBox.Checked;
		}
       
		#endregion
       
       
		#region - Device selection & setup -
       
		/// <summary>
		/// Gets the <see cref="Irrlicht.Video.DriverType"/> that the user selected.
		/// </summary>
		/// <returns><see cref="Irrlicht.Video.DriverType"/> the user selected.</returns>
		private video.DriverType PickDriver()
		{
			if (rbD3D9.Checked) return Irrlicht.Video.DriverType.DIRECT3D9;
			if (rbD3D8.Checked) return Irrlicht.Video.DriverType.DIRECT3D8;
			if (rbOGL.Checked) return Irrlicht.Video.DriverType.OPENGL;
			if (rbSoft1.Checked) return Irrlicht.Video.DriverType.SOFTWARE;
			if (rbSoft2.Checked) return Irrlicht.Video.DriverType.SOFTWARE2;
			return Irrlicht.Video.DriverType.NULL_DRIVER;
		}
       
		/// <summary>
		/// Sets up the device to run in a <see cref="Control"/> with the specified <see cref="Irrlicht.Video.DriverType"/>.
		/// </summary>
		/// <param name="c">Winforms <see cref="Control"/> that Irrlicht is to render in.</param>
		/// <param name="driver"><see cref="Irrlicht.Video.DriverType"/> to use when setting up the IrrlichtDevice.</param>
		/// <returns>True if the setup went well, False otherwise.</returns>
		private bool SetDevice(Control c, video.DriverType driverType)
		{
			if (quitting) return false;
           
			if (device != null) 
			{
				device.CloseDevice();
			}
           
			CreatePanel();
           
			device = new IrrlichtDevice(driverType,
				new core.Dimension2D(c.ClientRectangle.Width, c.ClientRectangle.Height),
				32, false, false, false, true, c.Handle);
           
			if (device == null) return false;
           
			device.ResizeAble = true;
			gui.IGUIEnvironment env = device.GUIEnvironment;
			video.IVideoDriver driver = device.VideoDriver;
			scene.ISceneManager smgr = device.SceneManager;
			driver.SetTextureCreationFlag(video.TextureCreationFlag.ALWAYS_32_BIT, true);
           
			// This sets the gui font from a bmp file.
			gui.IGUISkin skin = env.Skin;
			gui.IGUIFont font = env.GetFont(path + "fonthaettenschweiler.bmp");
			if (font != null) skin.Font = font;
           
			// This is loaded with the default values for the native engine.
			smgr.AddCameraSceneNodeMaya(null, -1500, 500, 1500, -1);
			irrLogo = driver.GetTexture(path + "irrlichtlogoaligned.jpg");
           
			caption = captBase + " (using " + driver.Name + " driver)";
			this.Text = caption;
           
			return true;
		}
       
		/// <summary>
		/// Since the panel gets destroyed with device.CloseDevice,
		/// we are creating it separately from the standard
		/// Visual Studio section.
		/// </summary>
		private void CreatePanel()
		{
			if (panelMemory == null || panelMemory == Rectangle.Empty)
				panelMemory = new Rectangle(12, 12, 481, 373);
           
			this.pnlIrrlicht = new System.Windows.Forms.Panel();
			// 
			// pnlIrrlicht
			// 
			this.pnlIrrlicht.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
				| System.Windows.Forms.AnchorStyles.Left)
				| System.Windows.Forms.AnchorStyles.Right)));
			this.pnlIrrlicht.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.pnlIrrlicht.Location = panelMemory.Location;
			this.pnlIrrlicht.Name = "pnlIrrlicht";
			this.pnlIrrlicht.Size = panelMemory.Size;
			this.pnlIrrlicht.TabIndex = 0;
			this.pnlIrrlicht.Resize += new EventHandler(pnlIrrlicht_Resize);
			this.pnlIrrlicht.Move += new EventHandler(pnlIrrlicht_Move);
			this.toolStripContainer1.ContentPanel.Controls.Add(this.pnlIrrlicht);
			this.toolStripContainer1.ContentPanel.ResumeLayout(false);
			this.toolStripContainer1.ContentPanel.PerformLayout();
		}
       
		/// <summary>
		/// We are using this to rember the location / size of the Irrlicht panel.
		/// </summary>
		/// <param name="sender">Event's sender.</param>
		/// <param name="e">Event arguments.</param>
		void pnlIrrlicht_Move(object sender, EventArgs e)
		{
			panelMemory.Location = pnlIrrlicht.Location;
			panelMemory.Size = pnlIrrlicht.Size;
		}
       
		/// <summary>
		/// We are using this to rember the location / size of the Irrlicht panel.
		/// </summary>
		/// <param name="sender">Event's sender.</param>
		/// <param name="e">Event arguments.</param>
		void pnlIrrlicht_Resize(object sender, EventArgs e)
		{
			panelMemory.Location = pnlIrrlicht.Location;
			panelMemory.Size = pnlIrrlicht.Size;
		}
       
		#endregion
       
		/// <summary>
		/// Starts the Irrlicht rendering loop.
		/// </summary>
		/// <param name="c">The <see cref="Control"/> that Irrlicht is running in.</param>
		private void RunDevice(Control c)
		{
			if (model == null) LoadModel(path + startupModelFile);
			while (!paused && !quitting && device.Run() && device.VideoDriver != null) 
			{
				device.VideoDriver.BeginScene(true, true, new Irrlicht.Video.Color(150, 50, 50, 50));
				device.SceneManager.DrawAll();
				device.GUIEnvironment.DrawAll();
				core.Position2D logoLocation = new core.Position2D(c.ClientRectangle.Left + 20, c.ClientRectangle.Bottom - 40);
				device.VideoDriver.Draw2DImage(irrLogo, logoLocation);
				device.VideoDriver.EndScene();
				if (!quitting && !paused) txtFPS.Text = device.VideoDriver.FPS + " FPS";
			}
		}
       
	}
}
// Retrieved from "http://www.irrforge.org/index.php/CS_Tutorial_9"
// This page has been accessed 283 times. This page was last modified 01:00, 26 Jan 2006. 

