namespace Editor.Scripts;

public sealed class StrmDestroy : ParamCommand
{
	public StrmDestroy(Command strm)
		: base(Opcode.StrmDestroy, strm)
	{
	}
}
