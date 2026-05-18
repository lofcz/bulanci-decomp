using System;
using System.ComponentModel;
using System.Drawing;
using System.Reflection;
using System.Resources;
using System.Windows.Forms;
using Editor.Localize;
using Editor.Temp;

namespace Editor;

public class MainForm : Form
{
	private MainMenu mainMenu;

	private MenuItem menuItem1;

	private MenuItem menuItem2;

	private MenuItem menuItem3;

	private MenuItem menuItem4;

	private MenuItem menuItem5;

	private MenuItem menuItem6;

	private MenuItem menuItem7;

	private MenuItem menuItem8;

	private MenuItem menuItem9;

	private IContainer components;

	private MenuItem menuItem10;

	private MenuItem menuItem11;

	private ToolBar toolBar;

	private ImageList imageList;

	private ToolBarButton toolBarButtonNew;

	private ToolBarButton toolBarButtonOpen;

	private ToolBarButton toolBarButtonSep1;

	private ToolBarButton toolBarButtonSave;

	private ToolBarButton toolBarButtonSaveAs;

	private ToolBarButton toolBarButtonSep2;

	private ToolBarButton toolBarButtonProps;

	private ToolBarButton toolBarButtonCompile;

	private ContextMenu contextMenuProps;

	private MenuItem menuItem12;

	private MenuItem menuItem13;

	private MenuItem menuItem14;

	private ToolBarButton toolBarButtonSep3;

	private ToolBarButton toolBarButtonArrow;

	private ToolBarButton toolBarButtonObstacle;

	private ToolBarButton toolBarButtonImage;

	private Level _level;

	private ToolBarButton toolBarButtonSep4;

	private ToolBarButton toolBarButtonIns1;

	private ToolBarButton toolBarButtonIns2;

	private ToolBarButton toolBarButtonIns3;

	private MenuItem menuItem15;

	private MenuItem menuItem16;

	private MenuItem menuItem17;

	private MenuItem menuItem18;

	private MenuItem menuItem20;

	private MenuItem menuItem21;

	private MenuItem menuItem22;

	private MenuItem menuItem23;

	private MenuItem menuItem19;

	private MenuItem menuItem24;

	private MenuItem menuItem25;

	private MenuItem menuItem27;

	private MenuItem menuItem26;

	private ResourceEventHandler _resource_inserted;

	private ResourceEventHandler _resource_modified;

	private ResourceEventHandler _resource_removed;

	private ItemEventHandler _item_inserted;

	private ItemEventHandler _item_modified;

	private ItemEventHandler _item_removed;

	private MenuItem menuItem28;

	private MenuItem menuItem29;

	private Panel panel1;

	private TreeView treeView1;

	private Splitter splitter1;

	private Splitter splitter2;

	private LevelControl levelControl;

	private TreeView treeView2;

	private ToolBarButton toolBarButtonTeleport;

	private MenuItem menuItem30;

	private ImageList imageListTree;

	private MenuItem menuItem31;

	private ToolBarButton toolBarButtonMine;

	private MenuItem menuItem32;

	private ToolBarButton toolBarButtonOpponents;

	private Localizer _localizer;

	public LevelControl LevelControl => levelControl;

	public TreeView TreeView => treeView1;

	public Level Level
	{
		get
		{
			return _level;
		}
		set
		{
			if (_level != null)
			{
				_level.ItemInserted -= _item_inserted;
				_level.ItemModified -= _item_modified;
				_level.ItemRemoved -= _item_removed;
				_level.ResourceInserted -= _resource_inserted;
				_level.ResourceModified -= _resource_modified;
				_level.ResourceRemoved -= _resource_removed;
			}
			_level = value;
			if (_level != null)
			{
				_level.ItemInserted += _item_inserted;
				_level.ItemModified += _item_modified;
				_level.ItemRemoved += _item_removed;
				_level.ResourceInserted += _resource_inserted;
				_level.ResourceModified += _resource_modified;
				_level.ResourceRemoved += _resource_removed;
			}
			LevelControl.Level = _level;
			UpdateLevelCommands();
			UpdateLayerCommands();
			LevelLoaded();
		}
	}

