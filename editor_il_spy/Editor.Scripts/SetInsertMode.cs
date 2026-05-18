namespace Editor.Scripts;

public sealed class SetInsertMode : ParamCommand
{
	public SetInsertMode(Command v)
		: base(Opcode.SetInsertMode, v)
	{
	}

	public SetInsertMode(int mode)
		: base(Opcode.SetInsertMode, new IntConst(mode))
	{
	}
}
