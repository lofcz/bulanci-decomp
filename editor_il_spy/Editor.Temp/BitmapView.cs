using System.Drawing;

namespace Editor.Temp;

public class BitmapView : View
{
	private Image _img;

	private ContentAlignment _align = ContentAlignment.MiddleCenter;

	private bool _autosize;

	public Image Image
	{
		get
		{
			return _img;
		}
		set
		{
			_img = value;
			UpdateSize();
		}
	}

	public bool AutoSize
	{
		get
		{
			return _autosize;
		}
		set
		{
			_autosize = value;
			UpdateSize();
		}
	}

	public ContentAlignment ImageAlign
	{
		get
		{
			return _align;
		}
		set
		{
			_align = value;
			OnFaceChanged();
		}
	}

	protected override void OnPaint(Graphics g)
	{
		if (_img == null)
		{
			g.FillRectangle(new SolidBrush(Color.Black), base.ClientBounds);
		}
		else
		{
			g.DrawImage(_img, new Rectangle(Tools.AlignSize(_img.Size, base.ClientBounds, _align), _img.Size));
		}
	}

	private bool UpdateSize()
	{
		if (AutoSize)
		{
			if (Image != null)
			{
				base.Size = Image.Size;
			}
			else
			{
				base.Size = Size.Empty;
			}
			OnFaceChanged();
			return true;
		}
		OnFaceChanged();
		return false;
	}

	public BitmapView()
	{
	}

	public BitmapView(Point p, Image img)
	{
		base.Location = p;
		Image = img;
		ImageAlign = ContentAlignment.TopLeft;
		AutoSize = true;
	}

	public BitmapView(Rectangle rect, Image img, ContentAlignment align, bool autosize)
		: base(rect)
	{
		Image = img;
		ImageAlign = align;
		AutoSize = autosize;
	}
}
