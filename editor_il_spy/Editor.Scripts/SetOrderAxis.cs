namespace Editor.Scripts;

public sealed class SetOrderAxis : ParamCommand
{
	public SetOrderAxis(Command obj, Command axis)
		: base(Opcode.SetOrderAxis, obj, axis)
	{
	}

	public SetOrderAxis(Command obj, int axis)
		: base(Opcode.SetOrderAxis, obj, new IntConst(axis))
	{
	}
}
