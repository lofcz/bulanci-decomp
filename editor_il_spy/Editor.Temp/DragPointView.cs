using System;
using System.Drawing;
using System.Windows.Forms;

namespace Editor.Temp;

public sealed class DragPointView : View
{
	private const int _minsize = 10;

	private DragPointType _dpt = DragPointType.Unknown;

	private View _view;

	private bool _locked;

	public static int MinSize => 10;

	public DragPointType DragPointType
	{
		get
		{
			return _dpt;
		}
		set
		{
			_dpt = value;
			UpdateRectangle();
			UpdateCursor();
		}
	}

	public View View => _view;

	public DragPointView(View view, DragPointType dpt)
	{
		_dpt = dpt;
		_view = view;
		UpdateRectangle();
		UpdateCursor();
		view.BoundsChanged += view_BoundsChanged;
		base.EventMask = (EventType)2055;
		base.DragMove = true;
		base.DragGroup = (int)(10 + dpt);
	}

	private void UpdateRectangle()
	{
		if (_view != null && DragPointType != DragPointType.Unknown)
		{
			base.Bounds = ToRectangle(_view.Bounds, DragPointType);
		}
	}

	private void UpdateCursor()
	{
		base.Cursor = ToCursor(DragPointType);
	}

	protected override void OnPaint(Graphics g)
	{
		g.FillRectangle(new SolidBrush(Color.Green), base.ClientBounds);
		g.DrawRectangle(new Pen(Color.White), 0, 0, base.Size.Width - 1, base.Size.Height - 1);
	}

	public static Rectangle ToRectangle(Rectangle r, DragPointType dpt)
	{
		return dpt switch
		{
			DragPointType.Up => new Rectangle(r.X + r.Width / 2 - 2, r.Y - 2, 5, 5), 
			DragPointType.Down => new Rectangle(r.X + r.Width / 2 - 2, r.Y + r.Height - 2 - 1, 5, 5), 
			DragPointType.Left => new Rectangle(r.X - 2, r.Y + r.Height / 2 - 2, 5, 5), 
			DragPointType.Right => new Rectangle(r.X + r.Width - 2 - 1, r.Y + r.Height / 2 - 2, 5, 5), 
			DragPointType.LeftUp => new Rectangle(r.X - 2, r.Y - 2, 5, 5), 
			DragPointType.RightUp => new Rectangle(r.X + r.Width - 2 - 1, r.Y - 2, 5, 5), 
			DragPointType.RightDown => new Rectangle(r.X + r.Width - 2 - 1, r.Y + r.Height - 2 - 1, 5, 5), 
			DragPointType.LeftDown => new Rectangle(r.X - 2, r.Y + r.Height - 2 - 1, 5, 5), 
			_ => Rectangle.Empty, 
		};
	}

	private Rectangle GrowRect(Rectangle r, Point chg, GrowMode mode)
	{
		if ((mode & GrowMode.LoY) != GrowMode.None)
		{
			if (r.Height - chg.Y < MinSize)
			{
				chg.Y = r.Height - MinSize;
			}
			r.Y += chg.Y;
			r.Height -= chg.Y;
		}
		else if ((mode & GrowMode.HiY) != GrowMode.None)
		{
			if (r.Height + chg.Y < MinSize)
			{
				chg.Y = MinSize - r.Height;
			}
			r.Height += chg.Y;
		}
		if ((mode & GrowMode.LoX) != GrowMode.None)
		{
			if (r.Width - chg.X < MinSize)
			{
				chg.X = r.Width - MinSize;
			}
			r.X += chg.X;
			r.Width -= chg.X;
		}
		else if ((mode & GrowMode.HiX) != GrowMode.None)
		{
			if (r.Width + chg.X < MinSize)
			{
				chg.X = MinSize - r.Width;
			}
			r.Width += chg.X;
		}
		return r;
	}

	private Rectangle ToViewBounds(Rectangle r, Point chg)
	{
		return DragPointType switch
		{
			DragPointType.Up => GrowRect(r, chg, GrowMode.LoY), 
			DragPointType.Down => GrowRect(r, chg, GrowMode.HiY), 
			DragPointType.Left => GrowRect(r, chg, GrowMode.LoX), 
			DragPointType.Right => GrowRect(r, chg, GrowMode.HiX), 
			DragPointType.LeftUp => GrowRect(r, chg, GrowMode.Lo), 
			DragPointType.RightUp => GrowRect(r, chg, (GrowMode)6), 
			DragPointType.LeftDown => GrowRect(r, chg, (GrowMode)9), 
			DragPointType.RightDown => GrowRect(r, chg, GrowMode.Hi), 
			_ => r, 
		};
	}

	public static Cursor ToCursor(DragPointType dpt)
	{
		switch (dpt)
		{
		case DragPointType.Up:
		case DragPointType.Down:
			return Cursors.SizeNS;
		case DragPointType.Right:
		case DragPointType.Left:
			return Cursors.SizeWE;
		case DragPointType.LeftUp:
		case DragPointType.RightDown:
			return Cursors.SizeNWSE;
		case DragPointType.RightUp:
		case DragPointType.LeftDown:
			return Cursors.SizeNESW;
		default:
			return Cursors.Default;
		}
	}

	public override void HandleEvent(Event e)
	{
		base.HandleEvent(e);
		if (e.What == EventType.MouseDown)
		{
			DragStart((e as MouseEvent).Where);
			e.Clear(this);
		}
		if (e.What == EventType.Broadcast && HandleDragging(e))
		{
			switch ((ViewCommands)(e as BroadcastEvent).Command)
			{
			case ViewCommands.DragStart:
				_view.SetStateFlag(ViewState.Dragging, on: true);
				break;
			case ViewCommands.DragFinish:
				_view.SetStateFlag(ViewState.Dragging, on: false);
				break;
			}
		}
	}

	protected override void DragLocationLimit(ref Rectangle rect)
	{
		rect.X = Math.Max(0, Math.Min(base.Parent.Size.Width - rect.Width, rect.X));
		rect.Y = Math.Max(0, Math.Min(base.Parent.Size.Height - rect.Height, rect.Y));
		if (DragPointType == DragPointType.Up || DragPointType == DragPointType.Down)
		{
			rect.X = base.Bounds.X;
		}
		if (DragPointType == DragPointType.Left || DragPointType == DragPointType.Right)
		{
			rect.Y = base.Bounds.Y;
		}
		Point chg = new Point(rect.X - base.BoundsBackup.X, rect.Y - base.BoundsBackup.Y);
		_locked = true;
		_view.ChangeBounds(ToViewBounds(_view.BoundsBackup, chg));
		_locked = false;
		rect = ToRectangle(_view.Bounds, DragPointType);
	}

	private void view_BoundsChanged(object sender, EventArgs e)
	{
		if (!_locked)
		{
			UpdateRectangle();
		}
	}
}