	public MainForm()
	{
		InitializeComponent();
		UserSettings.Instance.LoadSettings();
		LevelControl.LayerInsertVisibleChanged += LayerVisibleChanged;
		LevelControl.LevelItemSelected += LevelControl_LevelItemSelected;
		_resource_inserted = ResourceInserted;
		_resource_modified = ResourceModified;
		_resource_removed = ResourceRemoved;
		_item_inserted = ItemInserted;
		_item_modified = ItemModified;
		_item_removed = ItemRemoved;
		UpdateLevelCommands();
		UpdateLayerCommands();
		Level = Level.OpenLast();
		_localizer = new FormLocalizer(Tools.Localizer, this);
		_localizer.AddObject(mainMenu);
		_localizer.AddObject(contextMenuProps);
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
		this.components = new System.ComponentModel.Container();
		System.Resources.ResourceManager resourceManager = new System.Resources.ResourceManager(typeof(Editor.MainForm));
		this.mainMenu = new System.Windows.Forms.MainMenu();
		this.menuItem1 = new System.Windows.Forms.MenuItem();
		this.menuItem2 = new System.Windows.Forms.MenuItem();
		this.menuItem3 = new System.Windows.Forms.MenuItem();
		this.menuItem4 = new System.Windows.Forms.MenuItem();
		this.menuItem5 = new System.Windows.Forms.MenuItem();
		this.menuItem11 = new System.Windows.Forms.MenuItem();
		this.menuItem10 = new System.Windows.Forms.MenuItem();
		this.menuItem6 = new System.Windows.Forms.MenuItem();
		this.menuItem7 = new System.Windows.Forms.MenuItem();
		this.menuItem8 = new System.Windows.Forms.MenuItem();
		this.menuItem9 = new System.Windows.Forms.MenuItem();
		this.menuItem15 = new System.Windows.Forms.MenuItem();
		this.menuItem18 = new System.Windows.Forms.MenuItem();
		this.menuItem17 = new System.Windows.Forms.MenuItem();
		this.menuItem16 = new System.Windows.Forms.MenuItem();
		this.menuItem23 = new System.Windows.Forms.MenuItem();
		this.menuItem24 = new System.Windows.Forms.MenuItem();
		this.menuItem25 = new System.Windows.Forms.MenuItem();
		this.menuItem26 = new System.Windows.Forms.MenuItem();
		this.menuItem30 = new System.Windows.Forms.MenuItem();
		this.menuItem32 = new System.Windows.Forms.MenuItem();
		this.menuItem19 = new System.Windows.Forms.MenuItem();
		this.menuItem22 = new System.Windows.Forms.MenuItem();
		this.menuItem21 = new System.Windows.Forms.MenuItem();
		this.menuItem20 = new System.Windows.Forms.MenuItem();
		this.menuItem28 = new System.Windows.Forms.MenuItem();
		this.menuItem29 = new System.Windows.Forms.MenuItem();
		this.toolBar = new System.Windows.Forms.ToolBar();
		this.toolBarButtonNew = new System.Windows.Forms.ToolBarButton();
		this.toolBarButtonOpen = new System.Windows.Forms.ToolBarButton();
		this.toolBarButtonSep1 = new System.Windows.Forms.ToolBarButton();
		this.toolBarButtonSave = new System.Windows.Forms.ToolBarButton();
		this.toolBarButtonSaveAs = new System.Windows.Forms.ToolBarButton();
		this.toolBarButtonSep2 = new System.Windows.Forms.ToolBarButton();
		this.toolBarButtonProps = new System.Windows.Forms.ToolBarButton();
		this.contextMenuProps = new System.Windows.Forms.ContextMenu();
		this.menuItem12 = new System.Windows.Forms.MenuItem();
		this.menuItem13 = new System.Windows.Forms.MenuItem();
		this.menuItem14 = new System.Windows.Forms.MenuItem();
		this.menuItem27 = new System.Windows.Forms.MenuItem();
		this.menuItem31 = new System.Windows.Forms.MenuItem();
		this.toolBarButtonOpponents = new System.Windows.Forms.ToolBarButton();
		this.toolBarButtonCompile = new System.Windows.Forms.ToolBarButton();
		this.toolBarButtonSep3 = new System.Windows.Forms.ToolBarButton();
		this.toolBarButtonArrow = new System.Windows.Forms.ToolBarButton();
		this.toolBarButtonObstacle = new System.Windows.Forms.ToolBarButton();
		this.toolBarButtonImage = new System.Windows.Forms.ToolBarButton();
		this.toolBarButtonTeleport = new System.Windows.Forms.ToolBarButton();
		this.toolBarButtonMine = new System.Windows.Forms.ToolBarButton();
		this.toolBarButtonSep4 = new System.Windows.Forms.ToolBarButton();
		this.toolBarButtonIns1 = new System.Windows.Forms.ToolBarButton();
		this.toolBarButtonIns2 = new System.Windows.Forms.ToolBarButton();
		this.toolBarButtonIns3 = new System.Windows.Forms.ToolBarButton();
		this.imageList = new System.Windows.Forms.ImageList(this.components);
		this.panel1 = new System.Windows.Forms.Panel();
		this.treeView2 = new System.Windows.Forms.TreeView();
		this.imageListTree = new System.Windows.Forms.ImageList(this.components);
		this.splitter1 = new System.Windows.Forms.Splitter();
		this.treeView1 = new System.Windows.Forms.TreeView();
		this.splitter2 = new System.Windows.Forms.Splitter();
		this.levelControl = new Editor.Temp.LevelControl();
		this.panel1.SuspendLayout();
		base.SuspendLayout();
		this.mainMenu.MenuItems.AddRange(new System.Windows.Forms.MenuItem[4] { this.menuItem1, this.menuItem15, this.menuItem23, this.menuItem28 });
		this.menuItem1.Index = 0;
		this.menuItem1.MenuItems.AddRange(new System.Windows.Forms.MenuItem[10] { this.menuItem2, this.menuItem3, this.menuItem4, this.menuItem5, this.menuItem11, this.menuItem10, this.menuItem6, this.menuItem7, this.menuItem8, this.menuItem9 });
		this.menuItem1.Text = "{113}";
		this.menuItem2.Index = 0;
		this.menuItem2.Text = "{116}";
		this.menuItem2.Click += new System.EventHandler(menuItem2_Click);
		this.menuItem3.Index = 1;
		this.menuItem3.Text = "{117}";
		this.menuItem3.Click += new System.EventHandler(menuItem3_Click);
		this.menuItem4.Index = 2;
		this.menuItem4.Text = "{118}";
		this.menuItem4.Click += new System.EventHandler(menuItem4_Click);
		this.menuItem5.Index = 3;
		this.menuItem5.Text = "{119}";
		this.menuItem5.Click += new System.EventHandler(menuItem5_Click);
		this.menuItem11.Index = 4;
		this.menuItem11.Text = "-";
		this.menuItem10.Index = 5;
		this.menuItem10.Text = "{120}";
		this.menuItem10.Click += new System.EventHandler(menuItem10_Click);
		this.menuItem6.Index = 6;
		this.menuItem6.Text = "-";
		this.menuItem7.Index = 7;
		this.menuItem7.Text = "{121}";
		this.menuItem7.Click += new System.EventHandler(menuItem7_Click);
		this.menuItem8.Index = 8;
		this.menuItem8.Text = "-";
		this.menuItem9.Index = 9;
		this.menuItem9.Text = "{122}";
		this.menuItem9.Click += new System.EventHandler(menuItem9_Click);
		this.menuItem15.Index = 1;
		this.menuItem15.MenuItems.AddRange(new System.Windows.Forms.MenuItem[3] { this.menuItem18, this.menuItem17, this.menuItem16 });
		this.menuItem15.Text = "{114}";
		this.menuItem18.Index = 0;
		this.menuItem18.Text = "{123}";
		this.menuItem18.Click += new System.EventHandler(menuItem18_Click);
		this.menuItem17.Index = 1;
		this.menuItem17.Text = "{124}";
		this.menuItem17.Click += new System.EventHandler(menuItem17_Click);
		this.menuItem16.Index = 2;
		this.menuItem16.Text = "{125}";
		this.menuItem16.Click += new System.EventHandler(menuItem16_Click);
		this.menuItem23.Index = 2;
		this.menuItem23.MenuItems.AddRange(new System.Windows.Forms.MenuItem[9] { this.menuItem24, this.menuItem25, this.menuItem26, this.menuItem30, this.menuItem32, this.menuItem19, this.menuItem22, this.menuItem21, this.menuItem20 });
		this.menuItem23.Text = "{115}";
		this.menuItem24.Index = 0;
		this.menuItem24.Text = "{126}";
		this.menuItem24.Click += new System.EventHandler(menuItem24_Click);
		this.menuItem25.Index = 1;
		this.menuItem25.Text = "{127}";
		this.menuItem25.Click += new System.EventHandler(menuItem25_Click);
		this.menuItem26.Index = 2;
		this.menuItem26.Text = "{128}";
		this.menuItem26.Click += new System.EventHandler(menuItem26_Click);
		this.menuItem30.Index = 3;
		this.menuItem30.Text = "{149}";
		this.menuItem30.Click += new System.EventHandler(menuItem30_Click);
		this.menuItem32.Index = 4;
		this.menuItem32.Text = "{187}";
		this.menuItem32.Click += new System.EventHandler(menuItem32_Click);
		this.menuItem19.Index = 5;
		this.menuItem19.Text = "-";
		this.menuItem22.Index = 6;
		this.menuItem22.Text = "{129}";
		this.menuItem22.Click += new System.EventHandler(menuItem22_Click);
		this.menuItem21.Index = 7;
		this.menuItem21.Text = "{130}";
		this.menuItem21.Click += new System.EventHandler(menuItem21_Click);
		this.menuItem20.Index = 8;
		this.menuItem20.Text = "{131}";
		this.menuItem20.Click += new System.EventHandler(menuItem20_Click);
		this.menuItem28.Index = 3;
		this.menuItem28.MenuItems.AddRange(new System.Windows.Forms.MenuItem[1] { this.menuItem29 });
		this.menuItem28.Text = "{112}";
		this.menuItem29.Index = 0;
		this.menuItem29.Text = "{111}";
		this.menuItem29.Click += new System.EventHandler(menuItem29_Click);
		this.toolBar.Buttons.AddRange(new System.Windows.Forms.ToolBarButton[19]
		{
			this.toolBarButtonNew, this.toolBarButtonOpen, this.toolBarButtonSep1, this.toolBarButtonSave, this.toolBarButtonSaveAs, this.toolBarButtonSep2, this.toolBarButtonProps, this.toolBarButtonOpponents, this.toolBarButtonCompile, this.toolBarButtonSep3,
			this.toolBarButtonArrow, this.toolBarButtonObstacle, this.toolBarButtonImage, this.toolBarButtonTeleport, this.toolBarButtonMine, this.toolBarButtonSep4, this.toolBarButtonIns1, this.toolBarButtonIns2, this.toolBarButtonIns3
		});
		this.toolBar.ButtonSize = new System.Drawing.Size(24, 24);
		this.toolBar.Divider = false;
		this.toolBar.DropDownArrows = true;
		this.toolBar.ImageList = this.imageList;
		this.toolBar.ImeMode = System.Windows.Forms.ImeMode.NoControl;
		this.toolBar.Location = new System.Drawing.Point(0, 0);
		this.toolBar.Name = "toolBar";
		this.toolBar.ShowToolTips = true;
		this.toolBar.Size = new System.Drawing.Size(648, 34);
		this.toolBar.TabIndex = 0;
		this.toolBar.ButtonClick += new System.Windows.Forms.ToolBarButtonClickEventHandler(toolBar_ButtonClick);
		this.toolBarButtonNew.ImageIndex = 0;
		this.toolBarButtonNew.Tag = "menuItem2";
		this.toolBarButtonNew.ToolTipText = "{136}";
		this.toolBarButtonOpen.ImageIndex = 1;
		this.toolBarButtonOpen.Tag = "menuItem3";
		this.toolBarButtonOpen.ToolTipText = "{137}";
		this.toolBarButtonSep1.Style = System.Windows.Forms.ToolBarButtonStyle.Separator;
		this.toolBarButtonSave.ImageIndex = 2;
		this.toolBarButtonSave.Tag = "menuItem4";
		this.toolBarButtonSave.ToolTipText = "{138}";
		this.toolBarButtonSaveAs.ImageIndex = 3;
		this.toolBarButtonSaveAs.Tag = "menuItem5";
		this.toolBarButtonSaveAs.ToolTipText = "{139}";
		this.toolBarButtonSep2.Style = System.Windows.Forms.ToolBarButtonStyle.Separator;
		this.toolBarButtonProps.DropDownMenu = this.contextMenuProps;
		this.toolBarButtonProps.ImageIndex = 4;
		this.toolBarButtonProps.Style = System.Windows.Forms.ToolBarButtonStyle.DropDownButton;
		this.toolBarButtonProps.Tag = "menuItem7";
		this.toolBarButtonProps.ToolTipText = "{140}";
		this.contextMenuProps.MenuItems.AddRange(new System.Windows.Forms.MenuItem[5] { this.menuItem12, this.menuItem13, this.menuItem14, this.menuItem27, this.menuItem31 });
		this.contextMenuProps.RightToLeft = System.Windows.Forms.RightToLeft.No;
		this.menuItem12.Index = 0;
		this.menuItem12.Text = "{132}";
		this.menuItem12.Click += new System.EventHandler(menuItem12_Click);
		this.menuItem13.Index = 1;
		this.menuItem13.Text = "{133}";
		this.menuItem13.Click += new System.EventHandler(menuItem13_Click);
		this.menuItem14.Index = 2;
		this.menuItem14.Text = "{134}";
		this.menuItem14.Click += new System.EventHandler(menuItem14_Click);
		this.menuItem27.Index = 3;
		this.menuItem27.Text = "{135}";
		this.menuItem27.Click += new System.EventHandler(menuItem27_Click);
		this.menuItem31.Index = 4;
		this.menuItem31.Text = "{163}";
		this.menuItem31.Click += new System.EventHandler(menuItem31_Click);
		this.toolBarButtonOpponents.ImageIndex = 14;
		this.toolBarButtonOpponents.Tag = "menuItem31";
		this.toolBarButtonOpponents.ToolTipText = "{190}";
		this.toolBarButtonCompile.ImageIndex = 5;
		this.toolBarButtonCompile.Tag = "menuItem10";
		this.toolBarButtonCompile.ToolTipText = "{141}";
		this.toolBarButtonSep3.Style = System.Windows.Forms.ToolBarButtonStyle.Separator;
		this.toolBarButtonArrow.ImageIndex = 6;
		this.toolBarButtonArrow.Tag = "menuItem24";
		this.toolBarButtonArrow.ToolTipText = "{142}";
		this.toolBarButtonObstacle.ImageIndex = 7;
		this.toolBarButtonObstacle.Tag = "menuItem25";
		this.toolBarButtonObstacle.ToolTipText = "{143}";
		this.toolBarButtonImage.ImageIndex = 8;
		this.toolBarButtonImage.Tag = "menuItem26";
		this.toolBarButtonImage.ToolTipText = "{144}";
		this.toolBarButtonTeleport.ImageIndex = 12;
		this.toolBarButtonTeleport.Tag = "menuItem30";
		this.toolBarButtonTeleport.ToolTipText = "{148}";
		this.toolBarButtonMine.ImageIndex = 13;
		this.toolBarButtonMine.Tag = "menuItem32";
		this.toolBarButtonMine.ToolTipText = "{188}";
		this.toolBarButtonSep4.Style = System.Windows.Forms.ToolBarButtonStyle.Separator;
		this.toolBarButtonIns1.ImageIndex = 9;
		this.toolBarButtonIns1.Tag = "menuItem22";
		this.toolBarButtonIns1.ToolTipText = "{145}";
		this.toolBarButtonIns2.ImageIndex = 10;
		this.toolBarButtonIns2.Tag = "menuItem21";
		this.toolBarButtonIns2.ToolTipText = "{146}";
		this.toolBarButtonIns3.ImageIndex = 11;
		this.toolBarButtonIns3.Tag = "menuItem20";
		this.toolBarButtonIns3.ToolTipText = "{147}";
		this.imageList.ColorDepth = System.Windows.Forms.ColorDepth.Depth24Bit;
		this.imageList.ImageSize = new System.Drawing.Size(24, 24);
		this.imageList.ImageStream = (System.Windows.Forms.ImageListStreamer)resourceManager.GetObject("imageList.ImageStream");
		this.imageList.TransparentColor = System.Drawing.Color.Lime;
		this.panel1.Controls.Add(this.treeView2);
		this.panel1.Controls.Add(this.splitter1);
		this.panel1.Controls.Add(this.treeView1);
		this.panel1.Dock = System.Windows.Forms.DockStyle.Left;
		this.panel1.Location = new System.Drawing.Point(0, 34);
		this.panel1.Name = "panel1";
		this.panel1.Size = new System.Drawing.Size(192, 423);
		this.panel1.TabIndex = 1;
		this.treeView2.Dock = System.Windows.Forms.DockStyle.Fill;
		this.treeView2.HideSelection = false;
		this.treeView2.ImageList = this.imageListTree;
		this.treeView2.Location = new System.Drawing.Point(0, 195);
		this.treeView2.Name = "treeView2";
		this.treeView2.Size = new System.Drawing.Size(192, 228);
		this.treeView2.TabIndex = 2;
		this.treeView2.KeyDown += new System.Windows.Forms.KeyEventHandler(treeView2_KeyDown);
		this.treeView2.MouseUp += new System.Windows.Forms.MouseEventHandler(treeView2_MouseUp);
		this.treeView2.DoubleClick += new System.EventHandler(treeView2_DoubleClick);
		this.imageListTree.ColorDepth = System.Windows.Forms.ColorDepth.Depth32Bit;
		this.imageListTree.ImageSize = new System.Drawing.Size(13, 13);
		this.imageListTree.ImageStream = (System.Windows.Forms.ImageListStreamer)resourceManager.GetObject("imageListTree.ImageStream");
		this.imageListTree.TransparentColor = System.Drawing.Color.Lime;
		this.splitter1.Dock = System.Windows.Forms.DockStyle.Top;
		this.splitter1.Location = new System.Drawing.Point(0, 192);
		this.splitter1.Name = "splitter1";
		this.splitter1.Size = new System.Drawing.Size(192, 3);
		this.splitter1.TabIndex = 1;
		this.splitter1.TabStop = false;
		this.treeView1.Dock = System.Windows.Forms.DockStyle.Top;
		this.treeView1.HideSelection = false;
		this.treeView1.ImageList = this.imageListTree;
		this.treeView1.Location = new System.Drawing.Point(0, 0);
		this.treeView1.Name = "treeView1";
		this.treeView1.SelectedImageIndex = 1;
		this.treeView1.Size = new System.Drawing.Size(192, 192);
		this.treeView1.TabIndex = 0;
		this.treeView1.KeyDown += new System.Windows.Forms.KeyEventHandler(treeView1_KeyDown);
		this.treeView1.MouseUp += new System.Windows.Forms.MouseEventHandler(treeView1_MouseUp);
		this.treeView1.DoubleClick += new System.EventHandler(treeView1_DoubleClick);
		this.treeView1.AfterSelect += new System.Windows.Forms.TreeViewEventHandler(treeView1_AfterSelect);
		this.splitter2.Location = new System.Drawing.Point(192, 34);
		this.splitter2.Name = "splitter2";
		this.splitter2.Size = new System.Drawing.Size(3, 423);
		this.splitter2.TabIndex = 2;
		this.splitter2.TabStop = false;
		this.levelControl.AutoScroll = true;
		this.levelControl.Cursor = System.Windows.Forms.Cursors.Default;
		this.levelControl.Dock = System.Windows.Forms.DockStyle.Fill;
		this.levelControl.DrawSelect = Editor.Temp.DrawSelect.Arrow;
		this.levelControl.LayerInsert = Editor.Temp.LayerType.Layer0;
		this.levelControl.Level = null;
		this.levelControl.Location = new System.Drawing.Point(195, 34);
		this.levelControl.Name = "levelControl";
		this.levelControl.SelectedLevelItem = null;
		this.levelControl.ShowBackground = true;
		this.levelControl.ShowForeground = true;
		this.levelControl.ShowMiddle = true;
		this.levelControl.Size = new System.Drawing.Size(453, 423);
		this.levelControl.TabIndex = 3;
		this.levelControl.ViewSize = new System.Drawing.Size(0, 0);
		this.levelControl.ViewSizeMode = Editor.Temp.SizeMode.Auto;
		this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
		base.ClientSize = new System.Drawing.Size(648, 457);
		base.Controls.Add(this.levelControl);
		base.Controls.Add(this.splitter2);
		base.Controls.Add(this.panel1);
		base.Controls.Add(this.toolBar);
		base.Icon = (System.Drawing.Icon)resourceManager.GetObject("$this.Icon");
		base.Menu = this.mainMenu;
		base.Name = "MainForm";
		base.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
		this.Text = "{19}";
		base.Closing += new System.ComponentModel.CancelEventHandler(MainForm_Closing);
		base.Load += new System.EventHandler(MainForm_Load);
		this.panel1.ResumeLayout(false);
		base.ResumeLayout(false);
	}

