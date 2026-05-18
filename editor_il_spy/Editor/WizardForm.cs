using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Resources;
using System.Windows.Forms;

namespace Editor;

public class WizardForm : Form
{
	private PictureBox pictureBox;

	private GroupBox groupBox2;

	private Container components;

	private GroupBox groupBox1;

	protected ArrayList _listSheets = new ArrayList();

	private WizardNavigation wizardNavigation;

	private int m_nCurrentSheet = -1;

	private bool _allowclose;

	protected WizardNavigation Navigation => wizardNavigation;

	public SheetControl CurrentControl
	{
		get
		{
			if (m_nCurrentSheet < 0)
			{
				return null;
			}
			return _listSheets[m_nCurrentSheet] as SheetControl;
		}
	}

	protected int SheetIndex
	{
		get
		{
			return m_nCurrentSheet;
		}
		set
		{
			if (value < 0 || value >= _listSheets.Count || m_nCurrentSheet == value)
			{
				return;
			}
			SuspendLayout();
			if (m_nCurrentSheet >= 0)
			{
				Control control = _listSheets[m_nCurrentSheet] as Control;
				control.Visible = false;
				base.Controls.Remove(control);
			}
			m_nCurrentSheet = value;
			Navigation.Prev.Enabled = false;
			if (m_nCurrentSheet >= 0)
			{
				Control control2 = _listSheets[m_nCurrentSheet] as Control;
				control2.TabIndex = 0;
				base.Controls.Add(control2);
				base.Controls.SetChildIndex(control2, 0);
				(control2 as SheetControl).OnActivated(this);
				control2.Visible = true;
				control2.Focus();
				if (m_nCurrentSheet > 0)
				{
					Navigation.Prev.Enabled = true;
				}
				if (m_nCurrentSheet + 1 == _listSheets.Count)
				{
					Navigation.Next.Text = Tools.Localizer.GetString(110);
					Navigation.Next.DialogResult = DialogResult.OK;
				}
				else
				{
					Navigation.Next.Text = Tools.Localizer.GetString(13);
					Navigation.Next.DialogResult = DialogResult.None;
				}
			}
			ResumeLayout(performLayout: true);
		}
	}

	public WizardForm()
	{
		InitializeComponent();
		base.AcceptButton = Navigation.Next;
		base.CancelButton = Navigation.Cancel;
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
		System.Resources.ResourceManager resourceManager = new System.Resources.ResourceManager(typeof(Editor.WizardForm));
		this.pictureBox = new System.Windows.Forms.PictureBox();
		this.wizardNavigation = new Editor.WizardNavigation();
		this.groupBox2 = new System.Windows.Forms.GroupBox();
		this.groupBox1 = new System.Windows.Forms.GroupBox();
		base.SuspendLayout();
		this.pictureBox.Dock = System.Windows.Forms.DockStyle.Top;
		this.pictureBox.Image = (System.Drawing.Image)resourceManager.GetObject("pictureBox.Image");
		this.pictureBox.ImeMode = System.Windows.Forms.ImeMode.NoControl;
		this.pictureBox.Location = new System.Drawing.Point(0, 0);
		this.pictureBox.Name = "pictureBox";
		this.pictureBox.Size = new System.Drawing.Size(450, 61);
		this.pictureBox.SizeMode = System.Windows.Forms.PictureBoxSizeMode.AutoSize;
		this.pictureBox.TabIndex = 0;
		this.pictureBox.TabStop = false;
		this.wizardNavigation.Dock = System.Windows.Forms.DockStyle.Bottom;
		this.wizardNavigation.Location = new System.Drawing.Point(0, 279);
		this.wizardNavigation.Name = "wizardNavigation";
		this.wizardNavigation.Size = new System.Drawing.Size(450, 48);
		this.wizardNavigation.TabIndex = 1;
		this.wizardNavigation.NavigationNext += new System.EventHandler(wizardNavigation_NavigationNext);
		this.wizardNavigation.NavigationPrev += new System.EventHandler(wizardNavigation_NavigationPrev);
		this.groupBox2.Dock = System.Windows.Forms.DockStyle.Bottom;
		this.groupBox2.Location = new System.Drawing.Point(0, 275);
		this.groupBox2.Name = "groupBox2";
		this.groupBox2.Size = new System.Drawing.Size(450, 4);
		this.groupBox2.TabIndex = 3;
		this.groupBox2.TabStop = false;
		this.groupBox1.Dock = System.Windows.Forms.DockStyle.Top;
		this.groupBox1.Location = new System.Drawing.Point(0, 61);
		this.groupBox1.Name = "groupBox1";
		this.groupBox1.Size = new System.Drawing.Size(450, 4);
		this.groupBox1.TabIndex = 2;
		this.groupBox1.TabStop = false;
		this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
		base.ClientSize = new System.Drawing.Size(450, 327);
		base.Controls.Add(this.groupBox1);
		base.Controls.Add(this.groupBox2);
		base.Controls.Add(this.wizardNavigation);
		base.Controls.Add(this.pictureBox);
		base.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
		base.ImeMode = System.Windows.Forms.ImeMode.NoControl;
		base.MaximizeBox = false;
		base.MinimizeBox = false;
		base.Name = "WizardForm";
		base.ShowInTaskbar = false;
		base.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
		base.Closing += new System.ComponentModel.CancelEventHandler(WizardForm_Closing);
		base.Load += new System.EventHandler(WizardForm_Load);
		base.ResumeLayout(false);
	}

	private void WizardForm_Load(object sender, EventArgs e)
	{
		foreach (SheetControl listSheet in _listSheets)
		{
			listSheet.Dock = DockStyle.Fill;
			listSheet.Enabled = true;
			listSheet.Visible = false;
		}
		if (_listSheets.Count > 0)
		{
			SheetIndex = 0;
		}
	}

	private void wizardNavigation_NavigationNext(object sender, EventArgs e)
	{
		_allowclose = false;
		SheetControl currentControl = CurrentControl;
		if (currentControl != null && currentControl.OnValidateSheet(this))
		{
			SheetIndex++;
			_allowclose = true;
		}
	}

	private void wizardNavigation_NavigationPrev(object sender, EventArgs e)
	{
		SheetIndex--;
	}

	private void WizardForm_Closing(object sender, CancelEventArgs e)
	{
		if (base.DialogResult == DialogResult.OK && !_allowclose)
		{
			e.Cancel = true;
		}
	}
}
