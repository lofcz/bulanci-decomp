using System.IO;

namespace Editor.Scripts;

public class IfNotEqual : ParamCommand
{
	private string _label;

	public IfNotEqual(Command op1, Command op2, string label)
		: base(Opcode.IfNotEqual, op1, op2)
	{
		_label = label;
	}

	public IfNotEqual(Command op1, int op2, string label)
		: base(Opcode.IfNotEqual, op1, new IntConst(op2))
	{
		_label = label;
	}

	public override void WriteCode(BinaryWriter writer)
	{
		base.WriteCode(writer);
		WriteLabel(writer, _label);
	}
}
