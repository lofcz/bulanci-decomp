using System.Drawing;
using System.Drawing.Drawing2D;

namespace Editor.Temp;

public class ObstacleView : ItemView
{
	private bool _firethrough;

	private Brush _brush;

	private Pen _pen;

	public bool FireThrough
	{
		get
		{
			return _firethrough;
		}
		set
		{
			if (_firethrough != value)
			{
				_firethrough = value;
				UpdateBrush();
				OnFaceChanged();
			}
		}
	}

	private void UpdateBrush()
	{
		if (!FireThrough)
		{
			_brush = new SolidBrush(Color.FromArgb(84, Color.Red));
		}
		else
		{
			_brush = new HatchBrush(HatchStyle.LargeCheckerBoard, Color.Empty, Color.FromArgb(84, Color.Red));
		}
	}

	protected override void OnPaint(Graphics g)
	{
		g.FillRectangle(_brush, base.ClientBounds);
		if (base.Selected)
		{
			g.DrawRectangle(_pen, 0, 0, base.Size.Width - 1, base.Size.Height - 1);
		}
	}

	public override void Update()
	{
		base.Update();
		FireThrough = (base.LevelItem as ObstacleLevelItem).FireThrough;
	}

	public ObstacleView(LevelItem item)
		: base(item)
	{
		_pen = new Pen(Color.White);
		UpdateBrush();
		base.ResizeEnabled = true;
	}
}
