using System;
using System.Collections;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Windows.Forms;

namespace Editor.Temp;

public class View
{
	private Rectangle _bounds;

	private Rectangle _backup_bounds = Rectangle.Empty;

	private ViewState _state = (ViewState)33;

	private ViewOptions _options;

	private EventType _events;

	private GrowMode _grow;

	private View _parent;

	private View _focused;

	private View _under_mouse;

	private ViewCollection _childs;

	private SelectedViewCollection _selected;

	private object _tag;

	private Cursor _cursor = Cursors.Default;

	private int _drag_group;

	private static DragCtx _drag_ctx;

	public object Tag
	{
		get
		{
			return _tag;
		}
		set
		{
			_tag = value;
		}
	}

	public int ZOrder
	{
		get
		{
			if (Parent == null)
			{
				return -1;
			}
			return Parent.Childs.IndexOf(this);
		}
		set
		{
			if (Parent != null && Parent.Childs.MoveTo(this, value))
			{
				Parent.OnFaceChanged();
			}
		}
	}

	public Cursor Cursor
	{
		get
		{
			return _cursor;
		}
		set
		{
			_cursor = value;
		}
	}

	public GrowMode GrowMode
	{
		get
		{
			return _grow;
		}
		set
		{
			_grow = value;
		}
	}

	public View Parent => _parent;

	public View Current => _focused;

	public View ViewUnderMouse => _under_mouse;

	public View ViewUnderMouseFinal
	{
		get
		{
			View view = (UnderMouse ? this : null);
			if (view != null && ViewUnderMouse != null)
			{
				view = ViewUnderMouse.ViewUnderMouseFinal;
			}
			return view;
		}
	}

	public virtual bool UnderMouse
	{
		get
		{
			if (Parent == null)
			{
				return false;
			}
			return Parent.ViewUnderMouse == this;
		}
	}

	public ViewCollection Childs
	{
		get
		{
			if (_childs == null)
			{
				_childs = new ViewCollection(this);
			}
			return _childs;
		}
	}

	public SelectedViewCollection SelectedChilds
	{
		get
		{
			if (HasChilds && _selected == null)
			{
				_selected = new SelectedViewCollection(this);
			}
			return _selected;
		}
	}

	public bool MouseCaptured
	{
		get
		{
			return MouseCapturedBy == this;
		}
		set
		{
			View mouseCapturedBy = MouseCapturedBy;
			if (mouseCapturedBy == this && !value)
			{
				MouseCapturedBy = null;
			}
			else if (mouseCapturedBy == null && value)
			{
				MouseCapturedBy = this;
			}
		}
	}

	protected virtual View MouseCapturedBy
	{
		get
		{
			if (Parent == null)
			{
				return null;
			}
			return Parent.MouseCapturedBy;
		}
		set
		{
			if (Parent != null)
			{
				Parent.MouseCapturedBy = value;
			}
		}
	}

	public virtual Point MouseLast
	{
		get
		{
			if (Parent == null)
			{
				return new Point(int.MinValue, int.MinValue);
			}
			return Parent.MouseLast;
		}
	}

	protected EventType EventMask
	{
		get
		{
			return _events;
		}
		set
		{
			_events = value;
		}
	}

	public ViewOptions Options
	{
		get
		{
			return _options;
		}
		set
		{
			_options = value;
		}
	}

	public bool HasChilds => _childs != null;

	public bool Chained => GetStateFlag(ViewState.Chained);

	public bool Dragging => GetStateFlag(ViewState.Dragging);

	public bool Enabled
	{
		get
		{
			return GetStateFlag(ViewState.Enabled);
		}
		set
		{
			SetStateFlag(ViewState.Enabled, value);
		}
	}

	public bool Disabled
	{
		get
		{
			return !Enabled;
		}
		set
		{
			Enabled = !value;
		}
	}

	public bool Visible
	{
		get
		{
			return GetStateFlag(ViewState.Visible);
		}
		set
		{
			SetStateFlag(ViewState.Visible, value);
		}
	}

