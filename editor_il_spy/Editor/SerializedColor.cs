using System.Drawing;
using System.Xml.Serialization;

namespace Editor;

public sealed class SerializedColor
{
	private byte _red;

	private byte _green;

	private byte _blue;

	private byte _alpha;

	[XmlAttribute]
	public byte Red
	{
		get
		{
			return _red;
		}
		set
		{
			_red = value;
		}
	}

	[XmlAttribute]
	public byte Green
	{
		get
		{
			return _green;
		}
		set
		{
			_green = value;
		}
	}

	[XmlAttribute]
	public byte Blue
	{
		get
		{
			return _blue;
		}
		set
		{
			_blue = value;
		}
	}

	[XmlAttribute]
	public byte Alpha
	{
		get
		{
			return _alpha;
		}
		set
		{
			_alpha = value;
		}
	}

	[XmlIgnore]
	public Color Color
	{
		get
		{
			return Color.FromArgb(Alpha, Red, Green, Blue);
		}
		set
		{
			_red = value.R;
			_green = value.G;
			_blue = value.B;
			_alpha = value.A;
		}
	}

	public SerializedColor()
	{
		Color = Color.Black;
	}

	public SerializedColor(Color color)
	{
		Color = color;
	}

	public static implicit operator Color(SerializedColor color)
	{
		return color.Color;
	}

	public static implicit operator SerializedColor(Color color)
	{
		return new SerializedColor(color);
	}
}
