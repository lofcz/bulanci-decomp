using System.Drawing;

namespace Editor.Temp;

public sealed class MouseEvent : Event
{
	private Point _where;

	private MouseButtons _buttons;

	private bool _double_click;

	private ControlKey _key;

	private View _under;

	public Point Where => _where;

	public int X => _where.X;

	public int Y => _where.Y;

	public MouseButtons Buttons => _buttons;

	public bool DoubleClick => _double_click;

	public ControlKey Key => _key;

	public bool Shift => (_key & ControlKey.Shift) != 0;

	public bool Ctrl => (_key & ControlKey.Ctrl) != 0;

	public bool Alt => (_key & ControlKey.Alt) != 0;

	public View Under
	{
		get
		{
			return _under;
		}
		set
		{
			_under = value;
		}
	}

	public MouseEvent(EventType type, Point where, MouseButtons buttons, bool dblclick, ControlKey key)
		: base(type)
	{
		_where = where;
		_buttons = buttons;
		_double_click = dblclick;
		_key = key;
	}

	public MouseEvent(EventType type, Point where, ControlKey key)
		: base(type)
	{
		_where = where;
		_buttons = MouseButtons.None;
		_key = key;
		_double_click = false;
	}
}
