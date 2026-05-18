namespace Editor.Scripts;

public sealed class Rand : ParamCommand
{
	public Rand(Command p1, Command p2)
		: base(Opcode.Rand, p1, p2)
	{
	}

	public Rand(int p1, int p2)
		: base(Opcode.Rand, new IntConst(p1), new IntConst(p2))
	{
	}
}