	public bool Selected
	{
		get
		{
			return GetStateFlag(ViewState.Selected);
		}
		set
		{
			SetStateFlag((ViewState)66, value);
		}
	}

	public bool Focused
	{
		get
		{
			return GetStateFlag(ViewState.Focused);
		}
		set
		{
			SetStateFlag(ViewState.Focused, value);
		}
	}

	public bool FirstClick
	{
		get
		{
			return GetOptionFlag(ViewOptions.FirstClick);
		}
		set
		{
			SetOptionFlag(ViewOptions.FirstClick, value);
		}
	}

	public bool Selectable
	{
		get
		{
			return GetOptionFlag(ViewOptions.Selectable);
		}
		set
		{
			SetOptionFlag(ViewOptions.Selectable, value);
		}
	}

	public bool TopSelect
	{
		get
		{
			return GetOptionFlag(ViewOptions.TopSelect);
		}
		set
		{
			SetOptionFlag(ViewOptions.TopSelect, value);
		}
	}

	public bool MultiSelect
	{
		get
		{
			return GetOptionFlag(ViewOptions.MultiSelect);
		}
		set
		{
			SetOptionFlag(ViewOptions.MultiSelect, value);
		}
	}

	public bool DragMove
	{
		get
		{
			return GetOptionFlag(ViewOptions.DragMove);
		}
		set
		{
			SetOptionFlag(ViewOptions.DragMove, value);
		}
	}

	public int DragGroup
	{
		get
		{
			return _drag_group;
		}
		set
		{
			_drag_group = value;
		}
	}

	public Rectangle Bounds
	{
		get
		{
			return _bounds;
		}
		set
		{
			SizeLimits(out var min, out var max);
			Rectangle bounds = value;
			bounds.Width = Math.Min(max.Width, Math.Max(min.Width, bounds.Width));
			bounds.Height = Math.Min(max.Height, Math.Max(min.Height, bounds.Height));
			ChangeBounds(bounds);
		}
	}

	public Rectangle BoundsBackup => _backup_bounds;

	public Rectangle ClientBounds => new Rectangle(0, 0, Bounds.Width, Bounds.Height);

	public Size Size
	{
		get
		{
			return Bounds.Size;
		}
		set
		{
			Bounds = new Rectangle(Location, value);
		}
	}

	public Point Location
	{
		get
		{
			return Bounds.Location;
		}
		set
		{
			Bounds = new Rectangle(value, Size);
		}
	}

	public event EventHandler FaceChanged;

	public event EventHandler BoundsChanged;

	public event EventHandler VisibleChanged;

	public event EventHandler SelectedChanged;

	public event EventHandler FocusedChanged;

	public event EventHandler EnabledChanged;

	public event EventHandler DraggingChanged;

	public void Paint(Graphics g)
	{
		Paint(g, ClientBounds);
	}

	public void Paint(Graphics g, Rectangle rectClient)
	{
		if (!Visible)
		{
			return;
		}
		rectClient.Offset(Bounds.Left, Bounds.Top);
		if (rectClient.IntersectsWith(Bounds))
		{
			rectClient.Intersect(Bounds);
			if (g.Clip.IsVisible(rectClient))
			{
				GraphicsState gstate = g.Save();
				g.SetClip(rectClient, CombineMode.Intersect);
				g.TranslateTransform(Bounds.Left, Bounds.Top);
				OnPaint(g);
				g.Restore(gstate);
			}
		}
	}

	protected virtual void OnPaint(Graphics g)
	{
		if (HasChilds)
		{
			int count = Childs.Count;
			while (count-- > 0)
			{
				Childs[count].Paint(g);
			}
		}
	}

	protected virtual void OnFaceChanged()
	{
		if (this.FaceChanged != null)
		{
			this.FaceChanged(this, EventArgs.Empty);
		}
		if (Parent != null)
		{
			Parent.OnFaceChanged();
		}
	}

