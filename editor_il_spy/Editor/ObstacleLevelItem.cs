using System;
using System.Drawing;
using System.Windows.Forms;
using System.Xml.Serialization;
using Editor.Scripts;

namespace Editor;

public sealed class ObstacleLevelItem : LevelItem, IMouseHandler, IKeybHandler
{
	private bool _fire_through;

	public override LevelItemType LevelItemType => LevelItemType.Obstacle;

	[XmlAttribute]
	public bool FireThrough
	{
		get
		{
			return _fire_through;
		}
		set
		{
			if (_fire_through != value)
			{
				_fire_through = value;
				ItemModified();
			}
		}
	}

	public override void GenerateScript(Function func)
	{
		Command command = new CreateObstacle(base.Rectangle);
		if (FireThrough)
		{
			command = new EnableFireThrough(command, enable: true);
		}
		func.InsertCommand(new InsertView(command));
	}

	public override void CopyFrom(LevelItem item)
	{
		Lock();
		base.CopyFrom(item);
		if (item is ObstacleLevelItem obstacleLevelItem)
		{
			FireThrough = obstacleLevelItem.FireThrough;
		}
		Unlock();
	}

	public ObstacleLevelItem()
	{
	}

	public ObstacleLevelItem(Rectangle rect)
		: base(rect)
	{
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
		contextMenu.MenuItems.Add(Tools.Localizer.GetString(90), OnFireThrough);
		contextMenu.MenuItems[contextMenu.MenuItems.Count - 1].Checked = FireThrough;
		contextMenu.MenuItems.Add(Tools.Localizer.GetString(73), OnProperties);
		contextMenu.MenuItems[contextMenu.MenuItems.Count - 1].DefaultItem = true;
		contextMenu.Show(sender, where);
	}

	private void OnFireThrough(object sender, EventArgs args)
	{
		FireThrough = !FireThrough;
	}

	private void OnProperties(object sender, EventArgs args)
	{
		new ObstaclePropertiesForm(this).ShowDialog();
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
}
