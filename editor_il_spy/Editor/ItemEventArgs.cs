using System;

namespace Editor;

public sealed class ItemEventArgs : EventArgs
{
	private LevelItem _item;

	public LevelItem LevelItem => _item;

	public ItemEventArgs(LevelItem item)
	{
		_item = item;
	}
}
