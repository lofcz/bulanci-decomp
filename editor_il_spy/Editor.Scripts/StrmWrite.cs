namespace Editor.Scripts;

public sealed class StrmWrite : ParamCommand
{
	public StrmWrite(Command strm, Command val, int size)
		: base(Opcode.StrmWrite, strm, val, new IntConst(size))
	{
	}
}
