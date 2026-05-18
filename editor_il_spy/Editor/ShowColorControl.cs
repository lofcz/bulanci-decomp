using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms;

namespace Editor;

public class ShowColorControl : UserControl
{
	private Container components;

	private const int WS_BORDER = 8388608;

	private const int WS_EX_CLIENTEDGE = 512;

	private BorderStyle _border = BorderStyle.Fixed3D;

	public Color Color
	{
		get
		{
			return BackColor;
		}
		set
		{
			BackColor = value;
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

	public ShowColorControl()
	{
		InitializeComponent();
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
		this.BackColor = System.Drawing.SystemColors.Control;
		base.Name = "ShowColorControl";
		base.Size = new System.Drawing.Size(150, 20);
	}
}
