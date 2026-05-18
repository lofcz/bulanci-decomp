using System.Drawing;
using System.Xml.Serialization;

namespace Editor;

public sealed class SerializedRectangle
{
	private int _x;

	private int _y;

	private int _width;

	private int _height;

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

	[XmlAttribute]
	public int Width
	{
		get
		{
			return _width;
		}
		set
		{
			_width = value;
		}
	}

	[XmlAttribute]
	public int Height
	{
		get
		{
			return _height;
		}
		set
		{
			_height = value;
		}
	}

	public SerializedRectangle()
	{
		_x = 0;
		_y = 0;
		_width = 0;
		_height = 0;
	}

	public SerializedRectangle(Rectangle rect)
	{
		_x = rect.X;
		_y = rect.Y;
		_width = rect.Width;
		_height = rect.Height;
	}

	public Rectangle ToRectangle()
	{
		return new Rectangle(_x, _y, _width, _height);
	}

	public static SerializedRectangle FromRectangle(Rectangle rect)
	{
		return new SerializedRectangle(rect);
	}
}
