using System.IO;

namespace Editor.Scripts;

public sealed class IntConst : Command
{
	private int _value;

	public IntConst(int v)
		: base(Opcode.IntConst)
	{
		_value = v;
	}

	public override void WriteCode(BinaryWriter writer)
	{
		base.WriteCode(writer);
		writer.Write(_value);
	}
}
