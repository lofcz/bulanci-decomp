using System.Windows.Forms;

namespace Editor.Temp;

public sealed class KeyEvent : Event
{
	private Keys _keys;

	public Keys Key => _keys;

	public bool Ctrl => (_keys & Keys.Control) != 0;

	public bool Alt => (_keys & Keys.Alt) != 0;

	public bool Shift => (_keys & Keys.Shift) != 0;

	public KeyEvent(EventType type, Keys keys)
		: base(type)
	{
		_keys = keys;
	}
}
