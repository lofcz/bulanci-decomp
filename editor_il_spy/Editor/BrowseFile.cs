using System;
using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms;

namespace Editor;

public class BrowseFile : UserControl
{
	private Button button;

	private TextBox textBox;

	private OpenFileDialog openFileDialog;

	private string _filter;

	private Container components;

	[Browsable(true)]
	[Localizable(true)]
	public string Title
	{
		get
		{
			return openFileDialog.Title;
		}
		set
		{
			openFileDialog.Title = value;
		}
	}

	[Browsable(true)]
	[Localizable(true)]
	public string Filter
	{
		get
		{
			return _filter;
		}
		set
		{
			_filter = value;
		}
	}

	[Browsable(true)]
	[DefaultValue("")]
	public new string Text
	{
		get
		{
			return textBox.Text;
		}
		set
		{
			textBox.Text = value;
		}
	}

	public BrowseFile()
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
		this.button = new System.Windows.Forms.Button();
		this.textBox = new System.Windows.Forms.TextBox();
		this.openFileDialog = new System.Windows.Forms.OpenFileDialog();
		base.SuspendLayout();
		this.button.Anchor = System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right;
		this.button.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25f, System.Drawing.FontStyle.Bold);
		this.button.ImeMode = System.Windows.Forms.ImeMode.NoControl;
		this.button.Location = new System.Drawing.Point(312, 0);
		this.button.Name = "button";
		this.button.Size = new System.Drawing.Size(24, 20);
		this.button.TabIndex = 1;
		this.button.Text = "...";
		this.button.Click += new System.EventHandler(button_Click);
		this.textBox.Anchor = System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left | System.Windows.Forms.AnchorStyles.Right;
		this.textBox.Location = new System.Drawing.Point(0, 0);
		this.textBox.Name = "textBox";
		this.textBox.ReadOnly = true;
		this.textBox.Size = new System.Drawing.Size(304, 20);
		this.textBox.TabIndex = 0;
		this.textBox.Text = "";
		base.Controls.Add(this.textBox);
		base.Controls.Add(this.button);
		base.Name = "BrowseFile";
		base.Size = new System.Drawing.Size(336, 20);
		base.ResumeLayout(false);
	}

	private void button_Click(object sender, EventArgs e)
	{
		openFileDialog.Filter = _filter;
		openFileDialog.FileName = textBox.Text;
		if (openFileDialog.ShowDialog(this) == DialogResult.OK)
		{
			textBox.Text = openFileDialog.FileName;
		}
	}
}
