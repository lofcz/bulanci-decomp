using System;
using System.ComponentModel;
using System.Drawing;
using System.Resources;
using System.Windows.Forms;

namespace Editor;

public class EnterBackgroundBitmap : SheetControl
{
	private BrowseFile browseBack;

	private Label labelBack;

	private SaveBitmapAs saveBitmapAs1;

	private ErrorProvider errorProvider;

	private Container components;

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
			browseBack.Text = _level.Background.FileName;
			saveBitmapAs1.SaveAsFormat = _level.Background.Format;
		}
	}

	public EnterBackgroundBitmap()
	{
		InitializeComponent();
	}

	public EnterBackgroundBitmap(Level level)
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
		System.Resources.ResourceManager resourceManager = new System.Resources.ResourceManager(typeof(Editor.EnterBackgroundBitmap));
		this.browseBack = new Editor.BrowseFile();
		this.labelBack = new System.Windows.Forms.Label();
		this.saveBitmapAs1 = new Editor.SaveBitmapAs();
		this.errorProvider = new System.Windows.Forms.ErrorProvider();
		base.SuspendLayout();
		this.browseBack.Anchor = System.Windows.Forms.AnchorStyles.Top;
		this.browseBack.Filter = "{42}";
		this.browseBack.Location = new System.Drawing.Point(32, 112);
		this.browseBack.Name = "browseBack";
		this.browseBack.Size = new System.Drawing.Size(288, 20);
		this.browseBack.TabIndex = 2;
		this.browseBack.Title = "{62}";
		this.labelBack.Anchor = System.Windows.Forms.AnchorStyles.Top;
		this.labelBack.ImeMode = System.Windows.Forms.ImeMode.NoControl;
		this.labelBack.Location = new System.Drawing.Point(32, 96);
		this.labelBack.Name = "labelBack";
		this.labelBack.Size = new System.Drawing.Size(288, 16);
		this.labelBack.TabIndex = 1;
		this.labelBack.Text = "{61}";
		this.saveBitmapAs1.Anchor = System.Windows.Forms.AnchorStyles.Top;
		this.saveBitmapAs1.Location = new System.Drawing.Point(32, 136);
		this.saveBitmapAs1.Name = "saveBitmapAs1";
		this.saveBitmapAs1.Size = new System.Drawing.Size(288, 16);
		this.saveBitmapAs1.TabIndex = 3;
		this.errorProvider.ContainerControl = this;
		this.errorProvider.Icon = (System.Drawing.Icon)resourceManager.GetObject("errorProvider.Icon");
		base.Controls.Add(this.saveBitmapAs1);
		base.Controls.Add(this.browseBack);
		base.Controls.Add(this.labelBack);
		base.Description = "{60}";
		base.Name = "EnterBackgroundBitmap";
		base.Size = new System.Drawing.Size(352, 176);
		base.ValidateSheet += new System.ComponentModel.CancelEventHandler(EnterBackgroundBitmap_ValidateSheet);
		base.Controls.SetChildIndex(this.labelBack, 0);
		base.Controls.SetChildIndex(this.browseBack, 0);
		base.Controls.SetChildIndex(this.saveBitmapAs1, 0);
		base.ResumeLayout(false);
	}

	private void EnterBackgroundBitmap_ValidateSheet(object sender, CancelEventArgs e)
	{
		errorProvider.SetError(browseBack, "");
		try
		{
			_level.Background.CallLoadData = true;
			_level.Background.FileName = browseBack.Text;
			_level.Background.Format = saveBitmapAs1.SaveAsFormat;
		}
		catch (Exception ex)
		{
			errorProvider.SetError(browseBack, ex.Message);
			e.Cancel = true;
		}
	}
}