	[STAThread]
	private static void Main()
	{
		try
		{
			Application.Run(new MainForm());
		}
		catch (Exception ex)
		{
			MessageBox.Show(ex.ToString());
		}
	}

	private void menuItem9_Click(object sender, EventArgs e)
	{
		Close();
	}

	private void ShowProps(int sheet)
	{
		new LevelPropertiesForm(Level, sheet).ShowDialog();
	}

	private void menuItem7_Click(object sender, EventArgs e)
	{
		ShowProps(0);
	}

	private void menuItem2_Click(object sender, EventArgs e)
	{
		Level level = Level.New();
		if (level != null)
		{
			Level = level;
		}
	}

	private void menuItem3_Click(object sender, EventArgs e)
	{
		Level level = Level.Open();
		if (level != null)
		{
			Level = level;
		}
	}

	private void menuItem4_Click(object sender, EventArgs e)
	{
		Level.Save();
	}

	private void menuItem5_Click(object sender, EventArgs e)
	{
		Level.SaveAs();
	}

	private void menuItem10_Click(object sender, EventArgs e)
	{
		Level.Compile();
	}

	private void menuItem12_Click(object sender, EventArgs e)
	{
		ShowProps(0);
	}

	private void menuItem13_Click(object sender, EventArgs e)
	{
		ShowProps(1);
	}

