using System.IO;

namespace Editor.Scripts;

public sealed class IfEqual : ParamCommand
{
	private string _label;

	public IfEqual(Command op1, Command op2, string label)
		: base(Opcode.IfEqual, op1, op2)
	{
		_label = label;
	}

	public IfEqual(Command op1, int op2, string label)
		: base(Opcode.IfEqual, op1, new IntConst(op2))
	{
		_label = label;
	}

	public override void WriteCode(BinaryWriter writer)
	{
		base.WriteCode(writer);
		WriteLabel(writer, _label);
	}
}
