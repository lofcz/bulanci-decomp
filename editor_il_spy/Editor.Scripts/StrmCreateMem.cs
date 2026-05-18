namespace Editor.Scripts;

public sealed class StrmCreateMem : ParamCommand
{
	public StrmCreateMem(int start, int resize)
		: base(Opcode.StrmCreateMem, new IntConst(start), new IntConst(resize))
	{
	}
}