	private void menuItem14_Click(object sender, EventArgs e)
	{
		ShowProps(2);
	}

	private void UpdateLevelCommands()
	{
		bool enabled = Level != null;
		MenuItem[] array = new MenuItem[4] { menuItem4, menuItem5, menuItem10, menuItem7 };
		for (int i = 0; i < array.Length; i++)
		{
			array[i].Enabled = enabled;
		}
		ToolBarButton[] array2 = new ToolBarButton[4] { toolBarButtonSave, toolBarButtonSaveAs, toolBarButtonProps, toolBarButtonCompile };
		for (int i = 0; i < array2.Length; i++)
		{
			array2[i].Enabled = enabled;
		}
	}

	private void toolBar_ButtonClick(object sender, ToolBarButtonClickEventArgs e)
	{
		string name = (e.Button.Tag as string) + "_Click";
		MethodInfo method = GetType().GetMethod(name, BindingFlags.Instance | BindingFlags.NonPublic);
		if (method != null)
		{
			method.Invoke(this, new object[2] { sender, e });
		}
	}

	private void MainForm_Closing(object sender, CancelEventArgs e)
	{
		if (Level != null && Level.Modified)
		{
			switch (Tools.Question(MessageBoxButtons.YesNoCancel, 108))
			{
			case DialogResult.Yes:
				Level.Save();
				break;
			case DialogResult.Cancel:
				e.Cancel = true;
				break;
			}
		}
		if (!e.Cancel)
		{
			Settings settings = UserSettings.Instance.Settings;
			settings.X = base.Bounds.X;
			settings.Y = base.Bounds.Y;
			settings.Width = base.Bounds.Width;
			settings.Height = base.Bounds.Height;
			settings.TreeWidth = TreeView.Bounds.Width;
			settings.TreeHeight = TreeView.Bounds.Height;
			UserSettings.Instance.SaveSettings();
			_localizer.Dispose();
			_localizer = null;
		}
	}