	protected virtual void OnBoundsChanged()
	{
		OnFaceChanged();
		if (this.BoundsChanged != null)
		{
			this.BoundsChanged(this, EventArgs.Empty);
		}
	}

	protected virtual void OnVisibleChanged()
	{
		if (this.VisibleChanged != null)
		{
			this.VisibleChanged(this, EventArgs.Empty);
		}
	}

	protected virtual void OnDraggingChanged()
	{
		if (this.DraggingChanged != null)
		{
			this.DraggingChanged(this, EventArgs.Empty);
		}
	}

	protected virtual void OnSelectedChanged()
	{
		if (this.SelectedChanged != null)
		{
			this.SelectedChanged(this, EventArgs.Empty);
		}
	}

	protected virtual void OnFocusedChanged()
	{
		if (this.FocusedChanged != null)
		{
			this.FocusedChanged(this, EventArgs.Empty);
		}
	}

	protected virtual void OnEnabledChanged()
	{
		if (this.EnabledChanged != null)
		{
			this.EnabledChanged(this, EventArgs.Empty);
		}
	}

	private bool GetOptionFlag(ViewOptions option)
	{
		return (_options & option) == option;
	}

	private void SetOptionFlag(ViewOptions option, bool on)
	{
		if (on)
		{
			_options |= option;
		}
		else
		{
			_options &= ~option;
		}
	}

	private bool GetStateFlag(ViewState state)
	{
		return (_state & state) == state;
	}

	private static bool IsStateChange(ViewState org, ViewState state, ViewState flag, bool on)
	{
		state &= flag;
		if (state == (ViewState)0)
		{
			return false;
		}
		ViewState viewState = org;
		viewState = ((!on) ? (viewState & ~state) : (viewState | state));
		return org != viewState;
	}

	public virtual bool SetStateFlag(ViewState state, bool on)
	{
		if (Parent == null || !Parent.MultiSelect)
		{
			state &= (ViewState)(-65);
		}
		if (on && IsStateChange(_state, state, (ViewState)18, on))
		{
			state |= (ViewState)18;
		}
		if (!on && Focused && IsStateChange(_state, state, (ViewState)33, on))
		{
			state |= ViewState.Focused;
		}
		if (!on && IsStateChange(_state, state, (ViewState)9, on))
		{
			state |= (ViewState)18;
		}
		if (!Selectable || Disabled || !Visible)
		{
			state &= (ViewState)(-19);
		}
		if (on && IsStateChange(_state, state, ViewState.Selected, on) && Parent != null)
		{
			if ((state & ViewState.MultiSelecting) == 0)
			{
				if (Parent.Current != null && Parent.Current != this && !Parent.Current.SetStateFlag(ViewState.Selected, on: false))
				{
					state &= (ViewState)(-19);
				}
				if (IsStateChange(_state, state, ViewState.Selected, on))
				{
					SendBroadcast(Parent, 4, null);
				}
			}
			else if (Parent.Current != null && Parent.Current != this && !Parent.Current.SetStateFlag(ViewState.Focused, on: false))
			{
				state &= (ViewState)(-17);
			}
		}
		if (!on && IsStateChange(_state, state, (ViewState)18, on))
		{
			state |= ViewState.Focused;
			if (!ReleaseFocus())
			{
				state &= (ViewState)(-17);
			}
			else if (_focused != null && !_focused.SetStateFlag(ViewState.Focused, on: false))
			{
				state &= (ViewState)(-17);
			}
		}
		if (state == (ViewState)0)
		{
			return false;
		}
		ViewState state2 = _state;
		state2 = ((!on) ? (state2 & ~state) : (state2 | state));
		if (_state == state2)
		{
			return false;
		}
		ViewState state3 = _state;
		_state = state2;
		if (HasChilds)
		{
			if (IsStateChange(state3, state, ViewState.Dragging, on))
			{
				foreach (View child in Childs)
				{
					child.SetStateFlag(ViewState.Dragging, on);
				}
			}
			if (IsStateChange(state3, state, ViewState.Chained, on))
			{
				foreach (View child2 in Childs)
				{
					child2.SetStateFlag(ViewState.Chained, on);
				}
			}
		}
		if (Parent != null)
		{
			if (IsStateChange(state3, state, ViewState.Selected, on))
			{
				if (on)
				{
					Parent.SelectedChilds.Add(this);
				}
				else
				{
					Parent.SelectedChilds.Remove(this);
				}
			}
			if (IsStateChange(state3, state, ViewState.Focused, on))
			{
				if (on)
				{
					Parent._focused = this;
				}
				else
				{
					Parent._focused = null;
				}
			}
			if (IsStateChange(state3, state, ViewState.Visible, on) && (on || Parent.ViewUnderMouse == this))
			{
				Parent.ResetUnderMouse();
			}
		}
		if (on && IsStateChange(state3, state, ViewState.Dragging, on))
		{
			_backup_bounds = _bounds;
		}
		if (IsStateChange(state3, state, ViewState.Enabled, on))
		{
			OnEnabledChanged();
		}
		if (IsStateChange(state3, state, ViewState.Visible, on))
		{
			OnVisibleChanged();
		}
		if (IsStateChange(state3, state, ViewState.Selected, on))
		{
			OnSelectedChanged();
		}
		if (IsStateChange(state3, state, ViewState.Focused, on))
		{
			OnFocusedChanged();
		}
		if (IsStateChange(state3, state, ViewState.Dragging, on))
		{
			OnDraggingChanged();
		}
		if (IsStateChange(state3, state, ViewState.FaceChanges, on))
		{
			OnFaceChanged();
		}
		else if (Parent != null && IsStateChange(state3, state, ViewState.Visible, on))
		{
			Parent.OnFaceChanged();
		}
		return true;
	}

