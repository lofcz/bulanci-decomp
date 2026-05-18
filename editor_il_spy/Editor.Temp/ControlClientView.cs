using System;
using System.Drawing;
using System.Windows.Forms;

namespace Editor.Temp;

public class ControlClientView : View
{
	private ViewControl _control;

	private SizeMode _sizemode;

	private View _capture_owner;

	private ControlKey _control_key;

	private Point _mouse_last = new Point(int.MinValue, int.MinValue);

	private bool _mouse_in;

	private MouseEventHandler _on_mouse_down;

	private MouseEventHandler _on_mouse_up;

	private MouseEventHandler _on_mouse_move;

	private KeyEventHandler _on_key_down;

	private KeyEventHandler _on_key_up;

	private EventHandler _on_resize;

	public ViewControl Control => _control;

	public SizeMode SizeMode
	{
		get
		{
			return _sizemode;
		}
		set
		{
			if (_sizemode != value)
			{
				_sizemode = value;
				OnSizeModeChanged();
			}
		}
	}

	protected override View MouseCapturedBy
	{
		get
		{
			return _capture_owner;
		}
		set
		{
			if (_capture_owner != value)
			{
				_capture_owner = value;
				if (_control != null)
				{
					_control.Capture = _capture_owner != null;
				}
			}
		}
	}

	public override Point MouseLast => _mouse_last;

	public override bool UnderMouse => _mouse_in;

	public event EventHandler SizeModeChanged;

	public ControlClientView()
	{
		base.EventMask = (EventType)3463;
		SetStateFlag(ViewState.Chained, on: true);
		_on_mouse_down = _control_MouseDown;
		_on_mouse_up = _control_MouseUp;
		_on_mouse_move = _control_MouseMove;
		_on_key_down = _control_KeyDown;
		_on_key_up = _control_KeyUp;
		_on_resize = _control_Resize;
	}

	internal void SetControl(ViewControl control)
	{
		if (_control != control)
		{
			if (_control != null)
			{
				_control.MouseDown -= _on_mouse_down;
				_control.MouseUp -= _on_mouse_up;
				_control.MouseMove -= _on_mouse_move;
				_control.KeyDown -= _on_key_down;
				_control.KeyUp -= _on_key_up;
				_control.Resize -= _on_resize;
				_control.Capture = false;
			}
			_control = control;
			if (_control != null)
			{
				_control.MouseDown += _on_mouse_down;
				_control.MouseUp += _on_mouse_up;
				_control.MouseMove += _on_mouse_move;
				_control.KeyDown += _on_key_down;
				_control.KeyUp += _on_key_up;
				_control.Resize += _on_resize;
				_control.Capture = _capture_owner != null;
			}
		}
	}

	private void OnSizeModeChanged()
	{
		if (this.SizeModeChanged != null)
		{
			this.SizeModeChanged(this, EventArgs.Empty);
		}
	}

	private Point CalcLocation(Rectangle rect)
	{
		if (_control == null)
		{
			return new Point(0, 0);
		}
		if ((_sizemode & SizeMode.Auto) != SizeMode.Fixed)
		{
			Size clientSize = _control.ClientSize;
			if (clientSize.Width == 0 || clientSize.Height == 0)
			{
				return rect.Location;
			}
			int x = rect.X;
			if ((_sizemode & SizeMode.AutoX) != SizeMode.Fixed)
			{
				x = 0;
				if (clientSize.Width > rect.Size.Width)
				{
					x = (clientSize.Width - rect.Size.Width) / 2;
				}
			}
			int y = rect.Y;
			if ((_sizemode & SizeMode.AutoY) != SizeMode.Fixed)
			{
				y = 0;
				if (clientSize.Height > rect.Size.Height)
				{
					y = (clientSize.Height - rect.Size.Height) / 2;
				}
			}
			return new Point(x, y);
		}
		return rect.Location;
	}

	public override bool ChangeBounds(Rectangle bounds)
	{
		bounds.Location = CalcLocation(bounds);
		return base.ChangeBounds(bounds);
	}

	public override void HandleEvent(Event e)
	{
		if (e.Positional)
		{
			_mouse_last = (e as MouseEvent).Where;
		}
		base.HandleEvent(e);
	}

	protected virtual bool IsInView(MouseEventArgs e)
	{
		return base.Bounds.Contains(e.X, e.Y);
	}

	private MouseEvent CreateMouseEvent(EventType type, MouseEventArgs e, View v)
	{
		Point point = new Point(e.X - Control.AutoScrollPosition.X, e.Y - Control.AutoScrollPosition.Y);
		MouseButtons buttons = MouseButtons.None;
		if (e.Button == System.Windows.Forms.MouseButtons.Left)
		{
			buttons = MouseButtons.Left;
		}
		else if (e.Button == System.Windows.Forms.MouseButtons.Right)
		{
			buttons = MouseButtons.Right;
		}
		else if (e.Button == System.Windows.Forms.MouseButtons.Middle)
		{
			buttons = MouseButtons.Middle;
		}
		return new MouseEvent(type, point, buttons, e.Clicks > 1, _control_key);
	}

	private void HandleMouseEvent(EventType type, MouseEventArgs e)
	{
		_mouse_in = _capture_owner != null || base.Bounds.Contains(e.X, e.Y);
		if (MouseCapturedBy != null)
		{
			MouseCapturedBy.HandleEvent(CreateMouseEvent(type, e, MouseCapturedBy));
		}
		else if (IsInView(e))
		{
			HandleEvent(CreateMouseEvent(type, e, this));
		}
		View viewUnderMouseFinal = base.ViewUnderMouseFinal;
		Control.Cursor = ((viewUnderMouseFinal != null) ? viewUnderMouseFinal.Cursor : Cursors.Default);
	}

	private void _control_MouseDown(object sender, MouseEventArgs e)
	{
		HandleMouseEvent(EventType.MouseDown, e);
	}

	private void _control_MouseUp(object sender, MouseEventArgs e)
	{
		HandleMouseEvent(EventType.MouseUp, e);
	}

	private void _control_MouseMove(object sender, MouseEventArgs e)
	{
		HandleMouseEvent(EventType.MouseMove, e);
	}

	private void _control_KeyDown(object sender, KeyEventArgs e)
	{
		switch (e.KeyCode)
		{
		case Keys.ControlKey:
			_control_key |= ControlKey.Ctrl;
			break;
		case Keys.Menu:
			_control_key |= ControlKey.Alt;
			break;
		case Keys.ShiftKey:
			_control_key |= ControlKey.Shift;
			break;
		}
		HandleEvent(new KeyEvent(EventType.KeyDown, e.KeyData));
	}

	private void _control_KeyUp(object sender, KeyEventArgs e)
	{
		switch (e.KeyCode)
		{
		case Keys.ControlKey:
			_control_key &= (ControlKey)(-3);
			break;
		case Keys.Menu:
			_control_key &= (ControlKey)(-2);
			break;
		case Keys.ShiftKey:
			_control_key &= (ControlKey)(-5);
			break;
		}
		HandleEvent(new KeyEvent(EventType.KeyUp, e.KeyData));
	}

	private void _control_Resize(object sender, EventArgs e)
	{
		base.Location = CalcLocation(base.Bounds);
	}
}
