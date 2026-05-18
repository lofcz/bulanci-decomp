namespace Editor.Scripts;

public sealed class StrmSend : ParamCommand
{
	public StrmSend(Command strm)
		: base(Opcode.StrmSend, strm)
	{
	}
}
