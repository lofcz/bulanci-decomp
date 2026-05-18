using System;
using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms;
using Editor.Temp;

namespace Editor;

public class EnterItemZOrder : SheetControl
{
	private Label labelZ;

	private Label labelLayer;

	private ComboBox comboLayer;

	private NumericUpDown numericZ;

	private Container components;

	private LevelItem _item;

	[Browsable(true)]
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

	[Browsable(true)]
	[DefaultValue(true)]
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
				comboLayer.SelectedItem = GetLayer(_item.Layer);
				numericZ.Value = _item.ZOrder;
			}
		}
	}

	public bool LayerEnabled
	{
		get
		{
			return comboLayer.Enabled;
		}
		set
		{
			comboLayer.Enabled = value;
			labelLayer.Enabled = value;
		}
	}

	public EnterItemZOrder()
	{
		InitializeComponent();
		comboLayer.Items.Add(GetLayer(LayerType.Layer0));
		comboLayer.Items.Add(GetLayer(LayerType.Layer1));
		comboLayer.Items.Add(GetLayer(LayerType.Layer2));
	}

	private object GetLayer(LayerType layer)
	{
		return layer switch
		{
			LayerType.Layer0 => Tools.Localizer.GetString(70), 
			LayerType.Layer1 => Tools.Localizer.GetString(71), 
			LayerType.Layer2 => Tools.Localizer.GetString(72), 
			_ => null, 
		};
	}

	private LayerType GetLayerFromItem(object item)
	{
		if (item == GetLayer(LayerType.Layer0))
		{
			return LayerType.Layer0;
		}
		if (item == GetLayer(LayerType.Layer1))
		{
			return LayerType.Layer1;
		}
		if (item == GetLayer(LayerType.Layer2))
		{
			return LayerType.Layer2;
		}
		return LayerType.None;
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
		this.labelZ = new System.Windows.Forms.Label();
		this.labelLayer = new System.Windows.Forms.Label();
		this.comboLayer = new System.Windows.Forms.ComboBox();
		this.numericZ = new System.Windows.Forms.NumericUpDown();
		((System.ComponentModel.ISupportInitialize)this.numericZ).BeginInit();
		base.SuspendLayout();
		this.labelZ.Anchor = System.Windows.Forms.AnchorStyles.Top;
		this.labelZ.ImeMode = System.Windows.Forms.ImeMode.NoControl;
		this.labelZ.Location = new System.Drawing.Point(80, 120);
		this.labelZ.Name = "labelZ";
		this.labelZ.Size = new System.Drawing.Size(64, 20);
		this.labelZ.TabIndex = 3;
		this.labelZ.Text = "{52}";
		this.labelZ.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
		this.labelLayer.Anchor = System.Windows.Forms.AnchorStyles.Top;
		this.labelLayer.ImeMode = System.Windows.Forms.ImeMode.NoControl;
		this.labelLayer.Location = new System.Drawing.Point(80, 96);
		this.labelLayer.Name = "labelLayer";
		this.labelLayer.Size = new System.Drawing.Size(64, 20);
		this.labelLayer.TabIndex = 1;
		this.labelLayer.Text = "{51}";
		this.labelLayer.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
		this.comboLayer.Anchor = System.Windows.Forms.AnchorStyles.Top;
		this.comboLayer.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
		this.comboLayer.ItemHeight = 13;
		this.comboLayer.Location = new System.Drawing.Point(144, 96);
		this.comboLayer.Name = "comboLayer";
		this.comboLayer.Size = new System.Drawing.Size(96, 21);
		this.comboLayer.TabIndex = 2;
		this.comboLayer.SelectedIndexChanged += new System.EventHandler(comboLayer_SelectedIndexChanged);
		this.numericZ.Anchor = System.Windows.Forms.AnchorStyles.Top;
		this.numericZ.Location = new System.Drawing.Point(144, 120);
		this.numericZ.Maximum = new decimal(new int[4] { 1000, 0, 0, 0 });
		this.numericZ.Name = "numericZ";
		this.numericZ.Size = new System.Drawing.Size(96, 20);
		this.numericZ.TabIndex = 4;
		this.numericZ.ValueChanged += new System.EventHandler(numericZ_ValueChanged);
		this.numericZ.Leave += new System.EventHandler(numericZ_Leave);
		base.Controls.Add(this.numericZ);
		base.Controls.Add(this.comboLayer);
		base.Controls.Add(this.labelLayer);
		base.Controls.Add(this.labelZ);
		this.Description = "{50}";
		base.Name = "EnterItemZOrder";
		base.Size = new System.Drawing.Size(352, 200);
		base.Controls.SetChildIndex(this.labelZ, 0);
		base.Controls.SetChildIndex(this.labelLayer, 0);
		base.Controls.SetChildIndex(this.comboLayer, 0);
		base.Controls.SetChildIndex(this.numericZ, 0);
		((System.ComponentModel.ISupportInitialize)this.numericZ).EndInit();
		base.ResumeLayout(false);
	}

	private void comboLayer_SelectedIndexChanged(object sender, EventArgs e)
	{
		if (_item != null)
		{
			_item.Layer = GetLayerFromItem(comboLayer.SelectedItem);
			numericZ.Value = _item.ZOrder;
		}
	}

	private void numericZ_ValueChanged(object sender, EventArgs e)
	{
		if (_item != null)
		{
			_item.ZOrder = Convert.ToInt32(numericZ.Value);
			numericZ.Value = _item.ZOrder;
		}
	}

	private void numericZ_Leave(object sender, EventArgs e)
	{
		numericZ_ValueChanged(sender, e);
	}
}
