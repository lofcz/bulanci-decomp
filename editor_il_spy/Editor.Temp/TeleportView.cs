using System.Drawing;

namespace Editor.Temp;

public class TeleportView : ItemView
{
	private Brush _brush;

	private Pen _pen;

	private bool _as_target;

	public bool ShowAsTarget
	{
		get
		{
			return _as_target;
		}
		set
		{
			if (_as_target != value)
			{
				_as_target = value;
				UpdateBrush();
				OnFaceChanged();
			}
		}
	}

	public new TeleportLevelItem LevelItem
	{
		get
		{
			return base.LevelItem as TeleportLevelItem;
		}
		set
		{
			base.LevelItem = value;
		}
	}

	private void UpdateBrush()
	{
		_brush = new SolidBrush(Color.FromArgb(84, ShowAsTarget ? Color.LightBlue : Color.Blue));
	}

	protected override void OnPaint(Graphics g)
	{
		g.FillRectangle(_brush, base.ClientBounds);
		if (base.Selected)
		{
			g.DrawRectangle(_pen, 0, 0, base.Size.Width - 1, base.Size.Height - 1);
		}
	}

	public override void Update()
	{
		base.Update();
		SendBroadcast(base.Parent, 100, base.Selected ? LevelItem : null);
	}

	public override void HandleEvent(Event e)
	{
		base.HandleEvent(e);
		if (e is BroadcastEvent { Command: 100 } broadcastEvent && broadcastEvent.Info != LevelItem)
		{
			ShowAsTarget = (broadcastEvent.Info as TeleportLevelItem)?.ContainsTarget(LevelItem.Guid) ?? false;
		}
	}

	public override bool SetStateFlag(ViewState state, bool on)
	{
		bool selected = base.Selected;
		bool result = base.SetStateFlag(state, on);
		if (selected != base.Selected && base.Parent != null)
		{
			SendBroadcast(base.Parent, 100, base.Selected ? LevelItem : null);
		}
		return result;
	}

	public TeleportView(LevelItem item)
		: base(item)
	{
		_pen = new Pen(Color.White);
		UpdateBrush();
	}
}
