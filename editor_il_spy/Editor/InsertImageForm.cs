using System;
using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms;
using Editor.Localize;
using Editor.ResourceItems;

namespace Editor;

public class InsertImageForm : Form
{
	private FormNavigation formNavigation;

	private Label label;

	private ComboBox comboBox1;

	private Button button1;

	private Container components;

	private Level _level;

	private Localizer _localizer;

	public Level Level => _level;

	public BitmapResourceItem BitmapResource => Level.Resources[Convert.ToString(comboBox1.SelectedItem)] as BitmapResourceItem;

	public InsertImageForm(Level level)
	{
		InitializeComponent();
		_level = level;
		foreach (ResourceItem resource in Level.Resources)
		{
			if (!resource.IsReserved && resource is BitmapResourceItem)
			{
				comboBox1.Items.Add(resource.Name);
				if (comboBox1.SelectedItem == null)
				{
					comboBox1.SelectedItem = resource.Name;
				}
			}
		}
		_localizer = new Localizer(Tools.Localizer, this);
		_localizer.Localized = true;
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
		this.formNavigation = new Editor.FormNavigation();
		this.label = new System.Windows.Forms.Label();
		this.comboBox1 = new System.Windows.Forms.ComboBox();
		this.button1 = new System.Windows.Forms.Button();
		base.SuspendLayout();
		this.formNavigation.Dock = System.Windows.Forms.DockStyle.Bottom;
		this.formNavigation.Location = new System.Drawing.Point(0, 69);
		this.formNavigation.Name = "formNavigation";
		this.formNavigation.Size = new System.Drawing.Size(426, 40);
		this.formNavigation.TabIndex = 3;
		this.label.ImeMode = System.Windows.Forms.ImeMode.NoControl;
		this.label.Location = new System.Drawing.Point(14, 24);
		this.label.Name = "label";
		this.label.Size = new System.Drawing.Size(128, 18);
		this.label.TabIndex = 0;
		this.label.Text = "{24}";
		this.label.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
		this.comboBox1.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
		this.comboBox1.ItemHeight = 13;
		this.comboBox1.Location = new System.Drawing.Point(144, 22);
		this.comboBox1.Name = "comboBox1";
		this.comboBox1.Size = new System.Drawing.Size(184, 21);
		this.comboBox1.TabIndex = 1;
		this.button1.ImeMode = System.Windows.Forms.ImeMode.NoControl;
		this.button1.Location = new System.Drawing.Point(336, 22);
		this.button1.Name = "button1";
		this.button1.Size = new System.Drawing.Size(75, 22);
		this.button1.TabIndex = 2;
		this.button1.Text = "{25}";
		this.button1.Click += new System.EventHandler(button1_Click);
		this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
		base.ClientSize = new System.Drawing.Size(426, 109);
		base.Controls.Add(this.button1);
		base.Controls.Add(this.comboBox1);
		base.Controls.Add(this.label);
		base.Controls.Add(this.formNavigation);
		base.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
		base.ImeMode = System.Windows.Forms.ImeMode.NoControl;
		base.MaximizeBox = false;
		base.MinimizeBox = false;
		base.Name = "InsertImageForm";
		base.ShowInTaskbar = false;
		base.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
		this.Text = "{26}";
		base.Closing += new System.ComponentModel.CancelEventHandler(InsertImageForm_Closing);
		base.ResumeLayout(false);
	}

	private void button1_Click(object sender, EventArgs e)
	{
		NewBitmapWizard newBitmapWizard = new NewBitmapWizard(Level);
		if (newBitmapWizard.ShowDialog() == DialogResult.OK)
		{
			comboBox1.Items.Add(newBitmapWizard.BitmapResource.Name);
			comboBox1.SelectedItem = newBitmapWizard.BitmapResource.Name;
		}
	}

	private void InsertImageForm_Closing(object sender, CancelEventArgs e)
	{
		_localizer.Dispose();
		_localizer = null;
	}
}