	protected virtual bool ReleaseFocus()
	{
		return true;
	}

	internal void ViewInserted(View view)
	{
		view._parent = this;
		if (Chained)
		{
			view.SetStateFlag(ViewState.Chained, on: true);
		}
		if (view.Selected)
		{
			SelectedChilds.Add(view);
		}
		if (view.Visible)
		{
			ResetUnderMouse();
		}
		view.OnInserted();
		OnFaceChanged();
	}

	internal void ViewRemoved(View view)
	{
		view.OnRemoved();
		if (ViewUnderMouse == view)
		{
			ResetUnderMouse();
		}
		view.MouseCaptured = false;
		if (view.Selected)
		{
			SelectedChilds.Remove(view);
		}
		if (Chained)
		{
			view.SetStateFlag(ViewState.Chained, on: false);
		}
		view._parent = null;
		OnFaceChanged();
	}

	protected virtual void OnInserted()
	{
	}

	protected virtual void OnRemoved()
	{
	}

	public bool Focus(bool multiselecting)
	{
		ViewState viewState = ViewState.Focused;
		if (multiselecting)
		{
			viewState |= ViewState.MultiSelecting;
		}
		return SetStateFlag(viewState, on: true);
	}

	private static void HandleEventInt(View view, Event e)
	{
		if (view == null || (view.Disabled && (e.Positional || e.Focused)) || (e.What & view.EventMask) == 0)
		{
			return;
		}
		if (e.What == EventType.Broadcast)
		{
			BroadcastEvent broadcastEvent = e as BroadcastEvent;
			if (broadcastEvent.Parent != null && view.Parent != broadcastEvent.Parent)
			{
				return;
			}
		}
		view.HandleEvent(e);
	}

	protected virtual void OnMouseEnter()
	{
	}

	protected virtual void OnMouseLeave()
	{
	}

	private void SetUnderMouse(View v)
	{
		if (_under_mouse == v)
		{
			return;
		}
		if (_under_mouse != null)
		{
			if (_under_mouse._under_mouse != null)
			{
				_under_mouse.SetUnderMouse(null);
			}
			_under_mouse.OnMouseLeave();
			_under_mouse.SetUnderMouse(null);
		}
		_under_mouse = v;
		if (_under_mouse != null)
		{
			_under_mouse.OnMouseEnter();
		}
	}

