using System;
using System.ComponentModel;
using System.Drawing;
using System.Resources;
using System.Windows.Forms;

namespace Editor;

public class EnterLevelSound : SheetControl
{
	private BrowseFile browseSound;

	private Label labelSound;

	private Container components;

	private ErrorProvider errorProvider;

	private Level _level;

	[Browsable(false)]
	[DefaultValue(null)]
	public Level Level
	{
		get
		{
			return _level;
		}
		set
		{
			_level = value;
			browseSound.Text = _level.SoundName;
		}
	}

	public EnterLevelSound()
	{
		InitializeComponent();
	}

	public EnterLevelSound(Level level)
	{
		InitializeComponent();
		Level = level;
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
		System.Resources.ResourceManager resourceManager = new System.Resources.ResourceManager(typeof(Editor.EnterLevelSound));
		this.browseSound = new Editor.BrowseFile();
		this.labelSound = new System.Windows.Forms.Label();
		this.errorProvider = new System.Windows.Forms.ErrorProvider();
		base.SuspendLayout();
		this.browseSound.Anchor = System.Windows.Forms.AnchorStyles.Top;
		this.browseSound.Filter = "{45}";
		this.browseSound.Location = new System.Drawing.Point(32, 112);
		this.browseSound.Name = "browseSound";
		this.browseSound.Size = new System.Drawing.Size(288, 20);
		this.browseSound.TabIndex = 2;
		this.browseSound.Title = "{46}";
		this.labelSound.Anchor = System.Windows.Forms.AnchorStyles.Top;
		this.labelSound.ImeMode = System.Windows.Forms.ImeMode.NoControl;
		this.labelSound.Location = new System.Drawing.Point(32, 96);
		this.labelSound.Name = "labelSound";
		this.labelSound.Size = new System.Drawing.Size(288, 16);
		this.labelSound.TabIndex = 1;
		this.labelSound.Text = "{44}";
		this.errorProvider.ContainerControl = this;
		this.errorProvider.Icon = (System.Drawing.Icon)resourceManager.GetObject("errorProvider.Icon");
		base.Controls.Add(this.browseSound);
		base.Controls.Add(this.labelSound);
		base.Description = "{43}";
		base.Name = "EnterLevelSound";
		base.Size = new System.Drawing.Size(352, 176);
		base.ValidateSheet += new System.ComponentModel.CancelEventHandler(EnterLevelSound_ValidateSheet);
		base.Controls.SetChildIndex(this.labelSound, 0);
		base.Controls.SetChildIndex(this.browseSound, 0);
		base.ResumeLayout(false);
	}

	private void EnterLevelSound_ValidateSheet(object sender, CancelEventArgs e)
	{
		errorProvider.SetError(browseSound, "");
		try
		{
			_level.SoundName = browseSound.Text;
		}
		catch (Exception ex)
		{
			errorProvider.SetError(browseSound, ex.Message);
			e.Cancel = true;
		}
	}
}
