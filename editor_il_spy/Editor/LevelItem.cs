using System;
using System.Drawing;
using System.Windows.Forms;
using System.Xml.Serialization;
using Editor.Scripts;
using Editor.Temp;

namespace Editor;

public abstract class LevelItem : IHasName, ICloneable, IKeybHandler
{
	private Rectangle _rect = Rectangle.Empty;

	private LayerType _layer;

	private Level _level;

	private int _locked;

	private int _changes;

	private string _name;

	public Level Level => _level;

	public abstract LevelItemType LevelItemType { get; }

	[XmlIgnore]
	public Rectangle Rectangle
	{
		get
		{
			return _rect;
		}
		set
		{
			if (_rect != value)
			{
				_rect = value;
				if (Level != null)
				{
					Level.OnItemModified(this);
				}
			}
		}
	}

	[XmlIgnore]
	public int ZOrder
	{
		get
		{
			if (_level == null || _layer == LayerType.None)
			{
				return 0;
			}
			return _level.Layer(_layer).IndexOf(this);
		}
		set
		{
			if (_level != null && _layer != LayerType.None)
			{
				int num = Math.Min(_level.Layer(_layer).Count - 1, Math.Max(0, value));
				if (num != ZOrder)
				{
					_level.Layer(_layer).Insert(num, this);
				}
			}
		}
	}

	[XmlElement("Rectangle")]
	public SerializedRectangle SerializedRectangle
	{
		get
		{
			return new SerializedRectangle(Rectangle);
		}
		set
		{
			Rectangle = value.ToRectangle();
		}
	}

	[XmlIgnore]
	public LayerType Layer
	{
		get
		{
			return _layer;
		}
		set
		{
			if (_layer != value)
			{
				if (_level != null)
				{
					Level level = _level;
					level.Layer(_layer).Remove(this);
					_layer = value;
					level.Layer(_layer).Insert(0, this);
				}
				else
				{
					_layer = value;
				}
			}
		}
	}

	[XmlAttribute]
	public string Name
	{
		get
		{
			return _name;
		}
		set
		{
			if (_name != value)
			{
				_name = value;
				if (Level != null)
				{
					Level.OnItemModified(this);
				}
			}
		}
	}

	public bool IsReserved => false;

	private LevelItemCollection Collection => Layer switch
	{
		LayerType.Layer0 => Level.ItemsLayer0, 
		LayerType.Layer1 => Level.ItemsLayer1, 
		LayerType.Layer2 => Level.ItemsLayer2, 
		_ => null, 
	};

	protected void Lock()
	{
		_locked++;
	}

	protected void Unlock()
	{
		if (_locked > 0)
		{
			_locked--;
			if (_locked == 0 && _changes > 0)
			{
				_changes = 0;
				ItemModified();
			}
		}
	}

	protected void ItemModified()
	{
		if (_locked > 0)
		{
			_changes++;
		}
		else if (_level != null)
		{
			_level.OnItemModified(this);
		}
	}

	internal void SetLayer(LayerType layer)
	{
		_layer = layer;
	}

	internal void SetLevel(Level level)
	{
		_level = level;
	}

	public virtual void GenerateScript(Function func)
	{
	}

	public void Remove()
	{
		if (Level != null)
		{
			Collection.Remove(this);
		}
	}

	public virtual void CopyFrom(LevelItem item)
	{
		Lock();
		if (item != null)
		{
			Name = item.Name;
			Rectangle = item.Rectangle;
			Layer = item.Layer;
		}
		Unlock();
	}

	public void MoveUp()
	{
		ZOrder--;
	}

	public void MoveDown()
	{
		ZOrder++;
	}

	public void MoveTop()
	{
		ZOrder = 0;
	}

	public void MoveBottom()
	{
		ZOrder = int.MaxValue;
	}

	public LevelItem()
	{
	}

	public LevelItem(Rectangle rect)
	{
		_rect = rect;
	}

	public object Clone()
	{
		LevelItem obj = GetType().GetConstructor(Type.EmptyTypes).Invoke(null) as LevelItem;
		obj.CopyFrom(this);
		return obj;
	}

	protected void AppendMenuMove(ContextMenu menu, bool withlayer)
	{
		int num = -1;
		if (_level != null && _layer != LayerType.None)
		{
			num = _level.Layer(_layer).Count - 1;
		}
		int zOrder = ZOrder;
		MenuItem[] array = new MenuItem[withlayer ? 8 : 4];
		array[0] = new MenuItem(Tools.Localizer.GetString(79), OnMoveTop);
		array[1] = new MenuItem(Tools.Localizer.GetString(80), OnMoveUp);
		MenuItem obj = array[0];
		bool enabled = (array[1].Enabled = num != -1 && zOrder > 0);
		obj.Enabled = enabled;
		array[2] = new MenuItem(Tools.Localizer.GetString(81), OnMoveDown);
		array[3] = new MenuItem(Tools.Localizer.GetString(82), OnMoveBottom);
		MenuItem obj2 = array[2];
		enabled = (array[3].Enabled = num != -1 && zOrder < num);
		obj2.Enabled = enabled;
		if (withlayer)
		{
			array[4] = new MenuItem("-");
			array[5] = new MenuItem(Tools.Localizer.GetString(83), OnMoveToLayer0);
			array[6] = new MenuItem(Tools.Localizer.GetString(84), OnMoveToLayer1);
			array[7] = new MenuItem(Tools.Localizer.GetString(85), OnMoveToLayer2);
			array[5 + LayerToInt(Layer)].Enabled = false;
		}
		menu.MenuItems.Add(Tools.Localizer.GetString(86), array);
		menu.MenuItems.Add(Tools.Localizer.GetString(87), OnRemove);
	}

	private int LayerToInt(LayerType lt)
	{
		return lt switch
		{
			LayerType.Layer0 => 0, 
			LayerType.Layer1 => 1, 
			LayerType.Layer2 => 2, 
			_ => -1, 
		};
	}

	private void OnMoveTop(object sender, EventArgs args)
	{
		MoveTop();
	}

	private void OnMoveUp(object sender, EventArgs args)
	{
		MoveUp();
	}

	private void OnMoveDown(object sender, EventArgs args)
	{
		MoveDown();
	}

	private void OnMoveBottom(object sender, EventArgs args)
	{
		MoveBottom();
	}

	private void OnMoveToLayer0(object sender, EventArgs args)
	{
		Layer = LayerType.Layer0;
	}

	private void OnMoveToLayer1(object sender, EventArgs args)
	{
		Layer = LayerType.Layer1;
	}

	private void OnMoveToLayer2(object sender, EventArgs args)
	{
		Layer = LayerType.Layer2;
	}

	private void OnRemove(object sender, EventArgs args)
	{
		Level.Layer(Layer).Remove(this);
	}

	public virtual bool OnKeyDown(Control sender, Keys key)
	{
		if (key == Keys.Delete)
		{
			OnRemove(sender, EventArgs.Empty);
			return true;
		}
		return false;
	}
}
