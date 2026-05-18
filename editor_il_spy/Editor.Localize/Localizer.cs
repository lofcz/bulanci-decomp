using System.Collections;
using System.ComponentModel;

namespace Editor.Localize;

public class Localizer : Component
{
	private LocalizerManager _manager;

	private ArrayList _properties;

	private bool _localized;

	public LocalizerManager Manager
	{
		get
		{
			return _manager;
		}
		set
		{
			if (_manager != value)
			{
				if (_manager != null && Localized)
				{
					RestoreArray();
				}
				_manager = value;
				if (_manager != null && Localized)
				{
					LocalizeArray(_properties);
				}
			}
		}
	}

	public bool Localized
	{
		get
		{
			return _localized;
		}
		set
		{
			if (_localized == value)
			{
				return;
			}
			_localized = value;
			if (Manager != null)
			{
				if (_localized)
				{
					LocalizeArray(_properties);
				}
				else
				{
					RestoreArray();
				}
			}
		}
	}

	public Localizer()
	{
		_manager = null;
		_properties = new ArrayList();
	}

	public Localizer(LocalizerManager manager)
	{
		_manager = manager;
		_properties = new ArrayList();
	}

	public Localizer(LocalizerManager manager, object obj)
	{
		_manager = manager;
		_properties = Manager.ParseObject(obj);
	}

	public void AddObject(object obj)
	{
		if (Manager != null)
		{
			ArrayList arrayList = Manager.ParseObject(obj);
			if (Localized)
			{
				LocalizeArray(arrayList);
			}
			_properties.AddRange(arrayList);
		}
	}

	protected override void Dispose(bool disposing)
	{
		if (_properties != null)
		{
			_properties.Clear();
			_properties = null;
		}
		_manager = null;
		base.Dispose(disposing);
	}

	private void LocalizeArray(ArrayList list)
	{
		foreach (LocalizedProperty item in list)
		{
			item.Localize(Manager.ResourceSet);
		}
	}

	private void RestoreArray()
	{
		foreach (LocalizedProperty property in _properties)
		{
			property.Restore();
		}
	}
}
