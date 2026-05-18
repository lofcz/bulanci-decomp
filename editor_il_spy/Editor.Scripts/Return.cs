namespace Editor.Scripts;

public sealed class Return : ParamCommand
{
	public Return(Command v)
		: base(Opcode.Return, v)
	{
	}

	public Return()
		: base(Opcode.Return, new IntConst(0))
	{
	}
}
