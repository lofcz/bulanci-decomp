namespace Editor.Scripts;

public sealed class IsServer : Command
{
	public IsServer()
		: base(Opcode.IsServer)
	{
	}
}
