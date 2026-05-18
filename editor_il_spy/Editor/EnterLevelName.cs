using System;
using System.ComponentModel;
using System.Drawing;
using System.Resources;
using System.Windows.Forms;

namespace Editor;

public class EnterLevelName : SheetControl
{
	private Label labelName;

	private TextBox textBoxName;

	private Label label1;

	private TextBox textBoxBy;

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
			textBoxName.Text = _level.Name;
			textBoxBy.Text = _level.CreatedBy;
		}
	}

	public EnterLevelName()
	{
		InitializeComponent();
	}

	public EnterLevelName(Level level)
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
		System.Resources.ResourceManager resourceManager = new System.Resources.ResourceManager(typeof(Editor.EnterLevelName));
		this.labelName = new System.Windows.Forms.Label();
		this.textBoxName = new System.Windows.Forms.TextBox();
		this.label1 = new System.Windows.Forms.Label();
		this.textBoxBy = new System.Windows.Forms.TextBox();
		this.errorProvider = new System.Windows.Forms.ErrorProvider();
		base.SuspendLayout();
		this.labelName.Anchor = System.Windows.Forms.AnchorStyles.Top;
		this.labelName.ImeMode = System.Windows.Forms.ImeMode.NoControl;
		this.labelName.Location = new System.Drawing.Point(32, 80);
		this.labelName.Name = "labelName";
		this.labelName.Size = new System.Drawing.Size(288, 16);
		this.labelName.TabIndex = 1;
		this.labelName.Text = "{48}";
		this.textBoxName.Anchor = System.Windows.Forms.AnchorStyles.Top;
		this.textBoxName.Location = new System.Drawing.Point(32, 96);
		this.textBoxName.MaxLength = 32;
		this.textBoxName.Name = "textBoxName";
		this.textBoxName.Size = new System.Drawing.Size(288, 20);
		this.textBoxName.TabIndex = 2;
		this.textBoxName.Text = "";
		this.label1.Anchor = System.Windows.Forms.AnchorStyles.Top;
		this.label1.ImeMode = System.Windows.Forms.ImeMode.NoControl;
		this.label1.Location = new System.Drawing.Point(32, 128);
		this.label1.Name = "label1";
		this.label1.Size = new System.Drawing.Size(288, 16);
		this.label1.TabIndex = 3;
		this.label1.Text = "{49}";
		this.textBoxBy.Anchor = System.Windows.Forms.AnchorStyles.Top;
		this.textBoxBy.Location = new System.Drawing.Point(32, 144);
		this.textBoxBy.Name = "textBoxBy";
		this.textBoxBy.Size = new System.Drawing.Size(288, 20);
		this.textBoxBy.TabIndex = 4;
		this.textBoxBy.Text = "";
		this.errorProvider.ContainerControl = this;
		this.errorProvider.Icon = (System.Drawing.Icon)resourceManager.GetObject("errorProvider.Icon");
		base.Controls.Add(this.textBoxBy);
		base.Controls.Add(this.label1);
		base.Controls.Add(this.textBoxName);
		base.Controls.Add(this.labelName);
		base.Description = "{47}";
		base.Name = "EnterLevelName";
		base.Size = new System.Drawing.Size(352, 200);
		base.ValidateSheet += new System.ComponentModel.CancelEventHandler(EnterLevelName_ValidateSheet);
		base.Controls.SetChildIndex(this.labelName, 0);
		base.Controls.SetChildIndex(this.textBoxName, 0);
		base.Controls.SetChildIndex(this.label1, 0);
		base.Controls.SetChildIndex(this.textBoxBy, 0);
		base.ResumeLayout(false);
	}

	private void EnterLevelName_ValidateSheet(object sender, CancelEventArgs e)
	{
		errorProvider.SetError(textBoxName, "");
		errorProvider.SetError(textBoxBy, "");
		try
		{
			_level.Name = textBoxName.Text;
		}
		catch (Exception ex)
		{
			errorProvider.SetError(textBoxName, ex.Message);
			e.Cancel = true;
		}
		try
		{
			_level.CreatedBy = textBoxBy.Text;
		}
		catch (Exception ex2)
		{
			errorProvider.SetError(textBoxBy, ex2.Message);
			e.Cancel = true;
		}
	}
}
