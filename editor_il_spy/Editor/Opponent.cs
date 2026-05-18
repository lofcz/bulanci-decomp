using System;
using System.Xml.Serialization;

namespace Editor;

public class Opponent : ICloneable
{
	private int _speed;

	private ColorType _color;

	private WeaponType _weapon;

	private int _hits;

	private Level _level;

	[XmlAttribute]
	public int Speed
	{
		get
		{
			return _speed;
		}
		set
		{
			int num = Math.Min(200, Math.Max(40, value));
			if (_speed != num)
			{
				_speed = num;
				OpponentChanged();
			}
		}
	}

	[XmlAttribute]
	public ColorType Color
	{
		get
		{
			return _color;
		}
		set
		{
			if (_color != value)
			{
				_color = value;
				OpponentChanged();
			}
		}
	}

	[XmlAttribute]
	public WeaponType Weapon
	{
		get
		{
			return _weapon;
		}
		set
		{
			if (_weapon != value)
			{
				_weapon = value;
				OpponentChanged();
			}
		}
	}

	[XmlAttribute]
	public int Hits
	{
		get
		{
			return _hits;
		}
		set
		{
			if (_hits != value)
			{
				_hits = value;
				OpponentChanged();
			}
		}
	}

	[XmlIgnore]
	public bool Undead
	{
		get
		{
			return Hits == 1000000;
		}
		set
		{
			Hits = ((!value) ? 1 : 1000000);
		}
	}

	[XmlIgnore]
	public Level Level
	{
		get
		{
			return _level;
		}
		set
		{
			_level = value;
		}
	}

	private void OpponentChanged()
	{
		if (_level != null)
		{
			_level.OnOpponentModified(this);
		}
	}

	public void Remove()
	{
		if (Level != null)
		{
			Level.Opponents.Remove(this);
		}
	}

	public Opponent()
	{
		_speed = 100;
		_color = ColorType.Random;
		_weapon = WeaponType.Gun;
		_hits = 1;
		_level = null;
	}

	public Opponent(Opponent src)
	{
		_speed = src.Speed;
		_color = src.Color;
		_weapon = src.Weapon;
		_hits = src.Hits;
		_level = null;
	}

	public Opponent(int speed, ColorType color, WeaponType weapon, int hits)
	{
		_speed = speed;
		_color = color;
		_weapon = weapon;
		_hits = hits;
		_level = null;
	}

	public object Clone()
	{
		return new Opponent(this);
	}
}