	public void UpdateLayerCommands()
	{
		bool flag = Level != null;
		bool flag2 = LevelControl.ShowForeground || LevelControl.ShowMiddle || LevelControl.ShowBackground;
		menuItem16.Enabled = flag;
		menuItem17.Enabled = flag;
		menuItem18.Enabled = flag;
		ToolBarButton toolBarButton = toolBarButtonIns1;
		bool enabled = (menuItem20.Enabled = flag && flag2);
		toolBarButton.Enabled = enabled;
		ToolBarButton toolBarButton2 = toolBarButtonIns2;
		enabled = (menuItem21.Enabled = flag && flag2);
		toolBarButton2.Enabled = enabled;
		ToolBarButton toolBarButton3 = toolBarButtonIns3;
		enabled = (menuItem22.Enabled = flag && flag2);
		toolBarButton3.Enabled = enabled;
		ToolBarButton toolBarButton4 = toolBarButtonArrow;
		enabled = (menuItem24.Enabled = flag && flag2);
		toolBarButton4.Enabled = enabled;
		ToolBarButton toolBarButton5 = toolBarButtonObstacle;
		enabled = (menuItem25.Enabled = flag && flag2);
		toolBarButton5.Enabled = enabled;
		ToolBarButton toolBarButton6 = toolBarButtonImage;
		enabled = (menuItem26.Enabled = flag && flag2);
		toolBarButton6.Enabled = enabled;
		ToolBarButton toolBarButton7 = toolBarButtonTeleport;
		enabled = (menuItem30.Enabled = flag && flag2);
		toolBarButton7.Enabled = enabled;
		ToolBarButton toolBarButton8 = toolBarButtonMine;
		enabled = (menuItem32.Enabled = flag && flag2);
		toolBarButton8.Enabled = enabled;
		menuItem16.Checked = LevelControl.ShowBackground;
		menuItem17.Checked = LevelControl.ShowMiddle;
		menuItem18.Checked = LevelControl.ShowForeground;
		ToolBarButton toolBarButton9 = toolBarButtonIns3;
		enabled = (menuItem20.Checked = LevelControl.LayerInsert == LayerType.Layer2);
		toolBarButton9.Pushed = enabled;
		ToolBarButton toolBarButton10 = toolBarButtonIns2;
		enabled = (menuItem21.Checked = LevelControl.LayerInsert == LayerType.Layer1);
		toolBarButton10.Pushed = enabled;
		ToolBarButton toolBarButton11 = toolBarButtonIns1;
		enabled = (menuItem22.Checked = LevelControl.LayerInsert == LayerType.Layer0);
		toolBarButton11.Pushed = enabled;
		ToolBarButton toolBarButton12 = toolBarButtonArrow;
		enabled = (menuItem24.Checked = LevelControl.DrawSelect == DrawSelect.Arrow);
		toolBarButton12.Pushed = enabled;
		ToolBarButton toolBarButton13 = toolBarButtonObstacle;
		enabled = (menuItem25.Checked = LevelControl.DrawSelect == DrawSelect.Obstacle);
		toolBarButton13.Pushed = enabled;
		ToolBarButton toolBarButton14 = toolBarButtonImage;
		enabled = (menuItem26.Checked = LevelControl.DrawSelect == DrawSelect.Image);
		toolBarButton14.Pushed = enabled;
		ToolBarButton toolBarButton15 = toolBarButtonTeleport;
		enabled = (menuItem30.Checked = LevelControl.DrawSelect == DrawSelect.Teleport);
		toolBarButton15.Pushed = enabled;
		ToolBarButton toolBarButton16 = toolBarButtonMine;
		enabled = (menuItem32.Checked = LevelControl.DrawSelect == DrawSelect.Mine);
		toolBarButton16.Pushed = enabled;
	}

