using System;
using System.Drawing;
using System.Windows.Forms;

namespace Editor.Temp;

public class ItemView : View
{
	private DragPoints _drag;

	private bool _resize;

	public LevelItem LevelItem
	{
		get
		{
			return base.Tag as LevelItem;
		}
		set
		{
			base.Tag = value;
		}
	}

	protected bool ResizeEnabled
	{
		get
		{
			return _resize;
		}
		set
		{
			_resize = value;
		}
	}

	public LevelControl Control
	{
		get
		{
			if (base.Parent == null)
			{
				return null;
			}
			return (base.Parent as LevelView).Control;
		}
	}

	public virtual void Update()
	{
		Rectangle rectangle = LevelItem.Rectangle;
		rectangle.Offset(2, 2);
		base.Bounds = rectangle;
	}

	protected override void DragLocationLimit(ref Rectangle rect)
	{
		rect.X = Math.Max(2, Math.Min(base.Parent.Size.Width - rect.Width - 2, rect.X));
		rect.Y = Math.Max(2, Math.Min(base.Parent.Size.Height - rect.Height - 2, rect.Y));
	}

	protected override void OnDraggingChanged()
	{
		base.OnDraggingChanged();
		if (!base.Dragging)
		{
			Rectangle bounds = base.Bounds;
			bounds.Offset(-2, -2);
			LevelItem.Rectangle = bounds;
		}
	}

	public override bool SetStateFlag(ViewState state, bool on)
	{
		bool selected = base.Selected;
		bool result = base.SetStateFlag(state, on);
		if (selected != base.Selected && base.Parent != null)
		{
			if (!selected)
			{
				if (ResizeEnabled)
				{
					_drag = new DragPoints(this);
				}
				base.Cursor = Cursors.SizeAll;
				Control.OnLevelItemSelected(LevelItem);
			}
			else
			{
				if (_drag != null)
				{
					_drag.Clear();
					_drag = null;
				}
				base.Cursor = Cursors.Default;
			}
		}
		return result;
	}

	public override void HandleEvent(Event e)
	{
		base.HandleEvent(e);
		MouseEvent mouseEvent = e as MouseEvent;
		if (e.What == EventType.MouseDown)
		{
			if (LevelItem is IMouseHandler mouseHandler && mouseEvent.DoubleClick)
			{
				SendBroadcast(base.Parent, 4, this);
				mouseHandler.OnDoubleClick(Control, mouseEvent.Where);
				e.Clear(this);
			}
		}
		else if (e.What == EventType.MouseUp && base.Tag is IMouseHandler mouseHandler2 && mouseEvent.Buttons == MouseButtons.Right)
		{
			SendBroadcast(base.Parent, 4, this);
			mouseHandler2.OnRightClick(Control, mouseEvent.Where);
			e.Clear(this);
		}
	}

	public ItemView(LevelItem item)
	{
		base.Selectable = true;
		base.EventMask = (EventType)2055;
		base.DragMove = true;
		base.DragGroup = 1;
		LevelItem = item;
	}
}
