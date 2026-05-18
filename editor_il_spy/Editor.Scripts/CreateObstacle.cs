using System.Drawing;

namespace Editor.Scripts;

public sealed class CreateObstacle : ParamCommand
{
	public CreateObstacle(Command ax, Command ay, Command bx, Command by)
		: base(Opcode.CreateObstacle, ax, ay, bx, by)
	{
	}

	public CreateObstacle(Rectangle rect)
		: base(Opcode.CreateObstacle, new IntConst(rect.X), new IntConst(rect.Y), new IntConst(rect.X + rect.Width), new IntConst(rect.Y + rect.Height))
	{
	}

	public CreateObstacle(int x, int y, int width, int height)
		: base(Opcode.CreateObstacle, new IntConst(x), new IntConst(y), new IntConst(x + width), new IntConst(y + height))
	{
	}
}
