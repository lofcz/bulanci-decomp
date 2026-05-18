using System;
using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms;

namespace Editor.Temp;

public class ViewControl : UserControl
{
	private Container components;

	private ControlClientView _view;

	private const int WS_BORDER = 8388608;

	private const int WS_EX_CLIENTEDGE = 512;

	private BorderStyle _border = BorderStyle.Fixed3D;

	private EventHandler _bounds_changed;

	private EventHandler _face_changed;

	private Size _size = Size.Empty;

	private SizeMode _sm;

	public ControlClientView View
	{
		get
		{
			return _view;
		}
		set
		{
			if (_view != value)
			{
				if (_view != null)
				{
					_view.BoundsChanged -= _bounds_changed;
					_view.FaceChanged -= _face_changed;
					_view.SetControl(null);
				}
				_view = value;
				if (_view != null)
				{
					_view.SetControl(this);
					_view.BoundsChanged += _bounds_changed;
					_view.FaceChanged += _face_changed;
					_view.Size = _size;
					_view.SizeMode = _sm;
				}
				Invalidate();
			}
		}
	}

	public Size ViewSize
	{
		get
		{
			return _size;
		}
		set
		{
			_size = value;
			if (_view != null)
			{
				_view.Size = value;
			}
		}
	}

	public SizeMode ViewSizeMode
	{
		get
		{
			return _sm;
		}
		set
		{
			_sm = value;
			if (_view != null)
			{
				_view.SizeMode = value;
			}
		}
	}

	[Browsable(true)]
	[Category("Appearance")]
	[DefaultValue(BorderStyle.Fixed3D)]
	[Description("The border style of the control.")]
	public new BorderStyle BorderStyle
	{
		get
		{
			return _border;
		}
		set
		{
			_border = value;
			UpdateStyles();
		}
	}

	protected override CreateParams CreateParams
	{
		get
		{
			CreateParams createParams = base.CreateParams;
			createParams.Style &= -8388609;
			createParams.ExStyle &= -513;
			switch (_border)
			{
			case BorderStyle.Fixed3D:
				createParams.ExStyle |= 512;
				break;
			case BorderStyle.FixedSingle:
				createParams.Style |= 8388608;
				break;
			}
			return createParams;
		}
	}

	public ViewControl()
	{
		_face_changed = _view_FaceChanged;
		_bounds_changed = _view_BoundsChanged;
		InitializeComponent();
		SetStyle(ControlStyles.UserPaint | ControlStyles.Opaque | ControlStyles.ResizeRedraw | ControlStyles.AllPaintingInWmPaint | ControlStyles.DoubleBuffer, value: true);
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
		this.AutoScroll = true;
		base.Name = "ViewControl";
	}

	private void _view_BoundsChanged(object sender, EventArgs args)
	{
		base.AutoScrollMinSize = _view.Size;
		Invalidate();
	}

	private void _view_FaceChanged(object sender, EventArgs args)
	{
		Invalidate();
	}

	protected override void OnPaint(PaintEventArgs e)
	{
		if (_view != null)
		{
			e.Graphics.TranslateTransform(base.AutoScrollPosition.X, base.AutoScrollPosition.Y);
			_view.Paint(e.Graphics);
		}
	}
}
