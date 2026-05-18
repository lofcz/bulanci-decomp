using System;
using System.Drawing;
using System.Windows.Forms;

namespace Editor.Temp;

public class GroundView : View
{
	private ImageView _view;

	private Pen _pen;

	private int _started_at;

	public ImageView View => _view;

	public GroundView(ImageView view)
	{
		_view = view;
		UpdateRectangle();
		base.Cursor = Cursors.HSplit;
		view.BoundsChanged += view_BoundsChanged;
		base.EventMask = (EventType)2055;
		base.DragMove = true;
		base.DragGroup = 100;
		_pen = new Pen(Color.Yellow);
	}

	public void UpdateRectangle()
	{
		if (_view != null)
		{
			base.Bounds = new Rectangle(_view.Bounds.Left, _view.Bounds.Bottom + _view.Ground - 2, _view.Bounds.Width, 3);
		}
	}

	protected override void OnPaint(Graphics g)
	{
		g.DrawLine(_pen, 0, 1, base.Size.Width, 1);
	}

	private void view_BoundsChanged(object sender, EventArgs args)
	{
		UpdateRectangle();
	}

	protected override void DragLocationLimit(ref Rectangle rect)
	{
		rect.X = base.Bounds.X;
		rect.Y = Math.Max(1, Math.Min(base.Parent.Size.Height - rect.Height - 2 + 1, rect.Y));
	}

	public override void HandleEvent(Event e)
	{
		base.HandleEvent(e);
		if (e.What == EventType.MouseDown)
		{
			DragStart((e as MouseEvent).Where);
			e.Clear(this);
		}
		if (e.What == EventType.Broadcast)
		{
			HandleDragging(e);
		}
	}

	protected override void OnDraggingChanged()
	{
		base.OnDraggingChanged();
		if (base.Dragging)
		{
			_started_at = base.Bounds.Y;
		}
		else
		{
			_view.Ground += base.Bounds.Y - _started_at;
		}
	}
}
