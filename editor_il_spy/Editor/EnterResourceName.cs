using System;
using System.ComponentModel;
using System.Drawing;
using System.Resources;
using System.Windows.Forms;

namespace Editor;

public class EnterResourceName : SheetControl
{
	private Label labelName;

	private TextBox textBoxName;

	private Container components;

	private ErrorProvider errorProvider;

	private ResourceItem _item;

	[Browsable(true)]
	[DefaultValue(null)]
	public ResourceItem Resource
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
				textBoxName.Text = _item.Name;
			}
		}
	}

	public EnterResourceName(ResourceItem item)
	{
		InitializeComponent();
		Resource = item;
	}

	public EnterResourceName()
	{
		InitializeComponent();
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
		System.Resources.ResourceManager resourceManager = new System.Resources.ResourceManager(typeof(Editor.EnterResourceName));
		this.labelName = new System.Windows.Forms.Label();
		this.textBoxName = new System.Windows.Forms.TextBox();
		this.errorProvider = new System.Windows.Forms.ErrorProvider();
		base.SuspendLayout();
		this.labelName.Anchor = System.Windows.Forms.AnchorStyles.Top;
		this.labelName.ImeMode = System.Windows.Forms.ImeMode.NoControl;
		this.labelName.Location = new System.Drawing.Point(32, 96);
		this.labelName.Name = "labelName";
		this.labelName.Size = new System.Drawing.Size(288, 16);
		this.labelName.TabIndex = 1;
		this.labelName.Text = "{38}";
		this.textBoxName.Anchor = System.Windows.Forms.AnchorStyles.Top;
		this.textBoxName.Location = new System.Drawing.Point(32, 112);
		this.textBoxName.MaxLength = 128;
		this.textBoxName.Name = "textBoxName";
		this.textBoxName.Size = new System.Drawing.Size(288, 20);
		this.textBoxName.TabIndex = 2;
		this.textBoxName.Text = "";
		this.errorProvider.ContainerControl = this;
		this.errorProvider.Icon = (System.Drawing.Icon)resourceManager.GetObject("errorProvider.Icon");
		base.Controls.Add(this.textBoxName);
		base.Controls.Add(this.labelName);
		base.Description = "{36}";
		base.Name = "EnterResourceName";
		base.Size = new System.Drawing.Size(352, 184);
		base.ValidateSheet += new System.ComponentModel.CancelEventHandler(EnterResourceName_ValidateSheet);
		base.Controls.SetChildIndex(this.labelName, 0);
		base.Controls.SetChildIndex(this.textBoxName, 0);
		base.ResumeLayout(false);
	}

	private void EnterResourceName_ValidateSheet(object sender, CancelEventArgs e)
	{
		errorProvider.SetError(textBoxName, "");
		try
		{
			_item.Name = textBoxName.Text;
		}
		catch (Exception ex)
		{
			e.Cancel = true;
			errorProvider.SetError(textBoxName, ex.Message);
		}
	}
}