	private void LayerVisibleChanged(object sender, EventArgs e)
	{
		UpdateLayerCommands();
	}

	private void menuItem16_Click(object sender, EventArgs e)
	{
		LevelControl.ShowBackground = !LevelControl.ShowBackground;
	}

	private void menuItem17_Click(object sender, EventArgs e)
	{
		LevelControl.ShowMiddle = !LevelControl.ShowMiddle;
	}

	private void menuItem18_Click(object sender, EventArgs e)
	{
		LevelControl.ShowForeground = !LevelControl.ShowForeground;
	}

	private void menuItem20_Click(object sender, EventArgs e)
	{
		LevelControl.LayerInsert = LayerType.Layer2;
	}

	private void menuItem21_Click(object sender, EventArgs e)
	{
		LevelControl.LayerInsert = LayerType.Layer1;
	}

	private void menuItem22_Click(object sender, EventArgs e)
	{
		LevelControl.LayerInsert = LayerType.Layer0;
	}

	private void menuItem24_Click(object sender, EventArgs e)
	{
		LevelControl.DrawSelect = DrawSelect.Arrow;
	}

	private void menuItem25_Click(object sender, EventArgs e)
	{
		LevelControl.DrawSelect = DrawSelect.Obstacle;
	}

	private void menuItem26_Click(object sender, EventArgs e)
	{
		LevelControl.DrawSelect = DrawSelect.Image;
	}

