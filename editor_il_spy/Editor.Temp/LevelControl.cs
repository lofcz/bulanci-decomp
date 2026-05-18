using System;
using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms;
using Editor.ResourceItems;

namespace Editor.Temp;

public class LevelControl : ViewControl
{
	private Container components;

	private Level _level;

	private LayerType _visible = (LayerType)7;

	private LayerType _insert = LayerType.Layer0;

	private DrawSelect _draw;

	private ItemEventHandler _item_inserted;

	private ItemEventHandler _item_modified;

	private ItemEventHandler _item_removed;

	private ResourceEventHandler _resource_modified;

	private bool _doselect = true;

	private LevelItem _selected_item;

	private TextureBrush _brush;

	public new LevelView View
	{
		get
		{
			return base.View as LevelView;
		}
		set
		{
			base.View = value;
		}
	}

	public bool ShowBackground
	{
		get
		{
			return GetShowFlag(LayerType.Layer2);
		}
		set
		{
			SetShowFlag(LayerType.Layer2, value);
		}
	}

	public bool ShowMiddle
	{
		get
		{
			return GetShowFlag(LayerType.Layer1);
		}
		set
		{
			SetShowFlag(LayerType.Layer1, value);
		}
	}

	public bool ShowForeground
	{
		get
		{
			return GetShowFlag(LayerType.Layer0);
		}
		set
		{
			SetShowFlag(LayerType.Layer0, value);
		}
	}

	public LayerType LayerInsert
	{
		get
		{
			return _insert;
		}
		set
		{
			if (_insert != value)
			{
				_insert = value;
				_visible |= _insert;
				_draw = DrawSelect.Arrow;
				View.VisibleLayers = _visible;
				if (this.LayerInsertVisibleChanged != null)
				{
					this.LayerInsertVisibleChanged(this, EventArgs.Empty);
				}
			}
		}
	}

	public DrawSelect DrawSelect
	{
		get
		{
			return _draw;
		}
		set
		{
			if (_draw != value)
			{
				_draw = value;
				if (_draw == DrawSelect.Obstacle || _draw == DrawSelect.Teleport)
				{
					_insert = LayerType.Layer2;
					_visible |= LayerType.Layer2;
					View.VisibleLayers = _visible;
				}
				else if (_draw == DrawSelect.Mine)
				{
					_insert = LayerType.Layer1;
					_visible |= LayerType.Layer1;
					View.VisibleLayers = _visible;
				}
				if (this.LayerInsertVisibleChanged != null)
				{
					this.LayerInsertVisibleChanged(this, EventArgs.Empty);
				}
			}
		}
	}

	public Level Level
	{
		get
		{
			return _level;
		}
		set
		{
			if (_level != null)
			{
				View.Clear();
				_level.ItemInserted -= _item_inserted;
				_level.ItemModified -= _item_modified;
				_level.ItemRemoved -= _item_removed;
				_level.ResourceModified -= _resource_modified;
			}
			_level = value;
			if (_level != null)
			{
				View.Background = _level.Background.Bitmap;
				_level.ItemInserted += _item_inserted;
				_level.ItemModified += _item_modified;
				_level.ItemRemoved += _item_removed;
				_level.ResourceModified += _resource_modified;
				LoadLayer(LayerType.Layer2);
				LoadLayer(LayerType.Layer1);
				LoadLayer(LayerType.Layer0);
			}
			Invalidate();
		}
	}

	public LevelItemCollection CurrentLayer => Level.Layer(LayerInsert);

	public LevelItem SelectedLevelItem
	{
		get
		{
			return _selected_item;
		}
		set
		{
			if (_selected_item == value)
			{
				return;
			}
			_selected_item = null;
			View.SendBroadcast(View, 4, null);
			foreach (View child in View.Childs)
			{
				if (child.Tag == value)
				{
					child.Selected = true;
					break;
				}
			}
		}
	}

	public event EventHandler LayerInsertVisibleChanged;

	public event ItemEventHandler LevelItemSelected;

	public void ToggleLayerVisible(LayerType layer)
	{
		SetShowFlag(layer, !GetShowFlag(layer));
	}

	public bool LayerVisible(LayerType layer)
	{
		return GetShowFlag(layer);
	}

	private bool GetShowFlag(LayerType layer)
	{
		return (_visible & layer) == layer;
	}

