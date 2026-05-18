using System;
using System.Drawing;
using System.Windows.Forms;
using Editor.Scripts;

namespace Editor;

public sealed class MineLevelItem : LevelItem, IMouseHandler, IKeybHandler
{
	public override LevelItemType LevelItemType => LevelItemType.Mine;

	public override void GenerateScript(Function func)
	{
		Command v = new CreateMine(base.Rectangle.X + base.Rectangle.Width / 2, base.Rectangle.Y + base.Rectangle.Height / 2);
		func.InsertCommand(new InsertView(v));
	}

	public MineLevelItem()
	{
	}

	public MineLevelItem(Point p)
	{
		Bitmap bitmap = Tools.Texts.GetObject("mine") as Bitmap;
		p.X -= bitmap.Size.Width / 2;
		p.Y -= bitmap.Size.Height / 2;
		base.Rectangle = new Rectangle(p, bitmap.Size);
	}

	public void OnDoubleClick(Control sender, Point where)
	{
		OnProperties(sender, EventArgs.Empty);
	}

	public void OnRightClick(Control sender, Point where)
	{
		ContextMenu contextMenu = new ContextMenu();
		AppendMenuMove(contextMenu, withlayer: true);
		contextMenu.MenuItems.Add("-");
		contextMenu.MenuItems.Add(Tools.Localizer.GetString(73), OnProperties);
		contextMenu.MenuItems[contextMenu.MenuItems.Count - 1].DefaultItem = true;
		contextMenu.Show(sender, where);
	}

	private void OnProperties(object sender, EventArgs args)
	{
		new MinePropertiesForm(this).ShowDialog();
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