	private void menuItem27_Click(object sender, EventArgs e)
	{
		ShowProps(3);
	}

	private void LevelLoaded()
	{
		TreeView.BeginUpdate();
		TreeView.Nodes.Clear();
		treeView2.Nodes.Clear();
		if (Level != null)
		{
			TreeNode treeNode = new TreeNode(Tools.Localizer.GetString(70), 4, 4);
			treeNode.Tag = new LayerNode(LevelControl, LayerType.Layer0);
			TreeView.Nodes.Add(treeNode);
			LoadLayer(TreeView.Nodes[0], Level.ItemsLayer0);
			treeNode = new TreeNode(Tools.Localizer.GetString(71), 4, 4);
			treeNode.Tag = new LayerNode(LevelControl, LayerType.Layer1);
			TreeView.Nodes.Add(treeNode);
			LoadLayer(TreeView.Nodes[1], Level.ItemsLayer1);
			treeNode = new TreeNode(Tools.Localizer.GetString(72), 4, 4);
			treeNode.Tag = new LayerNode(LevelControl, LayerType.Layer2);
			TreeView.Nodes.Add(treeNode);
			LoadLayer(TreeView.Nodes[2], Level.ItemsLayer2);
			treeNode = new TreeNode(Tools.Localizer.GetString(89), 3, 3);
			treeNode.Tag = new ResourcesNode(Level);
			treeView2.Nodes.Add(treeNode);
			foreach (ResourceItem resource in Level.Resources)
			{
				ResourceInserted(this, new ResourceEventArgs(resource));
			}
			treeView2.Nodes[0].Expand();
		}
		TreeView.EndUpdate();
	}

	private void LoadLayer(TreeNode node, LevelItemCollection items)
	{
		foreach (LevelItem item in items)
		{
			ItemInserted(this, new ItemEventArgs(item));
		}
	}

	private void ResourceInserted(object sender, ResourceEventArgs args)
	{
		if (!args.ResourceItem.IsReserved)
		{
			TreeNode treeNode = new TreeNode(args.ResourceItem.Name, 5, 5);
			treeNode.Tag = args.ResourceItem;
			treeView2.Nodes[0].Nodes.Add(treeNode);
		}
	}

	private void ResourceModified(object sender, ResourceEventArgs args)
	{
		foreach (TreeNode node in treeView2.Nodes[0].Nodes)
		{
			if (node.Tag == args.ResourceItem)
			{
				node.Text = args.ResourceItem.Name;
			}
		}
	}

	private void ResourceRemoved(object sender, ResourceEventArgs args)
	{
		foreach (TreeNode node in treeView2.Nodes[0].Nodes)
		{
			if (node.Tag == args.ResourceItem)
			{
				treeView2.Nodes[0].Nodes.Remove(node);
				break;
			}
		}
	}

	private TreeNode LayerToNode(LayerType layer)
	{
		return layer switch
		{
			LayerType.Layer0 => TreeView.Nodes[0], 
			LayerType.Layer1 => TreeView.Nodes[1], 
			LayerType.Layer2 => TreeView.Nodes[2], 
			_ => null, 
		};
	}

