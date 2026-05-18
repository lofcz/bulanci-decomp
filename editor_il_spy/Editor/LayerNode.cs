using System;
using System.Drawing;
using System.Windows.Forms;
using Editor.Temp;

namespace Editor;

public class LayerNode : IMouseHandler
{
	private LevelControl _control;

	private LayerType _layer;

	public LevelControl Control => _control;

	public LayerType Layer => _layer;

	public LayerNode(LevelControl ctrl, LayerType layer)
	{
		_control = ctrl;
		_layer = layer;
	}

	public void OnDoubleClick(Control sender, Point where)
	{
		Control.LayerInsert = Layer;
	}

	public void OnRightClick(Control sender, Point where)
	{
		ContextMenu contextMenu = new ContextMenu();
		contextMenu.MenuItems.Add(Tools.Localizer.GetString(74), OnActive);
		contextMenu.MenuItems.Add(Tools.Localizer.GetString(75), OnVisible);
		contextMenu.MenuItems[0].Checked = Control.LayerInsert == Layer;
		contextMenu.MenuItems[1].Checked = Control.LayerVisible(Layer);
		contextMenu.MenuItems[0].DefaultItem = true;
		contextMenu.Show(sender, where);
	}

	private void OnActive(object sender, EventArgs args)
	{
		Control.LayerInsert = Layer;
	}

	private void OnVisible(object sender, EventArgs args)
	{
		Control.ToggleLayerVisible(Layer);
	}
}
