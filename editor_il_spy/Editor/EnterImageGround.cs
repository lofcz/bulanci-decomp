using System;
using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms;
using Editor.ResourceItems;
using Editor.Temp;

namespace Editor;

public class EnterImageGround : SheetControl
{
	private Label label1;

	private NumericUpDown numericUpDown1;

	private Container components;

	private ImageLevelItem _item;

	private ItemEventHandler _inserted;

	public ImageLevelItem ImageLevelItem
	{
		get
		{
			return _item;
		}
		set
		{
			if (_item != null)
			{
				_item.Level.ItemInserted -= _inserted;
			}
			_item = value;
			if (_item != null)
			{
				_item.Level.ItemInserted += _inserted;
			}
			UpdateControls();
		}
	}

	public bool GroundEnabled
	{
		get
		{
			return numericUpDown1.Enabled;
		}
		set
		{
			numericUpDown1.Enabled = value;
			label1.Enabled = value;
		}
	}

	private void UpdateControls()
	{
		if (_item != null)
		{
			numericUpDown1.Value = _item.Ground;
			GroundEnabled = _item.Layer == LayerType.Layer1;
		}
	}

	private void ItemInserted(object sender, ItemEventArgs args)
	{
		if (args.LevelItem == _item)
		{
			UpdateControls();
		}
	}

	public EnterImageGround()
	{
		InitializeComponent();
		_inserted = ItemInserted;
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
		this.label1 = new System.Windows.Forms.Label();
		this.numericUpDown1 = new System.Windows.Forms.NumericUpDown();
		((System.ComponentModel.ISupportInitialize)this.numericUpDown1).BeginInit();
		base.SuspendLayout();
		this.label1.Anchor = System.Windows.Forms.AnchorStyles.Top;
		this.label1.ImeMode = System.Windows.Forms.ImeMode.NoControl;
		this.label1.Location = new System.Drawing.Point(80, 96);
		this.label1.Name = "label1";
		this.label1.Size = new System.Drawing.Size(72, 20);
		this.label1.TabIndex = 1;
		this.label1.Text = "{54}";
		this.label1.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
		this.numericUpDown1.Anchor = System.Windows.Forms.AnchorStyles.Top;
		this.numericUpDown1.Location = new System.Drawing.Point(160, 96);
		this.numericUpDown1.Maximum = new decimal(new int[4] { 1000, 0, 0, 0 });
		this.numericUpDown1.Minimum = new decimal(new int[4] { 1000, 0, 0, -2147483648 });
		this.numericUpDown1.Name = "numericUpDown1";
		this.numericUpDown1.Size = new System.Drawing.Size(72, 20);
		this.numericUpDown1.TabIndex = 2;
		this.numericUpDown1.ValueChanged += new System.EventHandler(numericUpDown1_ValueChanged);
		base.Controls.Add(this.numericUpDown1);
		base.Controls.Add(this.label1);
		base.Description = "{53}";
		base.Name = "EnterImageGround";
		base.Size = new System.Drawing.Size(352, 200);
		base.Controls.SetChildIndex(this.label1, 0);
		base.Controls.SetChildIndex(this.numericUpDown1, 0);
		((System.ComponentModel.ISupportInitialize)this.numericUpDown1).EndInit();
		base.ResumeLayout(false);
	}

	private void numericUpDown1_ValueChanged(object sender, EventArgs e)
	{
		if (_item != null)
		{
			int bottom = _item.Rectangle.Bottom;
			int num = Math.Max(-bottom + 1, Math.Min(BackgroundResourceItem.BitmapSize.Height - bottom, Convert.ToInt32(numericUpDown1.Value)));
			if (numericUpDown1.Value != (decimal)num)
			{
				numericUpDown1.Value = num;
			}
			_item.Ground = num;
		}
	}
}
