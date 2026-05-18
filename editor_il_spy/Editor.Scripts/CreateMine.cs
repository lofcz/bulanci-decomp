namespace Editor.Scripts;

public sealed class CreateMine : ParamCommand
{
	public CreateMine(int x, int y)
		: base(Opcode.CreateMine, new IntConst(x), new IntConst(y))
	{
	}
}