	private void ItemInserted(object sender, ItemEventArgs args)
	{
		int num = 0;
		LevelItem levelItem = args.LevelItem;
		TreeNode treeNode = new TreeNode(levelItem.Name);
		treeNode.Tag = levelItem;
		switch (levelItem.LevelItemType)
		{
		case LevelItemType.Obstacle:
			num = 0;
			break;
		case LevelItemType.Image:
			num = 1;
			break;
		case LevelItemType.Teleport:
			num = 2;
			break;
		case LevelItemType.Mine:
			num = 6;
			break;
		}
		int imageIndex = (treeNode.SelectedImageIndex = num);
		treeNode.ImageIndex = imageIndex;
		LayerToNode(levelItem.Layer).Nodes.Insert(levelItem.ZOrder, treeNode);
	}

	private void ItemModified(object sender, ItemEventArgs args)
	{
		foreach (TreeNode node in LayerToNode(args.LevelItem.Layer).Nodes)
		{
			if (node.Tag == args.LevelItem)
			{
				node.Text = args.LevelItem.Name;
				break;
			}
		}
	}

	private void ItemRemoved(object sender, ItemEventArgs args)
	{
		foreach (TreeNode node in LayerToNode(args.LevelItem.Layer).Nodes)
		{
			if (node.Tag == args.LevelItem)
			{
				node.Parent.Nodes.Remove(node);
				break;
			}
		}
	}

	private void MainForm_Load(object sender, EventArgs e)
	{
		try
		{
			Settings settings = UserSettings.Instance.Settings;
			Rectangle bounds = new Rectangle(settings.X, settings.Y, settings.Width, settings.Height);
			base.Bounds = bounds;
			bounds = TreeView.Bounds;
			bounds.Height = settings.TreeHeight;
			bounds.Width = settings.TreeWidth;
			TreeView.Bounds = bounds;
		}
		catch
		{
		}
	}

	private void LevelControl_LevelItemSelected(object sender, ItemEventArgs args)
	{
		LevelItem levelItem = args.LevelItem;
		foreach (TreeNode node in LayerToNode(levelItem.Layer).Nodes)
		{
			if (node.Tag == levelItem)
			{
				TreeView.SelectedNode = node;
				break;
			}
		}
	}

	private void menuItem29_Click(object sender, EventArgs e)
	{
		new AboutForm().ShowDialog();
	}

	private void treeView1_DoubleClick(object sender, EventArgs e)
	{
		TreeNode selectedNode = TreeView.SelectedNode;
		if (selectedNode != null && selectedNode.Tag is IMouseHandler mouseHandler)
		{
			mouseHandler.OnDoubleClick(TreeView, new Point(selectedNode.Bounds.X, selectedNode.Bounds.Y + selectedNode.Bounds.Height));
		}
	}

	private void treeView1_MouseUp(object sender, MouseEventArgs e)
	{
		if (e.Button != System.Windows.Forms.MouseButtons.Right || e.Clicks != 1)
		{
			return;
		}
		TreeNode nodeAt = TreeView.GetNodeAt(e.X, e.Y);
		if (nodeAt != null)
		{
			TreeView.SelectedNode = nodeAt;
			if (nodeAt.Tag is IMouseHandler mouseHandler)
			{
				mouseHandler.OnRightClick(TreeView, new Point(nodeAt.Bounds.X, nodeAt.Bounds.Y + nodeAt.Bounds.Height));
			}
		}
	}

	private void treeView1_AfterSelect(object sender, TreeViewEventArgs e)
	{
		if (e.Node.Tag is LevelItem)
		{
			LevelControl.SelectedLevelItem = e.Node.Tag as LevelItem;
		}
	}

	private void treeView1_KeyDown(object sender, KeyEventArgs e)
	{
		TreeNode selectedNode = TreeView.SelectedNode;
		if (selectedNode != null && selectedNode.Tag is IKeybHandler keybHandler)
		{
			keybHandler.OnKeyDown(TreeView, e.KeyData);
		}
	}

	private void treeView2_DoubleClick(object sender, EventArgs e)
	{
		TreeNode selectedNode = treeView2.SelectedNode;
		if (selectedNode != null && selectedNode.Tag is IMouseHandler mouseHandler)
		{
			mouseHandler.OnDoubleClick(treeView2, new Point(selectedNode.Bounds.X, selectedNode.Bounds.Y + selectedNode.Bounds.Height));
		}
	}

	private void treeView2_MouseUp(object sender, MouseEventArgs e)
	{
		if (e.Button != System.Windows.Forms.MouseButtons.Right || e.Clicks != 1)
		{
			return;
		}
		TreeNode nodeAt = treeView2.GetNodeAt(e.X, e.Y);
		if (nodeAt != null)
		{
			treeView2.SelectedNode = nodeAt;
			if (nodeAt.Tag is IMouseHandler mouseHandler)
			{
				mouseHandler.OnRightClick(treeView2, new Point(nodeAt.Bounds.X, nodeAt.Bounds.Y + nodeAt.Bounds.Height));
			}
		}
	}

	private void treeView2_KeyDown(object sender, KeyEventArgs e)
	{
		TreeNode selectedNode = treeView2.SelectedNode;
		if (selectedNode != null && selectedNode.Tag is IKeybHandler keybHandler)
		{
			keybHandler.OnKeyDown(treeView2, e.KeyData);
		}
	}

	private void menuItem30_Click(object sender, EventArgs e)
	{
		LevelControl.DrawSelect = DrawSelect.Teleport;
	}

	private void menuItem31_Click(object sender, EventArgs e)
	{
		ShowProps(4);
	}

	private void menuItem32_Click(object sender, EventArgs e)
	{
		LevelControl.DrawSelect = DrawSelect.Mine;
	}
}
