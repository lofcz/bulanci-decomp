using System.Drawing;

namespace Editor.Temp;

public class MineView : ItemView
{
	private Bitmap _bitmap;

	private Pen _pen;

	public override bool ContainsMouse(Point where)
	{
		if (!base.ContainsMouse(where))
		{
			return false;
		}
		if (_bitmap == null)
		{
			return true;
		}
		Point point = MakeLocal(where);
		return _bitmap.GetPixel(point.X, point.Y).ToArgb() != 0;
	}

	protected override void OnPaint(Graphics g)
	{
		if (_bitmap != null)
		{
			g.DrawImage(_bitmap, new Rectangle(Point.Empty, _bitmap.Size));
		}
		if (base.Selected)
		{
			g.DrawRectangle(_pen, 0, 0, base.Size.Width - 1, base.Size.Height - 1);
		}
	}

	public MineView(LevelItem item)
		: base(item)
	{
		_pen = new Pen(Color.White);
		_bitmap = Editor.Tools.Texts.GetObject("mine") as Bitmap;
		_bitmap.MakeTransparent(Color.FromArgb(0, 255, 0));
	}
}
