using System;
using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms;
using Editor.ResourceItems;
using Editor.Temp;

namespace Editor;

public class EnterItemBounds : SheetControl
{
	private Label labelX;

	private Label labelY;

	private Label labelWidth;

	private Label labelHeight;

	private NumericUpDown numericX;

	private NumericUpDown numericY;

	private NumericUpDown numericHeight;

	private NumericUpDown numericWidth;

	private Container components;

	private LevelItem _item;

	[Browsable(false)]
	private new string Description
	{
		get
		{
			return base.Description;
		}
		set
		{
			base.Description = value;
		}
	}

	[Browsable(false)]
	private new bool DescriptionVisible
	{
		get
		{
			return base.DescriptionVisible;
		}
		set
		{
			base.DescriptionVisible = value;
		}
	}

	public bool SizeEnabled
	{
		get
		{
			return labelWidth.Enabled;
		}
		set
		{
			Label label = labelWidth;
			bool enabled = (labelHeight.Enabled = value);
			label.Enabled = enabled;
			NumericUpDown numericUpDown = numericHeight;
			enabled = (numericWidth.Enabled = value);
			numericUpDown.Enabled = enabled;
		}
	}

	public LevelItem LevelItem
	{
		get
		{
			return _item;
		}
		set
		{
			_item = value;
			NumericUpDown numericUpDown = numericX;
			decimal minimum = (numericY.Minimum = default(decimal));
			numericUpDown.Minimum = minimum;
			NumericUpDown numericUpDown2 = numericX;
			minimum = (numericY.Maximum = 2147483647m);
			numericUpDown2.Maximum = minimum;
			NumericUpDown numericUpDown3 = numericWidth;
			minimum = (numericHeight.Minimum = default(decimal));
			numericUpDown3.Minimum = minimum;
			NumericUpDown numericUpDown4 = numericWidth;
			minimum = (numericHeight.Maximum = 2147483647m);
			numericUpDown4.Maximum = minimum;
			if (_item != null)
			{
				numericX.Value = _item.Rectangle.X;
				numericY.Value = _item.Rectangle.Y;
				numericWidth.Value = _item.Rectangle.Width;
				numericHeight.Value = _item.Rectangle.Height;
			}
		}
	}

