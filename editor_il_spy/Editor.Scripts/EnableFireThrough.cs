namespace Editor.Scripts;

public sealed class EnableFireThrough : ParamCommand
{
	public EnableFireThrough(Command obj, Command enable)
		: base(Opcode.EnableFireThrough, obj, enable)
	{
	}

	public EnableFireThrough(Command obj, bool enable)
		: base(Opcode.EnableFireThrough, obj, new IntConst(enable ? 1 : 0))
	{
	}
}
