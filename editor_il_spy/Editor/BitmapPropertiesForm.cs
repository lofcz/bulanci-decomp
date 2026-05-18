using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms;
using Editor.Localize;
using Editor.ResourceItems;

namespace Editor;

public class BitmapPropertiesForm : Form
{
	private FormNavigation formNavigation;

	private TabControl tabControl;

	private TabPage tabBitmap;

	private EnterBitmap enterBitmap;

	private TabPage tabTransparency;

	private EnterZeroColor enterZeroColor;

	private TabPage tabName;

	private EnterResourceName enterResourceName;

	private Container components;

	private BitmapResourceItem _original;

	private BitmapResourceItem _editing;

	private Localizer _localizer;

	public FormNavigation Navigation => formNavigation;

	public BitmapPropertiesForm(BitmapResourceItem item)
	{
		InitializeComponent();
		base.AcceptButton = Navigation.OK;
		base.CancelButton = Navigation.Cancel;
		_original = item;
		_editing = new Level(item.Level).Resources[item.Name] as BitmapResourceItem;
		enterBitmap.BitmapResource = _editing;
		enterZeroColor.BitmapResource = _editing;
		enterResourceName.Resource = _editing;
		_localizer = new FormLocalizer(Tools.Localizer, this);
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
		this.tabControl = new System.Windows.Forms.TabControl();
		this.tabBitmap = new System.Windows.Forms.TabPage();
		this.enterBitmap = new Editor.EnterBitmap();
		this.tabTransparency = new System.Windows.Forms.TabPage();
		this.enterZeroColor = new Editor.EnterZeroColor();
		this.tabName = new System.Windows.Forms.TabPage();
		this.enterResourceName = new Editor.EnterResourceName();
		this.tabControl.SuspendLayout();
		this.tabBitmap.SuspendLayout();
		this.tabTransparency.SuspendLayout();
		this.tabName.SuspendLayout();
		base.SuspendLayout();
		this.formNavigation.Dock = System.Windows.Forms.DockStyle.Bottom;
		this.formNavigation.Location = new System.Drawing.Point(0, 215);
		this.formNavigation.Name = "formNavigation";
		this.formNavigation.Size = new System.Drawing.Size(346, 48);
		this.formNavigation.TabIndex = 0;
		this.tabControl.Controls.Add(this.tabName);
		this.tabControl.Controls.Add(this.tabBitmap);
		this.tabControl.Controls.Add(this.tabTransparency);
		this.tabControl.Dock = System.Windows.Forms.DockStyle.Fill;
		this.tabControl.ItemSize = new System.Drawing.Size(0, 18);
		this.tabControl.Location = new System.Drawing.Point(0, 0);
		this.tabControl.Name = "tabControl";
		this.tabControl.SelectedIndex = 0;
		this.tabControl.Size = new System.Drawing.Size(346, 215);
		this.tabControl.TabIndex = 1;
		this.tabBitmap.Controls.Add(this.enterBitmap);
		this.tabBitmap.Location = new System.Drawing.Point(4, 22);
		this.tabBitmap.Name = "tabBitmap";
		this.tabBitmap.Size = new System.Drawing.Size(338, 189);
		this.tabBitmap.TabIndex = 0;
		this.tabBitmap.Text = "{64}";
		this.enterBitmap.BitmapResource = null;
		this.enterBitmap.Description = "{57}";
		this.enterBitmap.Dock = System.Windows.Forms.DockStyle.Fill;
		this.enterBitmap.Location = new System.Drawing.Point(0, 0);
		this.enterBitmap.Name = "enterBitmap";
		this.enterBitmap.Size = new System.Drawing.Size(338, 189);
		this.enterBitmap.TabIndex = 0;
		this.tabTransparency.Controls.Add(this.enterZeroColor);
		this.tabTransparency.Location = new System.Drawing.Point(4, 22);
		this.tabTransparency.Name = "tabTransparency";
		this.tabTransparency.Size = new System.Drawing.Size(338, 189);
		this.tabTransparency.TabIndex = 1;
		this.tabTransparency.Text = "{65}";
		this.enterZeroColor.BitmapResource = null;
		this.enterZeroColor.Description = "{31}";
		this.enterZeroColor.Dock = System.Windows.Forms.DockStyle.Fill;
		this.enterZeroColor.Location = new System.Drawing.Point(0, 0);
		this.enterZeroColor.Name = "enterZeroColor";
		this.enterZeroColor.Size = new System.Drawing.Size(338, 189);
		this.enterZeroColor.TabIndex = 0;
		this.tabName.Controls.Add(this.enterResourceName);
		this.tabName.Location = new System.Drawing.Point(4, 22);
		this.tabName.Name = "tabName";
		this.tabName.Size = new System.Drawing.Size(338, 189);
		this.tabName.TabIndex = 2;
		this.tabName.Text = "{66}";
		this.enterResourceName.Description = "{36}";
		this.enterResourceName.Dock = System.Windows.Forms.DockStyle.Fill;
		this.enterResourceName.Location = new System.Drawing.Point(0, 0);
		this.enterResourceName.Name = "enterResourceName";
		this.enterResourceName.Size = new System.Drawing.Size(338, 189);
		this.enterResourceName.TabIndex = 0;
		this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
		base.ClientSize = new System.Drawing.Size(346, 263);
		base.Controls.Add(this.tabControl);
		base.Controls.Add(this.formNavigation);
		base.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
		base.MaximizeBox = false;
		base.MinimizeBox = false;
		base.Name = "BitmapPropertiesForm";
		base.ShowInTaskbar = false;
		base.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
		this.Text = "{63}";
		base.Closing += new System.ComponentModel.CancelEventHandler(BitmapPropertiesForm_Closing);
		this.tabControl.ResumeLayout(false);
		this.tabBitmap.ResumeLayout(false);
		this.tabTransparency.ResumeLayout(false);
		this.tabName.ResumeLayout(false);
		base.ResumeLayout(false);
	}

	private void BitmapPropertiesForm_Closing(object sender, CancelEventArgs e)
	{
		if (base.DialogResult == DialogResult.OK)
		{
			bool flag = false;
			int num = 0;
			SheetControl[] array = new SheetControl[3] { enterBitmap, enterZeroColor, enterResourceName };
			for (int i = 0; i < array.Length; i++)
			{
				if (!array[i].OnValidateSheet(this))
				{
					e.Cancel = true;
					if (!flag)
					{
						tabControl.SelectedIndex = num;
						flag = true;
					}
				}
				num++;
			}
			if (!e.Cancel)
			{
				_original.CopyFrom(_editing);
			}
		}
		if (!e.Cancel)
		{
			_localizer.Dispose();
			_localizer = null;
		}
	}
}
