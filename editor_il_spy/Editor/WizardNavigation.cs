using System;
using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms;

namespace Editor;

public class WizardNavigation : UserControl
{
	private Button buttonCancel;

	private Button buttonNext;

	private Button buttonPrev;

	private Container components;

	public Button Prev => buttonPrev;

	public Button Next => buttonNext;

	public Button Cancel => buttonCancel;

	public event EventHandler NavigationPrev;

	public event EventHandler NavigationNext;

	public WizardNavigation()
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
		this.buttonCancel = new System.Windows.Forms.Button();
		this.buttonNext = new System.Windows.Forms.Button();
		this.buttonPrev = new System.Windows.Forms.Button();
		base.SuspendLayout();
		this.buttonCancel.Anchor = System.Windows.Forms.AnchorStyles.Right;
		this.buttonCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
		this.buttonCancel.ImeMode = System.Windows.Forms.ImeMode.NoControl;
		this.buttonCancel.Location = new System.Drawing.Point(312, 16);
		this.buttonCancel.Name = "buttonCancel";
		this.buttonCancel.TabIndex = 2;
		this.buttonCancel.Text = "{11}";
		this.buttonNext.Anchor = System.Windows.Forms.AnchorStyles.Right;
		this.buttonNext.ImeMode = System.Windows.Forms.ImeMode.NoControl;
		this.buttonNext.Location = new System.Drawing.Point(184, 16);
		this.buttonNext.Name = "buttonNext";
		this.buttonNext.TabIndex = 1;
		this.buttonNext.Text = "{13}";
		this.buttonNext.Click += new System.EventHandler(buttonNext_Click);
		this.buttonPrev.Anchor = System.Windows.Forms.AnchorStyles.Right;
		this.buttonPrev.ImeMode = System.Windows.Forms.ImeMode.NoControl;
		this.buttonPrev.Location = new System.Drawing.Point(104, 16);
		this.buttonPrev.Name = "buttonPrev";
		this.buttonPrev.TabIndex = 0;
		this.buttonPrev.Text = "{12}";
		this.buttonPrev.Click += new System.EventHandler(buttonPrev_Click);
		base.Controls.Add(this.buttonPrev);
		base.Controls.Add(this.buttonNext);
		base.Controls.Add(this.buttonCancel);
		base.Name = "WizardNavigation";
		base.Size = new System.Drawing.Size(400, 56);
		base.ResumeLayout(false);
	}

	private void buttonPrev_Click(object sender, EventArgs e)
	{
		if (this.NavigationPrev != null)
		{
			this.NavigationPrev(sender, e);
		}
	}

	private void buttonNext_Click(object sender, EventArgs e)
	{
		if (this.NavigationNext != null)
		{
			this.NavigationNext(sender, e);
		}
	}
}