	protected void ResetUnderMouse()
	{
		if (!HasChilds)
		{
			return;
		}
		Point mouseLast = MouseLast;
		if (mouseLast == new Point(int.MinValue, int.MinValue))
		{
			return;
		}
		foreach (View child in Childs)
		{
			if (child.ContainsMouse(mouseLast))
			{
				if (ViewUnderMouse != child)
				{
					SetUnderMouse(child);
					child.ResetUnderMouse();
				}
				break;
			}
		}
	}

	public virtual void HandleEvent(Event e)
	{
		MouseEvent mouseEvent = e as MouseEvent;
		if (mouseEvent != null && HasChilds)
		{
			foreach (View child in Childs)
			{
				if (child.ContainsMouse(mouseEvent.Where))
				{
					SetUnderMouse(child);
					break;
				}
			}
		}
		if (e.What == EventType.MouseDown)
		{
			if (!mouseEvent.DoubleClick)
			{
				if (Selected && DragMove && mouseEvent.Buttons == MouseButtons.Left)
				{
					DragStart((e as MouseEvent).Where);
					e.Clear(this);
				}
				if (!Selected && Enabled && Selectable && (!Focus((e as MouseEvent).Ctrl) || !FirstClick))
				{
					e.Clear(this);
				}
			}
		}
		else if (e.What == EventType.MouseMove)
		{
			if (_drag_ctx != null && _drag_ctx.Source == this)
			{
				DragContinue((e as MouseEvent).Where);
				e.Clear(this);
			}
		}
		else if (e.What == EventType.MouseUp && _drag_ctx != null && _drag_ctx.Source == this)
		{
			DragFinish((e as MouseEvent).Where);
			e.Clear(this);
		}
		if (e.What == EventType.Broadcast && Selected && DragMove)
		{
			HandleDragging(e);
		}
		if (e.What == EventType.Broadcast)
		{
			BroadcastEvent broadcastEvent = e as BroadcastEvent;
			if (broadcastEvent.Parent == Parent && broadcastEvent.Command == 4 && broadcastEvent.Sender != this)
			{
				Selected = false;
			}
		}
		if (e.Focused)
		{
			HandleEventInt(_focused, e);
		}
		else if (e.Positional)
		{
			HandleEventInt(ViewUnderMouse, e);
		}
		else
		{
			if (e.What == EventType.Nothing || !HasChilds)
			{
				return;
			}
			foreach (View item in new ArrayList(Childs))
			{
				HandleEventInt(item, e);
			}
		}
	}

	public void DragMoveBy(Point change)
	{
		Rectangle rect = BoundsBackup;
		rect.Offset(change);
		DragLocationLimit(ref rect);
		ChangeBounds(rect);
	}

	protected bool HandleDragging(Event e)
	{
		if (_drag_ctx != null && _drag_ctx.Source.Parent == Parent && _drag_ctx.DragGroup == DragGroup)
		{
			if (!(e is BroadcastEvent broadcastEvent))
			{
				return false;
			}
			switch ((ViewCommands)broadcastEvent.Command)
			{
			case ViewCommands.DragStart:
				SetStateFlag(ViewState.Dragging, on: true);
				return true;
			case ViewCommands.DragContinue:
				DragMoveBy(_drag_ctx.Offset);
				return true;
			case ViewCommands.DragFinish:
				SetStateFlag(ViewState.Dragging, on: false);
				return true;
			}
		}
		return false;
	}

	public Point MakeGlobal(Point p)
	{
		for (View view = this; view != null; view = view.Parent)
		{
			p.X += view.Bounds.X;
			p.Y += view.Bounds.Y;
		}
		return p;
	}

	public Point MakeLocal(Point p)
	{
		for (View view = this; view != null; view = view.Parent)
		{
			p.X -= view.Bounds.X;
			p.Y -= view.Bounds.Y;
		}
		return p;
	}

