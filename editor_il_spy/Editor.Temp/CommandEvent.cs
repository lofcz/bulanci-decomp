namespace Editor.Temp;

public class CommandEvent : Event
{
	private int _command;

	private object _info;

	public int Command => _command;

	public object Info => _info;

	public CommandEvent(int cmd, object info)
		: base(EventType.Command)
	{
		_command = cmd;
		_info = info;
	}

	protected CommandEvent(EventType type, int cmd, object info)
		: base(type)
	{
		_command = cmd;
		_info = info;
	}
}
