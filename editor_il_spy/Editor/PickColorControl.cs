using System;
using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms;

namespace Editor;

public class PickColorControl : UserControl
{
	private ShowColorControl showColorControl;

	private Button button;

	private ColorDialog colorDialog;

	private Container components;

	public Color Color
	{
		get
		{
			return showColorControl.Color;
		}
		set
		{
			showColorControl.Color = value;
			colorDialog.Color = value;
		}
	}

	public PickColorControl()
	{
		InitializeComponent();
		colorDialog.Color = showColorControl.Color;
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
		this.showColorControl = new Editor.ShowColorControl();
		this.button = new System.Windows.Forms.Button();
		this.colorDialog = new System.Windows.Forms.ColorDialog();
		base.SuspendLayout();
		this.showColorControl.Anchor = System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left | System.Windows.Forms.AnchorStyles.Right;
		this.showColorControl.BackColor = System.Drawing.SystemColors.Control;
		this.showColorControl.Color = System.Drawing.SystemColors.Control;
		this.showColorControl.Location = new System.Drawing.Point(0, 0);
		this.showColorControl.Name = "showColorControl";
		this.showColorControl.Size = new System.Drawing.Size(86, 20);
		this.showColorControl.TabIndex = 0;
		this.button.Anchor = System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right;
		this.button.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25f, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, 238);
		this.button.Location = new System.Drawing.Point(96, 0);
		this.button.Name = "button";
		this.button.Size = new System.Drawing.Size(24, 20);
		this.button.TabIndex = 1;
		this.button.Text = "...";
		this.button.Click += new System.EventHandler(button_Click);
		base.Controls.Add(this.button);
		base.Controls.Add(this.showColorControl);
		base.Name = "PickColorControl";
		base.Size = new System.Drawing.Size(120, 20);
		base.ResumeLayout(false);
	}

	private void button_Click(object sender, EventArgs e)
	{
		if (colorDialog.ShowDialog() == DialogResult.OK)
		{
			Color = colorDialog.Color;
		}
	}
}
