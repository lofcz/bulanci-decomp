using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms;
using Editor.Localize;

namespace Editor;

public class MinePropertiesForm : Form
{
	private FormNavigation formNavigation;

	private TabControl tabControl;

	private TabPage tabPage1;

	private TabPage tabPage2;

	private TabPage tabPage3;

	private Container components;

	private MineLevelItem _original;

	private MineLevelItem _backup;

	private bool _modified;

	private EnterItemBounds enterItemBounds;

	private EnterItemName enterItemName;

	private EnterItemZOrder enterItemZOrder;

	private Localizer _localizer;

	public MinePropertiesForm(MineLevelItem item)
	{
		InitializeComponent();
		base.AcceptButton = formNavigation.OK;
		base.CancelButton = formNavigation.Cancel;
		_original = item;
		_modified = _original.Level.Modified;
		_backup = _original.Clone() as MineLevelItem;
		enterItemName.LevelItem = item;
		enterItemBounds.LevelItem = item;
		enterItemZOrder.LevelItem = item;
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
		this.tabPage1 = new System.Windows.Forms.TabPage();
		this.enterItemName = new Editor.EnterItemName();
		this.tabPage2 = new System.Windows.Forms.TabPage();
		this.enterItemBounds = new Editor.EnterItemBounds();
		this.tabPage3 = new System.Windows.Forms.TabPage();
		this.enterItemZOrder = new Editor.EnterItemZOrder();
		this.tabControl.SuspendLayout();
		this.tabPage1.SuspendLayout();
		this.tabPage2.SuspendLayout();
		this.tabPage3.SuspendLayout();
		base.SuspendLayout();
		this.formNavigation.Dock = System.Windows.Forms.DockStyle.Bottom;
		this.formNavigation.Location = new System.Drawing.Point(0, 215);
		this.formNavigation.Name = "formNavigation";
		this.formNavigation.Size = new System.Drawing.Size(347, 48);
		this.formNavigation.TabIndex = 1;
		this.tabControl.Controls.Add(this.tabPage1);
		this.tabControl.Controls.Add(this.tabPage2);
		this.tabControl.Controls.Add(this.tabPage3);
		this.tabControl.Dock = System.Windows.Forms.DockStyle.Fill;
		this.tabControl.Location = new System.Drawing.Point(0, 0);
		this.tabControl.Name = "tabControl";
		this.tabControl.SelectedIndex = 0;
		this.tabControl.Size = new System.Drawing.Size(347, 215);
		this.tabControl.TabIndex = 0;
		this.tabPage1.Controls.Add(this.enterItemName);
		this.tabPage1.Location = new System.Drawing.Point(4, 22);
		this.tabPage1.Name = "tabPage1";
		this.tabPage1.Size = new System.Drawing.Size(339, 189);
		this.tabPage1.TabIndex = 0;
		this.tabPage1.Text = "{20}";
		this.enterItemName.Description = "{150}";
		this.enterItemName.Dock = System.Windows.Forms.DockStyle.Fill;
		this.enterItemName.LevelItem = null;
		this.enterItemName.Location = new System.Drawing.Point(0, 0);
		this.enterItemName.Name = "enterItemName";
		this.enterItemName.Size = new System.Drawing.Size(339, 189);
		this.enterItemName.TabIndex = 0;
		this.tabPage2.Controls.Add(this.enterItemBounds);
		this.tabPage2.Location = new System.Drawing.Point(4, 22);
		this.tabPage2.Name = "tabPage2";
		this.tabPage2.Size = new System.Drawing.Size(339, 189);
		this.tabPage2.TabIndex = 1;
		this.tabPage2.Text = "{6}";
		this.enterItemBounds.Description = "{1}";
		this.enterItemBounds.Dock = System.Windows.Forms.DockStyle.Fill;
		this.enterItemBounds.LevelItem = null;
		this.enterItemBounds.Location = new System.Drawing.Point(0, 0);
		this.enterItemBounds.Name = "enterItemBounds";
		this.enterItemBounds.Size = new System.Drawing.Size(339, 189);
		this.enterItemBounds.SizeEnabled = false;
		this.enterItemBounds.TabIndex = 0;
		this.tabPage3.Controls.Add(this.enterItemZOrder);
		this.tabPage3.Location = new System.Drawing.Point(4, 22);
		this.tabPage3.Name = "tabPage3";
		this.tabPage3.Size = new System.Drawing.Size(339, 189);
		this.tabPage3.TabIndex = 2;
		this.tabPage3.Text = "{29}";
		this.enterItemZOrder.Description = "{50}";
		this.enterItemZOrder.Dock = System.Windows.Forms.DockStyle.Fill;
		this.enterItemZOrder.LayerEnabled = false;
		this.enterItemZOrder.LevelItem = null;
		this.enterItemZOrder.Location = new System.Drawing.Point(0, 0);
		this.enterItemZOrder.Name = "enterItemZOrder";
		this.enterItemZOrder.Size = new System.Drawing.Size(339, 189);
		this.enterItemZOrder.TabIndex = 0;
		this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
		base.ClientSize = new System.Drawing.Size(347, 263);
		base.Controls.Add(this.tabControl);
		base.Controls.Add(this.formNavigation);
		base.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
		base.MaximizeBox = false;
		base.MinimizeBox = false;
		base.Name = "MinePropertiesForm";
		base.ShowInTaskbar = false;
		base.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
		this.Text = "{189}";
		base.Closing += new System.ComponentModel.CancelEventHandler(MinePropertiesForm_Closing);
		this.tabControl.ResumeLayout(false);
		this.tabPage1.ResumeLayout(false);
		this.tabPage2.ResumeLayout(false);
		this.tabPage3.ResumeLayout(false);
		base.ResumeLayout(false);
	}

	private void MinePropertiesForm_Closing(object sender, CancelEventArgs e)
	{
		_localizer.Dispose();
		_localizer = null;
		enterItemName.LevelItem = null;
		enterItemBounds.LevelItem = null;
		enterItemZOrder.LevelItem = null;
		if (base.DialogResult != DialogResult.OK)
		{
			_original.CopyFrom(_backup);
			_original.Level.Modified = _modified;
		}
	}
}
