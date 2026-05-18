using System.Reflection;
using System.Resources;

namespace Editor.Localize;

public class LocalizedProperty
{
	private object _object;

	private PropertyInfo _pi;

	private int _text_id;

	private object[] _index;

	public LocalizedProperty(object obj, PropertyInfo pi, int id, object[] index)
	{
		_object = obj;
		_pi = pi;
		_text_id = id;
		_index = index;
	}

	public void Localize(ResourceSet texts)
	{
		_pi.SetValue(_object, texts.GetString(_text_id.ToString()), _index);
	}

	public void Restore()
	{
		_pi.SetValue(_object, "{" + _text_id + "}", _index);
	}
}
