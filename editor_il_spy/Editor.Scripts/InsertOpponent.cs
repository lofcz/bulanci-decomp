namespace Editor.Scripts;

public sealed class InsertOpponent : ParamCommand
{
	public InsertOpponent(Opponent opponent)
		: base(Opcode.InsertOpponent, new IntConst((int)opponent.Color), new IntConst(opponent.Hits), new IntConst(opponent.Speed), new IntConst((int)opponent.Weapon))
	{
	}
}
