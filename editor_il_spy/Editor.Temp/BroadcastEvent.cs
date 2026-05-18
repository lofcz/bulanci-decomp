namespace Editor.Temp;

public class BroadcastEvent : CommandEvent
{
	private View _parent;

	public View Parent => _parent;

	public BroadcastEvent(int cmd, object info)
		: base(EventType.Broadcast, cmd, info)
	{
		_parent = null;
	}

	public BroadcastEvent(View parent, int cmd, object info)
		: base(EventType.Broadcast, cmd, info)
	{
		_parent = parent;
	}
}
