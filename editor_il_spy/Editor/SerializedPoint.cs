using System.Drawing;
using System.Xml.Serialization;

namespace Editor;

public sealed class SerializedPoint
{
	private int _x;

	private int _y;

	[XmlAttribute]
	public int X
	{
		get
		{
			return _x;
		}
		set
		{
			_x = value;
		}
	}

	[XmlAttribute]
	public int Y
	{
		get
		{
			return _y;
		}
		set
		{
			_y = value;
		}
	}

	public SerializedPoint()
	{
		_x = 0;
		_y = 0;
	}

	public SerializedPoint(Point p)
	{
		_x = p.X;
		_y = p.Y;
	}

	public Point ToPoint()
	{
		return new Point(_x, _y);
	}
}
