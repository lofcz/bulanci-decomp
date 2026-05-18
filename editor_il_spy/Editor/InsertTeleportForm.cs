using System;
using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms;
using Editor.Localize;

namespace Editor;

public class InsertTeleportForm : Form
{
	private Label label;

	private FormNavigation formNavigation;

	private ComboBox comboBox;

	private Container components;

	private Localizer _localizer;

	public TeleportLevelItem Teleport => comboBox.SelectedItem as TeleportLevelItem;

	public InsertTeleportForm(TeleportLevelItem teleport)
	{
		InitializeComponent();
		base.AcceptButton = formNavigation.OK;
		base.CancelButton = formNavigation.Cancel;
		formNavigation.OK.Enabled = false;
		_localizer = new Localizer(Tools.Localizer, this);
		_localizer.Localized = true;
		comboBox.DisplayMember = "Name";
		foreach (LevelItem item in teleport.Level.Items)
		{
			if (item is TeleportLevelItem teleportLevelItem && !(teleportLevelItem.Guid == teleport.Guid) && !teleport.ContainsTarget(teleportLevelItem.Guid))
			{
				comboBox.Items.Add(teleportLevelItem);
			}
		}
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
		this.label = new System.Windows.Forms.Label();
		this.formNavigation = new Editor.FormNavigation();
		this.comboBox = new System.Windows.Forms.ComboBox();
		base.SuspendLayout();
		this.label.Location = new System.Drawing.Point(16, 16);
		this.label.Name = "label";
		this.label.Size = new System.Drawing.Size(100, 21);
		this.label.TabIndex = 0;
		this.label.Text = "{158}";
		this.label.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
		this.formNavigation.Dock = System.Windows.Forms.DockStyle.Bottom;
		this.formNavigation.Location = new System.Drawing.Point(0, 53);
		this.formNavigation.Name = "formNavigation";
		this.formNavigation.Size = new System.Drawing.Size(266, 48);
		this.formNavigation.TabIndex = 2;
		this.comboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
		this.comboBox.Location = new System.Drawing.Point(128, 16);
		this.comboBox.Name = "comboBox";
		this.comboBox.Size = new System.Drawing.Size(121, 21);
		this.comboBox.TabIndex = 1;
		this.comboBox.SelectedIndexChanged += new System.EventHandler(comboBox_SelectedIndexChanged);
		this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
		base.ClientSize = new System.Drawing.Size(266, 101);
		base.Controls.Add(this.comboBox);
		base.Controls.Add(this.formNavigation);
		base.Controls.Add(this.label);
		base.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
		base.MaximizeBox = false;
		base.MinimizeBox = false;
		base.Name = "InsertTeleportForm";
		base.ShowInTaskbar = false;
		base.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
		this.Text = "{157}";
		base.Closing += new System.ComponentModel.CancelEventHandler(InsertTeleportForm_Closing);
		base.ResumeLayout(false);
	}

	private void InsertTeleportForm_Closing(object sender, CancelEventArgs e)
	{
		_localizer.Dispose();
		_localizer = null;
	}

	private void comboBox_SelectedIndexChanged(object sender, EventArgs e)
	{
		formNavigation.OK.Enabled = true;
	}
}
