using System.IO;

namespace Editor.Scripts;

public sealed class StrConst : Command
{
	private string _value;

	public StrConst(string v)
		: base(Opcode.StrConst)
	{
		_value = v;
	}

	public override void WriteCode(BinaryWriter writer)
	{
		base.WriteCode(writer);
		for (int i = 0; i < _value.Length; i++)
		{
			writer.Write((ushort)_value[i]);
		}
		writer.Write((ushort)0);
	}
}
