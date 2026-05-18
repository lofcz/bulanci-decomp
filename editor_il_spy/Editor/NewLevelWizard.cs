using System.ComponentModel;
using System.Drawing;
using Editor.Localize;

namespace Editor;

public class NewLevelWizard : WizardForm
{
	private IContainer components;

	private Localizer _localizer;

	public NewLevelWizard(Level level)
	{
		InitializeComponent();
		_listSheets.Add(new EnterLevelName(level));
		_listSheets.Add(new EnterLoadBitmap(level));
		_listSheets.Add(new EnterBackgroundBitmap(level));
		_listSheets.Add(new EnterLevelSound(level));
		_localizer = new FormLocalizer(Tools.Localizer, this);
		for (int i = 0; i < _listSheets.Count; i++)
		{
			_localizer.AddObject(_listSheets[i]);
		}
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
		this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
		base.ClientSize = new System.Drawing.Size(450, 319);
		base.Name = "NewLevelWizard";
		this.Text = "{17}";
		base.Closing += new System.ComponentModel.CancelEventHandler(NewLevelWizard_Closing);
	}

	private void NewLevelWizard_Closing(object sender, CancelEventArgs e)
	{
		_localizer.Dispose();
		_localizer = null;
	}
}
