using System;

namespace Editor;

public sealed class ResourceEventArgs : EventArgs
{
	private ResourceItem _item;

	public ResourceItem ResourceItem => _item;

	public ResourceEventArgs(ResourceItem item)
	{
		_item = item;
	}
}
