using System;
using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms;

namespace Editor;

public class EnterItemName : SheetControl
{
	private TextBox textBoxName;

	private Label labelName;

	private ErrorProvider errorProvider;

	private IContainer components;

	private LevelItem _item;

	public LevelItem LevelItem
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

	public EnterItemName()
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
		this.components = new System.ComponentModel.Container();
		this.textBoxName = new System.Windows.Forms.TextBox();
		this.labelName = new System.Windows.Forms.Label();
		this.errorProvider = new System.Windows.Forms.ErrorProvider(this.components);
		((System.ComponentModel.ISupportInitialize)this.errorProvider).BeginInit();
		base.SuspendLayout();
		this.textBoxName.Anchor = System.Windows.Forms.AnchorStyles.Top;
		this.textBoxName.Location = new System.Drawing.Point(32, 112);
		this.textBoxName.MaxLength = 128;
		this.textBoxName.Name = "textBoxName";
		this.textBoxName.Size = new System.Drawing.Size(288, 20);
		this.textBoxName.TabIndex = 1;
		this.textBoxName.TextChanged += new System.EventHandler(textBoxName_TextChanged);
		this.labelName.Anchor = System.Windows.Forms.AnchorStyles.Top;
		this.labelName.Location = new System.Drawing.Point(32, 96);
		this.labelName.Name = "labelName";
		this.labelName.Size = new System.Drawing.Size(288, 16);
		this.labelName.TabIndex = 0;
		this.labelName.Text = "{151}";
		this.errorProvider.ContainerControl = this;
		base.Controls.Add(this.labelName);
		base.Controls.Add(this.textBoxName);
		base.Description = "{150}";
		base.Name = "EnterItemName";
		base.Controls.SetChildIndex(this.textBoxName, 0);
		base.Controls.SetChildIndex(this.labelName, 0);
		((System.ComponentModel.ISupportInitialize)this.errorProvider).EndInit();
		base.ResumeLayout(false);
		base.PerformLayout();
	}

	private void textBoxName_TextChanged(object sender, EventArgs e)
	{
		errorProvider.SetError(textBoxName, "");
		try
		{
			LevelItem.Name = textBoxName.Text;
		}
		catch (Exception ex)
		{
			errorProvider.SetError(textBoxName, ex.Message);
		}
	}
}
