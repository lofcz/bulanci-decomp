using System;
using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms;

namespace Editor;

public class EnterOpponents : SheetControl
{
	private CheckBox checkVampires;

	private CheckBox checkOpponents;

	private NumericUpDown numericCount;

	private Button buttonDetails;

	private Container components;

	private Level _level;

	public Level Level
	{
		get
		{
			return _level;
		}
		set
		{
			checkVampires.Checked = false;
			checkOpponents.Checked = false;
			numericCount.Value = 1m;
			_level = value;
			if (_level != null)
			{
				checkVampires.Checked = _level.Vampires;
				checkOpponents.Checked = _level.Opponents.Count > 0;
				numericCount.Value = Math.Max(_level.Opponents.Count, 1);
			}
		}
	}

	public EnterOpponents()
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
		this.checkVampires = new System.Windows.Forms.CheckBox();
		this.checkOpponents = new System.Windows.Forms.CheckBox();
		this.numericCount = new System.Windows.Forms.NumericUpDown();
		this.buttonDetails = new System.Windows.Forms.Button();
		((System.ComponentModel.ISupportInitialize)this.numericCount).BeginInit();
		base.SuspendLayout();
		this.checkVampires.Location = new System.Drawing.Point(32, 100);
		this.checkVampires.Name = "checkVampires";
		this.checkVampires.Size = new System.Drawing.Size(200, 16);
		this.checkVampires.TabIndex = 1;
		this.checkVampires.Text = "{160}";
		this.checkVampires.CheckedChanged += new System.EventHandler(checkVampires_CheckedChanged);
		this.checkOpponents.Location = new System.Drawing.Point(32, 122);
		this.checkOpponents.Name = "checkOpponents";
		this.checkOpponents.Size = new System.Drawing.Size(200, 16);
		this.checkOpponents.TabIndex = 2;
		this.checkOpponents.Text = "{161}";
		this.checkOpponents.CheckedChanged += new System.EventHandler(checkOpponents_CheckedChanged);
		this.numericCount.Enabled = false;
		this.numericCount.Location = new System.Drawing.Point(236, 120);
		this.numericCount.Maximum = new decimal(new int[4] { 4, 0, 0, 0 });
		this.numericCount.Minimum = new decimal(new int[4] { 1, 0, 0, 0 });
		this.numericCount.Name = "numericCount";
		this.numericCount.Size = new System.Drawing.Size(36, 20);
		this.numericCount.TabIndex = 3;
		this.numericCount.Value = new decimal(new int[4] { 1, 0, 0, 0 });
		this.numericCount.ValueChanged += new System.EventHandler(numericCount_ValueChanged);
		this.buttonDetails.Enabled = false;
		this.buttonDetails.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25f, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, 238);
		this.buttonDetails.Location = new System.Drawing.Point(276, 120);
		this.buttonDetails.Name = "buttonDetails";
		this.buttonDetails.Size = new System.Drawing.Size(30, 20);
		this.buttonDetails.TabIndex = 4;
		this.buttonDetails.Text = "...";
		this.buttonDetails.Click += new System.EventHandler(buttonDetails_Click);
		base.Controls.Add(this.buttonDetails);
		base.Controls.Add(this.numericCount);
		base.Controls.Add(this.checkOpponents);
		base.Controls.Add(this.checkVampires);
		base.Description = "{159}";
		base.Name = "EnterOpponents";
		base.Controls.SetChildIndex(this.checkVampires, 0);
		base.Controls.SetChildIndex(this.checkOpponents, 0);
		base.Controls.SetChildIndex(this.numericCount, 0);
		base.Controls.SetChildIndex(this.buttonDetails, 0);
		((System.ComponentModel.ISupportInitialize)this.numericCount).EndInit();
		base.ResumeLayout(false);
	}

	private void checkOpponents_CheckedChanged(object sender, EventArgs e)
	{
		numericCount.Enabled = checkOpponents.Checked;
		buttonDetails.Enabled = checkOpponents.Checked;
		if (!checkOpponents.Checked && Level.Opponents.Count > 0)
		{
			numericCount.Value = 1m;
			Level.Opponents.Clear();
		}
		else if (checkOpponents.Checked && Level.Opponents.Count == 0)
		{
			Level.Opponents.Add(new Opponent());
		}
	}

	private void numericCount_ValueChanged(object sender, EventArgs e)
	{
		int num = Convert.ToInt32(numericCount.Value);
		while (num < Level.Opponents.Count)
		{
			Level.Opponents.RemoveAt(Level.Opponents.Count - 1);
		}
		while (num > Level.Opponents.Count)
		{
			Level.Opponents.Add(new Opponent());
		}
	}

	private void checkVampires_CheckedChanged(object sender, EventArgs e)
	{
		Level.Vampires = checkVampires.Checked;
	}

	private void buttonDetails_Click(object sender, EventArgs e)
	{
		new OpponentPropertiesForm(Level).ShowDialog();
	}
}
