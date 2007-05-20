using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using Irrlicht;
using Irrlicht.Core;
using Irrlicht.Scene;

namespace _14.WindowsForm
{
	/// <summary>
	/// Shows irrlicht running in a windows .net form
	/// </summary>
	/// <remarks>
	/// Create as a normal windows application, add the usual irrlicht refs and dll's..
	/// This has been collapsed from 3 files into one for posting.
	/// </remarks>
	public partial class Form1 : Form
				   {
					   IrrlichtDevice device;

					   public Form1()
					   {
						   InitializeComponent();
					   }

					   private void Form1_Load(object sender, EventArgs e)
					   {
						   // this is not necessarily the best place to load and 
						   // initialise Irrlicht, however, it does work. 
						   this.Show();
						   runIrrlichtInWindowsFormTest(pictureBox1);
					   }

					   void runIrrlichtInWindowsFormTest(Control c)
					   {
						   device = new IrrlichtDevice(Irrlicht.Video.DriverType.DIRECT3D9,
							   new Dimension2D(c.Width, c.Height),
							   32, false, false, false, true, c.Handle);
						   ICameraSceneNode cam = device.SceneManager.AddCameraSceneNode(null, new Vector3D(), new Vector3D(), -1);
						   ISceneNodeAnimator anim = device.SceneManager.CreateFlyCircleAnimator(new Vector3D(0, 10, 0), 30.0f, 0.003f);
						   cam.AddAnimator(anim);
						   ISceneNode cube = device.SceneManager.AddTestSceneNode(25, null, -1, new Vector3D());
						   cube.SetMaterialTexture(0, device.VideoDriver.GetTexture("../../../media/rockwall.bmp"));
						   cube.SetMaterialFlag(MaterialFlag.LIGHTING, false);
						   // draw everything
						   // Note, using device.WindowActive will not work on a control, since we don't
						   // really activate controls..
						   while (device.Run() && c.Enabled)
						   {
							   device.VideoDriver.BeginScene(true, true, new Irrlicht.Video.Color(255, 0, 0, 50));
							   device.SceneManager.DrawAll();
							   device.GUIEnvironment.DrawAll();
							   device.VideoDriver.EndScene();
						   }
					   }

					   private void button1_Click(object sender, EventArgs e)
					   {
						   pictureBox1.Enabled = false;
						   this.Close();
					   }
					   #region Ripped from the Form1.Designer.cs
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

					   #region Windows Form Designer generated code

					   /// <summary>
					   /// Required method for Designer support - do not modify
					   /// the contents of this method with the code editor.
					   /// </summary>
					   private void InitializeComponent()
					   {
						   this.pictureBox1 = new System.Windows.Forms.PictureBox();
						   this.button1 = new System.Windows.Forms.Button();
						   this.label1 = new System.Windows.Forms.Label();
						   ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
						   this.SuspendLayout();
						   // 
						   // pictureBox1
						   // 
						   this.pictureBox1.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
						   this.pictureBox1.Location = new System.Drawing.Point(12, 37);
						   this.pictureBox1.Name = "pictureBox1";
						   this.pictureBox1.Size = new System.Drawing.Size(267, 207);
						   this.pictureBox1.TabIndex = 0;
						   this.pictureBox1.TabStop = false;
						   // 
						   // button1
						   // 
						   this.button1.Location = new System.Drawing.Point(206, 250);
						   this.button1.Name = "button1";
						   this.button1.Size = new System.Drawing.Size(75, 23);
						   this.button1.TabIndex = 1;
						   this.button1.Text = "Quit";
						   this.button1.UseVisualStyleBackColor = true;
						   this.button1.Click += new System.EventHandler(this.button1_Click);
						   // 
						   // label1
						   // 
						   this.label1.AutoSize = true;
						   this.label1.Location = new System.Drawing.Point(12, 13);
						   this.label1.Name = "label1";
						   this.label1.Size = new System.Drawing.Size(94, 13);
						   this.label1.TabIndex = 2;
						   this.label1.Text = "Irrlicht in a window";
						   // 
						   // Form1
						   // 
						   this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
						   this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
						   this.ClientSize = new System.Drawing.Size(292, 293);
						   this.Controls.Add(this.label1);
						   this.Controls.Add(this.button1);
						   this.Controls.Add(this.pictureBox1);
						   this.Name = "Form1";
						   this.Text = "Form1";
						   this.Load += new System.EventHandler(this.Form1_Load);
						   ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
						   this.ResumeLayout(false);
						   this.PerformLayout();

					   }

					   #endregion

					   private System.Windows.Forms.PictureBox pictureBox1;
					   private System.Windows.Forms.Button button1;
					   private System.Windows.Forms.Label label1;
					   #endregion
					   #region ripped from Program.cs
					   /// <summary>
					   /// The main entry point for the application.
					   /// </summary>
					   [STAThread]
					   static void Main()
					   {
						   Application.EnableVisualStyles();
						   Application.SetCompatibleTextRenderingDefault(false);
						   Application.Run(new Form1());
					   }
					   #endregion
				   }
}
// Retrieved from "http://www.irrforge.org/index.php/CS_Tutorial_14"
// This page has been accessed 299 times. This page was last modified 17:29, 22 Jan 2006. 

