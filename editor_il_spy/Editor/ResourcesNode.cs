using System;
using System.Drawing;
using System.Windows.Forms;

namespace Editor;

public class ResourcesNode : IMouseHandler
{
	private Level _level;

	public Level Level => _level;

	public ResourcesNode(Level level)
	{
		_level = level;
	}

	public void OnDoubleClick(Control sender, Point where)
	{
	}

	public void OnRightClick(Control sender, Point where)
	{
		ContextMenu contextMenu = new ContextMenu();
		contextMenu.MenuItems.Add(Tools.Localizer.GetString(102), OnInsert);
		contextMenu.MenuItems.Add(Tools.Localizer.GetString(103), OnRemove);
		contextMenu.Show(sender, where);
	}

	private void OnInsert(object sender, EventArgs args)
	{
		new NewBitmapWizard(Level).ShowDialog();
	}

	private void OnRemove(object sender, EventArgs args)
	{
		int count = Level.Resources.Count;
		while (count-- > 0)
		{
			if (!Level.Resources[count].IsUsed)
			{
				Level.Resources.RemoveAt(count);
			}
		}
	}
}
