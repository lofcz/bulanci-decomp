using System.Drawing;
using System.Drawing.Drawing2D;

namespace Editor.Temp;

public sealed class SelectView : View
{
	private Brush _brush;

	private Pen _pen;

	public SelectView()
	{
		_brush = new SolidBrush(Color.FromArgb(64, Color.White));
		_pen = new Pen(Color.White);
		_pen.DashStyle = DashStyle.DashDotDot;
	}

	protected override void OnPaint(Graphics g)
	{
		g.FillRectangle(_brush, base.ClientBounds);
		g.DrawRectangle(_pen, 0, 0, base.Size.Width - 1, base.Size.Height - 1);
	}
}
