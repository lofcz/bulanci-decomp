namespace Editor.Scripts;

public sealed class SetCommStrm : ParamCommand
{
	public SetCommStrm(Command strm)
		: base(Opcode.SetCommStrm, strm)
	{
	}
}
