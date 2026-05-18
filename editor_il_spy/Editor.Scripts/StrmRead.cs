namespace Editor.Scripts;

public sealed class StrmRead : ParamCommand
{
	public StrmRead(Command strm, int size)
		: base(Opcode.StrmRead, strm, new IntConst(size))
	{
	}
}
