using System.Windows.Forms;

namespace Editor.Localize;

public class FormLocalizer : Localizer
{
	private Form _form;

	public FormLocalizer(LocalizerManager manager, Form form)
		: base(manager, form)
	{
		_form = form;
		_form.KeyPreview = true;
		_form.KeyDown += _form_KeyDown;
	}

	protected override void Dispose(bool disposing)
	{
		_form.KeyDown -= _form_KeyDown;
		_form = null;
		base.Dispose(disposing);
	}

	private void _form_KeyDown(object sender, KeyEventArgs e)
	{
		if (e.Shift && e.Control && e.KeyCode == Keys.A)
		{
			base.Localized = !base.Localized;
		}
	}
}
