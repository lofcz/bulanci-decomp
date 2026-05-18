using System;
using System.ComponentModel;
using System.Drawing;
using System.Resources;
using System.Windows.Forms;
using Editor.ResourceItems;

namespace Editor;

public class EnterBitmap : SheetControl
{
	private Label labelName;

	private BrowseFile browseBitmap;

	private Container components;

	private ErrorProvider errorProvider;

	private BitmapResourceItem _item;

	public BitmapResourceItem BitmapResource
	{
		get
		{
			return _item;
		}
		set
		{
			_item = value;
			if (_item != null)
			{
				browseBitmap.Text = _item.FileName;
			}
		}
	}

	public EnterBitmap()
	{
		InitializeComponent();
	}

	public EnterBitmap(BitmapResourceItem item)
	{
		InitializeComponent();
		BitmapResource = item;
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
		System.Resources.ResourceManager resourceManager = new System.Resources.ResourceManager(typeof(Editor.EnterBitmap));
		this.labelName = new System.Windows.Forms.Label();
		this.browseBitmap = new Editor.BrowseFile();
		this.errorProvider = new System.Windows.Forms.ErrorProvider();
		base.SuspendLayout();
		this.labelName.Anchor = System.Windows.Forms.AnchorStyles.Top;
		this.labelName.ImeMode = System.Windows.Forms.ImeMode.NoControl;
		this.labelName.Location = new System.Drawing.Point(32, 96);
		this.labelName.Name = "labelName";
		this.labelName.Size = new System.Drawing.Size(288, 16);
		this.labelName.TabIndex = 1;
		this.labelName.Text = "{58}";
		this.browseBitmap.Anchor = System.Windows.Forms.AnchorStyles.Top;
		this.browseBitmap.Filter = "{42}";
		this.browseBitmap.Location = new System.Drawing.Point(32, 112);
		this.browseBitmap.Name = "browseBitmap";
		this.browseBitmap.Size = new System.Drawing.Size(288, 20);
		this.browseBitmap.TabIndex = 2;
		this.browseBitmap.Title = "{59}";
		this.errorProvider.ContainerControl = this;
		this.errorProvider.Icon = (System.Drawing.Icon)resourceManager.GetObject("errorProvider.Icon");
		base.Controls.Add(this.browseBitmap);
		base.Controls.Add(this.labelName);
		base.Description = "{57}";
		base.Name = "EnterBitmap";
		base.Size = new System.Drawing.Size(352, 184);
		base.ValidateSheet += new System.ComponentModel.CancelEventHandler(EnterBitmap_ValidateSheet);
		base.Controls.SetChildIndex(this.labelName, 0);
		base.Controls.SetChildIndex(this.browseBitmap, 0);
		base.ResumeLayout(false);
	}

	private void EnterBitmap_ValidateSheet(object sender, CancelEventArgs e)
	{
		errorProvider.SetError(browseBitmap, "");
		try
		{
			_item.Format = BitmapFormat.Special;
			_item.CallLoadData = true;
			_item.FileName = browseBitmap.Text;
		}
		catch (Exception ex)
		{
			e.Cancel = true;
			errorProvider.SetError(browseBitmap, ex.Message);
		}
	}
}
