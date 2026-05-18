namespace Editor.Scripts;

public sealed class StrmSetSize : ParamCommand
{
	public StrmSetSize(Command strm, Command size)
		: base(Opcode.StrmSetSize, strm, size)
	{
	}

	public StrmSetSize(Command strm, int size)
		: base(Opcode.StrmSetSize, strm, new IntConst(size))
	{
	}

	public StrmSetSize(Command strm)
		: base(Opcode.StrmSetSize, strm, new IntConst(0))
	{
	}
}
