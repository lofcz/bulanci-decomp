using System;
using System.Drawing;
using System.Windows.Forms;
using System.Xml.Serialization;
using Editor.ResourceItems;
using Editor.Scripts;
using Editor.Temp;

namespace Editor;

public sealed class ImageLevelItem : LevelItem, IMouseHandler, IKeybHandler
{
	private string _resource_name = "";

	private int _ground;

	public override LevelItemType LevelItemType => LevelItemType.Image;

	[XmlAttribute("Resource")]
	public string BitmapName
	{
		get
		{
			return _resource_name;
		}
		set
		{
			if (_resource_name != value)
			{
				_resource_name = value;
				ItemModified();
			}
		}
	}

	[XmlAttribute]
	public int Ground
	{
		get
		{
			if (base.Layer != LayerType.Layer1)
			{
				return 0;
			}
			return _ground;
		}
		set
		{
			if (_ground != value)
			{
				_ground = value;
				ItemModified();
			}
		}
	}

	public BitmapResourceItem BitmapResource => base.Level.Resources[BitmapName] as BitmapResourceItem;

	public override void GenerateScript(Function func)
	{
		Command command = new CreateImage(base.Rectangle.X, base.Rectangle.Y, BitmapResource);
		if (base.Layer == LayerType.Layer1)
		{
			command = new SetOrderAxis(command, Ground);
		}
		func.InsertCommand(new InsertView(command));
	}

	public override void CopyFrom(LevelItem item)
	{
		Lock();
		base.CopyFrom(item);
		if (item is ImageLevelItem imageLevelItem)
		{
			BitmapName = imageLevelItem.BitmapName;
			Ground = imageLevelItem.Ground;
		}
		Unlock();
	}

	public ImageLevelItem()
	{
	}

	public ImageLevelItem(Point p, BitmapResourceItem item)
	{
		base.Rectangle = new Rectangle(p, item.Bitmap.Size);
		BitmapName = item.Name;
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
		new ImagePropertiesForm(this).ShowDialog();
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
