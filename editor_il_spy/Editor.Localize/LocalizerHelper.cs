using System;
using System.Collections;
using System.Reflection;

namespace Editor.Localize;

public class LocalizerHelper
{
	private LocalizerManager _manager;

	protected LocalizerManager Manager => _manager;

	public LocalizerHelper(LocalizerManager manager)
	{
		_manager = manager;
	}

	protected bool AddStringProperty(ArrayList list, string name, object obj)
	{
		return AddStringProperty(list, name, obj, null);
	}

	protected bool AddStringProperty(ArrayList list, string name, object obj, object[] index)
	{
		PropertyInfo property = obj.GetType().GetProperty(name, BindingFlags.Instance | BindingFlags.Public | BindingFlags.NonPublic);
		if (property != null)
		{
			return AddStringProperty(list, property, obj, index);
		}
		return false;
	}

	protected bool AddStringProperty(ArrayList list, PropertyInfo pi, object obj)
	{
		if (pi.CanRead && pi.CanWrite && pi.PropertyType == typeof(string))
		{
			string text = Convert.ToString(pi.GetValue(obj, null));
			if (text != null && text.Length > 2 && text[0] == '{' && text[text.Length - 1] == '}')
			{
				int id = Convert.ToInt32(text.Substring(1, text.Length - 2));
				if (text == "{" + id + "}")
				{
					list.Add(new LocalizedProperty(obj, pi, id, null));
					return true;
				}
			}
		}
		return false;
	}

	protected bool AddStringProperty(ArrayList list, PropertyInfo pi, object obj, object[] index)
	{
		if (index == null)
		{
			return AddStringProperty(list, pi, obj);
		}
		string text = Convert.ToString(pi.GetValue(obj, index));
		if (text != null && text.Length > 2 && text[0] == '{' && text[text.Length - 1] == '}')
		{
			int id = Convert.ToInt32(text.Substring(1, text.Length - 2));
			if (text == "{" + id + "}")
			{
				list.Add(new LocalizedProperty(obj, pi, id, index));
				return true;
			}
		}
		return false;
	}

	public virtual ArrayList ParseObject(object obj)
	{
		ArrayList arrayList = new ArrayList();
		PropertyInfo[] properties = obj.GetType().GetProperties(BindingFlags.Instance | BindingFlags.Public | BindingFlags.NonPublic);
		foreach (PropertyInfo pi in properties)
		{
			AddStringProperty(arrayList, pi, obj);
		}
		return arrayList;
	}
}
