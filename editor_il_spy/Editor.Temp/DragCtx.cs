using System.Drawing;

namespace Editor.Temp;

internal sealed class DragCtx
{
	private Point _started;

	private Point _now;

	private Point _offs;

	private View _source;

	public View Source => _source;

	public int DragGroup => Source.DragGroup;

	public Point Offset => _offs;

	public Point MouseStarted => _started;

	public Point MouseNow
	{
		get
		{
			return _now;
		}
		set
		{
			_now = value;
			_offs = new Point(_now.X - _started.X, _now.Y - _started.Y);
		}
	}

	public DragCtx(View source, Point started)
	{
		_source = source;
		_started = started;
		_now = started;
	}
}
