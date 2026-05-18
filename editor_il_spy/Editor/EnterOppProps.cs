using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms;

namespace Editor;

public class EnterOppProps : SheetControl
{
	private Label labelColor;

	private Label labelHits;

	private Label labelSpeed;

	private Label labelWeapon;

	private ComboBox comboColor;

	private ComboBox comboWeapon;

	private NumericUpDown numericSpeed;

	private NumericUpDown numericHits;

	private Container components;

	private Opponent _opponent;

	private static WeaponType[] map = new WeaponType[5]
	{
		WeaponType.Random,
		WeaponType.Gun,
		WeaponType.ShotGun,
		WeaponType.MachineGun,
		WeaponType.BombThrower
	};

	public EnterOppProps(Opponent opponent)
	{
		InitializeComponent();
		_opponent = opponent;
		numericSpeed.Value = _opponent.Speed;
		numericHits.Value = _opponent.Hits - 1;
		comboColor.SelectedIndex = (int)(_opponent.Color + 1);
		for (int i = 0; i < map.Length; i++)
		{
			if (_opponent.Weapon == map[i])
			{
				comboWeapon.SelectedIndex = i;
				break;
			}
		}
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
		this.labelColor = new System.Windows.Forms.Label();
		this.labelHits = new System.Windows.Forms.Label();
		this.labelSpeed = new System.Windows.Forms.Label();
		this.labelWeapon = new System.Windows.Forms.Label();
		this.comboColor = new System.Windows.Forms.ComboBox();
		this.comboWeapon = new System.Windows.Forms.ComboBox();
		this.numericSpeed = new System.Windows.Forms.NumericUpDown();
		this.numericHits = new System.Windows.Forms.NumericUpDown();
		((System.ComponentModel.ISupportInitialize)this.numericSpeed).BeginInit();
		((System.ComponentModel.ISupportInitialize)this.numericHits).BeginInit();
		base.SuspendLayout();
		this.labelColor.Location = new System.Drawing.Point(32, 28);
		this.labelColor.Name = "labelColor";
		this.labelColor.Size = new System.Drawing.Size(100, 16);
		this.labelColor.TabIndex = 0;
		this.labelColor.Text = "{164}";
		this.labelColor.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
		this.labelHits.Location = new System.Drawing.Point(32, 100);
		this.labelHits.Name = "labelHits";
		this.labelHits.Size = new System.Drawing.Size(100, 16);
		this.labelHits.TabIndex = 6;
		this.labelHits.Text = "{167}";
		this.labelHits.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
		this.labelSpeed.Location = new System.Drawing.Point(32, 76);
		this.labelSpeed.Name = "labelSpeed";
		this.labelSpeed.Size = new System.Drawing.Size(100, 16);
		this.labelSpeed.TabIndex = 4;
		this.labelSpeed.Text = "{166}";
		this.labelSpeed.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
		this.labelWeapon.Location = new System.Drawing.Point(32, 52);
		this.labelWeapon.Name = "labelWeapon";
		this.labelWeapon.Size = new System.Drawing.Size(100, 16);
		this.labelWeapon.TabIndex = 2;
		this.labelWeapon.Text = "{165}";
		this.labelWeapon.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
		this.comboColor.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
		this.comboColor.Items.AddRange(new object[9] { "{168}", "{169}", "{170}", "{171}", "{172}", "{173}", "{174}", "{175}", "{176}" });
		this.comboColor.Location = new System.Drawing.Point(144, 24);
		this.comboColor.Name = "comboColor";
		this.comboColor.Size = new System.Drawing.Size(121, 21);
		this.comboColor.TabIndex = 1;
		this.comboWeapon.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
		this.comboWeapon.Items.AddRange(new object[5] { "{177}", "{178}", "{179}", "{180}", "{181}" });
		this.comboWeapon.Location = new System.Drawing.Point(144, 48);
		this.comboWeapon.Name = "comboWeapon";
		this.comboWeapon.Size = new System.Drawing.Size(121, 21);
		this.comboWeapon.TabIndex = 3;
		this.numericSpeed.Increment = new decimal(new int[4] { 10, 0, 0, 0 });
		this.numericSpeed.Location = new System.Drawing.Point(144, 72);
		this.numericSpeed.Maximum = new decimal(new int[4] { 200, 0, 0, 0 });
		this.numericSpeed.Minimum = new decimal(new int[4] { 40, 0, 0, 0 });
		this.numericSpeed.Name = "numericSpeed";
		this.numericSpeed.Size = new System.Drawing.Size(56, 20);
		this.numericSpeed.TabIndex = 5;
		this.numericSpeed.Value = new decimal(new int[4] { 40, 0, 0, 0 });
		this.numericHits.Location = new System.Drawing.Point(144, 96);
		this.numericHits.Maximum = new decimal(new int[4] { 500, 0, 0, 0 });
		this.numericHits.Name = "numericHits";
		this.numericHits.Size = new System.Drawing.Size(56, 20);
		this.numericHits.TabIndex = 7;
		this.numericHits.Value = new decimal(new int[4] { 1, 0, 0, 0 });
		base.Controls.Add(this.numericHits);
		base.Controls.Add(this.numericSpeed);
		base.Controls.Add(this.comboWeapon);
		base.Controls.Add(this.comboColor);
		base.Controls.Add(this.labelWeapon);
		base.Controls.Add(this.labelSpeed);
		base.Controls.Add(this.labelHits);
		base.Controls.Add(this.labelColor);
		base.DescriptionVisible = false;
		base.Name = "EnterOppProps";
		base.Size = new System.Drawing.Size(296, 146);
		base.ValidateSheet += new System.ComponentModel.CancelEventHandler(EnterOppProps_ValidateSheet);
		base.Controls.SetChildIndex(this.labelColor, 0);
		base.Controls.SetChildIndex(this.labelHits, 0);
		base.Controls.SetChildIndex(this.labelSpeed, 0);
		base.Controls.SetChildIndex(this.labelWeapon, 0);
		base.Controls.SetChildIndex(this.comboColor, 0);
		base.Controls.SetChildIndex(this.comboWeapon, 0);
		base.Controls.SetChildIndex(this.numericSpeed, 0);
		base.Controls.SetChildIndex(this.numericHits, 0);
		((System.ComponentModel.ISupportInitialize)this.numericSpeed).EndInit();
		((System.ComponentModel.ISupportInitialize)this.numericHits).EndInit();
		base.ResumeLayout(false);
	}

	private void EnterOppProps_ValidateSheet(object sender, CancelEventArgs e)
	{
		_opponent.Speed = (int)numericSpeed.Value;
		_opponent.Hits = (int)numericHits.Value + 1;
		_opponent.Color = (ColorType)(comboColor.SelectedIndex - 1);
		_opponent.Weapon = map[comboWeapon.SelectedIndex];
	}
}
