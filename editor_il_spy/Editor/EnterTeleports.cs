using System;
using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms;

namespace Editor;

public class EnterTeleports : SheetControl
{
	private ListBox listBox;

	private Button buttonInsert;

	private Button buttonRemove;

	private Container components;

	private TeleportLevelItem _item;

	public TeleportLevelItem LevelItem
	{
		get
		{
			return _item;
		}
		set
		{
			listBox.Items.Clear();
			buttonRemove.Enabled = false;
			_item = value;
			if (_item == null)
			{
				return;
			}
			listBox.DisplayMember = "Name";
			foreach (string target in _item.Targets)
			{
				listBox.Items.Add(_item.Level.GetItemByGuid(target));
			}
		}
	}

	public EnterTeleports()
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
		this.listBox = new System.Windows.Forms.ListBox();
		this.buttonRemove = new System.Windows.Forms.Button();
		this.buttonInsert = new System.Windows.Forms.Button();
		base.SuspendLayout();
		this.listBox.Anchor = System.Windows.Forms.AnchorStyles.Top;
		this.listBox.Location = new System.Drawing.Point(40, 88);
		this.listBox.Name = "listBox";
		this.listBox.Size = new System.Drawing.Size(168, 69);
		this.listBox.TabIndex = 0;
		this.listBox.SelectedIndexChanged += new System.EventHandler(listBox_SelectedIndexChanged);
		this.buttonRemove.Anchor = System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right;
		this.buttonRemove.Location = new System.Drawing.Point(232, 120);
		this.buttonRemove.Name = "buttonRemove";
		this.buttonRemove.TabIndex = 2;
		this.buttonRemove.Text = "{156}";
		this.buttonRemove.Click += new System.EventHandler(buttonRemove_Click);
		this.buttonInsert.Anchor = System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right;
		this.buttonInsert.Location = new System.Drawing.Point(232, 88);
		this.buttonInsert.Name = "buttonInsert";
		this.buttonInsert.TabIndex = 1;
		this.buttonInsert.Text = "{155}";
		this.buttonInsert.Click += new System.EventHandler(buttonInsert_Click);
		base.Controls.Add(this.buttonInsert);
		base.Controls.Add(this.buttonRemove);
		base.Controls.Add(this.listBox);
		base.Description = "{154}";
		base.Name = "EnterTeleports";
		base.Controls.SetChildIndex(this.listBox, 0);
		base.Controls.SetChildIndex(this.buttonRemove, 0);
		base.Controls.SetChildIndex(this.buttonInsert, 0);
		base.ResumeLayout(false);
	}

	private void buttonInsert_Click(object sender, EventArgs e)
	{
		InsertTeleportForm insertTeleportForm = new InsertTeleportForm(LevelItem);
		if (insertTeleportForm.ShowDialog() == DialogResult.OK && LevelItem.AddTarget(insertTeleportForm.Teleport))
		{
			listBox.Items.Add(insertTeleportForm.Teleport);
		}
	}

	private void listBox_SelectedIndexChanged(object sender, EventArgs e)
	{
		buttonRemove.Enabled = listBox.SelectedItem != null;
	}

	private void buttonRemove_Click(object sender, EventArgs e)
	{
		if (LevelItem.RemoveTarget(listBox.SelectedItem as TeleportLevelItem))
		{
			listBox.Items.Remove(listBox.SelectedItem);
		}
	}
}
