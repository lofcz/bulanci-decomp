using System.IO;

namespace Editor.Scripts;

public sealed class LoadPreface : Command
{
	private ResourceItem _item;

	public LoadPreface(ResourceItem item)
		: base(Opcode.LoadPreface)
	{
		_item = item;
	}

	public override void WriteCode(BinaryWriter writer)
	{
		base.WriteCode(writer);
		writer.Write(_item.ID);
	}
}
