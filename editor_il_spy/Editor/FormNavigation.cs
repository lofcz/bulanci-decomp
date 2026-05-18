using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms;

namespace Editor;

public class FormNavigation : UserControl
{
	private Button buttonOK;

	private Button buttonCancel;

	private Container components;

	public Button OK => buttonOK;

	public Button Cancel => buttonCancel;

	public FormNavigation()
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
		this.buttonOK = new System.Windows.Forms.Button();
		this.buttonCancel = new System.Windows.Forms.Button();
		base.SuspendLayout();
		this.buttonOK.Anchor = System.Windows.Forms.AnchorStyles.None;
		this.buttonOK.DialogResult = System.Windows.Forms.DialogResult.OK;
		this.buttonOK.ImeMode = System.Windows.Forms.ImeMode.NoControl;
		this.buttonOK.Location = new System.Drawing.Point(83, 16);
		this.buttonOK.Name = "buttonOK";
		this.buttonOK.TabIndex = 0;
		this.buttonOK.Text = "{10}";
		this.buttonCancel.Anchor = System.Windows.Forms.AnchorStyles.None;
		this.buttonCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
		this.buttonCancel.ImeMode = System.Windows.Forms.ImeMode.NoControl;
		this.buttonCancel.Location = new System.Drawing.Point(179, 16);
		this.buttonCancel.Name = "buttonCancel";
		this.buttonCancel.TabIndex = 1;
		this.buttonCancel.Text = "{11}";
		base.Controls.Add(this.buttonCancel);
		base.Controls.Add(this.buttonOK);
		base.Name = "FormNavigation";
		base.Size = new System.Drawing.Size(336, 56);
		base.ResumeLayout(false);
	}
}
