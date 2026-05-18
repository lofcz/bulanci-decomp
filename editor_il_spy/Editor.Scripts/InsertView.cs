namespace Editor.Scripts;

public sealed class InsertView : ParamCommand
{
	public InsertView(Command v)
		: base(Opcode.InsertView, v)
	{
	}
}
