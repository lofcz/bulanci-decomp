using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms;
using Editor.Localize;

namespace Editor;

public class OpponentPropertiesForm : Form
{
	private FormNavigation formNavigation;

	private TabControl tabControl;

	private Container components;

	private TabPage[] tabPage;

	private EnterOppProps[] props;

	private Localizer _localizer;

	private Level _original;

	private Level _temp;

	public OpponentPropertiesForm(Level level)
	{
		InitializeComponent();
		base.AcceptButton = formNavigation.OK;
		base.CancelButton = formNavigation.Cancel;
		CopyFrom(_temp = new Level(), _original = level);
		tabPage = new TabPage[_temp.Opponents.Count];
		props = new EnterOppProps[_temp.Opponents.Count];
		SuspendLayout();
		tabControl.SuspendLayout();
		for (int i = 0; i < _temp.Opponents.Count; i++)
		{
			tabPage[i] = new TabPage();
			tabPage[i].SuspendLayout();
			tabControl.Controls.Add(tabPage[i]);
			props[i] = new EnterOppProps(_temp.Opponents[i]);
			tabPage[i].Controls.Add(props[i]);
			tabPage[i].Location = new Point(4, 22);
			tabPage[i].Name = "tabPage" + i;
			tabPage[i].Size = new Size(298, 149);
			tabPage[i].TabIndex = 0;
			tabPage[i].Text = "{" + (i + 183) + "}";
			props[i].Dock = DockStyle.Fill;
			props[i].Name = "enterOppProps" + i;
			props[i].TabIndex = 0;
			tabPage[i].ResumeLayout(performLayout: false);
		}
		tabControl.ResumeLayout(performLayout: false);
		ResumeLayout(performLayout: false);
		_localizer = new FormLocalizer(Tools.Localizer, this);
		_localizer.Localized = true;
	}

	private void CopyFrom(Level to, Level from)
	{
		to.Opponents.CopyFrom(from.Opponents);
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
		base.SuspendLayout();
		this.formNavigation.Dock = System.Windows.Forms.DockStyle.Bottom;
		this.formNavigation.Location = new System.Drawing.Point(0, 175);
		this.formNavigation.Name = "formNavigation";
		this.formNavigation.Size = new System.Drawing.Size(306, 48);
		this.formNavigation.TabIndex = 1;
		this.tabControl.Dock = System.Windows.Forms.DockStyle.Fill;
		this.tabControl.Location = new System.Drawing.Point(0, 0);
		this.tabControl.Name = "tabControl";
		this.tabControl.SelectedIndex = 0;
		this.tabControl.Size = new System.Drawing.Size(306, 175);
		this.tabControl.TabIndex = 0;
		this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
		base.ClientSize = new System.Drawing.Size(306, 223);
		base.Controls.Add(this.tabControl);
		base.Controls.Add(this.formNavigation);
		base.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
		base.MaximizeBox = false;
		base.MinimizeBox = false;
		base.Name = "OpponentPropertiesForm";
		base.ShowInTaskbar = false;
		base.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
		this.Text = "{182}";
		base.Closing += new System.ComponentModel.CancelEventHandler(OpponentPropertiesForm_Closing);
		base.ResumeLayout(false);
	}

	private void OpponentPropertiesForm_Closing(object sender, CancelEventArgs e)
	{
		_localizer.Dispose();
		_localizer = null;
		if (base.DialogResult == DialogResult.OK)
		{
			EnterOppProps[] array = props;
			for (int i = 0; i < array.Length; i++)
			{
				array[i].OnValidateSheet(this);
			}
			CopyFrom(_original, _temp);
		}
	}
}
