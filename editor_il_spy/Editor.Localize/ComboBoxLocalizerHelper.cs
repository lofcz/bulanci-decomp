using System.Collections;
using System.Windows.Forms;

namespace Editor.Localize;

public class ComboBoxLocalizerHelper : LocalizerHelper
{
	public ComboBoxLocalizerHelper(LocalizerManager manager)
		: base(manager)
	{
	}

	public override ArrayList ParseObject(object obj)
	{
		ArrayList arrayList = new ArrayList();
		ComboBox comboBox = obj as ComboBox;
		string displayMember = comboBox.DisplayMember;
		if (displayMember != null && displayMember.Length > 0)
		{
			foreach (object item in comboBox.Items)
			{
				AddStringProperty(arrayList, displayMember, item);
			}
		}
		else
		{
			for (int i = 0; i < comboBox.Items.Count; i++)
			{
				AddStringProperty(arrayList, "Item", comboBox.Items, new object[1] { i });
			}
		}
		return arrayList;
	}
}
