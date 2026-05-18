using System.IO;

namespace Editor.Scripts;

public sealed class Goto : Command
{
	private string _label;

	public Goto(string label)
		: base(Opcode.Goto)
	{
		_label = label;
	}

	public override void WriteCode(BinaryWriter writer)
	{
		base.WriteCode(writer);
		WriteLabel(writer, _label);
	}
}
