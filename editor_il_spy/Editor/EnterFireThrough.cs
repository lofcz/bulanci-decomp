using System;
using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms;

namespace Editor;

public class EnterFireThrough : SheetControl
{
	private CheckBox checkBox;

	private Container components;

	private ObstacleLevelItem _item;

	public ObstacleLevelItem ObstacleLevelItem
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
				checkBox.Checked = _item.FireThrough;
			}
		}
	}

	public EnterFireThrough()
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
		this.checkBox = new System.Windows.Forms.CheckBox();
		base.SuspendLayout();
		this.checkBox.Anchor = System.Windows.Forms.AnchorStyles.Top;
		this.checkBox.ImeMode = System.Windows.Forms.ImeMode.NoControl;
		this.checkBox.Location = new System.Drawing.Point(72, 96);
		this.checkBox.Name = "checkBox";
		this.checkBox.Size = new System.Drawing.Size(208, 24);
		this.checkBox.TabIndex = 1;
		this.checkBox.Text = "{56}";
		this.checkBox.Click += new System.EventHandler(checkBox_Click);
		base.Controls.Add(this.checkBox);
		base.Description = "{55}";
		base.Name = "EnterFireThrough";
		base.Size = new System.Drawing.Size(352, 200);
		base.Controls.SetChildIndex(this.checkBox, 0);
		base.ResumeLayout(false);
	}

	private void checkBox_Click(object sender, EventArgs e)
	{
		if (_item != null)
		{
			_item.FireThrough = checkBox.Checked;
		}
	}
}
