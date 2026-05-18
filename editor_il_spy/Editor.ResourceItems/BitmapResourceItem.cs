using System;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using System.Xml.Serialization;

namespace Editor.ResourceItems;

public class BitmapResourceItem : FileResourceItem, IMouseHandler
{
	private Bitmap _bitmap;

	private BitmapFormat _format = BitmapFormat.Jpeg;

	private Point _zero_at = Point.Empty;

	private Color _zero_color = Color.Empty;

	private ZeroColor _zero;

	[XmlIgnore]
	public Bitmap Bitmap
	{
		get
		{
			return _bitmap;
		}
		set
		{
			if (_bitmap != value)
			{
				_bitmap = value;
				ItemModified();
			}
		}
	}

	[XmlAttribute]
	public BitmapFormat Format
	{
		get
		{
			if (_zero != ZeroColor.NotDefined)
			{
				return BitmapFormat.Special;
			}
			return _format;
		}
		set
		{
			BitmapFormat bitmapFormat = value;
			if (_zero != ZeroColor.NotDefined)
			{
				bitmapFormat = BitmapFormat.Special;
			}
			if (_format != bitmapFormat)
			{
				_format = bitmapFormat;
				if (_format != BitmapFormat.Special)
				{
					_zero_at = Point.Empty;
					_zero = ZeroColor.NotDefined;
				}
				ItemModified();
			}
		}
	}

	public byte[] BitmapData
	{
		get
		{
			if (Bitmap == null)
			{
				return null;
			}
			MemoryStream memoryStream = new MemoryStream(1048576);
			Bitmap.Save(memoryStream, ImageFormat.Png);
			return memoryStream.ToArray();
		}
		set
		{
			if (value == null)
			{
				Bitmap = null;
			}
			else
			{
				Bitmap = new Bitmap(new MemoryStream(value, writable: false));
			}
		}
	}

	[XmlAttribute]
	public ZeroColor Transparency
	{
		get
		{
			return _zero;
		}
		set
		{
			if (_zero != value)
			{
				_zero = value;
				ItemModified();
			}
		}
	}

	[XmlIgnore]
	public Point TransparentColorAt
	{
		get
		{
			return _zero_at;
		}
		set
		{
			if (_zero == ZeroColor.ByLocation && _zero_at != value)
			{
				_zero_at = value;
				GetZeroColor();
				ItemModified();
			}
		}
	}

	[XmlElement("TransparentColorAt")]
	public SerializedPoint TransparentColorAt2
	{
		get
		{
			return new SerializedPoint(TransparentColorAt);
		}
		set
		{
			TransparentColorAt = value.ToPoint();
		}
	}

	[XmlIgnore]
	public Color TransparentColor
	{
		get
		{
			return Transparency switch
			{
				ZeroColor.NotDefined => Color.Empty, 
				ZeroColor.ByLocation => GetZeroColor(), 
				_ => _zero_color, 
			};
		}
		set
		{
			if (_zero_color != value)
			{
				_zero_color = value;
				ItemModified();
			}
		}
	}

	[XmlElement("TransparentColor")]
	public SerializedColor TransparentSerializedColor
	{
		get
		{
			return TransparentColor;
		}
		set
		{
			TransparentColor = value;
		}
	}

	public override uint ClassID => (uint)Format;

	private Color GetZeroColor()
	{
		if (_bitmap == null)
		{
			return Color.Empty;
		}
		if (_zero_at.X < 0 || _zero_at.Y < 0 || _zero_at.X >= _bitmap.Size.Width || _zero_at.Y >= _bitmap.Size.Height)
		{
			_zero_at = Point.Empty;
		}
		return _bitmap.GetPixel(_zero_at.X, _zero_at.Y);
	}

	protected unsafe void WriteBitmap(BinaryWriter writer, Bitmap bitmap)
	{
		MemoryStream memoryStream = new MemoryStream();
		switch (Format)
		{
		case BitmapFormat.Jpeg:
			bitmap.Save(memoryStream, ImageFormat.Jpeg);
			break;
		case BitmapFormat.Bmp:
			bitmap.Save(memoryStream, ImageFormat.Bmp);
			break;
		case BitmapFormat.Special:
		{
			BinaryWriter binaryWriter = new BinaryWriter(memoryStream);
			binaryWriter.Write(bitmap.Size.Width);
			binaryWriter.Write(bitmap.Size.Height);
			binaryWriter.Write(5);
			uint value = uint.MaxValue;
			if (Transparency != ZeroColor.NotDefined)
			{
				value = (uint)(TransparentColor.R * 65536 + TransparentColor.G * 256 + TransparentColor.B);
			}
			BitmapData bitmapData = bitmap.LockBits(new Rectangle(Point.Empty, bitmap.Size), ImageLockMode.ReadOnly, PixelFormat.Format24bppRgb);
			int num = Math.Abs(bitmapData.Stride);
			binaryWriter.Write(num);
			binaryWriter.Write(value);
			binaryWriter.Write(byte.MaxValue);
			binaryWriter.Write((int)Math.Pow(2.0, 24.0));
			binaryWriter.Write((byte)0);
			int num2 = bitmapData.Height * num;
			byte* ptr = (byte*)bitmapData.Scan0.ToPointer();
			if (bitmapData.Stride < 0)
			{
				ptr += num2;
			}
			byte[] array = new byte[num];
			while (num2 > 0)
			{
				if (bitmapData.Stride < 0)
				{
					ptr -= num;
				}
				Marshal.Copy(new IntPtr(ptr), array, 0, num);
				binaryWriter.Write(array, 0, num);
				if (bitmapData.Stride > 0)
				{
					ptr += num;
				}
				num2 -= num;
			}
			bitmap.UnlockBits(bitmapData);
			break;
		}
		}
		if (memoryStream.Length > 0)
		{
			writer.Write(memoryStream.GetBuffer(), 0, (int)memoryStream.Length);
		}
	}

	public override void Write(BinaryWriter writer)
	{
		WriteBitmap(writer, Bitmap);
	}

	public override void LoadData()
	{
		_bitmap = new Bitmap(base.FileName);
	}

	public override void CopyFrom(ResourceItem item)
	{
		Lock();
		base.CopyFrom(item);
		if (item is BitmapResourceItem bitmapResourceItem)
		{
			Format = bitmapResourceItem.Format;
			Bitmap = bitmapResourceItem.Bitmap;
			Transparency = bitmapResourceItem.Transparency;
			switch (Transparency)
			{
			case ZeroColor.ByLocation:
				TransparentColorAt = bitmapResourceItem.TransparentColorAt;
				break;
			case ZeroColor.ByColor:
				TransparentColor = bitmapResourceItem.TransparentColor;
				break;
			}
		}
		Unlock();
	}

	public void OnDoubleClick(Control sender, Point where)
	{
		OnProperties(this, EventArgs.Empty);
	}

	public void OnRightClick(Control sender, Point where)
	{
		ContextMenu contextMenu = new ContextMenu();
		contextMenu.MenuItems.Add(Tools.Localizer.GetString(87), OnRemove);
		contextMenu.MenuItems.Add(Tools.Localizer.GetString(73), OnProperties);
		contextMenu.MenuItems[1].DefaultItem = true;
		contextMenu.Show(sender, where);
	}

	private void OnProperties(object sender, EventArgs args)
	{
		new BitmapPropertiesForm(this).ShowDialog();
	}

	private void OnRemove(object sender, EventArgs args)
	{
		try
		{
			base.Level.Resources.Remove(this);
		}
		catch (Exception exc)
		{
			Tools.ShowError(exc);
		}
	}
}
