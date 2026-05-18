using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms;
using Editor.Localize;

namespace Editor;

public class ObstaclePropertiesForm : Form
{
	private FormNavigation formNavigation;

	private TabControl tabControl1;

	private TabPage tabBounds;

	private EnterItemBounds enterItemBounds;

	private Container components;

	private ObstacleLevelItem _original;

	private ObstacleLevelItem _backup;

	private TabPage tabZOrder;

	private int _zorder;

	private TabPage tabFireThrough;

	private bool _modified;

	private EnterFireThrough enterFireThrough;

	private TabPage tabName;

	private EnterItemZOrder enterItemZOrder;

	private EnterItemName enterItemName;

	private Localizer _localizer;

	public ObstaclePropertiesForm(ObstacleLevelItem item)
	{
		InitializeComponent();
		components = new Container();
		base.AcceptButton = formNavigation.OK;
		base.CancelButton = formNavigation.Cancel;
		_original = item;
		_modified = _original.Level.Modified;
		_backup = _original.Clone() as ObstacleLevelItem;
		_zorder = _original.ZOrder;
		enterItemName.LevelItem = _original;
		enterItemBounds.LevelItem = _original;
		enterItemZOrder.LevelItem = _original;
		enterFireThrough.ObstacleLevelItem = _original;
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
		this.tabControl1 = new System.Windows.Forms.TabControl();
		this.tabName = new System.Windows.Forms.TabPage();
		this.tabBounds = new System.Windows.Forms.TabPage();
		this.tabZOrder = new System.Windows.Forms.TabPage();
		this.tabFireThrough = new System.Windows.Forms.TabPage();
		this.enterItemName = new Editor.EnterItemName();
		this.enterItemBounds = new Editor.EnterItemBounds();
		this.enterItemZOrder = new Editor.EnterItemZOrder();
		this.enterFireThrough = new Editor.EnterFireThrough();
		this.formNavigation = new Editor.FormNavigation();
		this.tabControl1.SuspendLayout();
		this.tabName.SuspendLayout();
		this.tabBounds.SuspendLayout();
		this.tabZOrder.SuspendLayout();
		this.tabFireThrough.SuspendLayout();
		base.SuspendLayout();
		this.tabControl1.Controls.Add(this.tabName);
		this.tabControl1.Controls.Add(this.tabBounds);
		this.tabControl1.Controls.Add(this.tabZOrder);
		this.tabControl1.Controls.Add(this.tabFireThrough);
		this.tabControl1.Dock = System.Windows.Forms.DockStyle.Fill;
		this.tabControl1.ItemSize = new System.Drawing.Size(48, 18);
		this.tabControl1.Location = new System.Drawing.Point(0, 0);
		this.tabControl1.Name = "tabControl1";
		this.tabControl1.SelectedIndex = 0;
		this.tabControl1.Size = new System.Drawing.Size(346, 215);
		this.tabControl1.TabIndex = 0;
		this.tabName.Controls.Add(this.enterItemName);
		this.tabName.Location = new System.Drawing.Point(4, 22);
		this.tabName.Name = "tabName";
		this.tabName.Size = new System.Drawing.Size(338, 189);
		this.tabName.TabIndex = 3;
		this.tabName.Text = "{20}";
		this.tabBounds.Controls.Add(this.enterItemBounds);
		this.tabBounds.Location = new System.Drawing.Point(4, 22);
		this.tabBounds.Name = "tabBounds";
		this.tabBounds.Size = new System.Drawing.Size(338, 189);
		this.tabBounds.TabIndex = 0;
		this.tabBounds.Text = "{6}";
		this.tabZOrder.Controls.Add(this.enterItemZOrder);
		this.tabZOrder.Location = new System.Drawing.Point(4, 22);
		this.tabZOrder.Name = "tabZOrder";
		this.tabZOrder.Size = new System.Drawing.Size(338, 189);
		this.tabZOrder.TabIndex = 1;
		this.tabZOrder.Text = "{7}";
		this.tabFireThrough.Controls.Add(this.enterFireThrough);
		this.tabFireThrough.Location = new System.Drawing.Point(4, 22);
		this.tabFireThrough.Name = "tabFireThrough";
		this.tabFireThrough.Size = new System.Drawing.Size(338, 189);
		this.tabFireThrough.TabIndex = 2;
		this.tabFireThrough.Text = "{8}";
		this.enterItemName.Description = "{150}";
		this.enterItemName.Dock = System.Windows.Forms.DockStyle.Fill;
		this.enterItemName.LevelItem = null;
		this.enterItemName.Location = new System.Drawing.Point(0, 0);
		this.enterItemName.Name = "enterItemName";
		this.enterItemName.Size = new System.Drawing.Size(338, 189);
		this.enterItemName.TabIndex = 0;
		this.enterItemBounds.Description = "{1}";
		this.enterItemBounds.Dock = System.Windows.Forms.DockStyle.Fill;
		this.enterItemBounds.LevelItem = null;
		this.enterItemBounds.Location = new System.Drawing.Point(0, 0);
		this.enterItemBounds.Name = "enterItemBounds";
		this.enterItemBounds.Size = new System.Drawing.Size(338, 189);
		this.enterItemBounds.SizeEnabled = true;
		this.enterItemBounds.TabIndex = 0;
		this.enterItemZOrder.Description = "{50}";
		this.enterItemZOrder.Dock = System.Windows.Forms.DockStyle.Fill;
		this.enterItemZOrder.LayerEnabled = false;
		this.enterItemZOrder.LevelItem = null;
		this.enterItemZOrder.Location = new System.Drawing.Point(0, 0);
		this.enterItemZOrder.Name = "enterItemZOrder";
		this.enterItemZOrder.Size = new System.Drawing.Size(338, 189);
		this.enterItemZOrder.TabIndex = 0;
		this.enterFireThrough.Description = "{55}";
		this.enterFireThrough.Dock = System.Windows.Forms.DockStyle.Fill;
		this.enterFireThrough.Location = new System.Drawing.Point(0, 0);
		this.enterFireThrough.Name = "enterFireThrough";
		this.enterFireThrough.ObstacleLevelItem = null;
		this.enterFireThrough.Size = new System.Drawing.Size(338, 189);
		this.enterFireThrough.TabIndex = 0;
		this.formNavigation.Dock = System.Windows.Forms.DockStyle.Bottom;
		this.formNavigation.Location = new System.Drawing.Point(0, 215);
		this.formNavigation.Name = "formNavigation";
		this.formNavigation.Size = new System.Drawing.Size(346, 48);
		this.formNavigation.TabIndex = 1;
		this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
		base.ClientSize = new System.Drawing.Size(346, 263);
		base.Controls.Add(this.tabControl1);
		base.Controls.Add(this.formNavigation);
		base.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
		base.MaximizeBox = false;
		base.MinimizeBox = false;
		base.Name = "ObstaclePropertiesForm";
		base.ShowInTaskbar = false;
		base.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
		this.Text = "{9}";
		base.Closing += new System.ComponentModel.CancelEventHandler(ObstaclePropertiesForm_Closing);
		this.tabControl1.ResumeLayout(false);
		this.tabName.ResumeLayout(false);
		this.tabBounds.ResumeLayout(false);
		this.tabZOrder.ResumeLayout(false);
		this.tabFireThrough.ResumeLayout(false);
		base.ResumeLayout(false);
	}

	private void ObstaclePropertiesForm_Closing(object sender, CancelEventArgs e)
	{
		_localizer.Dispose();
		_localizer = null;
		enterItemName.LevelItem = null;
		enterItemBounds.LevelItem = null;
		enterItemZOrder.LevelItem = null;
		enterFireThrough.ObstacleLevelItem = null;
		if (base.DialogResult != DialogResult.OK)
		{
			_original.CopyFrom(_backup);
			_original.ZOrder = _zorder;
			_original.Level.Modified = _modified;
		}
	}
}