	private void SetShowFlag(LayerType layer, bool on)
	{
		if (GetShowFlag(layer) == on)
		{
			return;
		}
		_draw = DrawSelect.Arrow;
		if (on)
		{
			_visible |= layer;
		}
		else
		{
			_visible &= ~layer;
		}
		if (on)
		{
			if (_visible == layer)
			{
				_insert = layer;
			}
		}
		else if ((_visible & _insert) == 0)
		{
			for (int i = 0; i < 3; i++)
			{
				if (((uint)_visible & (uint)(1 << i)) != 0)
				{
					_insert = (LayerType)(1 << i);
					break;
				}
			}
		}
		View.VisibleLayers = _visible;
		if (this.LayerInsertVisibleChanged != null)
		{
			this.LayerInsertVisibleChanged(this, EventArgs.Empty);
		}
	}

	public LevelControl()
	{
		InitializeComponent();
		View = new LevelView();
		_item_inserted = _level_ItemInserted;
		_item_modified = _level_ItemModified;
		_item_removed = _level_ItemRemoved;
		_resource_modified = _level_ResourceModified;
		_brush = new TextureBrush(Editor.Tools.Texts.GetObject("viewback") as Bitmap);
	}

	protected override void Dispose(bool disposing)
	{
		if (disposing && components != null)
		{
			components.Dispose();
		}
		base.Dispose(disposing);
	}

	private void InitializeComponent()
	{
		base.AutoScrollMinSize = new System.Drawing.Size(20, 20);
		this.Cursor = System.Windows.Forms.Cursors.Default;
		base.Name = "LevelControl";
		base.Size = new System.Drawing.Size(134, 134);
		base.ViewSizeMode = Editor.Temp.SizeMode.Auto;
	}

	private void LoadLayer(LayerType layer)
	{
		_doselect = false;
		foreach (LevelItem item in Level.Layer(layer))
		{
			_level_ItemInserted(this, new ItemEventArgs(item));
		}
		_doselect = true;
	}

	private void _level_ItemInserted(object sender, ItemEventArgs args)
	{
		LevelItem levelItem = args.LevelItem;
		ItemView itemView = null;
		if (levelItem is ObstacleLevelItem)
		{
			itemView = new ObstacleView(levelItem);
		}
		else if (levelItem is ImageLevelItem)
		{
			itemView = new ImageView(levelItem);
		}
		else if (levelItem is TeleportLevelItem)
		{
			itemView = new TeleportView(levelItem);
		}
		else if (levelItem is MineLevelItem)
		{
			itemView = new MineView(levelItem);
		}
		if (itemView != null)
		{
			itemView.Update();
			_selected_item = null;
			if (View.SelectedChilds.Count > 0)
			{
				View.SendBroadcast(View, 4, null);
			}
			View.Childs.Add(itemView);
			View.Childs.Sort(new ViewComparer());
			if (_doselect)
			{
				itemView.Selected = true;
			}
		}
	}

	private ItemView FindView(LevelItem li)
	{
		foreach (View child in View.Childs)
		{
			if (child.Tag == li)
			{
				return child as ItemView;
			}
		}
		return null;
	}

	private void _level_ItemModified(object sender, ItemEventArgs args)
	{
		FindView(args.LevelItem)?.Update();
	}

	private void _level_ItemRemoved(object sender, ItemEventArgs args)
	{
		View.Childs.Remove(FindView(args.LevelItem));
	}

	private void _level_ResourceModified(object sender, ResourceEventArgs args)
	{
		ResourceItem resourceItem = args.ResourceItem;
		if (resourceItem is BackgroundResourceItem)
		{
			View.Background = (resourceItem as BackgroundResourceItem).Bitmap;
		}
		else
		{
			if (resourceItem.IsReserved || !(resourceItem is BitmapResourceItem))
			{
				return;
			}
			foreach (View child in View.Childs)
			{
				if (child is ImageView imageView && !(imageView.ResourceName != resourceItem.Name))
				{
					imageView.SetBitmapResource(resourceItem as BitmapResourceItem, force: true);
				}
			}
		}
	}

	internal void OnLevelItemSelected(LevelItem item)
	{
		_selected_item = item;
		if (this.LevelItemSelected != null)
		{
			this.LevelItemSelected(this, new ItemEventArgs(item));
		}
	}

	protected override void OnPaint(PaintEventArgs e)
	{
		e.Graphics.FillRectangle(_brush, base.ClientRectangle);
		base.OnPaint(e);
	}
}
