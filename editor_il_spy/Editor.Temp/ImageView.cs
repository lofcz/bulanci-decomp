using System.Drawing;
using Editor.ResourceItems;

namespace Editor.Temp;

public class ImageView : ItemView
{
	private string _name = "";

	private bool _transparent;

	private Bitmap _bitmap;

	private GroundView _ground;

	private int _last_ground;

	private Pen _pen;

	public string ResourceName => _name;

	public int Ground
	{
		get
		{
			return _last_ground;
		}
		set
		{
			if (_last_ground != value)
			{
				_last_ground = value;
				(base.Tag as ImageLevelItem).Ground = _last_ground;
				if (_ground != null)
				{
					_ground.UpdateRectangle();
				}
			}
		}
	}

	public void SetBitmapResource(BitmapResourceItem item, bool force)
	{
		if (_name != item.Name || force)
		{
			_name = item.Name;
			_bitmap = new Bitmap(item.Bitmap);
			_transparent = false;
			if (item.Transparency != ZeroColor.NotDefined)
			{
				_bitmap.MakeTransparent(item.TransparentColor);
				_transparent = true;
			}
			OnFaceChanged();
		}
	}

	public override bool ContainsMouse(Point where)
	{
		if (!base.ContainsMouse(where))
		{
			return false;
		}
		if (!_transparent || _bitmap == null)
		{
			return true;
		}
		Point point = MakeLocal(where);
		return _bitmap.GetPixel(point.X, point.Y).ToArgb() != 0;
	}

	public override bool SetStateFlag(ViewState state, bool on)
	{
		bool selected = base.Selected;
		bool result = base.SetStateFlag(state, on);
		if (selected != base.Selected && base.Parent != null)
		{
			if (!selected)
			{
				if (base.LevelItem.Layer == LayerType.Layer1)
				{
					base.Parent.Childs.Insert(base.ZOrder, _ground = new GroundView(this));
				}
			}
			else if (_ground != null)
			{
				base.Parent.Childs.Remove(_ground);
				_ground = null;
			}
		}
		return result;
	}

	protected override void OnPaint(Graphics g)
	{
		if (_bitmap != null)
		{
			g.DrawImage(_bitmap, new Rectangle(Point.Empty, _bitmap.Size));
		}
		if (base.Selected)
		{
			g.DrawRectangle(_pen, 0, 0, base.Size.Width - 1, base.Size.Height - 1);
		}
	}

	public override void Update()
	{
		base.Update();
		ImageLevelItem imageLevelItem = base.LevelItem as ImageLevelItem;
		Ground = imageLevelItem.Ground;
		SetBitmapResource(imageLevelItem.BitmapResource, force: false);
	}

	public ImageView(LevelItem item)
		: base(item)
	{
		_pen = new Pen(Color.White);
	}
}
