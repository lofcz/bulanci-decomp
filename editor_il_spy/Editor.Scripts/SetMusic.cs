using System.IO;

namespace Editor.Scripts;

public sealed class SetMusic : Command
{
	private ResourceItem _item;

	public SetMusic(ResourceItem item)
		: base(Opcode.SetMusic)
	{
		_item = item;
	}

	public override void WriteCode(BinaryWriter writer)
	{
		base.WriteCode(writer);
		writer.Write(_item.ID);
		writer.Write(0);
	}
}
