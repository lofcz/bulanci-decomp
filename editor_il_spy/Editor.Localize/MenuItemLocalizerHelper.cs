using System.Collections;

namespace Editor.Localize;

public class MenuItemLocalizerHelper : MenuLocalizerHelper
{
	public MenuItemLocalizerHelper(LocalizerManager manager)
		: base(manager)
	{
	}

	public override ArrayList ParseObject(object obj)
	{
		ArrayList arrayList = base.ParseObject(obj);
		AddStringProperty(arrayList, "Text", obj);
		return arrayList;
	}
}
