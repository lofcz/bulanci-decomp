using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms;
using Editor.Localize;

namespace Editor;

public class ImagePropertiesForm : Form
{
	private FormNavigation formNavigation;

	private TabControl tabControl;

	private TabPage tabBounds;

	private EnterItemBounds enterItemBounds;

	private TabPage tabZOrder;

	private EnterItemZOrder enterItemZOrder;

	private Container components;

	private ImageLevelItem _original;

	private ImageLevelItem _backup;

	private int _zorder;

	private TabPage tabGround;

	private EnterImageGround enterImageGround;

	private bool _modified;

	private TabPage tabName;

	private EnterItemName enterItemName;

	private Localizer _localizer;

	public ImagePropertiesForm(ImageLevelItem item)
	{
		InitializeComponent();
		base.AcceptButton = formNavigation.OK;
		base.CancelButton = formNavigation.Cancel;
		_original = item;
		_modified = _original.Level.Modified;
		_backup = _original.Clone() as ImageLevelItem;
		_zorder = _original.ZOrder;
		enterItemName.LevelItem = item;
		enterItemBounds.LevelItem = item;
		enterItemZOrder.LevelItem = item;
		enterImageGround.ImageLevelItem = item;
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
		this.tabName = new System.Windows.Forms.TabPage();
		this.tabBounds = new System.Windows.Forms.TabPage();
		this.enterItemBounds = new Editor.EnterItemBounds();
		this.tabZOrder = new System.Windows.Forms.TabPage();
		this.enterItemZOrder = new Editor.EnterItemZOrder();
		this.tabGround = new System.Windows.Forms.TabPage();
		this.enterImageGround = new Editor.EnterImageGround();
		this.enterItemName = new Editor.EnterItemName();
		this.tabControl.SuspendLayout();
		this.tabName.SuspendLayout();
		this.tabBounds.SuspendLayout();
		this.tabZOrder.SuspendLayout();
		this.tabGround.SuspendLayout();
		base.SuspendLayout();
		this.formNavigation.Dock = System.Windows.Forms.DockStyle.Bottom;
		this.formNavigation.Location = new System.Drawing.Point(0, 215);
		this.formNavigation.Name = "formNavigation";
		this.formNavigation.Size = new System.Drawing.Size(346, 48);
		this.formNavigation.TabIndex = 1;
		this.tabControl.Controls.Add(this.tabName);
		this.tabControl.Controls.Add(this.tabBounds);
		this.tabControl.Controls.Add(this.tabZOrder);
		this.tabControl.Controls.Add(this.tabGround);
		this.tabControl.Dock = System.Windows.Forms.DockStyle.Fill;
		this.tabControl.ItemSize = new System.Drawing.Size(48, 18);
		this.tabControl.Location = new System.Drawing.Point(0, 0);
		this.tabControl.Name = "tabControl";
		this.tabControl.SelectedIndex = 0;
		this.tabControl.Size = new System.Drawing.Size(346, 215);
		this.tabControl.TabIndex = 0;
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
		this.enterItemBounds.Description = "{1}";
		this.enterItemBounds.Dock = System.Windows.Forms.DockStyle.Fill;
		this.enterItemBounds.LevelItem = null;
		this.enterItemBounds.Location = new System.Drawing.Point(0, 0);
		this.enterItemBounds.Name = "enterItemBounds";
		this.enterItemBounds.Size = new System.Drawing.Size(338, 189);
		this.enterItemBounds.SizeEnabled = false;
		this.enterItemBounds.TabIndex = 0;
		this.tabZOrder.Controls.Add(this.enterItemZOrder);
		this.tabZOrder.Location = new System.Drawing.Point(4, 22);
		this.tabZOrder.Name = "tabZOrder";
		this.tabZOrder.Size = new System.Drawing.Size(338, 189);
		this.tabZOrder.TabIndex = 1;
		this.tabZOrder.Text = "{29}";
		this.enterItemZOrder.Description = "{50}";
		this.enterItemZOrder.Dock = System.Windows.Forms.DockStyle.Fill;
		this.enterItemZOrder.LayerEnabled = true;
		this.enterItemZOrder.LevelItem = null;
		this.enterItemZOrder.Location = new System.Drawing.Point(0, 0);
		this.enterItemZOrder.Name = "enterItemZOrder";
		this.enterItemZOrder.Size = new System.Drawing.Size(338, 189);
		this.enterItemZOrder.TabIndex = 0;
		this.tabGround.Controls.Add(this.enterImageGround);
		this.tabGround.Location = new System.Drawing.Point(4, 22);
		this.tabGround.Name = "tabGround";
		this.tabGround.Size = new System.Drawing.Size(338, 189);
		this.tabGround.TabIndex = 2;
		this.tabGround.Text = "{30}";
		this.enterImageGround.Description = "{53}";
		this.enterImageGround.Dock = System.Windows.Forms.DockStyle.Fill;
		this.enterImageGround.GroundEnabled = true;
		this.enterImageGround.ImageLevelItem = null;
		this.enterImageGround.Location = new System.Drawing.Point(0, 0);
		this.enterImageGround.Name = "enterImageGround";
		this.enterImageGround.Size = new System.Drawing.Size(338, 189);
		this.enterImageGround.TabIndex = 0;
		this.enterItemName.Description = "{150}";
		this.enterItemName.Dock = System.Windows.Forms.DockStyle.Fill;
		this.enterItemName.LevelItem = null;
		this.enterItemName.Location = new System.Drawing.Point(0, 0);
		this.enterItemName.Name = "enterItemName";
		this.enterItemName.Size = new System.Drawing.Size(338, 189);
		this.enterItemName.TabIndex = 0;
		this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
		base.ClientSize = new System.Drawing.Size(346, 263);
		base.Controls.Add(this.tabControl);
		base.Controls.Add(this.formNavigation);
		base.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
		base.MaximizeBox = false;
		base.MinimizeBox = false;
		base.Name = "ImagePropertiesForm";
		base.ShowInTaskbar = false;
		base.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
		this.Text = "{28}";
		base.Closing += new System.ComponentModel.CancelEventHandler(ImagePropertiesForm_Closing);
		this.tabControl.ResumeLayout(false);
		this.tabName.ResumeLayout(false);
		this.tabBounds.ResumeLayout(false);
		this.tabZOrder.ResumeLayout(false);
		this.tabGround.ResumeLayout(false);
		base.ResumeLayout(false);
	}

	private void ImagePropertiesForm_Closing(object sender, CancelEventArgs e)
	{
		_localizer.Dispose();
		_localizer = null;
		enterItemName.LevelItem = null;
		enterItemBounds.LevelItem = null;
		enterItemZOrder.LevelItem = null;
		enterImageGround.ImageLevelItem = null;
		if (base.DialogResult != DialogResult.OK)
		{
			_original.CopyFrom(_backup);
			_original.ZOrder = _zorder;
			_original.Level.Modified = _modified;
		}
	}
}
