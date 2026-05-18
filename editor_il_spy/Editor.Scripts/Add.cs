namespace Editor.Scripts;

public sealed class Add : ParamCommand
{
	public Add(Command v1, Command v2)
		: base(Opcode.Add, v1, v2)
	{
	}

	public Add(Command v1, int v2)
		: base(Opcode.Add, v1, new IntConst(v2))
	{
	}
}
