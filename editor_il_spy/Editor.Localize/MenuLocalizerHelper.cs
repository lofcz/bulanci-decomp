using System.Collections;
using System.Windows.Forms;

namespace Editor.Localize;

public class MenuLocalizerHelper : LocalizerHelper
{
	public MenuLocalizerHelper(LocalizerManager manager)
		: base(manager)
	{
	}

	public override ArrayList ParseObject(object obj)
	{
		ArrayList arrayList = new ArrayList();
		foreach (MenuItem menuItem in (obj as Menu).MenuItems)
		{
			arrayList.AddRange(base.Manager.ParseObject(menuItem));
		}
		return arrayList;
	}
}