	public EnterItemBounds()
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
		this.labelX = new System.Windows.Forms.Label();
		this.labelY = new System.Windows.Forms.Label();
		this.numericX = new System.Windows.Forms.NumericUpDown();
		this.numericY = new System.Windows.Forms.NumericUpDown();
		this.labelWidth = new System.Windows.Forms.Label();
		this.labelHeight = new System.Windows.Forms.Label();
		this.numericHeight = new System.Windows.Forms.NumericUpDown();
		this.numericWidth = new System.Windows.Forms.NumericUpDown();
		((System.ComponentModel.ISupportInitialize)this.numericX).BeginInit();
		((System.ComponentModel.ISupportInitialize)this.numericY).BeginInit();
		((System.ComponentModel.ISupportInitialize)this.numericHeight).BeginInit();
		((System.ComponentModel.ISupportInitialize)this.numericWidth).BeginInit();
		base.SuspendLayout();
		this.labelX.Anchor = System.Windows.Forms.AnchorStyles.Top;
		this.labelX.ImeMode = System.Windows.Forms.ImeMode.NoControl;
		this.labelX.Location = new System.Drawing.Point(48, 100);
		this.labelX.Name = "labelX";
		this.labelX.Size = new System.Drawing.Size(40, 18);
		this.labelX.TabIndex = 1;
		this.labelX.Text = "{2}";
		this.labelX.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
		this.labelY.Anchor = System.Windows.Forms.AnchorStyles.Top;
		this.labelY.ImeMode = System.Windows.Forms.ImeMode.NoControl;
		this.labelY.Location = new System.Drawing.Point(48, 124);
		this.labelY.Name = "labelY";
		this.labelY.Size = new System.Drawing.Size(40, 18);
		this.labelY.TabIndex = 3;
		this.labelY.Text = "{3}";
		this.labelY.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
		this.numericX.Anchor = System.Windows.Forms.AnchorStyles.Top;
		this.numericX.Location = new System.Drawing.Point(92, 98);
		this.numericX.Name = "numericX";
		this.numericX.Size = new System.Drawing.Size(70, 20);
		this.numericX.TabIndex = 2;
		this.numericX.ValueChanged += new System.EventHandler(numericX_ValueChanged);
		this.numericX.Leave += new System.EventHandler(numericX_Leave);
		this.numericY.Anchor = System.Windows.Forms.AnchorStyles.Top;
		this.numericY.Location = new System.Drawing.Point(92, 122);
		this.numericY.Name = "numericY";
		this.numericY.Size = new System.Drawing.Size(70, 20);
		this.numericY.TabIndex = 4;
		this.numericY.ValueChanged += new System.EventHandler(numericY_ValueChanged);
		this.numericY.Leave += new System.EventHandler(numericY_Leave);
		this.labelWidth.Anchor = System.Windows.Forms.AnchorStyles.Top;
		this.labelWidth.ImeMode = System.Windows.Forms.ImeMode.NoControl;
		this.labelWidth.Location = new System.Drawing.Point(190, 100);
		this.labelWidth.Name = "labelWidth";
		this.labelWidth.Size = new System.Drawing.Size(40, 18);
		this.labelWidth.TabIndex = 5;
		this.labelWidth.Text = "{4}";
		this.labelWidth.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
		this.labelHeight.Anchor = System.Windows.Forms.AnchorStyles.Top;
		this.labelHeight.ImeMode = System.Windows.Forms.ImeMode.NoControl;
		this.labelHeight.Location = new System.Drawing.Point(190, 124);
		this.labelHeight.Name = "labelHeight";
		this.labelHeight.Size = new System.Drawing.Size(40, 18);
		this.labelHeight.TabIndex = 7;
		this.labelHeight.Text = "{5}";
		this.labelHeight.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
		this.numericHeight.Anchor = System.Windows.Forms.AnchorStyles.Top;
		this.numericHeight.Location = new System.Drawing.Point(234, 122);
		this.numericHeight.Name = "numericHeight";
		this.numericHeight.Size = new System.Drawing.Size(70, 20);
		this.numericHeight.TabIndex = 8;
		this.numericHeight.ValueChanged += new System.EventHandler(numericHeight_ValueChanged);
		this.numericHeight.Leave += new System.EventHandler(numericHeight_Leave);
		this.numericWidth.Anchor = System.Windows.Forms.AnchorStyles.Top;
		this.numericWidth.Location = new System.Drawing.Point(234, 98);
		this.numericWidth.Name = "numericWidth";
		this.numericWidth.Size = new System.Drawing.Size(70, 20);
		this.numericWidth.TabIndex = 6;
		this.numericWidth.ValueChanged += new System.EventHandler(numericWidth_ValueChanged);
		this.numericWidth.Leave += new System.EventHandler(numericWidth_Leave);
		base.Controls.Add(this.numericWidth);
		base.Controls.Add(this.numericHeight);
		base.Controls.Add(this.labelHeight);
		base.Controls.Add(this.labelWidth);
		base.Controls.Add(this.numericY);
		base.Controls.Add(this.numericX);
		base.Controls.Add(this.labelY);
		base.Controls.Add(this.labelX);
		this.Description = "{1}";
		base.Name = "EnterItemBounds";
		base.Size = new System.Drawing.Size(352, 196);
		base.Controls.SetChildIndex(this.labelX, 0);
		base.Controls.SetChildIndex(this.labelY, 0);
		base.Controls.SetChildIndex(this.numericX, 0);
		base.Controls.SetChildIndex(this.numericY, 0);
		base.Controls.SetChildIndex(this.labelWidth, 0);
		base.Controls.SetChildIndex(this.labelHeight, 0);
		base.Controls.SetChildIndex(this.numericHeight, 0);
		base.Controls.SetChildIndex(this.numericWidth, 0);
		((System.ComponentModel.ISupportInitialize)this.numericX).EndInit();
		((System.ComponentModel.ISupportInitialize)this.numericY).EndInit();
		((System.ComponentModel.ISupportInitialize)this.numericHeight).EndInit();
		((System.ComponentModel.ISupportInitialize)this.numericWidth).EndInit();
		base.ResumeLayout(false);
	}

	private void numericX_ValueChanged(object sender, EventArgs e)
	{
		if (LevelItem != null)
		{
			int num = Math.Min(BackgroundResourceItem.BitmapSize.Width - LevelItem.Rectangle.Width, Convert.ToInt32(numericX.Value));
			if (numericX.Value != (decimal)num)
			{
				numericX.Value = num;
			}
			Rectangle rectangle = LevelItem.Rectangle;
			rectangle.X = num;
			LevelItem.Rectangle = rectangle;
		}
	}

	private void numericX_Leave(object sender, EventArgs e)
	{
		numericX_ValueChanged(sender, e);
	}

	private void numericY_ValueChanged(object sender, EventArgs e)
	{
		if (LevelItem != null)
		{
			int num = Math.Min(BackgroundResourceItem.BitmapSize.Height - LevelItem.Rectangle.Height, Convert.ToInt32(numericY.Value));
			if (numericY.Value != (decimal)num)
			{
				numericY.Value = num;
			}
			Rectangle rectangle = LevelItem.Rectangle;
			rectangle.Y = num;
			LevelItem.Rectangle = rectangle;
		}
	}

	private void numericY_Leave(object sender, EventArgs e)
	{
		numericY_ValueChanged(sender, e);
	}

	private void numericWidth_ValueChanged(object sender, EventArgs e)
	{
		if (SizeEnabled && LevelItem != null)
		{
			int num = Math.Max(DragPointView.MinSize, Math.Min(BackgroundResourceItem.BitmapSize.Width - LevelItem.Rectangle.Left, Convert.ToInt32(numericWidth.Value)));
			if (numericWidth.Value != (decimal)num)
			{
				numericWidth.Value = num;
			}
			Rectangle rectangle = LevelItem.Rectangle;
			rectangle.Width = num;
			LevelItem.Rectangle = rectangle;
		}
	}

	private void numericWidth_Leave(object sender, EventArgs e)
	{
		numericWidth_ValueChanged(sender, e);
	}

	private void numericHeight_ValueChanged(object sender, EventArgs e)
	{
		if (SizeEnabled && LevelItem != null)
		{
			int num = Math.Max(DragPointView.MinSize, Math.Min(BackgroundResourceItem.BitmapSize.Height - LevelItem.Rectangle.Top, Convert.ToInt32(numericHeight.Value)));
			if (numericHeight.Value != (decimal)num)
			{
				numericHeight.Value = num;
			}
			Rectangle rectangle = LevelItem.Rectangle;
			rectangle.Height = num;
			LevelItem.Rectangle = rectangle;
		}
	}

	private void numericHeight_Leave(object sender, EventArgs e)
	{
		numericHeight_ValueChanged(sender, e);
	}
}
