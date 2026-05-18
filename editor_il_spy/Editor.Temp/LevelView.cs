using System;
using System.Collections;
using System.Drawing;
using System.Windows.Forms;

namespace Editor.Temp;

public class LevelView : ControlClientView
{
	public const int Border = 2;

	private Point _mouse_down = Point.Empty;

	private View _view_down;

	private BitmapView _background;

	public new LevelControl Control => base.Control as LevelControl;

	public Image Background
	{
		get
		{
			return _background.Image;
		}
		set
		{
			_background.Image = value;
			Size size = _background.Size;
			size.Width += 4;
			size.Height += 4;
			base.Size = size;
		}
	}

	public LayerType VisibleLayers
	{
		get
		{
			LayerType layerType = LayerType.None;
			foreach (View child in base.Childs)
			{
				if (child.Tag is LevelItem levelItem && child.Visible)
				{
					layerType |= levelItem.Layer;
				}
			}
			return layerType;
		}
		set
		{
			foreach (View item in Views(onlyselected: false))
			{
				LevelItem levelItem = item.Tag as LevelItem;
				item.Visible = (value & levelItem.Layer) != 0;
			}
		}
	}

	public LevelView()
	{
		base.MultiSelect = true;
		base.Childs.Add(_background = new BitmapView(new Point(2, 2), null));
	}

	public void Clear()
	{
		Background = null;
		base.Childs.Clear();
		base.Childs.Add(_background);
	}

	public ArrayList Views(bool onlyselected)
	{
		ArrayList arrayList = new ArrayList();
		foreach (View child in base.Childs)
		{
			if (child.Tag != null && (!onlyselected || (onlyselected && child.Selected)))
			{
				arrayList.Add(child);
			}
		}
		return arrayList;
	}

	protected override bool IsInView(MouseEventArgs e)
	{
		if (Control.Level != null)
		{
			return base.IsInView(e);
		}
		return false;
	}

	private void DeleteSelected()
	{
		foreach (View item in Views(onlyselected: true))
		{
			(item.Tag as LevelItem).Remove();
		}
	}

	public override void HandleEvent(Event e)
	{
		base.HandleEvent(e);
		if (e.What == EventType.MouseDown)
		{
			MouseEvent mouseEvent = e as MouseEvent;
			if (mouseEvent.DoubleClick || mouseEvent.Buttons != MouseButtons.Left)
			{
				return;
			}
			_mouse_down = MakeLocal(mouseEvent.Where);
			_mouse_down.X = Math.Max(2, Math.Min(2 + _background.Size.Width, _mouse_down.X));
			_mouse_down.Y = Math.Max(2, Math.Min(2 + _background.Size.Height, _mouse_down.Y));
			switch (Control.DrawSelect)
			{
			case DrawSelect.Arrow:
				_view_down = new SelectView();
				break;
			case DrawSelect.Obstacle:
				_view_down = new ObstacleView(null);
				break;
			case DrawSelect.Image:
			{
				InsertImageForm insertImageForm = new InsertImageForm(Control.Level);
				if (insertImageForm.ShowDialog() == DialogResult.OK)
				{
					LevelItem levelItem = new ImageLevelItem(new Point(_mouse_down.X - 2, _mouse_down.Y - 2), insertImageForm.BitmapResource);
					levelItem.Name = Control.Level.GenerateNewItemName("image");
					Control.CurrentLayer.Insert(0, levelItem);
				}
				break;
			}
			case DrawSelect.Teleport:
			{
				LevelItem levelItem = new TeleportLevelItem(new Point(_mouse_down.X - 2, _mouse_down.Y - 2));
				levelItem.Name = Control.Level.GenerateNewItemName("teleport");
				Control.CurrentLayer.Insert(0, levelItem);
				break;
			}
			case DrawSelect.Mine:
			{
				LevelItem levelItem = new MineLevelItem(new Point(_mouse_down.X - 2, _mouse_down.Y - 2));
				levelItem.Name = Control.Level.GenerateNewItemName("mine");
				Control.CurrentLayer.Insert(0, levelItem);
				break;
			}
			}
			if (_view_down != null)
			{
				_view_down.Bounds = new Rectangle(_mouse_down, Size.Empty);
				base.Childs.Add(_view_down);
				base.MouseCaptured = true;
			}
			e.Clear(this);
		}
		else if (e.What == EventType.MouseMove)
		{
			if (_view_down != null)
			{
				Point point = MakeLocal((e as MouseEvent).Where);
				Point mouse_down = _mouse_down;
				if (mouse_down.X > point.X)
				{
					int x = point.X;
					point.X = mouse_down.X;
					mouse_down.X = x;
				}
				if (mouse_down.Y > point.Y)
				{
					int y = point.Y;
					point.Y = mouse_down.Y;
					mouse_down.Y = y;
				}
				mouse_down.X = Math.Max(mouse_down.X, 2);
				mouse_down.Y = Math.Max(mouse_down.Y, 2);
				point.X = Math.Min(point.X, _background.Size.Width + 2);
				point.Y = Math.Min(point.Y, _background.Size.Height + 2);
				_view_down.Bounds = new Rectangle(mouse_down, new Size(point.X - mouse_down.X, point.Y - mouse_down.Y));
				e.Clear(this);
			}
		}
		else if (e.What == EventType.MouseUp)
		{
			if (_view_down == null)
			{
				return;
			}
			Rectangle bounds = _view_down.Bounds;
			base.Childs.Remove(_view_down);
			_view_down = null;
			base.MouseCaptured = false;
			if (Control.DrawSelect == DrawSelect.Arrow)
			{
				if (!(e as MouseEvent).Ctrl)
				{
					SendBroadcast(this, 4, null);
				}
				if (bounds.Width > 2 && bounds.Height > 2)
				{
					foreach (View item in Views(onlyselected: false))
					{
						if (item.Bounds.IntersectsWith(bounds))
						{
							item.Selected = true;
						}
					}
				}
				e.Clear(this);
			}
			else if (Control.DrawSelect == DrawSelect.Obstacle)
			{
				if (bounds.Width > 2 && bounds.Height > 2)
				{
					bounds.Offset(-2, -2);
					LevelItem levelItem2 = new ObstacleLevelItem(bounds);
					levelItem2.Name = Control.Level.GenerateNewItemName("obstacle");
					Control.CurrentLayer.Insert(0, levelItem2);
				}
				e.Clear(this);
			}
		}
		else
		{
			if (e.What != EventType.KeyDown)
			{
				return;
			}
			if (base.SelectedChilds.Count == 1)
			{
				if (base.SelectedChilds[0].Tag is IKeybHandler keybHandler)
				{
					keybHandler.OnKeyDown(Control, (e as KeyEvent).Key);
				}
				e.Clear(this);
			}
			else if ((e as KeyEvent).Key == Keys.Delete)
			{
				DeleteSelected();
				e.Clear(this);
			}
		}
	}

	protected override void OnPaint(Graphics g)
	{
		Brush brush = new SolidBrush(Color.FromArgb(187, 187, 187));
		g.FillRectangle(brush, base.ClientBounds);
		base.OnPaint(g);
	}
}
