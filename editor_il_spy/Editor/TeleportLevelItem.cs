using System;
using System.Collections;
using System.Drawing;
using System.Windows.Forms;
using System.Xml.Serialization;
using Editor.Scripts;

namespace Editor;

public sealed class TeleportLevelItem : LevelItem, IHasGuid, IMouseHandler, IKeybHandler, IHasTraceArea
{
	public static readonly Size PillowSize = new Size(51, 37);

	private ArrayList _targets = new ArrayList();

	private string _guid = "";

	private int _script_id = -1;

	public override LevelItemType LevelItemType => LevelItemType.Teleport;

	[XmlAttribute]
	public string Guid
	{
		get
		{
			return _guid;
		}
		set
		{
			if (_guid != value)
			{
				_guid = value;
				if (base.Level != null)
				{
					base.Level.OnItemModified(this);
				}
			}
		}
	}

	[XmlArrayItem("Target", typeof(string))]
	public ArrayList Targets
	{
		get
		{
			return _targets;
		}
		set
		{
			if (_targets != value)
			{
				_targets = value;
				if (base.Level != null)
				{
					base.Level.OnItemModified(this);
				}
			}
		}
	}

	[XmlIgnore]
	public int TraceAreaID
	{
		get
		{
			return _script_id;
		}
		set
		{
			_script_id = value;
		}
	}

	public bool IsTraceAreaEnabled => IsSource;

	public Rectangle TraceAreaRectangle => new Rectangle(base.Rectangle.X - 5, base.Rectangle.Y - 5, base.Rectangle.Width + 10, base.Rectangle.Height + 10);

	public Point TeleportTo
	{
		get
		{
			Rectangle traceAreaRectangle = TraceAreaRectangle;
			return new Point(traceAreaRectangle.X + (traceAreaRectangle.Width - PillowSize.Width) / 2, traceAreaRectangle.Y + (traceAreaRectangle.Height - PillowSize.Height) / 2);
		}
	}

	public TraceAreaFlags TraceAreaFlags => (TraceAreaFlags)14;

	public bool IsSource => _targets.Count > 0;

	public TeleportLevelItem()
	{
	}

	public TeleportLevelItem(Point p)
	{
		base.Rectangle = new Rectangle(p.X, p.Y, 70, 60);
		AssignNewGuid();
	}

	public override void CopyFrom(LevelItem item)
	{
		Lock();
		base.CopyFrom(item);
		if (item is TeleportLevelItem teleportLevelItem)
		{
			Targets = new ArrayList(teleportLevelItem.Targets);
			Guid = teleportLevelItem.Guid;
		}
		Unlock();
	}

	public bool ContainsTarget(string guid)
	{
		foreach (string target in Targets)
		{
			if (target == guid)
			{
				return true;
			}
		}
		return false;
	}

	public bool AddTarget(TeleportLevelItem item)
	{
		if (ContainsTarget(item.Guid))
		{
			return false;
		}
		_targets.Add(item.Guid);
		if (base.Level != null)
		{
			base.Level.OnItemModified(this);
		}
		return true;
	}

	public bool RemoveTarget(TeleportLevelItem item)
	{
		int count = Targets.Count;
		Targets.Remove(item.Guid);
		if (Targets.Count < count)
		{
			if (base.Level != null)
			{
				base.Level.OnItemModified(this);
			}
			return true;
		}
		return false;
	}

	public void AssignNewGuid()
	{
		Guid = Tools.GenerateGuid();
	}

	private void OnProperties(object sender, EventArgs args)
	{
		new TeleportPropertiesForm(this).ShowDialog();
	}

	public void OnDoubleClick(Control sender, Point where)
	{
		OnProperties(sender, EventArgs.Empty);
	}

	public void OnRightClick(Control sender, Point where)
	{
		ContextMenu contextMenu = new ContextMenu();
		AppendMenuMove(contextMenu, withlayer: false);
		contextMenu.MenuItems.Add("-");
		contextMenu.MenuItems.Add(Tools.Localizer.GetString(73), OnProperties);
		contextMenu.MenuItems[contextMenu.MenuItems.Count - 1].DefaultItem = true;
		contextMenu.Show(sender, where);
	}

	public override bool OnKeyDown(Control sender, Keys key)
	{
		if (!base.OnKeyDown(sender, key))
		{
			if (key == (Keys.Return | Keys.Alt))
			{
				OnProperties(sender, EventArgs.Empty);
				return true;
			}
			return false;
		}
		return true;
	}

	public void GenerateOnEnterTrace(string player, Function func)
	{
		if (_targets.Count != 0)
		{
			if (_targets.Count > 1)
			{
				func.InsertCommand(new TeleportFrom(TraceAreaID, new GetLocalVar(player)));
				return;
			}
			TeleportLevelItem teleportLevelItem = base.Level.GetItemByGuid((string)_targets[0]) as TeleportLevelItem;
			func.InsertCommand(new MakeTeleport(new GetLocalVar(player), teleportLevelItem.TeleportTo.X, teleportLevelItem.TeleportTo.Y));
		}
	}

	public void GenerateOnLeaveTrace(string player, Function func)
	{
	}
}
