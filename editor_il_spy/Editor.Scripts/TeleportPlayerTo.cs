namespace Editor.Scripts;

public class TeleportPlayerTo : ParamCommand
{
	public TeleportPlayerTo(Command player, Command x, Command y, bool always)
		: base(Opcode.TeleportPlayerTo, player, x, y, new IntConst(always ? 1 : 0))
	{
	}
}
