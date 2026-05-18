using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms;
using Editor.ResourceItems;

namespace Editor;

public class SaveBitmapAs : UserControl
{
	private RadioButton radioJPG;

	private RadioButton radioBMP;

	private Label labelSaveAs;

	private Container components;

	[Browsable(true)]
	[DefaultValue(BitmapFormat.Jpeg)]
	public BitmapFormat SaveAsFormat
	{
		get
		{
			if (radioBMP.Checked)
			{
				return BitmapFormat.Bmp;
			}
			if (radioJPG.Checked)
			{
				return BitmapFormat.Jpeg;
			}
			return BitmapFormat.Special;
		}
		set
		{
			radioBMP.Checked = value == BitmapFormat.Bmp;
			radioJPG.Checked = value == BitmapFormat.Jpeg;
			Label label = labelSaveAs;
			RadioButton radioButton = radioBMP;
			bool flag = (radioJPG.Enabled = value != BitmapFormat.Special);
			bool enabled = (radioButton.Enabled = flag);
			label.Enabled = enabled;
		}
	}

	public SaveBitmapAs()
	{
		InitializeComponent();
		SaveAsFormat = BitmapFormat.Jpeg;
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
		this.radioJPG = new System.Windows.Forms.RadioButton();
		this.radioBMP = new System.Windows.Forms.RadioButton();
		this.labelSaveAs = new System.Windows.Forms.Label();
		base.SuspendLayout();
		this.radioJPG.Anchor = System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right;
		this.radioJPG.ImeMode = System.Windows.Forms.ImeMode.NoControl;
		this.radioJPG.Location = new System.Drawing.Point(128, 0);
		this.radioJPG.Name = "radioJPG";
		this.radioJPG.Size = new System.Drawing.Size(46, 16);
		this.radioJPG.TabIndex = 2;
		this.radioJPG.Text = "{16}";
		this.radioBMP.Anchor = System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right;
		this.radioBMP.ImeMode = System.Windows.Forms.ImeMode.NoControl;
		this.radioBMP.Location = new System.Drawing.Point(80, 0);
		this.radioBMP.Name = "radioBMP";
		this.radioBMP.Size = new System.Drawing.Size(46, 16);
		this.radioBMP.TabIndex = 1;
		this.radioBMP.Text = "{15}";
		this.labelSaveAs.Anchor = System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right;
		this.labelSaveAs.ImeMode = System.Windows.Forms.ImeMode.NoControl;
		this.labelSaveAs.Location = new System.Drawing.Point(0, 0);
		this.labelSaveAs.Name = "labelSaveAs";
		this.labelSaveAs.Size = new System.Drawing.Size(66, 16);
		this.labelSaveAs.TabIndex = 0;
		this.labelSaveAs.Text = "{14}";
		this.labelSaveAs.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
		base.Controls.Add(this.radioJPG);
		base.Controls.Add(this.radioBMP);
		base.Controls.Add(this.labelSaveAs);
		base.Name = "SaveBitmapAs";
		base.Size = new System.Drawing.Size(176, 16);
		base.ResumeLayout(false);
	}
}
