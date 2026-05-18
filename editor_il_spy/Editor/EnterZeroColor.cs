using System;
using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms;
using Editor.ResourceItems;

namespace Editor;

public class EnterZeroColor : SheetControl
{
	private CheckBox checkBox;

	private RadioButton radioLocation;

	private RadioButton radioColor;

	private Label labelX;

	private Label labelY;

	private NumericUpDown numericX;

	private NumericUpDown numericY;

	private Container components;

	private bool _last_location = true;

	private PickColorControl pickColor;

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
				if (_item.Transparency == ZeroColor.NotDefined)
				{
					checkBox.Checked = false;
					radioColor.Checked = false;
					radioLocation.Checked = false;
				}
				else if (_item.Transparency == ZeroColor.ByColor)
				{
					_last_location = false;
					checkBox.Checked = true;
					radioColor.Checked = true;
					radioLocation.Checked = false;
					pickColor.Color = _item.TransparentColor;
				}
				else if (_item.Transparency == ZeroColor.ByLocation)
				{
					_last_location = true;
					checkBox.Checked = true;
					radioColor.Checked = false;
					radioLocation.Checked = true;
					numericX.Value = _item.TransparentColorAt.X;
					numericY.Value = _item.TransparentColorAt.Y;
				}
				UpdateByItem();
			}
		}
	}

	public EnterZeroColor()
	{
		InitializeComponent();
	}

	public EnterZeroColor(BitmapResourceItem item)
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
		this.checkBox = new System.Windows.Forms.CheckBox();
		this.radioLocation = new System.Windows.Forms.RadioButton();
		this.radioColor = new System.Windows.Forms.RadioButton();
		this.labelX = new System.Windows.Forms.Label();
		this.labelY = new System.Windows.Forms.Label();
		this.numericX = new System.Windows.Forms.NumericUpDown();
		this.numericY = new System.Windows.Forms.NumericUpDown();
		this.pickColor = new Editor.PickColorControl();
		((System.ComponentModel.ISupportInitialize)this.numericX).BeginInit();
		((System.ComponentModel.ISupportInitialize)this.numericY).BeginInit();
		base.SuspendLayout();
		this.checkBox.Anchor = System.Windows.Forms.AnchorStyles.Top;
		this.checkBox.ImeMode = System.Windows.Forms.ImeMode.NoControl;
		this.checkBox.Location = new System.Drawing.Point(34, 88);
		this.checkBox.Name = "checkBox";
		this.checkBox.Size = new System.Drawing.Size(254, 16);
		this.checkBox.TabIndex = 1;
		this.checkBox.Text = "{32}";
		this.checkBox.CheckedChanged += new System.EventHandler(checkBox_CheckedChanged);
		this.radioLocation.Anchor = System.Windows.Forms.AnchorStyles.Top;
		this.radioLocation.Enabled = false;
		this.radioLocation.ImeMode = System.Windows.Forms.ImeMode.NoControl;
		this.radioLocation.Location = new System.Drawing.Point(56, 112);
		this.radioLocation.Name = "radioLocation";
		this.radioLocation.Size = new System.Drawing.Size(82, 16);
		this.radioLocation.TabIndex = 2;
		this.radioLocation.Text = "{33}";
		this.radioLocation.CheckedChanged += new System.EventHandler(radioLocation_CheckedChanged);
		this.radioColor.Anchor = System.Windows.Forms.AnchorStyles.Top;
		this.radioColor.Enabled = false;
		this.radioColor.ImeMode = System.Windows.Forms.ImeMode.NoControl;
		this.radioColor.Location = new System.Drawing.Point(56, 136);
		this.radioColor.Name = "radioColor";
		this.radioColor.Size = new System.Drawing.Size(82, 16);
		this.radioColor.TabIndex = 3;
		this.radioColor.Text = "{34}";
		this.radioColor.CheckedChanged += new System.EventHandler(radioColor_CheckedChanged);
		this.labelX.Anchor = System.Windows.Forms.AnchorStyles.Top;
		this.labelX.Enabled = false;
		this.labelX.ImeMode = System.Windows.Forms.ImeMode.NoControl;
		this.labelX.Location = new System.Drawing.Point(166, 112);
		this.labelX.Name = "labelX";
		this.labelX.Size = new System.Drawing.Size(14, 16);
		this.labelX.TabIndex = 4;
		this.labelX.Text = "{35}";
		this.labelY.Anchor = System.Windows.Forms.AnchorStyles.Top;
		this.labelY.Enabled = false;
		this.labelY.ImeMode = System.Windows.Forms.ImeMode.NoControl;
		this.labelY.Location = new System.Drawing.Point(246, 112);
		this.labelY.Name = "labelY";
		this.labelY.Size = new System.Drawing.Size(14, 16);
		this.labelY.TabIndex = 6;
		this.labelY.Text = "{37}";
		this.numericX.Anchor = System.Windows.Forms.AnchorStyles.Top;
		this.numericX.Enabled = false;
		this.numericX.Location = new System.Drawing.Point(182, 110);
		this.numericX.Name = "numericX";
		this.numericX.Size = new System.Drawing.Size(48, 20);
		this.numericX.TabIndex = 5;
		this.numericY.Anchor = System.Windows.Forms.AnchorStyles.Top;
		this.numericY.Enabled = false;
		this.numericY.Location = new System.Drawing.Point(262, 110);
		this.numericY.Name = "numericY";
		this.numericY.Size = new System.Drawing.Size(48, 20);
		this.numericY.TabIndex = 7;
		this.pickColor.Color = System.Drawing.SystemColors.Control;
		this.pickColor.Enabled = false;
		this.pickColor.Location = new System.Drawing.Point(166, 134);
		this.pickColor.Name = "pickColor";
		this.pickColor.Size = new System.Drawing.Size(90, 20);
		this.pickColor.TabIndex = 8;
		base.Controls.Add(this.pickColor);
		base.Controls.Add(this.numericY);
		base.Controls.Add(this.numericX);
		base.Controls.Add(this.labelY);
		base.Controls.Add(this.labelX);
		base.Controls.Add(this.radioColor);
		base.Controls.Add(this.radioLocation);
		base.Controls.Add(this.checkBox);
		base.Description = "{31}";
		base.Name = "EnterZeroColor";
		base.Size = new System.Drawing.Size(352, 196);
		base.Activated += new System.EventHandler(EnterZeroColor_Activated);
		base.ValidateSheet += new System.ComponentModel.CancelEventHandler(EnterZeroColor_ValidateSheet);
		base.Controls.SetChildIndex(this.checkBox, 0);
		base.Controls.SetChildIndex(this.radioLocation, 0);
		base.Controls.SetChildIndex(this.radioColor, 0);
		base.Controls.SetChildIndex(this.labelX, 0);
		base.Controls.SetChildIndex(this.labelY, 0);
		base.Controls.SetChildIndex(this.numericX, 0);
		base.Controls.SetChildIndex(this.numericY, 0);
		base.Controls.SetChildIndex(this.pickColor, 0);
		((System.ComponentModel.ISupportInitialize)this.numericX).EndInit();
		((System.ComponentModel.ISupportInitialize)this.numericY).EndInit();
		base.ResumeLayout(false);
	}

	private void radioColor_CheckedChanged(object sender, EventArgs e)
	{
		pickColor.Enabled = radioColor.Checked;
	}

	private void radioLocation_CheckedChanged(object sender, EventArgs e)
	{
		Label label = labelX;
		Label label2 = labelY;
		NumericUpDown numericUpDown = numericX;
		bool flag = (numericY.Enabled = radioLocation.Checked);
		bool flag3 = (numericUpDown.Enabled = flag);
		bool enabled = (label2.Enabled = flag3);
		label.Enabled = enabled;
	}

	private void checkBox_CheckedChanged(object sender, EventArgs e)
	{
		if (checkBox.Checked)
		{
			RadioButton radioButton = radioLocation;
			bool enabled = (radioColor.Enabled = true);
			radioButton.Enabled = enabled;
			radioLocation.Checked = _last_location;
			radioColor.Checked = !_last_location;
		}
		else
		{
			_last_location = radioLocation.Checked;
			RadioButton radioButton2 = radioLocation;
			bool enabled = (radioColor.Enabled = false);
			radioButton2.Enabled = enabled;
			radioLocation.Checked = false;
			radioColor.Checked = false;
		}
	}

	private void UpdateByItem()
	{
		decimal value = numericX.Value;
		decimal value2 = numericY.Value;
		NumericUpDown numericUpDown = numericX;
		NumericUpDown numericUpDown2 = numericY;
		NumericUpDown numericUpDown3 = numericX;
		decimal num = (numericY.Maximum = default(decimal));
		decimal num3 = (numericUpDown3.Maximum = num);
		decimal minimum = (numericUpDown2.Minimum = num3);
		numericUpDown.Minimum = minimum;
		Bitmap bitmap = _item.Bitmap;
		if (bitmap != null)
		{
			numericX.Maximum = bitmap.Width - 1;
			numericY.Maximum = bitmap.Height - 1;
		}
		numericX.Value = Math.Min(numericX.Maximum, Math.Max(numericX.Minimum, value));
		numericY.Value = Math.Min(numericY.Maximum, Math.Max(numericY.Minimum, value2));
	}

	private void EnterZeroColor_Activated(object sender, EventArgs e)
	{
		UpdateByItem();
	}

	private void EnterZeroColor_ValidateSheet(object sender, CancelEventArgs e)
	{
		if (!checkBox.Checked)
		{
			_item.Transparency = ZeroColor.NotDefined;
		}
		else if (radioLocation.Checked)
		{
			_item.Transparency = ZeroColor.ByLocation;
			_item.TransparentColorAt = new Point(Convert.ToInt32(numericX.Value), Convert.ToInt32(numericY.Value));
		}
		else
		{
			_item.Transparency = ZeroColor.ByColor;
			_item.TransparentColor = pickColor.Color;
		}
	}
}
