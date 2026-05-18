using System.Collections;
using System.Windows.Forms;

namespace Editor.Localize;

public class ToolBarLocalizerHelper : ControlLocalizerHelper
{
	public ToolBarLocalizerHelper(LocalizerManager manager)
		: base(manager)
	{
	}

	public override ArrayList ParseObject(object obj)
	{
		ArrayList arrayList = base.ParseObject(obj);
		foreach (ToolBarButton button in (obj as ToolBar).Buttons)
		{
			AddStringProperty(arrayList, "ToolTipText", button);
		}
		return arrayList;
	}
}
