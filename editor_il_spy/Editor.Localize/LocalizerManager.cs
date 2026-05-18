using System;
using System.Collections;
using System.Resources;
using System.Windows.Forms;

namespace Editor.Localize;

public class LocalizerManager
{
	private Hashtable _helpers = new Hashtable();

	private ResourceSet _texts;

	public ResourceSet ResourceSet
	{
		get
		{
			return _texts;
		}
		set
		{
			if (_texts != value)
			{
				_texts = value;
				OnResourceSetChanged();
			}
		}
	}

	public event EventHandler ResourceSetChanged;

	public LocalizerManager()
	{
		_helpers[typeof(object)] = new LocalizerHelper(this);
		_helpers[typeof(Control)] = new ControlLocalizerHelper(this);
		_helpers[typeof(BrowseFile)] = new BrowseFileLocalizerHelper(this);
		_helpers[typeof(Menu)] = new MenuLocalizerHelper(this);
		_helpers[typeof(MenuItem)] = new MenuItemLocalizerHelper(this);
		_helpers[typeof(ToolBar)] = new ToolBarLocalizerHelper(this);
		_helpers[typeof(ComboBox)] = new ComboBoxLocalizerHelper(this);
	}

	public ArrayList ParseObject(object obj)
	{
		Type type = obj.GetType();
		while (type != null)
		{
			if (_helpers[type] is LocalizerHelper localizerHelper)
			{
				return localizerHelper.ParseObject(obj);
			}
			type = type.BaseType;
		}
		return null;
	}

	public string GetString(int id)
	{
		return ResourceSet.GetString(id.ToString());
	}

	protected virtual void OnResourceSetChanged()
	{
		if (this.ResourceSetChanged != null)
		{
			this.ResourceSetChanged(this, EventArgs.Empty);
		}
	}
}
