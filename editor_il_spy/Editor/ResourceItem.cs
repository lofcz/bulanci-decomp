using System;
using System.IO;
using System.Windows.Forms;
using System.Xml.Serialization;

namespace Editor;

public abstract class ResourceItem : IHasName, ICloneable, IKeybHandler
{
	public const string LevelSound = "$$LevelSound$$";

	public const string LoadingBitmap = "$$LoadBitmap$$";

	public const string LevelSign = "$$LevelSign$$";

	public const string LevelBackground = "$$LevelBackground$$";

	private uint _id;

	private uint _size;

	private long _position;

	private int _locked;

	private int _changes;

	private string _name = "";

	private Level _level;

	[XmlIgnore]
	public uint ID
	{
		get
		{
			return _id;
		}
		set
		{
			_id = value;
		}
	}

	[XmlIgnore]
	public uint Size
	{
		get
		{
			return _size;
		}
		set
		{
			_size = value;
		}
	}

	[XmlIgnore]
	public long Position
	{
		get
		{
			return _position;
		}
		set
		{
			_position = value;
		}
	}

	[XmlIgnore]
	public abstract uint ClassID { get; }

	[XmlIgnore]
	public Level Level => _level;

	[XmlAttribute]
	public string Name
	{
		get
		{
			return _name;
		}
		set
		{
			if (_name != value)
			{
				if (value == null || value.Length < 3)
				{
					throw new Exception(Tools.Localizer.GetString(98));
				}
				if (Level != null)
				{
					Level.Resources.Rename(this, value);
				}
				string name = _name;
				_name = value;
				if (Level != null)
				{
					Level.ResourceRenamed(this, name);
				}
				ItemModified();
			}
		}
	}

	public bool IsUsed
	{
		get
		{
			if (Level != null)
			{
				return Level.IsResourceUsed(this);
			}
			return false;
		}
	}

	public bool IsReserved => IsNameReserved(Name);

	public void WriteHeader(BinaryWriter writer)
	{
		writer.Write(ID);
		writer.Write(ClassID);
		writer.Write(Position);
		writer.Write(Size);
	}

	public abstract void Write(BinaryWriter writer);

	internal void SetLevel(Level level)
	{
		_level = level;
	}

	protected void ItemModified()
	{
		if (_locked > 0)
		{
			_changes++;
		}
		else if (_level != null)
		{
			_level.OnResourceModified(this);
		}
	}

	public void Lock()
	{
		_locked++;
	}

	public void Unlock()
	{
		if (_locked > 0)
		{
			_locked--;
			if (_locked == 0 && _changes > 0)
			{
				_changes = 0;
				ItemModified();
			}
		}
	}

	public virtual void CopyFrom(ResourceItem item)
	{
		Lock();
		if (item != null)
		{
			_id = item._id;
			_size = item._size;
			_position = item._position;
			Name = item.Name;
		}
		Unlock();
	}

	public ResourceItem()
	{
	}

	public object Clone()
	{
		ResourceItem obj = GetType().GetConstructor(Type.EmptyTypes).Invoke(null) as ResourceItem;
		obj.CopyFrom(this);
		return obj;
	}

	public static bool IsNameReserved(string name)
	{
		return name switch
		{
			"$$LevelBackground$$" => true, 
			"$$LevelSign$$" => true, 
			"$$LevelSound$$" => true, 
			"$$LoadBitmap$$" => true, 
			_ => false, 
		};
	}

	public bool OnKeyDown(Control sender, Keys key)
	{
		try
		{
			if (key == Keys.Delete)
			{
				Level.Resources.Remove(this);
				return true;
			}
		}
		catch (Exception exc)
		{
			Tools.ShowError(exc);
			return true;
		}
		return false;
	}
}
