using System;
using System.ComponentModel;
using System.Drawing;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using Editor.Localize;

namespace Editor;

public class AboutForm : Form
{
	private Button button;

	private Label label1;

	private Label label2;

	private LinkLabel linkLabel1;

	private Container components;

	private Localizer _localizer;

	public AboutForm()
	{
		InitializeComponent();
		_localizer = new FormLocalizer(Tools.Localizer, this);
		_localizer.Localized = true;
	}

	protected override void Dispose(bool disposing)
	{
		if (disposing && components != null)
		{
			components.Dispose();
		}
		base.Dispose(disposing);
	}

	private void InitializeComponent()
	{
		this.button = new System.Windows.Forms.Button();
		this.label1 = new System.Windows.Forms.Label();
		this.label2 = new System.Windows.Forms.Label();
		this.linkLabel1 = new System.Windows.Forms.LinkLabel();
		base.SuspendLayout();
		this.button.Anchor = System.Windows.Forms.AnchorStyles.Top;
		this.button.DialogResult = System.Windows.Forms.DialogResult.Cancel;
		this.button.ImeMode = System.Windows.Forms.ImeMode.NoControl;
		this.button.Location = new System.Drawing.Point(75, 80);
		this.button.Name = "button";
		this.button.TabIndex = 0;
		this.button.Text = "{69}";
		this.label1.Anchor = System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left | System.Windows.Forms.AnchorStyles.Right;
		this.label1.ImeMode = System.Windows.Forms.ImeMode.NoControl;
		this.label1.Location = new System.Drawing.Point(16, 16);
		this.label1.Name = "label1";
		this.label1.Size = new System.Drawing.Size(196, 16);
		this.label1.TabIndex = 1;
		this.label1.Text = "{68}";
		this.label1.TextAlign = System.Drawing.ContentAlignment.TopCenter;
		this.label2.Anchor = System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left | System.Windows.Forms.AnchorStyles.Right;
		this.label2.ImeMode = System.Windows.Forms.ImeMode.NoControl;
		this.label2.Location = new System.Drawing.Point(16, 32);
		this.label2.Name = "label2";
		this.label2.Size = new System.Drawing.Size(196, 16);
		this.label2.TabIndex = 2;
		this.label2.Text = "Copyright (c) 2006 SleepTeam Labs";
		this.label2.TextAlign = System.Drawing.ContentAlignment.TopCenter;
		this.linkLabel1.Anchor = System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left | System.Windows.Forms.AnchorStyles.Right;
		this.linkLabel1.ImeMode = System.Windows.Forms.ImeMode.NoControl;
		this.linkLabel1.LinkArea = new System.Windows.Forms.LinkArea(0, 24);
		this.linkLabel1.Location = new System.Drawing.Point(16, 48);
		this.linkLabel1.Name = "linkLabel1";
		this.linkLabel1.Size = new System.Drawing.Size(196, 16);
		this.linkLabel1.TabIndex = 3;
		this.linkLabel1.TabStop = true;
		this.linkLabel1.Text = "www.sleepteam.com";
		this.linkLabel1.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
		this.linkLabel1.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(linkLabel1_LinkClicked);
		this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
		base.CancelButton = this.button;
		base.ClientSize = new System.Drawing.Size(224, 117);
		base.Controls.Add(this.linkLabel1);
		base.Controls.Add(this.label2);
		base.Controls.Add(this.label1);
		base.Controls.Add(this.button);
		base.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
		base.MaximizeBox = false;
		base.MinimizeBox = false;
		base.Name = "AboutForm";
		base.ShowInTaskbar = false;
		base.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
		this.Text = "{67}";
		base.Closing += new System.ComponentModel.CancelEventHandler(AboutForm_Closing);
		base.ResumeLayout(false);
	}

	[DllImport("shell32.dll", CharSet = CharSet.Unicode, EntryPoint = "ShellExecuteW")]
	private static extern IntPtr ShellExecute(IntPtr hwnd, string operation, string file, string pars, string dir, int showcmd);

	private void linkLabel1_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
	{
		ShellExecute(IntPtr.Zero, "open", "http://www.sleepteam.com/", null, null, 0);
	}

	private void AboutForm_Closing(object sender, CancelEventArgs e)
	{
		_localizer.Dispose();
		_localizer = null;
	}
}
