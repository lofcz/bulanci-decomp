using System;
using System.ComponentModel;
using System.Drawing;
using System.Resources;
using System.Windows.Forms;

namespace Editor;

public class EnterLoadBitmap : SheetControl
{
	private BrowseFile browseBitmap;

	private Container components;

	private ErrorProvider errorProvider;

	private Label label1;

	private SaveBitmapAs saveBitmapAs;

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
			browseBitmap.Text = _level.LoadBitmap.FileName;
			saveBitmapAs.SaveAsFormat = _level.LoadBitmap.Format;
		}
	}

	public EnterLoadBitmap()
	{
		InitializeComponent();
	}

	public EnterLoadBitmap(Level level)
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
		System.Resources.ResourceManager resourceManager = new System.Resources.ResourceManager(typeof(Editor.EnterLoadBitmap));
		this.browseBitmap = new Editor.BrowseFile();
		this.errorProvider = new System.Windows.Forms.ErrorProvider();
		this.label1 = new System.Windows.Forms.Label();
		this.saveBitmapAs = new Editor.SaveBitmapAs();
		base.SuspendLayout();
		this.browseBitmap.Anchor = System.Windows.Forms.AnchorStyles.Top;
		this.browseBitmap.Filter = "{42}";
		this.browseBitmap.Location = new System.Drawing.Point(32, 112);
		this.browseBitmap.Name = "browseBitmap";
		this.browseBitmap.Size = new System.Drawing.Size(288, 20);
		this.browseBitmap.TabIndex = 2;
		this.browseBitmap.Title = "{41}";
		this.errorProvider.ContainerControl = this;
		this.errorProvider.Icon = (System.Drawing.Icon)resourceManager.GetObject("errorProvider.Icon");
		this.label1.Anchor = System.Windows.Forms.AnchorStyles.Top;
		this.label1.ImeMode = System.Windows.Forms.ImeMode.NoControl;
		this.label1.Location = new System.Drawing.Point(32, 96);
		this.label1.Name = "label1";
		this.label1.Size = new System.Drawing.Size(288, 16);
		this.label1.TabIndex = 1;
		this.label1.Text = "{40}";
		this.saveBitmapAs.Anchor = System.Windows.Forms.AnchorStyles.Top;
		this.saveBitmapAs.Location = new System.Drawing.Point(32, 136);
		this.saveBitmapAs.Name = "saveBitmapAs";
		this.saveBitmapAs.Size = new System.Drawing.Size(288, 16);
		this.saveBitmapAs.TabIndex = 3;
		base.Controls.Add(this.saveBitmapAs);
		base.Controls.Add(this.label1);
		base.Controls.Add(this.browseBitmap);
		base.Description = "{39}";
		base.Name = "EnterLoadBitmap";
		base.Size = new System.Drawing.Size(352, 184);
		base.ValidateSheet += new System.ComponentModel.CancelEventHandler(EnterLoadBitmap_ValidateSheet);
		base.Controls.SetChildIndex(this.browseBitmap, 0);
		base.Controls.SetChildIndex(this.label1, 0);
		base.Controls.SetChildIndex(this.saveBitmapAs, 0);
		base.ResumeLayout(false);
	}

	private void EnterLoadBitmap_ValidateSheet(object sender, CancelEventArgs e)
	{
		errorProvider.SetError(browseBitmap, "");
		try
		{
			_level.LoadBitmap.CallLoadData = true;
			_level.LoadBitmap.FileName = browseBitmap.Text;
			_level.LoadBitmap.Format = saveBitmapAs.SaveAsFormat;
		}
		catch (Exception ex)
		{
			errorProvider.SetError(browseBitmap, ex.Message);
			e.Cancel = true;
		}
	}
}
