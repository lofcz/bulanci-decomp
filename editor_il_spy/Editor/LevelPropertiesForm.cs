using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms;
using Editor.Localize;

namespace Editor;

public class LevelPropertiesForm : Form
{
	private FormNavigation formNavigation;

	private TabControl tabControl;

	private TabPage tabName;

	private TabPage tabLoading;

	private TabPage tabBack;

	private Container components;

	private Level _levelOriginal;

	private EnterLevelName enterLevelName;

	private EnterLoadBitmap enterLoadBitmap;

	private EnterBackgroundBitmap enterBackground;

	private TabPage tabSound;

	private EnterLevelSound enterSound;

	private Level _levelTest;

	private TabPage tabOpponents;

	private EnterOpponents enterOpponents;

	private Localizer _localizer;

	private FormNavigation Navigation => formNavigation;

	public LevelPropertiesForm(Level level, int sheet)
	{
		InitializeComponent();
		base.AcceptButton = Navigation.OK;
		base.CancelButton = Navigation.Cancel;
		CopyFrom(_levelTest = new Level(), _levelOriginal = level);
		enterLevelName.Level = _levelTest;
		enterLoadBitmap.Level = _levelTest;
		enterBackground.Level = _levelTest;
		enterSound.Level = _levelTest;
		enterOpponents.Level = _levelTest;
		_localizer = new FormLocalizer(Tools.Localizer, this);
		_localizer.Localized = true;
		tabControl.SelectedIndex = sheet;
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
		this.enterLevelName = new Editor.EnterLevelName();
		this.tabLoading = new System.Windows.Forms.TabPage();
		this.enterLoadBitmap = new Editor.EnterLoadBitmap();
		this.tabBack = new System.Windows.Forms.TabPage();
		this.enterBackground = new Editor.EnterBackgroundBitmap();
		this.tabSound = new System.Windows.Forms.TabPage();
		this.enterSound = new Editor.EnterLevelSound();
		this.tabOpponents = new System.Windows.Forms.TabPage();
		this.enterOpponents = new Editor.EnterOpponents();
		this.tabControl.SuspendLayout();
		this.tabName.SuspendLayout();
		this.tabLoading.SuspendLayout();
		this.tabBack.SuspendLayout();
		this.tabSound.SuspendLayout();
		this.tabOpponents.SuspendLayout();
		base.SuspendLayout();
		this.formNavigation.Dock = System.Windows.Forms.DockStyle.Bottom;
		this.formNavigation.Location = new System.Drawing.Point(0, 215);
		this.formNavigation.Name = "formNavigation";
		this.formNavigation.Size = new System.Drawing.Size(346, 48);
		this.formNavigation.TabIndex = 3;
		this.tabControl.Controls.Add(this.tabName);
		this.tabControl.Controls.Add(this.tabLoading);
		this.tabControl.Controls.Add(this.tabBack);
		this.tabControl.Controls.Add(this.tabSound);
		this.tabControl.Controls.Add(this.tabOpponents);
		this.tabControl.Dock = System.Windows.Forms.DockStyle.Fill;
		this.tabControl.ItemSize = new System.Drawing.Size(42, 18);
		this.tabControl.Location = new System.Drawing.Point(0, 0);
		this.tabControl.Name = "tabControl";
		this.tabControl.SelectedIndex = 0;
		this.tabControl.Size = new System.Drawing.Size(346, 215);
		this.tabControl.TabIndex = 5;
		this.tabName.Controls.Add(this.enterLevelName);
		this.tabName.Location = new System.Drawing.Point(4, 22);
		this.tabName.Name = "tabName";
		this.tabName.Size = new System.Drawing.Size(338, 189);
		this.tabName.TabIndex = 0;
		this.tabName.Text = "{20}";
		this.enterLevelName.Description = "{47}";
		this.enterLevelName.Dock = System.Windows.Forms.DockStyle.Fill;
		this.enterLevelName.Location = new System.Drawing.Point(0, 0);
		this.enterLevelName.Name = "enterLevelName";
		this.enterLevelName.Size = new System.Drawing.Size(338, 189);
		this.enterLevelName.TabIndex = 0;
		this.tabLoading.Controls.Add(this.enterLoadBitmap);
		this.tabLoading.Location = new System.Drawing.Point(4, 22);
		this.tabLoading.Name = "tabLoading";
		this.tabLoading.Size = new System.Drawing.Size(338, 189);
		this.tabLoading.TabIndex = 1;
		this.tabLoading.Text = "{21}";
		this.enterLoadBitmap.Description = "{39}";
		this.enterLoadBitmap.Dock = System.Windows.Forms.DockStyle.Fill;
		this.enterLoadBitmap.Location = new System.Drawing.Point(0, 0);
		this.enterLoadBitmap.Name = "enterLoadBitmap";
		this.enterLoadBitmap.Size = new System.Drawing.Size(338, 189);
		this.enterLoadBitmap.TabIndex = 0;
		this.tabBack.Controls.Add(this.enterBackground);
		this.tabBack.Location = new System.Drawing.Point(4, 22);
		this.tabBack.Name = "tabBack";
		this.tabBack.Size = new System.Drawing.Size(338, 189);
		this.tabBack.TabIndex = 2;
		this.tabBack.Text = "{22}";
		this.enterBackground.Description = "{60}";
		this.enterBackground.Dock = System.Windows.Forms.DockStyle.Fill;
		this.enterBackground.Location = new System.Drawing.Point(0, 0);
		this.enterBackground.Name = "enterBackground";
		this.enterBackground.Size = new System.Drawing.Size(338, 189);
		this.enterBackground.TabIndex = 0;
		this.tabSound.Controls.Add(this.enterSound);
		this.tabSound.Location = new System.Drawing.Point(4, 22);
		this.tabSound.Name = "tabSound";
		this.tabSound.Size = new System.Drawing.Size(338, 189);
		this.tabSound.TabIndex = 3;
		this.tabSound.Text = "{23}";
		this.enterSound.Description = "{43}";
		this.enterSound.Dock = System.Windows.Forms.DockStyle.Fill;
		this.enterSound.Location = new System.Drawing.Point(0, 0);
		this.enterSound.Name = "enterSound";
		this.enterSound.Size = new System.Drawing.Size(338, 189);
		this.enterSound.TabIndex = 0;
		this.tabOpponents.Controls.Add(this.enterOpponents);
		this.tabOpponents.Location = new System.Drawing.Point(4, 22);
		this.tabOpponents.Name = "tabOpponents";
		this.tabOpponents.Size = new System.Drawing.Size(338, 189);
		this.tabOpponents.TabIndex = 4;
		this.tabOpponents.Text = "{162}";
		this.enterOpponents.Description = "{159}";
		this.enterOpponents.Dock = System.Windows.Forms.DockStyle.Fill;
		this.enterOpponents.Location = new System.Drawing.Point(0, 0);
		this.enterOpponents.Name = "enterOpponents";
		this.enterOpponents.Size = new System.Drawing.Size(338, 189);
		this.enterOpponents.TabIndex = 0;
		this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
		base.ClientSize = new System.Drawing.Size(346, 263);
		base.Controls.Add(this.tabControl);
		base.Controls.Add(this.formNavigation);
		base.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
		base.MaximizeBox = false;
		base.MinimizeBox = false;
		base.Name = "LevelPropertiesForm";
		base.ShowInTaskbar = false;
		base.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
		this.Text = "{27}";
		base.Closing += new System.ComponentModel.CancelEventHandler(LevelPropertiesForm_Closing);
		this.tabControl.ResumeLayout(false);
		this.tabName.ResumeLayout(false);
		this.tabLoading.ResumeLayout(false);
		this.tabBack.ResumeLayout(false);
		this.tabSound.ResumeLayout(false);
		this.tabOpponents.ResumeLayout(false);
		base.ResumeLayout(false);
	}

	private void CopyFrom(Level to, Level from)
	{
		to.Sign.CopyFrom(from.Sign);
		to.Background.CopyFrom(from.Background);
		to.Sound.CopyFrom(from.Sound);
		to.LoadBitmap.CopyFrom(from.LoadBitmap);
		to.Vampires = from.Vampires;
		to.Opponents.CopyFrom(from.Opponents);
	}

	private void LevelPropertiesForm_Closing(object sender, CancelEventArgs e)
	{
		if (base.DialogResult == DialogResult.OK)
		{
			bool flag = false;
			int num = 0;
			SheetControl[] array = new SheetControl[4] { enterLevelName, enterLoadBitmap, enterBackground, enterSound };
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
				CopyFrom(_levelOriginal, _levelTest);
			}
		}
		if (!e.Cancel)
		{
			_localizer.Dispose();
			_localizer = null;
		}
	}
}
