namespace Editor.Temp;

public class Event
{
	private EventType _type;

	private View _view;

	private object _sender;

	public EventType What => _type;

	public bool Focused => (_type & EventType.Focused) != 0;

	public bool Positional => (_type & EventType.Mouse) != 0;

	public bool IsEmpty => _type == EventType.Nothing;

	public object Sender
	{
		get
		{
			return _sender;
		}
		set
		{
			_sender = value;
		}
	}

	public Event()
	{
		_type = EventType.Nothing;
	}

	public Event(EventType t)
	{
		_type = t;
	}

	public void Clear(View v)
	{
		_type = EventType.Nothing;
		_view = v;
	}
}
