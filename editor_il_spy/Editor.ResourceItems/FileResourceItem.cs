using System.Xml.Serialization;

namespace Editor.ResourceItems;

public abstract class FileResourceItem : ResourceItem
{
	private string _name;

	private bool _load_data;

	[XmlIgnore]
	public bool CallLoadData
	{
		get
		{
			return _load_data;
		}
		set
		{
			_load_data = value;
		}
	}

	[XmlAttribute]
	public string FileName
	{
		get
		{
			return _name;
		}
		set
		{
			bool callLoadData = CallLoadData;
			CallLoadData = false;
			if (!(_name != value))
			{
				return;
			}
			string name = _name;
			_name = value;
			if (callLoadData)
			{
				try
				{
					LoadData();
				}
				catch
				{
					_name = name;
					throw;
				}
			}
			ItemModified();
		}
	}

	public virtual void LoadData()
	{
	}

	public override void CopyFrom(ResourceItem item)
	{
		Lock();
		base.CopyFrom(item);
		if (item is FileResourceItem fileResourceItem)
		{
			CallLoadData = false;
			FileName = fileResourceItem.FileName;
			CallLoadData = fileResourceItem.CallLoadData;
		}
		Unlock();
	}

	public FileResourceItem()
	{
	}
}
