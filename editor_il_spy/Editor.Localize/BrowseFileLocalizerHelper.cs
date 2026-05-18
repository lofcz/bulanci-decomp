using System.Collections;

namespace Editor.Localize;

public class BrowseFileLocalizerHelper : ControlLocalizerHelper
{
	public BrowseFileLocalizerHelper(LocalizerManager manager)
		: base(manager)
	{
	}

	public override ArrayList ParseObject(object obj)
	{
		ArrayList arrayList = base.ParseObject(obj);
		AddStringProperty(arrayList, "Title", obj);
		AddStringProperty(arrayList, "Filter", obj);
		return arrayList;
	}
}
