using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms;
using Editor.Localize;
using Editor.ResourceItems;

namespace Editor;

public class NewBitmapWizard : WizardForm
{
	private IContainer components;

	private BitmapResourceItem _item;

	private bool _modified;

	private Localizer _localizer;

	public BitmapResourceItem BitmapResource => _item;

	public NewBitmapWizard(Level level)
	{
		InitializeComponent();
		_item = new BitmapResourceItem();
		_item.Name = level.GenerateNewResourceName("bitmap");
		_modified = level.Modified;
		level.Resources.Add(_item);
		_listSheets.Add(new EnterBitmap(_item));
		_listSheets.Add(new EnterZeroColor(_item));
		_listSheets.Add(new EnterResourceName(_item));
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
		base.ImeMode = System.Windows.Forms.ImeMode.NoControl;
		base.Name = "NewBitmapWizard";
		this.Text = "{18}";
		base.Closing += new System.ComponentModel.CancelEventHandler(NewBitmapWizard_Closing);
	}

	private void NewBitmapWizard_Closing(object sender, CancelEventArgs e)
	{
		_localizer.Dispose();
		_localizer = null;
		if (base.DialogResult != DialogResult.OK)
		{
			Level level = _item.Level;
			level.Resources.Remove(_item);
			level.Modified = _modified;
		}
	}
}
