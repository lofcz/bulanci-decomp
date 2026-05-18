using System.Drawing;

namespace Editor.Scripts;

public sealed class DefineTraceArea : ParamCommand
{
	public DefineTraceArea(int id, Rectangle rect, int mask)
		: base(Opcode.DefineTraceArea, new IntConst(id), new IntConst(rect.Left), new IntConst(rect.Top), new IntConst(rect.Right), new IntConst(rect.Bottom), new IntConst(mask))
	{
	}
}