	public virtual bool ContainsMouse(Point where)
	{
		if (Visible)
		{
			return ClientBounds.Contains(MakeLocal(where));
		}
		return false;
	}

	private int Grow(int i, int size, int delta)
	{
		if ((_grow & GrowMode.Rel) == 0)
		{
			return i + delta;
		}
		return (i * size + (size - delta) / 2) / (size - delta);
	}

	protected virtual void DragLocationLimit(ref Rectangle rect)
	{
	}

	protected virtual Rectangle CalcBounds(Point change)
	{
		Point point = new Point(Bounds.Left, Bounds.Top);
		Point point2 = new Point(Bounds.Right, Bounds.Bottom);
		int width = Parent.Size.Width;
		if ((_grow & GrowMode.LoX) != GrowMode.None)
		{
			point.X = Grow(point.X, width, change.X);
		}
		if ((_grow & GrowMode.HiX) != GrowMode.None)
		{
			point2.X = Grow(point2.X, width, change.X);
		}
		width = Parent.Size.Height;
		if ((_grow & GrowMode.LoY) != GrowMode.None)
		{
			point.Y = Grow(point.Y, width, change.Y);
		}
		if ((_grow & GrowMode.HiY) != GrowMode.None)
		{
			point2.Y = Grow(point2.Y, width, change.Y);
		}
		SizeLimits(out var min, out var max);
		point2.X = point.X + Math.Min(max.Width, Math.Max(min.Width, point2.X - point.X));
		point2.Y = point.Y + Math.Min(max.Height, Math.Max(min.Height, point2.Y - point.Y));
		return new Rectangle(point.X, point.Y, point2.X - point.X, point2.Y - point.Y);
	}

	public virtual bool ChangeBounds(Rectangle bounds)
	{
		if (_bounds == bounds)
		{
			return false;
		}
		Rectangle bounds2 = _bounds;
		_bounds = bounds;
		OnBoundsChanged();
		if (HasChilds && Childs.Count > 0)
		{
			Point change = new Point(Size.Width - bounds2.Size.Width, Size.Height - bounds2.Size.Height);
			if (change.X != 0 || change.Y != 0)
			{
				foreach (View child in Childs)
				{
					child.ChangeBounds(child.CalcBounds(change));
				}
			}
		}
		return true;
	}

	protected virtual void SizeLimits(out Size min, out Size max)
	{
		min = new Size(0, 0);
		max = new Size(int.MaxValue, int.MaxValue);
	}

	public void SendBroadcast(View view, int cmd, object info)
	{
		if (view != null)
		{
			Event obj = new BroadcastEvent(view, cmd, info);
			obj.Sender = this;
			view.HandleEvent(obj);
		}
	}

	public void SendBroadcastAll(View view, int cmd, object info)
	{
		if (view != null)
		{
			Event obj = new BroadcastEvent(cmd, info);
			obj.Sender = this;
			view.HandleEvent(obj);
		}
	}

	protected void DragStart(Point mouse)
	{
		if (Parent != null && _drag_ctx == null)
		{
			MouseCaptured = true;
			_drag_ctx = new DragCtx(this, mouse);
			SendBroadcast(Parent, 1, _drag_ctx);
		}
	}

	protected void DragContinue(Point mouse)
	{
		if (_drag_ctx != null && _drag_ctx.Source == this)
		{
			_drag_ctx.MouseNow = mouse;
			SendBroadcast(Parent, 2, _drag_ctx);
		}
	}

	protected void DragFinish(Point mouse)
	{
		if (_drag_ctx != null && _drag_ctx.Source == this)
		{
			_drag_ctx.MouseNow = mouse;
			SendBroadcast(Parent, 3, _drag_ctx);
			_drag_ctx = null;
			MouseCaptured = false;
		}
	}

	internal void ChildsSorted()
	{
		OnFaceChanged();
	}

	public View()
	{
		_bounds = Rectangle.Empty;
	}

	public View(Rectangle rect)
	{
		_bounds = rect;
	}
}
