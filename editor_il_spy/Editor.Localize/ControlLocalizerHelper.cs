using System.Collections;
using System.Windows.Forms;

namespace Editor.Localize;

public class ControlLocalizerHelper : LocalizerHelper
{
	public ControlLocalizerHelper(LocalizerManager manager)
		: base(manager)
	{
	}

	public override ArrayList ParseObject(object obj)
	{
		ArrayList arrayList = new ArrayList();
		AddStringProperty(arrayList, "Text", obj);
		foreach (Control control in (obj as Control).Controls)
		{
			arrayList.AddRange(base.Manager.ParseObject(control));
		}
		return arrayList;
	}
}
