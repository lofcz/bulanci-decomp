using System;
using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms;

namespace Editor;

public class SheetControl : UserControl
{
	private Label labelDesc;

	[Browsable(true)]
	public string Description
	{
		get
		{
			return labelDesc.Text;
		}
		set
		{
			labelDesc.Text = value;
		}
	}

	[Browsable(true)]
	[DefaultValue(true)]
	public bool DescriptionVisible
	{
		get
		{
			return labelDesc.Visible;
		}
		set
		{
			labelDesc.Visible = value;
		}
	}

	public event CancelEventHandler ValidateSheet;

	public event EventHandler Activated;

	public SheetControl()
	{
		InitializeComponent();
		this.ValidateSheet = null;
		this.Activated = null;
	}

	public bool OnValidateSheet(object sender)
	{
		if (this.ValidateSheet != null)
		{
			CancelEventArgs e = new CancelEventArgs();
			this.ValidateSheet(sender, e);
			return !e.Cancel;
		}
		return true;
	}

	public void OnActivated(object sender)
	{
		if (this.Activated != null)
		{
			this.Activated(sender, EventArgs.Empty);
		}
	}

	private void InitializeComponent()
	{
		this.labelDesc = new System.Windows.Forms.Label();
		base.SuspendLayout();
		this.labelDesc.Anchor = System.Windows.Forms.AnchorStyles.Top;
		this.labelDesc.ImeMode = System.Windows.Forms.ImeMode.NoControl;
		this.labelDesc.Location = new System.Drawing.Point(32, 16);
		this.labelDesc.Name = "labelDesc";
		this.labelDesc.Size = new System.Drawing.Size(288, 56);
		this.labelDesc.TabIndex = 0;
		this.labelDesc.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
		base.Controls.Add(this.labelDesc);
		base.Name = "SheetControl";
		base.Size = new System.Drawing.Size(352, 180);
		base.ResumeLayout(false);
	}
}
