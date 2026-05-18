using System;
using System.IO;
using System.Xml.Serialization;

namespace Editor.ResourceItems;

public class ResourceSign : ResourceItem
{
	private DateTime _time = DateTime.Now;

	private string _content = "";

	private string _copyright = "";

	public override uint ClassID => 94u;

	[XmlAttribute]
	public string Content
	{
		get
		{
			return _content;
		}
		set
		{
			if (value == null || value.Length < 5)
			{
				throw new Exception(Tools.Localizer.GetString(100));
			}
			if (value.Length > 32)
			{
				throw new Exception(Tools.Localizer.GetString(101));
			}
			if (_content != value)
			{
				_content = value;
				ItemModified();
			}
		}
	}

	[XmlAttribute]
	public string Copyright
	{
		get
		{
			return _copyright;
		}
		set
		{
			if (_copyright != value)
			{
				_copyright = value;
				if (_copyright == null)
				{
					_copyright = "";
				}
				ItemModified();
			}
		}
	}

	public void UpdateTime()
	{
		_time = DateTime.Now;
	}

	public override void Write(BinaryWriter writer)
	{
		uint value = (uint)((_time.Year << 11) | (_time.Month << 6) | _time.Day);
		writer.Write(value);
		WriteString(writer, _content);
		WriteString(writer, _copyright);
		writer.Write((byte)0);
	}

	private void WriteString(BinaryWriter writer, string s)
	{
		int length = s.Length;
		writer.Write(length);
		for (int i = 0; i < length; i++)
		{
			writer.Write((ushort)s[i]);
		}
	}

	public override void CopyFrom(ResourceItem item)
	{
		Lock();
		base.CopyFrom(item);
		if (item is ResourceSign resourceSign)
		{
			_time = resourceSign._time;
			Content = resourceSign.Content;
			Copyright = resourceSign.Copyright;
		}
		Unlock();
	}

	public ResourceSign()
	{
		base.Name = "$$LevelSign$$";
	}
}
